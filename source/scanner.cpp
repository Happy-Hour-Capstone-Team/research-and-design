#include "scanner.hpp"

Scanner::Scanner(std::initializer_list<TokenRule> rules) : rules{rules} {}

std::vector<Token> Scanner::tokenize(const std::string &input) {
  std::vector<Token> tokens;
  int lineNumber{0};
  std::stringstream ss{input};
  for(std::string line; std::getline(ss, line, '\n'); lineNumber++) {
    std::string lexeme{""};
    for(char c : line) {
      if(std::isspace(c)) continue;
      const std::string newLexeme{lexeme + c};
      std::vector<TokenType> tokenTypeMatches{getTokenTypeMatches(newLexeme)};
      // If no matches, get highest priority match from previous lexeme.
      if(tokenTypeMatches.size() == 0) {
        tokens.push_back({lexeme, getTokenTypeMatches(lexeme)[0], lineNumber});
        lexeme = c;
      } else
        lexeme = newLexeme;
    }
    tokens.push_back({lexeme, getTokenTypeMatches(lexeme)[0], lineNumber});
  }
  return tokens;
  /*
  input = std::regex_replace(input, commentPattern, "");
  std::vector<Token> tokens;
  std::string currentToken;

  for(size_t i = 0; i < input.length(); i++) {
    char c = input[i];

    if(std::isspace(c) || c == '\n') {
      if(!currentToken.empty()) {
        TokenType tokenType{TokenType::Identifier}; // Default to Identifier
        for(const auto &rule : rules) {
          if(std::regex_match(currentToken, std::regex{rule.pattern})) {
            tokenType = rule.type;
          }
        }
        tokens.push_back({currentToken, tokenType});
        currentToken.clear();
      }
    } else {
      currentToken += c;
    }
  }

  if(!currentToken.empty()) {
    TokenType tokenType{TokenType::Identifier}; // Default to Identifier
    for(const auto &rule : rules) {
      if(std::regex_match(currentToken, std::regex{rule.pattern})) {
        tokenType = rule.type;
      }
    }
    tokens.push_back({currentToken, tokenType});
  }

  return tokens;
  */
}

std::vector<TokenType> Scanner::getTokenTypeMatches(const std::string &lexeme) {
  std::vector<TokenType> tokenTypeMatches{};
  for(TokenRule rule : rules) {
    if(std::regex_match(lexeme, std::regex{rule.first}))
      tokenTypeMatches.push_back(rule.second);
  }
  return tokenTypeMatches;
}