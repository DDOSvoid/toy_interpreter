#include <void/token.hpp>
#include <void/lexer.hpp>
#include <void/ast.hpp>
#include <void/parser.hpp>

#include <functional>
#include <memory>
#include <type_traits>

namespace Void {
  Parser::Parser() {
    // register prefix parse function
    _prefix_parse_func_map[Token::int_t] = std::bind(&Parser::parse_integer_literal, this);
    _prefix_parse_func_map[Token::string_t] = std::bind(&Parser::parse_string_literal, this);
    _prefix_parse_func_map[Token::true_t] = std::bind(&Parser::parse_boolean_literal, this); 
    _prefix_parse_func_map[Token::false_t] = std::bind(&Parser::parse_boolean_literal, this);
    _prefix_parse_func_map[Token::bang_t] = std::bind(&Parser::parse_prefix_expression, this); 
    _prefix_parse_func_map[Token::minus_t] = std::bind(&Parser::parse_prefix_expression, this);
    _prefix_parse_func_map[Token::if_t] = std::bind(&Parser::parse_if_expression, this);
    _prefix_parse_func_map[Token::left_paren_t] = std::bind(&Parser::parse_group_expression, this);
    _prefix_parse_func_map[Token::function_t] = std::bind(&Parser::parse_function_literal, this);
    _prefix_parse_func_map[Token::ident_t] = std::bind(&Parser::parse_identifier, this);
    _prefix_parse_func_map[Token::left_bracket_t] = std::bind(&Parser::parse_array_literal, this); 
    
    // register infix parse function
    using std::placeholders::_1;
    _infix_parse_func_map[Token::plus_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::minus_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::asterisk_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::slash_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::less_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::less_equal_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::greater_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::greater_equal_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::equal_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::not_equal_t] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_func_map[Token::left_paren_t] = std::bind(&Parser::parse_call_expression, this, _1);
    _infix_parse_func_map[Token::left_bracket_t] = std::bind(&Parser::parse_index_expression, this, _1); 
  }

  Parser::Parser(std::string const& input)
    : Parser() {
    Lexer lexer(input);
    Token token;
    do {
      token = lexer.read_token();
      _tokens.push_back(token); 
    } while (token.type != Token::eof_t);

    _cur = 0;
    _nxt = 1;
    if (static_cast<std::size_t>(_cur) < _tokens.size()) {
      _token = _tokens[_cur]; 
    } else {
      _token = {Token::eof_t, "EOF"};
    }
  }

  std::unique_ptr<Program> Parser::parse() {
    std::unique_ptr<Program> program(new Program);

    while (!cur_token_type_is(Token::eof_t)) {
      auto stmt = parse_statement();
      if (stmt) { 
	program->append(std::move(stmt));
      }
      next_token(); 
    }

    return program; 
  }

  std::vector<std::string> const& Parser::error() const {
    return _errors;
  }

  std::unique_ptr<Statement> Parser::parse_statement() {
    switch (_token.type) {
    case Token::let_t:
      return parse_let_statement();
    case Token::return_t:
      return parse_return_statement();
    default:
      return parse_expression_statement();
    }
  }

  std::unique_ptr<Expression> Parser::parse_expression(Precedence precedence) {
    auto prefix_it = _prefix_parse_func_map.find(_token.type);
    if (prefix_it == _prefix_parse_func_map.end()) {
      parse_error("prefix parse function", _token.literal);
      return nullptr; 
    }

    auto left = prefix_it->second();

    while (!peek_token_type_is(Token::semicolon_t) && precedence < peek_token_precedence()) {
      auto infix_it = _infix_parse_func_map.find(peek_token().type);
      if (infix_it == _infix_parse_func_map.end()) {
	return left;
      }

      next_token(); 
      
      auto expr = infix_it->second(std::move(left)); 
      left.swap(expr); 
    }
    
    return left;
  }

  std::unique_ptr<Statement> Parser::parse_let_statement() {
    auto stmt = std::make_unique<LetStatement>(_token);

    if (!expect_token_type_is(Token::ident_t)) {
      parse_error(Token::ident_t, peek_token()); 
      return nullptr; 
    }
    stmt->set_identifier(std::make_unique<Identifier>(_token));

    if (!expect_token_type_is(Token::assign_t)) {
      parse_error(Token::assign_t, peek_token());
      return nullptr;
    }

    next_token();

    if (auto expr = parse_expression(Precedence::lowest_p); expr) {
      stmt->set_expression(std::move(expr));
    } else {
      parse_error("expression", "let");
      return nullptr;
    }

    if (peek_token_type_is(Token::semicolon_t)) {
      next_token(); 
    }
    
    return stmt; 
  }

  std::unique_ptr<Statement> Parser::parse_return_statement() {
    auto stmt = std::make_unique<ReturnStatement>(_token);

    next_token();

    if (auto expr = parse_expression(Precedence::lowest_p); expr) {
	stmt->set_expression(std::move(expr)); 
    } else {
      parse_error("expression", "return");
      return nullptr;
    }

    if (peek_token_type_is(Token::semicolon_t)) {
      next_token();
    } 
    
    return stmt; 
  }

