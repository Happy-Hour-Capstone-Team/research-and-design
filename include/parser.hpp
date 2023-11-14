#pragma once
#include <string>
#include <vector>

enum class TokenType {
  Identifier,
  Integer,
  Real,
  LeftParenthesis,
  Equals,
  StatementEnd,
  Custom
};

struct Token {
  TokenType type;
  std::string lexeme;
};