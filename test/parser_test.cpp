#include <void/token.hpp>
#include <vector>
#include <void/ast.hpp>
#include <void/parser.hpp>
#include <gtest/gtest.h>
#include <any>
#include <memory>

using namespace Void;

void test_literal(std::any const& expect, Expression* expr) {
  ASSERT_TRUE(expr != nullptr);
  if (expect.type() == typeid(int)) {
    auto literal = dynamic_cast<IntegerLiteral*>(expr);
    ASSERT_TRUE(literal != nullptr);
    EXPECT_EQ(std::any_cast<int>(expect), literal->value()); 
  } else if (expect.type() == typeid(bool)) {
    auto literal = dynamic_cast<BooleanLiteral*>(expr);
    ASSERT_TRUE(literal != nullptr);
    EXPECT_EQ(std::any_cast<bool>(expect), literal->value());
  } else if (expect.type() == typeid(std::string)) {
    auto literal = dynamic_cast<StringLiteral*>(expr);
    ASSERT_TRUE(literal != nullptr);
    EXPECT_EQ(std::any_cast<std::string>(expect), literal->value());
  } else {
    ASSERT_TRUE(false); 
  }
}

std::string error_gen(Token::TokenType type, Token token) {
  std::string error =
    "expected next token to be `"
    + Token::to_string(type) 
    + "`, got `"
    + token.to_string()
    + "` instead at literal `"
    + token.literal
    + "`";
  return error;
}

std::string error_gen(std::string type, std::string name) {
  std::string error = "no `" + type + "` found for `" + name + "`";
  return error;
}

TEST(parser, TestLiteral) {
  std::string input = R"(
1 00234
true 
false 
"hello, world!"
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(5), stmts.size());

  std::vector<std::any> expects = {
    {1},
    {234},
    {true},
    {false},
    {std::string{"hello, world!"}}
  };

  for (int i = 0; i < 5; ++i) {
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    test_literal(expects[i], stmt->expression());
  }
}

TEST(parser, TestTrivialPrefixExpression) {
  std::string input = R"(
-1; -2;
!false;
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(3), stmts.size());

  std::vector<std::tuple<std::string, std::string, std::any>> expects = {
    {"(-1)", "-", {1}},
    {"(-2)", "-", {2}},
    {"(!false)", "!", {false}}, 
  };

  for (int i = 0; i < 3; ++i) {
    auto [str_expect, op_expect, value_expect] = expects[i];
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto prefix_expr = dynamic_cast<PrefixExpression*>(stmt->expression());
    ASSERT_TRUE(prefix_expr != nullptr);
    auto str = prefix_expr->to_string(); 
    auto op = prefix_expr->op();
    EXPECT_EQ(str_expect, str);
    EXPECT_EQ(op_expect, op);
    test_literal(value_expect, prefix_expr->right());
  }
}

TEST(parser, TestTrivialInfixExpression) {
  std::string input = R"(
1 + 2; 1 - 1; 1 * 1; 1 / 1; 10 == 01; 1 <= 0; 1 >= 1;
false == true; false != true;
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(9), stmts.size());

  std::vector<std::tuple<std::string, std::string, std::any, std::any>> expects = {
    {"(1 + 2)", "+", {1}, {2}},
    {"(1 - 1)", "-", {1}, {1}},
    {"(1 * 1)", "*", {1}, {1}},
    {"(1 / 1)", "/", {1}, {1}},
    {"(10 == 1)", "==", {10}, {1}},
    {"(1 <= 0)", "<=", {1}, {0}},
    {"(1 >= 1)", ">=", {1}, {1}},
    {"(false == true)", "==", {false}, {true}},
    {"(false != true)", "!=", {false}, {true}},
  };

  for (int i = 0; i < 9; ++i) {
    auto [str_expect, op_expect, value_left_expect, value_right_expect] = expects[i];
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto infix_expr = dynamic_cast<InfixExpression*>(stmt->expression());
    ASSERT_TRUE(infix_expr != nullptr);
    auto str = infix_expr->to_string(); 
    auto op = infix_expr->op();
    EXPECT_EQ(str_expect, str);
    EXPECT_EQ(op_expect, op);
    test_literal(value_left_expect, infix_expr->left());
    test_literal(value_right_expect, infix_expr->right());
  }
}

TEST(parser, TestTrivialLetStatement) {
  std::string input = R"(
let v = 0
let b = false
let s = "hello, world!"
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(3), stmts.size());

  std::vector<std::tuple<std::string, std::string, std::any>> expects = {
    {"let v = 0", "v", {0}},
    {"let b = false", "b", {false}},
    {"let s = \"hello, world!\"", "s", std::string{"hello, world!"}},
  };

  for (int i = 0; i < 3; ++i) {
    auto [str_expect, ident_expect, value_expect] = expects[i];
    auto stmt = dynamic_cast<LetStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    ASSERT_EQ(stmt->token_literal(), std::string("let"));
    auto str = stmt->to_string(); 
    auto ident = stmt->identier()->to_string();
    auto expr = stmt->expression();
    EXPECT_EQ(str_expect, str);
    EXPECT_EQ(ident_expect, ident);
    test_literal(value_expect, expr);
  }
}

