#include <void/token.hpp>

#include <map>
#include <string>

namespace Void { 
  std::map<std::string, Token::TokenType> Token::keywords = {
    {"let", Token::let_t},
    {"fn", Token::function_t},
    {"true", Token::true_t},
    {"false", Token::false_t},
    {"if", Token::if_t},
    {"else", Token::else_t},
    {"return", Token::return_t}
  };

  Token::TokenType Token::lookup(std::string const& name) {
    auto it = keywords.find(name);
    if (it == keywords.end()) {
      return Token::ident_t;
    }
    return it->second;
  }

  std::string Token::to_string(TokenType type) {
    static std::map<TokenType, std::string> t_to_s = {
      {minus_t, "minus"},
      {plus_t, "plus"},
      {asterisk_t, "asterisk"},
      {slash_t, "slash"},
      {bang_t, "bang"},
      {less_t, "less"},
      {less_equal_t, "less_equal"},
      {greater_t, "greater"},
      {greater_equal_t, "greater_equal"},
      {assign_t, "assign"},
      {equal_t, "equal"},
      {not_equal_t, "not_equal"},
      {left_paren_t, "left_paren"},
      {right_paren_t, "right_paren"},
      {left_brace_t, "left_brace"},
      {right_brace_t, "right_brace"},
      {left_bracket_t, "left_bracket"},
      {right_bracket_t, "right_bracket"},
      {comma_t, "comma"},
      {colon_t, "colon"},
      {semicolon_t, "semicolon"},
      {int_t, "int"},
      {string_t, "string"},
      {ident_t, "ident"},
      {let_t, "let"},
      {function_t, "function"},
      {true_t, "true"},
      {false_t, "false"},
      {if_t, "if"},
      {else_t, "else"},
      {return_t, "return"},
      {eof_t, "eof"},
      {illegal_t, "illegal"},
    };

    return t_to_s[type]; 
  }

  std::string Token::to_string() const {
    return literal;
  }
}
