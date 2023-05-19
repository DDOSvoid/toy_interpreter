#include <void/evaluator.hpp>
#include <void/token.hpp>
#include <vector>
#include <void/ast.hpp>
#include <void/parser.hpp>

#include <iostream>

std::string read_line() {
  std::string line;
  std::getline(std::cin, line);
  return line;
}

int main() {
  int status; 

  Void::Evaluator evaluator{};
  
  while (1) { 
    std::cout << ">> ";
    std::string line_str = read_line();
    if (line_str == "exit") {
      break;
    }
    auto res = evaluator.eval(line_str);
    std::cout << res->inspect() << std::endl;
  }
  return 0; 
}
