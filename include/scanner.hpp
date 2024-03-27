#pragma once

#include "errorReporter.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class Scanner {
  public:
  explicit Scanner(const std::string &iText,
                   ErrorReporter *const iErrorReporter = nullptr);

  Tokens tokenize();

  static void printTokens(const Tokens &tokens);

  private:
  void scanToken();
  void forwardSlash();
  void string();
  void longTokens();
  void number();
  void identifier();
  void addToken(const std::string &lexeme, Token::Type type);
  void newLine();
  void incPosCol(int i = 1);

  ErrorReporter *const errorReporter;
  std::string text;
  Tokens tokens;
  std::unordered_map<std::string, Token::Type> keywords{
      {"variable", Token::Type::Variable},
      {"constant", Token::Type::Constant},
      {"if", Token::Type::If},
      {"else", Token::Type::Else},
      {"for", Token::Type::For},
      {"while", Token::Type::While},
      {"or", Token::Type::Or},
      {"and", Token::Type::And},
      {"true", Token::Type::Boolean},
      {"false", Token::Type::Boolean},
      {"begin", Token::Type::Begin},
      {"end", Token::Type::End},
      {"mod", Token::Type::Modulus},
      {"subroutine", Token::Type::Subroutine},
      {"lambda", Token::Type::Lambda},
      {"return", Token::Type::Return},
      {"prototype", Token::Type::Prototype},
      {"from", Token::Type::From},
      {"public", Token::Type::Public},
      {"private", Token::Type::Private}};
  int pos{0}, line{1}, col{0};
};