TEST(parser, TestTrivialReturnStatement) {
  std::string input = R"(
return 1;
return true;
return "hello, world!"
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(3), stmts.size());

  std::vector<std::tuple<std::string, std::any>> expects = {
    {"return 1", {1}},
    {"return true", {true}},
    {"return \"hello, world!\"", std::string{"hello, world!"}},
  };

  for (int i = 0; i < 3; ++i) {
    auto [str_expect, value_expect] = expects[i];
    auto stmt = dynamic_cast<ReturnStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    ASSERT_EQ(stmt->token_literal(), std::string("return"));
    auto str = stmt->to_string(); 
    auto expr = stmt->expression();
    EXPECT_EQ(str_expect, str);
    test_literal(value_expect, expr);
  }
}


TEST(parser, TestExpression) {
  std::string input = R"(
-1+2*3<=false
!false==true;
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(2), stmts.size());

  std::vector<std::string> expects = {
    "(((-1) + (2 * 3)) <= false)",
    "((!false) == true)"
  };

  for (int i = 0; i < 2; ++i) {
    auto str_expect = expects[i];
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto expr = stmt->expression();
    ASSERT_TRUE(expr != nullptr);
    auto str = stmt->to_string(); 
    EXPECT_EQ(str_expect, str);
  }
}

/*TEST(parser, TestBlockStatement) {
  std::string input = R"(
{
let x = 1;
let y = 2;
let z = x + y / 2;
}
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(1), stmts.size());

  std::vector<std::string> expects = {
    "let x = 1",
    "let y = 2",
    "let z = (x + (y / 2))",
  };

  for (int i = 0; i < 1; ++i) {
    auto stmt = dynamic_cast<BlockStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto& block_stmts = stmt->statements();
    ASSERT_EQ(static_cast<std::size_t>(3), block_stmts.size());
    for (int j = 0; j < 3; ++j) {
      auto str_expect = expects[j];
      auto stmt = dynamic_cast<LetStatement*>(const_cast<Statement*>(block_stmts[i].get()));
      auto str = stmt->to_string();
      EXPECT_EQ(str_expect, str);
    }
  }
  }*/

TEST(parser, TestTrivialIfExpression) {
  std::string input = R"(
if(true){return false;}else{return 0;} 
if(1 + 2) { 0 } else { 0 }
if(false){} else {}
if(true) {}
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(4), stmts.size());

  std::vector<std::tuple<std::string, std::string, std::string, std::string>> expects = {
    {"if (true) { return false } else { return 0 }", "true", "{ return false }", "{ return 0 }"},
    {"if ((1 + 2)) { 0 } else { 0 }", "(1 + 2)", "{ 0 }", "{ 0 }"},
    {"if (false) {} else {}", "false", "{}", "{}"},
  };

  for (int i = 0; i < 3; ++i) {
    auto [str_expect, cond_expect, cons_expect, alt_expect] = expects[i];
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto expr = dynamic_cast<IfExpression*>(stmt->expression());
    auto str = expr->to_string(); 
    auto cond = expr->condition()->to_string();
    auto cons = expr->consequence()->to_string();
    auto alt = expr->alternative()->to_string();
    EXPECT_EQ(str_expect, str);
    EXPECT_EQ(cond_expect, cond);
    EXPECT_EQ(cons_expect, cons);
    EXPECT_EQ(alt_expect, alt); 
  }

  {
    std::string str_expect = "if (true) {}";
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[3].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto expr = dynamic_cast<IfExpression*>(stmt->expression());
    ASSERT_TRUE(expr != nullptr);
    EXPECT_EQ(str_expect, expr->to_string());
  }
}

TEST(parser, TestTrivialGroupExpression) {
  std::string input = R"(
(1 + 2) / 3
!(1 + 2 / 3) + 4
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(2), stmts.size());

  std::vector<std::string> expects = {
    {"((1 + 2) / 3)"},
    {"((!(1 + (2 / 3))) + 4)"},
  };

  for (int i = 0; i < 2; ++i) {
    auto str_expect = expects[i];
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto expr = dynamic_cast<Expression*>(stmt->expression());
    ASSERT_TRUE(expr != nullptr); 
    auto str = expr->to_string(); 
    EXPECT_EQ(str_expect, str);
  }
}

