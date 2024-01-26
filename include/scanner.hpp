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
  LeftParen,
  RightParen,
  Equals,
  Semicolon,
  Plus,
  Minus,
  Asterisk,
  ForwardSlash,
  Error
};

const std::array<std::string, 12> tokenTypeNames{"Variable",
                                                 "Identifier",
                                                 "Number",
                                                 "LeftParen",
                                                 "RightParen",
                                                 "Equals",
                                                 "Semicolon",
                                                 "Plus",
                                                 "Minus",
                                                 "Asterisk",
                                                 "ForwardSlash"};

std::ostream &operator<<(std::ostream &out, const TokenType tokenType);

struct Token {
  std::string lexeme;
  TokenType type;
  int line{1};
  bool operator==(const Token rhs) {
    return lexeme == rhs.lexeme && type == rhs.type && line == rhs.line;
  }
};

using TokenRule = std::pair<std::regex, TokenType>;

class Scanner {
  public:
  Scanner(std::initializer_list<TokenRule> rules);

  std::vector<Token> tokenize(std::string input);

  static void printTokens(const std::vector<Token> &tokens);

  private:
  std::vector<TokenType> getTokenTypeMatches(const std::string &lexeme);
  void addToken(std::vector<Token> &tokens,
                std::string &lexeme,
                const std::string &newLexeme,
                int lineNumber);
  std::vector<TokenRule> rules;
};
