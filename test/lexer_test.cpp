#include "gtest/gtest.h"
#include "void/token.hpp"
#include "void/lexer.hpp"

#include <vector>

using namespace Void;

TEST(Lexer, TestOperatorAndSymbol) {
  std::string input = "+-*/ < <= > >= ! != = == (][}{);";

  Lexer lexer(input);

  Token expect_tokens[] = {
    {Token::plus_t, "+"},
    {Token::minus_t, "-"},
    {Token::asterisk_t, "*"},
    {Token::slash_t, "/"},
    {Token::less_t, "<"},
    {Token::less_equal_t, "<="},
    {Token::greater_t, ">"},
    {Token::greater_equal_t, ">="},
    {Token::bang_t, "!"},
    {Token::not_equal_t, "!="},
    {Token::assign_t, "="},
    {Token::equal_t, "=="},
    {Token::left_paren_t, "("},
    {Token::right_bracket_t, "]"},
    {Token::left_bracket_t, "["},
    {Token::right_brace_t, "}"},
    {Token::left_brace_t, "{"},
    {Token::right_paren_t, ")"},
    {Token::semicolon_t, ";"},
    {Token::eof_t, "EOF"}
  };

  for (auto& expect_token : expect_tokens) {
    auto token = lexer.read_token();
    EXPECT_EQ(token.type, expect_token.type);
    EXPECT_EQ(token.literal, expect_token.literal);
  }
}

TEST(Lexer, TestNumber) {
  std::string input = "-10-1 12345";

  Lexer lexer(input);

  Token expect_tokens[] = {
    {Token::minus_t, "-"},
    {Token::int_t, "10"},
    {Token::minus_t, "-"},
    {Token::int_t, "1"},
    {Token::int_t, "12345"},
    {Token::eof_t, "EOF"},
  };

  for (auto& expect_token : expect_tokens) {
    auto token = lexer.read_token();
    EXPECT_EQ(token.type, expect_token.type);
    EXPECT_EQ(token.literal, expect_token.literal);
  }
}

TEST(Lexer, TestString) {
  std::string input = R"(
"hello, world!"
"1234""4321"
)";

  Lexer lexer(input);

  Token expect_tokens[] = {
    {Token::string_t, "hello, world!"},
    {Token::string_t, "1234"},
    {Token::string_t, "4321"},
  };

  for (auto& expect_token : expect_tokens) {
    auto token = lexer.read_token();
    EXPECT_EQ(token.type, expect_token.type);
    EXPECT_EQ(token.literal, expect_token.literal);
  }
}

TEST(Lexer, TestIndentAndKeywords) {
  std::string input = "let, fn return unit_test1 void false true false_true";

  Lexer lexer(input);

  Token expect_tokens[] = {
    {Token::let_t, "let"},
    {Token::comma_t, ","}, 
    {Token::function_t, "fn"},
    {Token::return_t, "return"},
    {Token::ident_t, "unit_test1"},
    {Token::ident_t, "void"},
    {Token::false_t, "false"},
    {Token::true_t, "true"},
    {Token::ident_t, "false_true"},
    {Token::eof_t, "EOF"},
  };

  for (auto& expect_token : expect_tokens) {
    auto token = lexer.read_token();
    EXPECT_EQ(token.type, expect_token.type);
    EXPECT_EQ(token.literal, expect_token.literal);
  }
}

TEST(Lexer, TestLexer) {
  std::string input = R"(
let add = fn (val) {
  return val
}
add(10)
let foo = fn (v1) {
  let calc = fn (v2) {
    return v1 + v2
  }
  return calc
}
)";

  Lexer lexer(input);

  Token expect_tokens[] = {
    {Token::let_t, "let"},
    {Token::ident_t, "add"},
    {Token::assign_t, "="},
    {Token::function_t, "fn"},
    {Token::left_paren_t, "("},
    {Token::ident_t, "val"},
    {Token::right_paren_t, ")"},
    {Token::left_brace_t, "{"},
    {Token::return_t, "return"},
    {Token::ident_t, "val"},
    {Token::right_brace_t, "}"},
    {Token::ident_t, "add"},
    {Token::left_paren_t, "("},
    {Token::int_t, "10"},
    {Token::right_paren_t, ")"},
    {Token::let_t, "let"},
    {Token::ident_t, "foo"},
    {Token::assign_t, "="},
    {Token::function_t, "fn"},
    {Token::left_paren_t, "("},
    {Token::ident_t, "v1"},
    {Token::right_paren_t, ")"},
    {Token::left_brace_t, "{"},
    {Token::let_t, "let"},
    {Token::ident_t, "calc"},
    {Token::assign_t, "="},
    {Token::function_t, "fn"},
    {Token::left_paren_t, "("},
    {Token::ident_t, "v2"},
    {Token::right_paren_t, ")"},
    {Token::left_brace_t, "{"},
    {Token::return_t, "return"},
    {Token::ident_t, "v1"},
    {Token::plus_t, "+"},
    {Token::ident_t, "v2"},
    {Token::right_brace_t, "}"},
    {Token::return_t, "return"},
    {Token::ident_t, "calc"},
    {Token::right_brace_t, "}"},
  };

  for (auto& expect_token : expect_tokens) {
    auto token = lexer.read_token();
    EXPECT_EQ(token.type, expect_token.type);
    EXPECT_EQ(token.literal, expect_token.literal);
  }
}

