#pragma once

#include <regex>
#include <sstream>
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
  int line{-1};
};

using TokenRule = std::pair<std::string, TokenType>;

class Scanner {
  public:
  Scanner(std::initializer_list<TokenRule> rules);

  std::vector<Token> tokenize(const std::string &input);

  private:
  std::vector<TokenType> getTokenTypeMatches(const std::string &lexeme);

  std::vector<TokenRule> rules;
};