  std::unique_ptr<Statement> Parser::parse_expression_statement() {
    auto stmt = std::make_unique<ExpressionStatement>(_token);

    // льеп ;
    if (cur_token_type_is(Token::semicolon_t)) {
      return nullptr;
    }

    if (auto expr = parse_expression(Precedence::lowest_p); expr) { 
      stmt->set_expression(std::move(expr));
    } else {
      return nullptr;
    }

    if (peek_token_type_is(Token::semicolon_t)) {
	next_token(); 
    }
    
    return stmt;
  }

  std::unique_ptr<Expression> Parser::parse_prefix_expression() {
    auto expr = std::make_unique<PrefixExpression>(_token);

    next_token();

    expr->set_right(parse_expression(Precedence::prefix_p));
    
    return expr;
  }

  std::unique_ptr<Expression> Parser::parse_infix_expression(std::unique_ptr<Expression> left) {
    auto expr = std::make_unique<InfixExpression>(_token);

    auto precedence = cur_token_precedence();
    
    next_token();

    expr->set_left(std::move(left));
    expr->set_right(parse_expression(precedence));

    return expr; 
  }

  std::unique_ptr<BlockStatement> Parser::parse_block_statement() {
    auto stmts = std::make_unique<BlockStatement>(_token);

    while (!peek_token_type_is(Token::right_brace_t) &&
	   !peek_token_type_is(Token::eof_t)) {
      next_token();
      
      auto stmt = parse_statement();
      if (stmt != nullptr) { 
	stmts->append(std::move(stmt));
      }
    }

    // 
    /*if (!cur_token_type_is(Token::right_brace_t)) {
      parse_error(Token::right_brace_t, _token);
      return nullptr; 
      }*/
    
    return stmts;
  }

  std::unique_ptr<Expression> Parser::parse_if_expression() {
    auto if_expr = std::make_unique<IfExpression>(_token);

    if (!expect_token_type_is(Token::left_paren_t)) {
      parse_error(Token::left_paren_t, peek_token());
      return nullptr;
    }

    // skip ( 
    next_token(); 

    if (auto expr = parse_expression(Precedence::lowest_p); expr) { 
      if_expr->set_condition(std::move(expr));
    } else {
      parse_error("expression", "if");
      return nullptr;
    }

    if (!expect_token_type_is(Token::right_paren_t)) {
      parse_error(Token::right_paren_t, peek_token());
      return nullptr; 
    }

    if (!expect_token_type_is(Token::left_brace_t)) {
      parse_error(Token::left_brace_t, peek_token());
      return nullptr;
    }

    if_expr->set_consequence(parse_block_statement());

    if (!expect_token_type_is(Token::right_brace_t)) {
      parse_error(Token::right_brace_t, peek_token());
      return nullptr;
    }

    if (!peek_token_type_is(Token::else_t)) {
      return if_expr;
    }

    // skip else
    next_token(); 

    if (!expect_token_type_is(Token::left_brace_t)) {
      parse_error(Token::left_brace_t, peek_token());
      return nullptr;
    }

    if_expr->set_alternative(parse_block_statement());

    if (!expect_token_type_is(Token::right_brace_t)) {
      parse_error(Token::right_brace_t, peek_token());
      return nullptr;
    }

    return if_expr;
  }

  std::unique_ptr<Expression> Parser::parse_group_expression() {
    // skip (
    next_token();

    auto expr = parse_expression(Precedence::lowest_p);

    if (!expect_token_type_is(Token::right_paren_t)) {
      parse_error(Token::right_paren_t, peek_token());
      return nullptr; 
    }

    return expr;
  }

  std::unique_ptr<Expression> Parser::parse_identifier() {
    return std::make_unique<Identifier>(_token);
  }

  std::unique_ptr<Expression> Parser::parse_call_expression(std::unique_ptr<Expression> left) {
    auto call_expr = std::make_unique<CallExpression>(_token);

    call_expr->set_function(std::move(left));

    while (!peek_token_type_is(Token::right_paren_t) &&
	   !peek_token_type_is(Token::eof_t)) {
      next_token();

      call_expr->append_arguments(parse_expression(Precedence::lowest_p));

      if (!peek_token_type_is(Token::right_paren_t) &&
	  !expect_token_type_is(Token::comma_t)) {
	parse_error(Token::comma_t, peek_token());
	return nullptr;
      }
    }

    if (!expect_token_type_is(Token::right_paren_t)) {
      parse_error(Token::right_paren_t, peek_token());
      return nullptr;
    }
    
    return call_expr;
  }

  std::unique_ptr<Expression> Parser::parse_index_expression(std::unique_ptr<Expression> left) {
    auto index_expr = std::make_unique<IndexExpression>(_token);

    index_expr->set_array(std::move(left));

    // skip [
    next_token(); 

    if (auto expr = parse_expression(Precedence::lowest_p); expr) { 
      index_expr->set_index(std::move(expr));
    } else {
      parse_error("expression", "index");
      return nullptr;
    }

    if (!expect_token_type_is(Token::right_bracket_t)) {
      parse_error(Token::right_bracket_t, peek_token());
      return nullptr;
    }
    
    return index_expr;
  }

