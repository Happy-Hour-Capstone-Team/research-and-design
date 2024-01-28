#include "token.hpp"

bool Token::operator==(const Type rhs) {
  return type == rhs;
}

bool Token::operator==(const Token &rhs) {
  return lexeme == rhs.lexeme && type == rhs.type && line == rhs.line &&
         col == rhs.col;
}

std::ostream &operator<<(std::ostream &out, const Token::Type type) {
  const int value{static_cast<int>(type)};
  if(value >= tokenTypeNames.size()) return out << "Error";
  return out << tokenTypeNames[value];
}

std::ostream &operator<<(std::ostream &out, const Token &token) {
  out << token.type << " (" << token.lexeme << ") on line " << token.line
      << ", column " << token.col;
  return out;
}