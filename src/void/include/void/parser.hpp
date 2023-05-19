#pragma once

#include "token.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "void/ast.hpp"
#include "void/token.hpp"

#include <memory>
#include <unordered_map>

namespace Void {
  class Parser {
  public:
    enum Precedence {
      unknown_p = 0, 
      lowest_p,
      equal_p, // == !=
      less_greater_p, // < <= or > >=
      sum_p, // + - 
      product_p, // * /
      prefix_p, // -x or !x
      call_p, // fn(x)
      index_p, // array[index]
    };
    
    Parser(std::string const&);

    std::unique_ptr<Program> parse();
    std::vector<std::string> const& error() const;

  private:
    Parser();

    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<Expression> parse_expression(Precedence);
    
    std::unique_ptr<Statement> parse_let_statement();
    std::unique_ptr<Statement> parse_return_statement();
    std::unique_ptr<Statement> parse_expression_statement();

    std::unique_ptr<Expression> parse_prefix_expression();
    std::unique_ptr<Expression> parse_infix_expression(std::unique_ptr<Expression>);

    std::unique_ptr<BlockStatement> parse_block_statement();
    
    std::unique_ptr<Expression> parse_if_expression();
    std::unique_ptr<Expression> parse_group_expression();
    std::unique_ptr<Expression> parse_identifier();

    std::unique_ptr<Expression> parse_call_expression(std::unique_ptr<Expression>);
    std::unique_ptr<Expression> parse_index_expression(std::unique_ptr<Expression>);

    std::unique_ptr<Expression> parse_integer_literal();
    std::unique_ptr<Expression> parse_string_literal();
    std::unique_ptr<Expression> parse_boolean_literal();
    std::unique_ptr<Expression> parse_function_literal();
    std::unique_ptr<Expression> parse_array_literal();

    Token next_token();
    Token peek_token();
    bool cur_token_type_is(Token::TokenType); 
    bool peek_token_type_is(Token::TokenType); 
    bool expect_token_type_is(Token::TokenType);
    Precedence cur_token_precedence();
    Precedence peek_token_precedence();
    void parse_error(std::string, std::string);
    void parse_error(Token::TokenType, Token);
    
  private:
    using ParsePrefixFunc = std::function<std::unique_ptr<Expression>()>;
    using ParseInfixFunc = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>&&)>;

    std::map<Token::TokenType, ParsePrefixFunc> _prefix_parse_func_map;
    std::map<Token::TokenType, ParseInfixFunc> _infix_parse_func_map;

    static std::map<Token::TokenType, Precedence> _t_to_p; 

    std::vector<std::string> _errors;
    
    std::vector<Token> _tokens;
    Token _token;
    int _cur;
    int _nxt;
  };
}
