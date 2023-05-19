#include <void/builtin.hpp>
#include <void/ast.hpp>
#include <void/object.hpp>
#include <void/parser.hpp>
#include <void/evaluator.hpp>
#include <memory>
#include <cstddef>
#include <vector>

namespace Void {
  Evaluator::Evaluator()
    : _env(std::make_unique<Environment>()) {}

  std::shared_ptr<Object> Evaluator::eval(std::string const& input) {
    Parser parser(input);
    _programs.emplace_back(parser.parse());
    return eval(_programs.back().get(), _env.get()); 
  }

  std::shared_ptr<Object> Evaluator::eval(AstNode* node, Environment* env) {
    if (auto program = dynamic_cast<Program*>(node)) {
      return eval_program(program, env); 
    } else if (auto let_stmt = dynamic_cast<LetStatement*>(node)) {
      return eval_let_statement(let_stmt, env); 
    } else if (auto ret_stmt = dynamic_cast<ReturnStatement*>(node)) {
      return eval_return_statement(ret_stmt, env); 
    } else if (auto expr_stmt = dynamic_cast<ExpressionStatement*>(node)) {
      return eval_expression_statement(expr_stmt, env); 
    } else if (auto prefix_expr = dynamic_cast<PrefixExpression*>(node)) {
      return eval_prefix_expression(prefix_expr, env);
    } else if (auto infix_expr = dynamic_cast<InfixExpression*>(node)) {
      return eval_infix_expression(infix_expr, env);
    } else if (auto if_expr = dynamic_cast<IfExpression*>(node)) {
      return eval_if_expression(if_expr, env);
    } else if (auto ident = dynamic_cast<Identifier*>(node)) {
      return eval_identifier(ident, env);
    } else if (auto block_stmt = dynamic_cast<BlockStatement*>(node)) {
      return eval_block_statement(block_stmt, env);
    } else if (auto call_expr = dynamic_cast<CallExpression*>(node)) {
      return eval_call_expression(call_expr, env);
    } else if (auto index_expr = dynamic_cast<IndexExpression*>(node)) {
      return eval_index_expression(index_expr, env);
    } else if (auto int_lit = dynamic_cast<IntegerLiteral*>(node)) {
      return eval_integer_literal(int_lit, env);
    } else if (auto bool_lit = dynamic_cast<BooleanLiteral*>(node)) {
      return eval_boolean_literal(bool_lit, env); 
    } else if (auto str_lit = dynamic_cast<StringLiteral*>(node)) {
      return eval_string_literal(str_lit, env); 
    } else if (auto arr_lit = dynamic_cast<ArrayLiteral*>(node)) {
      return eval_array_literal(arr_lit, env); 
    } else if (auto func_lit = dynamic_cast<FunctionLiteral*>(node)) {
      return eval_function_literal(func_lit, env);
    } else {
      return nullptr;
    }
  }

  std::shared_ptr<Object> Evaluator::eval_program(Program* program, Environment* env) {
    std::shared_ptr<Object> ret = std::make_shared<Null>();
    
    auto& stmts = program->statements();
    for (auto& stmt : stmts) {
      auto obj = eval(stmt.get(), env);

      if (obj->type() == Object::return_object_t) {
	return obj->cast<Return>()->value(); 
      } else if (obj->type() == Object::error_object_t) {
	return obj;
      }

      ret.swap(obj);
    }

    return ret;
  }

  std::shared_ptr<Object> Evaluator::eval_let_statement(LetStatement* node, Environment* env) {
    auto obj = eval(node->expression(), env);
    if (is_error(obj.get())) {
      return obj;
    }
    env->set(node->identier()->value(), std::move(obj));
    return null_obj;
  }

  std::shared_ptr<Object> Evaluator::eval_return_statement(ReturnStatement* node, Environment* env) {
    auto obj = eval(node->expression(), env);
    if (is_error(obj.get())) {
      return obj;
    }
    return std::make_shared<Return>(std::move(obj)); 
  }

  std::shared_ptr<Object> Evaluator::eval_expression_statement(ExpressionStatement* node, Environment* env) {
    return eval(node->expression(), env);
  }

