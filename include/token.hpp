#pragma once

#include <array>
#include <iostream>
#include <vector>

/**
 * KEY WORDS AND SYMBOLS
 * variable, constant, begin, end, if, else, while, or, and, true, false, {, },
 * ;, (, ), ==, !=, <, >, <=, >=, +, =, -, *, /, !
 */
struct Token {
  std::string lexeme;
  enum class Type {
    Variable = 0,
    Constant,
    If,
    Else,
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
    Percent,
    Comma,
    Caret,
    Function,
    Lambda,
    Error // Error needs to always be at the bottom of the list!
  } type;
  int line{-1};
  int col{-1};
  bool operator==(const Type rhs);
  bool operator==(const Token &rhs);
};

using Tokens = std::vector<Token>;

std::ostream &operator<<(std::ostream &out, const Token::Type type);
std::ostream &operator<<(std::ostream &out, const Token &token);

const std::array<std::string, 35> tokenTypeNames{
    "variable", "constant", "if",         "else",     "while",  "or",
    "and",      "Boolean",  "Identifier", "Number",   "String", "begin",
    "end",      "{",        "}",          ";",        "(",      ")",
    "==",       "!=",       "<",          ">",        "<=",     ">=",
    "=",        "*",        "/",          "+",        "-",      "!",
    "%",        ",",        "^",          "Function", "Lambda"};