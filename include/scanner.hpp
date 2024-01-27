#pragma once

#include <array>
#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <vector>

/**
 * KEY WORDS AND SYMBOLS
 * variable, constant, begin, end, if, else, while, or, and, true, false, {, },
 * ;, (, ), ==, !=, <, >, <=, >=, +, =, -, *, /, !
 */

enum class TokenType {
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
  Error
};

const std::array<std::string, 30> tokenTypeNames{
    "variable", "constant", "if",         "else",   "while",  "or",
    "and",      "Boolean",  "Identifier", "Number", "String", "begin",
    "end",      "{",        "}",          ";",      "(",      ")",
    "==",       "!=",       "<",          ">",      "<=",     ">=",
    "=",        "*",        "/",          "+",      "-",      "!"};

std::ostream &operator<<(std::ostream &out, const TokenType tokenType);

struct Token {
  std::string lexeme;
  TokenType type;
  int line{-1};
  int col{-1};

  bool operator==(const Token &rhs) {
    return type == rhs.type;
  }
};

std::ostream &operator<<(std::ostream &out, const Token &token);

using TokenRule = std::pair<std::regex, TokenType>;

class Scanner {
  public:
  Scanner();

  std::vector<Token> tokenize(const std::string &input);

  static void printTokens(const std::vector<Token> &tokens);

  private:
  void scanToken();
  bool smallTokens();
  void forwardSlash();
  void string();
  void longTokens();
  void number(std::string &lexeme);
  void identifier(std::string &lexeme);
  void addToken(const std::string &lexeme, TokenType type);

  std::string text;
  std::vector<Token> tokens;
  std::unordered_map<std::string, TokenType> keywords{
      {"variable", TokenType::Variable},
      {"constant", TokenType::Constant},
      {"if", TokenType::If},
      {"else", TokenType::Else},
      {"while", TokenType::While},
      {"or", TokenType::Or},
      {"and", TokenType::And},
      {"true", TokenType::Boolean},
      {"false", TokenType::Boolean},
      {"begin", TokenType::Begin},
      {"end", TokenType::End}};
  int pos{0}, line{1}, col{0};
};
