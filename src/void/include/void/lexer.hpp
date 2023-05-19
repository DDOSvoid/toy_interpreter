#pragma once

#include "token.hpp"

#include <string>

namespace Void {
  class Lexer {
  public:
    Lexer(std::string const&); 
    Token read_token();
  private:
    char read_char();
    char peek_char();
    void skip_whitespace(); // skip \n \r \t \r\n
    bool is_digit(char);
    bool is_alnum(char); // alpha, num, -
    std::string read_number();
    std::string read_string();
    std::string read_identifier(); 
  private:
    std::string _input;

    char _ch{}; // current char 
    int _cur{}; // current pos
    int _nxt{}; // next pos
  };
}


