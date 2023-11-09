#pragma once

#include <regex>
#include <vector>

enum class TokenType {
  Identifier,
  Integer,
  Real,
  LeftParenthesis,
  RightParenthesis,
  Equals,
  StatementEnd,
  Custom
};

struct Token {
  std::string lexeme;
  TokenType type;
};

struct TokenRule {
  std::string pattern;
  TokenType type;
};

class Tokenizer {
  public:
  Tokenizer(const std::string iCommentPattern,
            const std::vector<TokenRule> &iRules);

  std::vector<Token> tokenize(std::string input);

  private:
  std::regex commentPattern;
  std::vector<TokenRule> rules;
};
