#pragma once

#include <array>
#include <iostream>
#include <vector>

struct Token {
  std::string lexeme;
  enum class Type {
    Variable = 0,
    Constant,
    If,
    Else,
    For,
    While,
    Or,
    And,
    Boolean,
    Identifier,
    Number,
    String,
    Begin,
    End,
    LeftCurly,
    RightCurly,
    Semicolon,
    LeftParen,
    RightParen,
    EqualTo,
    NotEqualTo,
    LessThan,
    GreaterThan,
    LessThanOrEqualTo,
    GreaterThanOrEqualTo,
    Equal,
    Asterisk,
    ForwardSlash,
    Plus,
    Dash,
    Exclamation,
    Modulus,
    Comma,
    Dot,
    Subroutine,
    Lambda,
    Return,
    Prototype,
    From,
    Public,
    Private,
    Colon,
    Error // Error needs to always be at the bottom of the list!
  } type;
  bool constant{true};
  int line{-1};
  int col{-1};
  bool operator==(const Type rhs) const;
  bool operator==(const Token &rhs) const;
};

template <>
struct std::hash<Token> {
  std::size_t operator()(const Token &token) const noexcept {
    return std::hash<std::string>{}(token.lexeme);
  }
};

using Tokens = std::vector<Token>;

std::ostream &operator<<(std::ostream &out, const Token::Type type);
std::ostream &operator<<(std::ostream &out, const Token &token);

const std::array<std::string, 42> tokenTypeNames{
    "variable", "constant",  "if",      "else",       "for",        "while",
    "or",       "and",       "Boolean", "Identifier", "Number",     "String",
    "begin",    "end",       "{",       "}",          ";",          "(",
    ")",        "==",        "!=",      "<",          ">",          "<=",
    ">=",       "=",         "*",       "/",          "+",          "-",
    "!",        "mod",       ",",       ".",          "Subroutine", "Lambda",
    "return",   "prototype", "from",    "public",     "private",    ":"};