TEST(parser, TestTrivialFunctionLiteral) {
  std::string input = R"(
fn (a, b, c, d) { return d; }
fn () {}
fn (a,) {}
let a = fn (x, y) { return x + y; } 
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(4), stmts.size());

  std::vector<std::string> expects = {
    {"fn (a, b, c, d) { return d }"},
    {"fn () {}"},
    {"fn (a) {}"}, 
    {"let a = fn (x, y) { return (x + y) }"},
  };

  for (int i = 0; i < 3; ++i) { 
    auto str_expect = expects[i];
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto expr = dynamic_cast<FunctionLiteral*>(stmt->expression());
    ASSERT_TRUE(expr != nullptr); 
    auto str = expr->to_string(); 
    EXPECT_EQ(str_expect, str);
  }

  {
    auto str_expect = expects[3];
    auto stmt = dynamic_cast<LetStatement*>(const_cast<Statement*>(stmts[3].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto str = stmt->to_string(); 
    EXPECT_EQ(str_expect, str);
  }
}

TEST(parser, TestTrivialArrayLiteral) {
  std::string input = R"(
[1, 2, 3, 4];
[1 + 2 + -1 + -2, fn () {}];
[[1], [2, 2 + 3], [3]];
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(3), stmts.size());

  std::vector<std::string> expects = {
    {"[1, 2, 3, 4]"},
    {"[(((1 + 2) + (-1)) + (-2)), fn () {}]"},
    {"[[1], [2, (2 + 3)], [3]]"}, 
  };

  for (int i = 0; i < 3; ++i) { 
    auto str_expect = expects[i];
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto expr = dynamic_cast<ArrayLiteral*>(stmt->expression());
    ASSERT_TRUE(expr != nullptr); 
    auto str = expr->to_string(); 
    EXPECT_EQ(str_expect, str);
  }
}

TEST(parser, TestTrivialCallExpression) {
  std::string input = R"(
fn(a,b){return a+b;}(1,2);
fn(x,y){}(-1 * 2, fn(a, b){return a+b;});
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(2), stmts.size());

  std::vector<std::string> expects = {
    {"fn (a, b) { return (a + b) }(1, 2)"},
    {"fn (x, y) {}(((-1) * 2), fn (a, b) { return (a + b) })"},
  };

  for (int i = 0; i < 2; ++i) { 
    auto str_expect = expects[i];
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    auto expr = dynamic_cast<CallExpression*>(stmt->expression());
    ASSERT_TRUE(expr != nullptr); 
    auto str = expr->to_string(); 
    EXPECT_EQ(str_expect, str);
  }
}

TEST(parser, TestTrivialIndexExpression) {
  std::string input = R"(
[1, 2, 3, 4][0];
fn(){return [1, 2, 3, 4];}[0];
[fn(){}][0]();
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(3), stmts.size());

  std::vector<std::string> expects = {
    {"[1, 2, 3, 4][0]"},
    {"fn () { return [1, 2, 3, 4] }[0]"},
    {"[fn () {}][0]()"}
  };

  for (int i = 0; i < 3; ++i) { 
    auto str_expect = expects[i];
    auto stmt = dynamic_cast<ExpressionStatement*>(const_cast<Statement*>(stmts[i].get()));
    ASSERT_TRUE(stmt != nullptr);
    if (i < 2) { 
      auto expr = dynamic_cast<IndexExpression*>(stmt->expression());
      ASSERT_TRUE(expr != nullptr); 
      auto str = expr->to_string(); 
      EXPECT_EQ(str_expect, str);
    } else {
      auto expr = dynamic_cast<CallExpression*>(stmt->expression());
      ASSERT_TRUE(expr != nullptr); 
      auto str = expr->to_string(); 
      EXPECT_EQ(str_expect, str);
    }
  }
}

TEST(parser, TestLetStatementError) {
  std::string input = R"(
let 1 = 1;
let indent = ;
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);

  auto& errors = parser.error();
  ASSERT_EQ(static_cast<std::size_t>(4), errors.size());

  std::vector<std::string> error_expect = {
    error_gen(Token::ident_t, {Token::int_t, "1"}),
    error_gen("prefix parse function", "="),
    error_gen("prefix parse function", ";"),
    error_gen("expression", "let"),
  };

  for (int i = 0; i < 4; ++i) {
    EXPECT_EQ(error_expect[i], errors[i]);
  }
  
  auto& stmts = program->statements();
  ASSERT_EQ(static_cast<std::size_t>(2), stmts.size());

  std::vector<std::string> expects = {
    "1",
    "1",
  };

  for (int i = 0; i < 2; ++i) { 
    auto str_expect = expects[i];
    EXPECT_EQ(str_expect, stmts[i]->to_string()); 
  }
}

TEST(parser, TestReturnStatementError) {
  std::string input = R"(
return 
return ;
)";

  Parser parser(input);
  
  auto program = parser.parse();
  ASSERT_TRUE(program != nullptr);

  auto& errors = parser.error();
  ASSERT_EQ(static_cast<std::size_t>(2), errors.size());

  std::vector<std::string> error_expect = {
    error_gen("prefix parse function", "return"),
    error_gen("expression", "return"),
  };

  for (int i = 0; i < 2; ++i) {
    EXPECT_EQ(error_expect[i], errors[i]);
  }
}
