#pragma once

#include "errorReporter.hpp"
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief The scanner is responsible for breaking up a Wick program into "words"
 * called tokens and tagging them with additional information.
 *
 */
class Scanner {
  public:
  /**
   * @brief Constructs a scanner object based on the text to scan and an error
   * reporter.
   *
   * @param iText
   * @param iErrorReporter
   */
  explicit Scanner(const std::string &iText,
                   ErrorReporter *const iErrorReporter = nullptr);

  /**
   * @brief Breaks the provided text from the constructor into tokens.
   *
   * @return Tokens
   */
  Tokens tokenize();

  /**
   * @brief Static function for printing out a list of tokens.
   *
   * @param tokens
   */
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
  bool idChar(const char c);

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
