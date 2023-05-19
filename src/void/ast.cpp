#include <cstdlib>
#include <exception>
#include <malloc.h>
#include <vector>
#include <void/ast.hpp>
#include <void/token.hpp>
#include <cstdio>
#include <memory>

namespace Void {
  // Statement
  Statement::Statement(Token token)
    : _token(token) {}

  std::string Statement::token_literal() const {
    return _token.literal;
  }

  // Expression
  Expression::Expression(Token token)
    : _token(token) {}

  std::string Expression::token_literal() const {
    return _token.literal;
  }

  // Program
  std::string Program::to_string() const {
    std::string res;
    for (auto& stmt : _statements) {
      res += stmt->to_string(); 
    }
    return res;
  }

  std::string Program::token_literal() const {
    if (!_statements.empty()) {
      return _statements[0]->token_literal(); 
    } else {
      return "";
    }
  }

  std::vector<std::unique_ptr<Statement>> const& Program::statements() const {
    return _statements;
  }

  void Program::append(std::unique_ptr<Statement> stmt) {
    _statements.emplace_back(std::move(stmt)); 
  }

  // Identifier
  Identifier::Identifier(Token token)
    : Expression(token), _value(token.literal) {}

  std::string Identifier::to_string() const {
    return _value; 
  }

  std::string Identifier::value() const {
    return _value;
  }

  // LetStatement
  std::string LetStatement::to_string() const {
    return "let " + _identifier->to_string() + " = " + _expression->to_string(); 
  }

  Identifier* LetStatement::identier() const {
    return _identifier.get(); 
  }

  Expression* LetStatement::expression() const {
    return _expression.get(); 
  }

  void LetStatement::set_identifier(std::unique_ptr<Identifier> ident) {
    _identifier.swap(ident); 
  }

  void LetStatement::set_expression(std::unique_ptr<Expression> expr) {
    _expression.swap(expr); 
  }

  // ReturnStatment
  std::string ReturnStatement::to_string() const {
    return "return " + _expression->to_string(); 
  }

  Expression* ReturnStatement::expression() const {
    return _expression.get();
  }

  void ReturnStatement::set_expression(std::unique_ptr<Expression> expr) {
    _expression.swap(expr); 
  }

  // ExpressionStatement
  std::string ExpressionStatement::to_string() const {
    return (_expression == nullptr ? "" : _expression->to_string()); 
  }

  Expression* ExpressionStatement::expression() const {
    return _expression.get();
  }

  void ExpressionStatement::set_expression(std::unique_ptr<Expression> expr) {
    _expression.swap(expr); 
  }

  // BlockStatement
  std::string BlockStatement::to_string() const {
    std::string res;
    for (auto& stmt : _statements) {
      res += stmt->to_string(); 
    }
    if (!_statements.empty()) {
      res = "{ " + res + " }";
    } else {
      res = "{}";
    }
    return res;
  }

  std::vector<std::unique_ptr<Statement>> const& BlockStatement::statements() const {
    return _statements;
  }

  void BlockStatement::append(std::unique_ptr<Statement> stmt) {
    _statements.emplace_back(std::move(stmt)); 
  }

  // IntegerLiteral
  IntegerLiteral::IntegerLiteral(Token token)
    : Expression(token), _value(std::stoi(token.literal)) {}
 
  std::string IntegerLiteral::to_string() const {
    return std::to_string(_value);
  }

  int IntegerLiteral::value() const {
    return _value;
  }

  // BooleanLiteral
  BooleanLiteral::BooleanLiteral(Token token)
    : Expression(token), _value(token.type == Token::true_t) {}

  std::string BooleanLiteral::to_string() const {
    return _value ? "true" : "false";
  }

  bool BooleanLiteral::value() const {
    return _value;
  }

  // StringLiteral
  StringLiteral::StringLiteral(Token token)
    : Expression(token), _value(token.literal) {}

  std::string StringLiteral::to_string() const {
    return "\"" + _value + "\"";
  }

  std::string StringLiteral::value() const {
    return _value;
  }

  // ArrayLiteral
  std::string ArrayLiteral::to_string() const {
    std::string res;
    bool first = false; 
    for (auto& expr : _expressions) {
      if (first) {
	res += ", ";
      }
      first = true;
      res += expr->to_string();
    }
    return "[" + res + "]";
  }

  std::vector<std::unique_ptr<Expression>> const& ArrayLiteral::expressions() const {
    return _expressions;
  }

  void ArrayLiteral::append(std::unique_ptr<Expression> expr) {
    _expressions.emplace_back(std::move(expr)); 
  }

