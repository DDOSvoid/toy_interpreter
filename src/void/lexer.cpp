#include <void/token.hpp>
#include <void/lexer.hpp>

#include <cstddef>
#include <string>

namespace Void {
  Lexer::Lexer(std::string const& input)
    : _input(input) {
    _cur = 0;
    _nxt = 1;
    if (_input.size()) {
      _ch = _input[0];
    } else {
      _ch = 0; 
    }
  }

  Token Lexer::read_token() {
    skip_whitespace();
    Token token;
    
    switch (_ch) {
    case '-':
      token = {Token::minus_t, "-"};
      read_char();
      break;
    case '+':
      token = {Token::plus_t, "+"}; 
      read_char();
      break;
    case '*':
      token = {Token::asterisk_t, "*"};
      read_char();
      break;
    case '/':
      token = {Token::slash_t, "/"};
      read_char();
      break;
    case '!':
      if (peek_char() == '=') {
	read_char();
	token = {Token::not_equal_t, "!="};
      } else {
	token = {Token::bang_t, "!"};
      }
      read_char();
      break;
    case '<':
      if (peek_char() == '=') {
	read_char();
	token = {Token::less_equal_t, "<="}; 
      } else {
	token = {Token::less_t, "<"};
      }
      read_char();
      break;
    case '>':
      if (peek_char() == '=') {
	read_char();
	token = {Token::greater_equal_t, ">="};
      } else {
	token = {Token::greater_t, ">"};
      }
      read_char();
      break;
    case '=':
      if (peek_char() == '=') {
	read_char();
	token = {Token::equal_t, "=="};
      } else {
	token = {Token::assign_t, "="}; 
      }
      read_char();
      break;
    case '(':
      token = {Token::left_paren_t, "("};
      read_char();
      break;
    case ')':
      token = {Token::right_paren_t, ")"};
      read_char();
      break;
    case '[':
      token = {Token::left_bracket_t, "["};
      read_char();
      break;
    case ']':
      token = {Token::right_bracket_t, "]"};
      read_char();
      break;
    case '{':
      token = {Token::left_brace_t, "{"};
      read_char();
      break;
    case '}':
      token = {Token::right_brace_t, "}"};
      read_char();
      break;
    case ',':
      token = {Token::comma_t, ","};
      read_char();
      break;
    case ';':
      token = {Token::semicolon_t, ";"};
      read_char();
      break;
    case ':':
      token = {Token::colon_t, ":"};
      read_char();
      break;
    case '\0':
      token = {Token::eof_t, "EOF"};
      break;
    default:
      if (is_digit(_ch)) {
	token = {Token::int_t, read_number()}; 
      } else if (_ch == '"') {
	token = {Token::string_t, read_string()}; 
      } else if (is_alnum(_ch)) {
	std::string word = read_identifier();
	token = {Token::lookup(word), word};
      } else {
	token = {Token::illegal_t, ""};
      }
      break;
    }
    return token; 
  }

  char Lexer::read_char() {
    _cur = _nxt++;
    if (static_cast<std::size_t>(_cur) < _input.size()) {
      _ch = _input.at(_cur);
    } else {
      _ch = 0;
    }
    return _ch;
  }

  char Lexer::peek_char() {
    if (static_cast<std::size_t>(_nxt) < _input.size()) {
      return _input.at(_nxt);
    } else {
      return 0;
    }
  }

  void Lexer::skip_whitespace() {
    static char whitespaces[] = {' ', '\n', '\r', '\t'};
    while (std::find(std::begin(whitespaces),
		     std::end(whitespaces), 
		     _ch) != std::end(whitespaces)) {
      read_char();
    }
  }

  bool Lexer::is_digit(char ch) {
    return '0' <= ch && ch <= '9'; 
  }

  bool Lexer::is_alnum(char ch) {
    return is_digit(ch) || 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_';
  }

  std::string Lexer::read_number() {
    int cur = _cur;
    while (is_digit(_ch)) read_char();
    return _input.substr(cur, _cur - cur); 
  }

  std::string Lexer::read_string() {
    int cur = _cur + 1;
    do {
      read_char(); 
    } while (_ch != '"');
    std::string str = _input.substr(cur, _cur - cur);
    read_char();
    return str; 
  }

  std::string Lexer::read_identifier() {
    int cur = _cur;
    while (is_alnum(_ch)) read_char();
    return _input.substr(cur, _cur - cur);
  }
}

