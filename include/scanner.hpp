#pragma once

#include <array>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

enum class TokenType {
  Variable = 0,
  Identifier,
  Number,
  String,
  Semicolon,
  LeftCurly,
  RightCurly,
  LeftParen,
  RightParen,
  Equals,
  Plus,
  Dash,
  Asterisk,
  ForwardSlash,
  Error
};

const std::array<std::string, 15> tokenTypeNames{"Variable",
                                                 "Identifier",
                                                 "Number",
                                                 "String",
                                                 "Semicolon",
                                                 "LeftCurly",
                                                 "RightCurly",
                                                 "LeftParen",
                                                 "RightParen",
                                                 "Equals",
                                                 "Plus",
                                                 "Dash",
                                                 "Asterisk",
                                                 "ForwardSlash"};

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
  bool singleCharacter();
  void forwardSlash();
  void string();
  void longTokens();
  void number(std::string &lexeme);
  void identifier(std::string &lexeme);
  void addToken(const std::string &lexeme, TokenType type);

  std::string text;
  std::vector<Token> tokens;
  int pos{0}, line{1}, col{0};
};
