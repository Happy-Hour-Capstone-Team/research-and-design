#pragma once

#include <array>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

enum class TokenType {
  Variable = 0,
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

const std::array<std::string, 12> tokenTypeNames{"Variable",
                                                 "Identifier",
                                                 "Integer",
                                                 "Real",
                                                 "LeftParenthesis",
                                                 "RightParenthesis",
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

using TokenRule = std::pair<std::string, TokenType>;

struct CommentRules {
  std::string singleLine;
  std::string multiLineBegin;
  std::string multiLineEnd;
};

class Scanner {
  public:
  Scanner(std::initializer_list<TokenRule> rules, const CommentRules &comments);

  std::vector<Token> tokenize(std::string input);

  static void printTokens(const std::vector<Token> &tokens);

  private:
  bool shouldComment(const std::string &lexeme);
  std::vector<TokenType> getTokenTypeMatches(const std::string &lexeme);
  void addToken(std::vector<Token> &tokens,
                std::string &lexeme,
                const std::string &newLexeme,
                int lineNumber);
  std::vector<TokenRule> rules;
  CommentRules comments;
};
