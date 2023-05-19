#pragma once

#include <map>
#include <functional>
#include <string>

namespace Void { 
  class Token {
  public:
    enum TokenType {
      // operator
      minus_t, plus_t, asterisk_t, slash_t, bang_t,
      less_t, less_equal_t, greater_t, greater_equal_t,
      assign_t, equal_t, not_equal_t,

      //
      left_paren_t, right_paren_t, left_brace_t, right_brace_t, left_bracket_t,
      right_bracket_t, comma_t, colon_t, semicolon_t,

      //
      string_t, int_t,

      // identifier
      ident_t,

      // keywords
      let_t, function_t, true_t, false_t, if_t,
      else_t, return_t, 

      // eof
      eof_t,
      
      // illegal
      illegal_t, 
    };

    static std::map<std::string, TokenType> keywords;
    static TokenType lookup(std::string const&);
    static std::string to_string(TokenType); 
    std::string to_string() const;

    TokenType type; 
    std::string literal;
  };
}
