#include "main.hpp"

int main() {
  /**
   * TODO:
   *  - Properly test
   *  - Fix issues with single character patterns
   *
   */
  std::string input = "Wick/: This is another comment... :/ ( exampleInferType "
                      "= 1 ) ; // This is a comment";
  Scanner scanner{{"[[:alpha:]][[:alnum:]]*", TokenType::Identifier},
                  {"[[:digit:]]*", TokenType::Integer},
                  {"[[:digit:]]*\\.[[:digit:]]*", TokenType::Real},
                  {"\\(", TokenType::LeftParenthesis},
                  {"\\)", TokenType::RightParenthesis},
                  {"=", TokenType::Equals},
                  {";", TokenType::StatementEnd}};
  auto results = scanner.tokenize("test1 test1 test3");
  Scanner::printTokens(results);
  return 0;
}
