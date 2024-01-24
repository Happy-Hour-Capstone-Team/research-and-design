#pragma once

#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

enum class TokenType {
  Variable,
  Identifier,
  Integer,
  Real,
  LeftParenthesis,
  RightParenthesis,
  Equals,
  Semicolon,
  Plus,
  Minus,
  Asterisk,
  ForwardSlash,
};

struct Token {
  std::string lexeme;
  TokenType type;
  int line{-1};
  bool operator==(const Token &rhs) {
    return lexeme == rhs.lexeme && type == rhs.type && line == rhs.line;
  }
};

using TokenRule = std::pair<std::string, TokenType>;

class Scanner {
  public:
  Scanner(std::initializer_list<TokenRule> rules);

  std::vector<Token> tokenize(const std::string &input);

  static void printTokens(const std::vector<Token> &tokens);

  private:
  std::vector<TokenType> getTokenTypeMatches(const std::string &lexeme);
  void pushBackToken(std::vector<Token> &tokens,
                     const std::string &lexeme,
                     int lineNumber);
  void pushBackTokenWithLexemeUpdate(std::vector<Token> &tokens,
                                     std::string &lexeme,
                                     const std::string &newLexeme,
                                     int lineNumber);

  std::vector<TokenRule> rules;
};
