#pragma once

#include <vector>
#include <void/token.hpp>
#include <void/lexer.hpp>
#include <void/ast.hpp>
#include <void/parser.hpp>
#include <void/object.hpp>

#include <memory>

namespace Void {
  class Evaluator {
  public:
    Evaluator();

    std::shared_ptr<Object> eval(std::string const&); 
    
  private:
    std::shared_ptr<Object> eval(AstNode*, Environment*);
    
    std::shared_ptr<Object> eval_program(Program*, Environment*);
    
    std::shared_ptr<Object> eval_let_statement(LetStatement*, Environment*);
    std::shared_ptr<Object> eval_return_statement(ReturnStatement*, Environment*);
    std::shared_ptr<Object> eval_expression_statement(ExpressionStatement*, Environment*);

    std::shared_ptr<Object> eval_prefix_expression(PrefixExpression*, Environment*);
    std::shared_ptr<Object> eval_infix_expression(InfixExpression*, Environment*);

    std::shared_ptr<Object> eval_if_expression(IfExpression*, Environment*);
    std::shared_ptr<Object> eval_identifier(Identifier*, Environment*);
    std::shared_ptr<Object> eval_block_statement(BlockStatement*, Environment*);
    std::shared_ptr<Object> eval_call_expression(CallExpression*, Environment*);
    std::shared_ptr<Object> eval_index_expression(IndexExpression*, Environment*);
    
    std::shared_ptr<Object> eval_integer_literal(IntegerLiteral*, Environment*);
    std::shared_ptr<Object> eval_boolean_literal(BooleanLiteral*, Environment*);
    std::shared_ptr<Object> eval_string_literal(StringLiteral*, Environment*);
    std::shared_ptr<Object> eval_array_literal(ArrayLiteral*, Environment*);
    std::shared_ptr<Object> eval_function_literal(FunctionLiteral*, Environment*);

    std::shared_ptr<Object> eval_bang_operator_expression(Object*);
    std::shared_ptr<Object> eval_minus_operator_expression(Object*);
    std::shared_ptr<Object> eval_integer_infix_expression(std::string const& op, Integer*, Integer*);
    std::shared_ptr<Object> eval_string_infix_expression(std::string const& op, String*, String*);
    std::shared_ptr<Object> eval_array_infix_expression(std::string const& op, Array*, Array*);
    std::shared_ptr<Object> eval_apply_function(Function*, Environment*);

    bool is_truthy(Object*);
    bool is_error(Object*);
    bool is_null(Object*);
    
  private:
    std::unique_ptr<Environment> _env;

    std::vector<std::unique_ptr<Program>> _programs; // 
  };
}