  std::shared_ptr<Object> Evaluator::eval_prefix_expression(PrefixExpression* node, Environment* env) {
    auto obj = eval(node->right(), env); 
    if (node->op() == "!") {
      return eval_bang_operator_expression(obj.get());
    } else if (node->op() == "=") {
      return eval_minus_operator_expression(obj.get());
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> Evaluator::eval_infix_expression(InfixExpression* node, Environment* env) {
    auto left = eval(node->left(), env);
    auto right = eval(node->right(), env);
    auto op = node->op();
    if (left->type() == Object::integer_object_t &&
	right->type() == Object::integer_object_t) {
      return eval_integer_infix_expression(op, left->cast<Integer>(), right->cast<Integer>());
    } else if (left->type() == Object::string_object_t &&
	       right->type() == Object::string_object_t) {
      return eval_string_infix_expression(op, left->cast<String>(), right->cast<String>()); 
    } else if (left->type() == Object::array_object_t &&
	       right->type() == Object::array_object_t) {
      return eval_array_infix_expression(op, left->cast<Array>(), right->cast<Array>());
    } else if (left->type() != right->type()) {
      return std::make_shared<Error>();
    } else if (op == "==") {
      return left.get() == right.get() ? true_obj : false_obj;
    } else if (op == "!=") {
      return left.get() == right.get() ? false_obj : true_obj; 
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> Evaluator::eval_if_expression(IfExpression* node, Environment* env) {
    auto cond = eval(node->condition(), env);

    if (is_error(cond.get())) {
      return cond;
    }
    
    if (is_truthy(cond.get())) {
      return eval(node->consequence(), env); 
    } else {
      return eval(node->alternative(), env);
    }
  }

  std::shared_ptr<Object> Evaluator::eval_identifier(Identifier* node, Environment* env) {
    auto obj = env->get(node->value());
    if (is_null(obj.get())) {
      auto it = builtin_func_map.find(node->value());
      if (it != builtin_func_map.end()) {
	return it->second;
      }
    }
    return obj;
  }

  std::shared_ptr<Object> Evaluator::eval_block_statement(BlockStatement* node, Environment* env) {
    std::shared_ptr<Object> ret = std::make_shared<Null>();

    auto& stmts = node->statements();
    for (auto& stmt : stmts) {
      auto obj = eval(stmt.get(), env);

      if (obj->type() == Object::return_object_t) {
	return obj->cast<Return>()->value(); 
      } else if (obj->type() == Object::error_object_t) {
	return obj;
      }

      ret.swap(obj);
    }

    return ret;
  }

  std::shared_ptr<Object> Evaluator::eval_call_expression(CallExpression* node, Environment* env) {
    auto func_obj = eval(node->function(), env);
    if (func_obj->type() != Object::function_object_t &&
	func_obj->type() != Object::builtin_object_t) {
      if (func_obj->type() == Object::error_object_t) {
	return func_obj;
      } else { 
	return std::make_shared<Error>();
      }
    }


    std::vector<std::shared_ptr<Object>> args_obj;
    auto& args_expr = node->arguments();
    for (auto& expr : args_expr) {
      args_obj.emplace_back(eval(expr.get(), env)); 
    }

    
    if (func_obj->type() == Object::builtin_object_t) {
      return func_obj->cast<Builtin>()->run(args_obj); 
    }
    
    auto func_expr = func_obj->cast<Function>()->function();
    auto& params_expr = func_expr->parameters();
    if (args_expr.size() != params_expr.size()) {
      return std::make_shared<Error>();
    }
    for (std::size_t i = 0; i < args_expr.size(); ++i) {
      auto& param = params_expr[i];
      
      func_obj->cast<Function>()->env()->set(param->value(), args_obj[i]);
    }
    
    return eval_apply_function(func_obj->cast<Function>(), func_obj->cast<Function>()->env());
  }

  std::shared_ptr<Object> Evaluator::eval_index_expression(IndexExpression* node, Environment* env) {
    auto arr = eval(node->array(), env);
    if (arr->type() != Object::array_object_t) {
      return std::make_shared<Error>();
    }

    auto index = eval(node->index(), env);
    if (index->type() != Object::integer_object_t) {
      return std::make_shared<Error>();
    }

    return arr->cast<Array>()->elements()[index->cast<Integer>()->value()];
  }
  
  std::shared_ptr<Object> Evaluator::eval_integer_literal(IntegerLiteral* node, Environment* env) {
    return std::make_shared<Integer>(node->value()); 
  }

  std::shared_ptr<Object> Evaluator::eval_boolean_literal(BooleanLiteral* node, Environment* env) {
    return std::make_shared<Boolean>(node->value()); 
  }

  std::shared_ptr<Object> Evaluator::eval_string_literal(StringLiteral* node, Environment* env) {
    return std::make_shared<String>(node->value()); 
  }

  std::shared_ptr<Object> Evaluator::eval_array_literal(ArrayLiteral* node, Environment* env) {
    auto array = std::make_shared<Array>();
      
    auto& exprs = node->expressions(); 
    for (auto& expr : exprs) {
      auto obj = eval(expr.get(), env);
      if (is_error(obj.get())) {
	return obj;
      }
      array->append(std::move(obj));
    }

    return array;
  }

  std::shared_ptr<Object> Evaluator::eval_function_literal(FunctionLiteral* node, Environment* env) {
    return std::make_shared<Function>(node, env);
  }

  std::shared_ptr<Object> Evaluator::eval_bang_operator_expression(Object* obj) {
    return is_truthy(obj) ? false_obj : true_obj; 
  }

  std::shared_ptr<Object> Evaluator::eval_minus_operator_expression(Object* obj) {
    if (auto int_obj = dynamic_cast<Integer*>(obj)) {
      return std::make_shared<Integer>(-int_obj->value()); 
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> Evaluator::eval_integer_infix_expression(std::string const& op, Integer* left, Integer* right) {
    if (op == "+") {
      return std::make_shared<Integer>(left->value() + right->value()); 
    } else if (op == "-") {
      return std::make_shared<Integer>(left->value() - right->value());
    } else if (op == "*") {
      return std::make_shared<Integer>(left->value() * right->value()); 
    } else if (op == "/") {
      return std::make_shared<Integer>(left->value() / right->value()); 
    } else if (op == "<") {
      return std::make_shared<Boolean>(left->value() < right->value()); 
    } else if (op == "<=") {
      return std::make_shared<Boolean>(left->value() <= right->value()); 
    } else if (op == ">") {
      return std::make_shared<Boolean>(left->value() > right->value()); 
    } else if (op == ">=") {
      return std::make_shared<Boolean>(left->value() >= right->value()); 
    } else if (op == "==") {
      return std::make_shared<Boolean>(left->value() == right->value()); 
    } else if (op == "!=") {
      return std::make_shared<Boolean>(left->value() != right->value());
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> Evaluator::eval_string_infix_expression(std::string const& op, String* left, String* right) {
    if (op == "+") {
      return std::make_shared<String>(left->value() + right->value()); 
    } else if (op == "<") {
      return std::make_shared<Boolean>(left->value() < right->value()); 
    } else if (op == "<=") {
      return std::make_shared<Boolean>(left->value() <= right->value()); 
    } else if (op == ">") {
      return std::make_shared<Boolean>(left->value() > right->value()); 
    } else if (op == ">=") {
      return std::make_shared<Boolean>(left->value() >= right->value()); 
    } else if (op == "==") {
      return std::make_shared<Boolean>(left->value() == right->value()); 
    } else if (op == "!=") {
      return std::make_shared<Boolean>(left->value() != right->value());
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> Evaluator::eval_array_infix_expression(std::string const& op, Array* left, Array* right) {
    if (op == "+") {
      auto arr = std::make_shared<Array>();
      for (auto& elem : left->elements()) {
	arr->append(elem);
      }
      for (auto& elem : right->elements()) {
	arr->append(elem); 
      }
      return arr;
    } else {
      return std::make_shared<Null>();
    }
  }

  std::shared_ptr<Object> Evaluator::eval_apply_function(Function* func, Environment* env) {
    auto stmt = func->function()->body();
    return eval(stmt, env);
  }
  
  bool Evaluator::is_truthy(Object* obj) {
    if (obj == false_obj.get() || obj == null_obj.get()) {
      return false; 
    }
    return true;
  }

  bool Evaluator::is_error(Object* obj) {
    return obj->type() == Object::error_object_t;
  }

  bool Evaluator::is_null(Object* obj) {
    return obj->type() == Object::null_object_t;
  }
}