  // FunctionLiteral
  std::string FunctionLiteral::to_string() const {
    std::string para, body;
    bool first = false; 
    for (auto& ident : _parameters) {
      if (first) {
	para += ", "; 
      }
      first = true;
      para += ident->to_string(); 
    }
    if (_body != nullptr) {
      body = _body->to_string(); 
    }
    return "fn (" + para + ") " + body;
  }

  std::vector<std::unique_ptr<Identifier>> const& FunctionLiteral::parameters() const {
    return _parameters;
  }

  BlockStatement* FunctionLiteral::body() const {
    return _body.get();
  }

  void FunctionLiteral::append_parameters(std::unique_ptr<Identifier> ident) {
    _parameters.emplace_back(std::move(ident)); 
  }

  void FunctionLiteral::set_body(std::unique_ptr<BlockStatement> stmt) {
    _body.swap(stmt);
  }

  // IfExpression
  std::string IfExpression::to_string() const {
    std::string res, cond, cons, alt;
    cond = _condition->to_string();
    if (_consequence != nullptr) {
      cons = _consequence->to_string();
    }
    if (_alternative != nullptr) {
      alt = _alternative->to_string();
    }
    res += "if (" + cond + ") " + cons;
    if (_alternative != nullptr) {
      res += " else " + alt;
    }
    return res;
  }

  Expression* IfExpression::condition() const {
    return _condition.get();
  }

  BlockStatement* IfExpression::consequence() const {
    return _consequence.get();
  }

  BlockStatement* IfExpression::alternative() const {
    return _alternative.get();
  }

  void IfExpression::set_condition(std::unique_ptr<Expression> expr) {
    _condition.swap(expr); 
  }

  void IfExpression::set_consequence(std::unique_ptr<BlockStatement> stmt) {
    _consequence.swap(stmt); 
  }

  void IfExpression::set_alternative(std::unique_ptr<BlockStatement> stmt) {
    _alternative.swap(stmt);
  }

  // CallExpression
  std::string CallExpression::to_string() const {
    std::string arguments;
    bool first = false; 
    for (auto& expr : _arguments) {
      if (first) {
	arguments += ", ";
      }
      first = true;
      arguments += expr->to_string();
    }
    return _function->to_string() + "(" + arguments + ")";
  }

  Expression* CallExpression::function() const {
    return _function.get();
  }

  std::vector<std::unique_ptr<Expression>> const& CallExpression::arguments() const {
    return _arguments;
  }

  void CallExpression::set_function(std::unique_ptr<Expression> expr) {
    _function.swap(expr);
  }

  void CallExpression::append_arguments(std::unique_ptr<Expression> expr) {
    _arguments.emplace_back(std::move(expr)); 
  }

  // IndexExpression
  std::string IndexExpression::to_string() const {
    return _array->to_string() + "[" + _index->to_string() + "]";
  }

  Expression* IndexExpression::index() const {
    return _index.get();
  }
  
  Expression* IndexExpression::array() const {
    return _array.get(); 
  }

  void IndexExpression::set_index(std::unique_ptr<Expression> expr) {
    return _index.swap(expr); 
  }

  void IndexExpression::set_array(std::unique_ptr<Expression> expr) {
    return _array.swap(expr); 
  }

  // PrefixExpression
  PrefixExpression::PrefixExpression(Token token)
    : Expression(token), _op(token.literal) {}

  std::string PrefixExpression::to_string() const {
    if (_right == nullptr) {
      return "()";
    }
    return "(" + _op + _right->to_string() + ")"; 
  }

  std::string PrefixExpression::op() const {
    return _op; 
  }

  Expression* PrefixExpression::right() const {
    return _right.get(); 
  }

  void PrefixExpression::set_right(std::unique_ptr<Expression> expr) {
    _right.swap(expr); 
  }

  // InfixExpression
  InfixExpression::InfixExpression(Token token)
    : Expression(token), _op(token.literal) {}

  std::string InfixExpression::to_string() const {
    if (_left == nullptr || _right == nullptr) {
      return "()";
    }
    return "(" + _left->to_string() + " " + _op + " " + _right->to_string() + ")"; 
  }

  std::string InfixExpression::op() const {
    return _op;
  }

  Expression* InfixExpression::left() const {
    return _left.get(); 
  }

  Expression* InfixExpression::right() const {
    return _right.get(); 
  }

  void InfixExpression::set_left(std::unique_ptr<Expression> expr) {
    _left.swap(expr); 
  }

  void InfixExpression::set_right(std::unique_ptr<Expression> expr) {
    _right.swap(expr); 
  }
}
