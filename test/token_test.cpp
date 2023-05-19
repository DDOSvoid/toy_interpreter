#include "gtest/gtest.h"
#include "void/token.hpp"

using namespace Void;

TEST(Token, Testlookup) {
  
  EXPECT_EQ(Token::lookup("let"), Token::let_t);
  EXPECT_EQ(Token::lookup("fn"), Token::function_t);
  EXPECT_EQ(Token::lookup("abc"), Token::ident_t);
}

TEST(Token, TestToString) {

  EXPECT_EQ(Token::to_string(Token::let_t), std::string("let"));
  EXPECT_EQ(Token::to_string(Token::return_t), std::string("return"));
  EXPECT_EQ(Token::to_string(Token::asterisk_t), std::string("asterisk"));
}

