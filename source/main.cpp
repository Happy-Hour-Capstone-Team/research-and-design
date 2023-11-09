#include "main.hpp"

int main() {
  Tokenizer tokenizer{"//.*|/:.*:/",
                      {{"[A-Za-z_][A-Za-z0-9_]*", TokenType::Identifier},
                       {"\\d+", TokenType::Integer},
                       {"\\d+\\.\\d+", TokenType::Real},
                       {"\\(", TokenType::LeftParenthesis},
                       {"\\)", TokenType::RightParenthesis},
                       {"=", TokenType::Equals},
                       {";", TokenType::StatementEnd}}};

  /**
   * TODO:
   *  - Properly test
   *  - Fix issues with single character patterns
   *
   */
  std::string input = "Wick/: This is another comment... :/ ( exampleInferType "
                      "= 1 ) ; // This is a comment";
  std::vector<Token> tokens = tokenizer.tokenize(input);

  for(const Token &token : tokens) {
    std::cout << "(" << token.lexeme << ", ";
    switch(token.type) {
      case TokenType::Identifier: std::cout << "Identifier"; break;
      case TokenType::Integer: std::cout << "Integer"; break;
      case TokenType::Real: std::cout << "Real"; break;
      case TokenType::LeftParenthesis: std::cout << "Left Parenthesis"; break;
      case TokenType::RightParenthesis: std::cout << "Right Parenthesis"; break;
      case TokenType::Equals: std::cout << "Equals"; break;
      case TokenType::StatementEnd: std::cout << "Statement End"; break;
      case TokenType::Custom:
        std::cout << "Custom"; // Handle custom rules here
        break;
    }
    std::cout << ")\n";
  }

  return 0;
}
