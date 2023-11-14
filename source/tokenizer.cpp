#include "tokenizer.hpp"

Tokenizer::Tokenizer(const std::string iCommentPattern,
                     const std::vector<TokenRule> &iRules) :
    commentPattern{iCommentPattern},
    rules{iRules} {}

std::vector<Token> Tokenizer::tokenize(std::string input) {
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
}