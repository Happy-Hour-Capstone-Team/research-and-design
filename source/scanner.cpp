#include "scanner.hpp"

Scanner::Scanner(std::initializer_list<TokenRule> rules) : rules{rules} {}

std::vector<Token> Scanner::tokenize(const std::string &input) {
  std::vector<Token> tokens;
  int lineNumber{1};
  std::stringstream ss{input};
  for(std::string line; std::getline(ss, line, '\n'); lineNumber++) {
    std::string lexeme{""};
    for(char c : line) {
      if(isspace(c)) {        
        if(lexeme.empty()) continue;
        pushBackToken(tokens, lexeme, lineNumber);
        lexeme = "";
        continue;
      }
      const std::string newLexeme{lexeme + c};
      // If no matches, get highest priority match from previous lexeme.
      if(getTokenTypeMatches(newLexeme).size() == 0) {
        pushBackToken(tokens, lexeme, lineNumber);
        lexeme = c;
      } else
        lexeme = newLexeme;
    }
    pushBackToken(tokens, lexeme, lineNumber);
  }
  return tokens;
}

void Scanner::printTokens(const std::vector<Token> &tokens) {
  std::cout << "TOKENS:\n";
  for(const Token &token : tokens) {
    std::cout << "\t";
    switch(token.type) {
      case TokenType::Identifier: std::cout << "Identifier"; break;
      case TokenType::Integer: std::cout << "Integer"; break;
      case TokenType::Real: std::cout << "Real"; break;
      case TokenType::LeftParenthesis: std::cout << "Left Parenthesis"; break;
      case TokenType::RightParenthesis: std::cout << "Right Parenthesis"; break;
      case TokenType::Equals: std::cout << "Equals"; break;
      case TokenType::StatementEnd: std::cout << "Statement End"; break;
    }
    std::cout << ", " << token.lexeme << '\n';
  }
}

std::vector<TokenType> Scanner::getTokenTypeMatches(const std::string &lexeme) {
  std::vector<TokenType> tokenTypeMatches{};
  for(TokenRule rule : rules) {
    if(std::regex_match(lexeme, std::regex{rule.first}))
      tokenTypeMatches.push_back(rule.second);
  }
  return tokenTypeMatches;
}

void Scanner::pushBackToken(std::vector<Token> &tokens,
                            const std::string &lexeme,
                            int lineNumber) {
  tokens.push_back({lexeme, getTokenTypeMatches(lexeme)[0], lineNumber});
}