  std::unique_ptr<Expression> Parser::parse_integer_literal() {
    return std::make_unique<IntegerLiteral>(_token);
  }

  std::unique_ptr<Expression> Parser::parse_string_literal() {
    return std::make_unique<StringLiteral>(_token); 
  }

  std::unique_ptr<Expression> Parser::parse_boolean_literal() {
    return std::make_unique<BooleanLiteral>(_token); 
  }

  std::unique_ptr<Expression> Parser::parse_function_literal() {
    auto func = std::make_unique<FunctionLiteral>(_token);

    if (!expect_token_type_is(Token::left_paren_t)) {
      parse_error(Token::left_paren_t, peek_token());
      return nullptr; 
    }

    while (!peek_token_type_is(Token::right_paren_t) &&
	   !peek_token_type_is(Token::eof_t)) {
      if (!expect_token_type_is(Token::ident_t)) {
	parse_error(Token::ident_t, peek_token());
	return nullptr; 
      }
      func->append_parameters(std::make_unique<Identifier>(_token));

      if (!peek_token_type_is(Token::right_paren_t) &&
	  !expect_token_type_is(Token::comma_t)) {
	parse_error(Token::comma_t, peek_token());
	return nullptr;
      }
    }

    if (!expect_token_type_is(Token::right_paren_t)) {
      parse_error(Token::right_paren_t, peek_token());
      return nullptr; 
    }

    if (!expect_token_type_is(Token::left_brace_t)) {
      parse_error(Token::left_brace_t, peek_token());
      return nullptr;
    }
    
    func->set_body(parse_block_statement());

    if (!expect_token_type_is(Token::right_brace_t)) {
      parse_error(Token::right_brace_t, peek_token());
      return nullptr;
    }

    return func;
  }

  std::unique_ptr<Expression> Parser::parse_array_literal() {
    auto arr = std::make_unique<ArrayLiteral>(_token);

    while (!peek_token_type_is(Token::right_bracket_t) &&
	   !peek_token_type_is(Token::eof_t)) {
      next_token();
      
      arr->append(parse_expression(Precedence::lowest_p));
      
      if (!peek_token_type_is(Token::right_bracket_t) &&
	  !expect_token_type_is(Token::comma_t)) {
	parse_error(Token::comma_t, peek_token());
	return nullptr;
      }
    }

    if (!expect_token_type_is(Token::right_bracket_t)) {
      parse_error(Token::right_bracket_t, peek_token());
      return nullptr; 
    }

    return arr; 
  }
  
  Token Parser::next_token() {
    _cur = _nxt++;
    if (static_cast<std::size_t>(_cur) < _tokens.size()) {
      _token = _tokens[_cur]; 
    } else {
      _token = {Token::eof_t, "EOF"};
    }
    return _token; 
  }

  Token Parser::peek_token() {
    if (static_cast<std::size_t>(_nxt) < _tokens.size()) {
      return _tokens[_nxt]; 
    } else {
      return {Token::eof_t, "EOF"};
    }
  }

  bool Parser::cur_token_type_is(Token::TokenType type) {
    return _token.type == type; 
  }
  
  bool Parser::peek_token_type_is(Token::TokenType type) {
    return peek_token().type == type; 
  }

  bool Parser::expect_token_type_is(Token::TokenType type) {
    if (peek_token_type_is(type)) {
      next_token();
      return true;
    }
    return false;
  }

  std::map<Token::TokenType, Parser::Precedence> Parser::_t_to_p = {
    {Token::equal_t, Precedence::equal_p},
    {Token::not_equal_t, Precedence::equal_p},
    {Token::less_t, Precedence::less_greater_p},
    {Token::less_equal_t, Precedence::less_greater_p},
    {Token::greater_t, Precedence::less_greater_p},
    {Token::greater_equal_t, Precedence::less_greater_p},
    {Token::plus_t, Precedence::sum_p},
    {Token::minus_t, Precedence::sum_p},
    {Token::slash_t, Precedence::product_p},
    {Token::asterisk_t, Precedence::product_p},
    {Token::left_paren_t, Precedence::call_p},
    {Token::left_bracket_t, Precedence::index_p},
  };

  Parser::Precedence Parser::cur_token_precedence() {
    auto it = _t_to_p.find(_token.type);
    if (it != _t_to_p.end()) {
      return it->second; 
    }
    return Precedence::lowest_p;
  }

  Parser::Precedence Parser::peek_token_precedence() {
    auto it = _t_to_p.find(peek_token().type);
    if (it != _t_to_p.end()) {
      return it->second;
    } 
    return Precedence::lowest_p;
  }

  void Parser::parse_error(std::string str1, std::string str2) {
    std::string error = "no `" + str1 + "` found for `" + str2+ "`";
    _errors.emplace_back(std::move(error));
  }
  
  void Parser::parse_error(Token::TokenType type, Token token) {
    std::string error =
      "expected next token to be `"
      + Token::to_string(type) 
      + "`, got `"
      + token.to_string()
      + "` instead at literal `"
      + token.literal
      + "`";
    _errors.emplace_back(std::move(error));
  }
}
