#pragma once

#include <array>
#include <iostream>
#include <vector>

/**
 * @brief Structure for representing Wick tokens.
 *
 */
struct Token {
  std::string lexeme;

  /**
   * @brief The complete list of types of Wick tokens.
   *
   */
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

  /**
   * @brief Returns true if the type matches the tokens type.
   *
   * @param rhs
   * @return true
   * @return false
   */
  bool operator==(const Type rhs) const;

  /**
   * @brief Returns true if the lexeme and types are the same.
   *
   * @param rhs
   * @return true
   * @return false
   */
  bool operator==(const Token &rhs) const;
};

/**
 * @brief Used to hash tokens for the PersistentMap implementation.
 *
 * @tparam
 */
template <>
struct std::hash<Token> {
  std::size_t operator()(const Token &token) const noexcept {
    return std::hash<std::string>{}(token.lexeme);
  }
};

using Tokens = std::vector<Token>;

/**
 * @brief Prints out a tokens type.
 *
 * @param out
 * @param type
 * @return std::ostream&
 */
std::ostream &operator<<(std::ostream &out, const Token::Type type);

/**
 * @brief Prints out a token.
 *
 * @param out
 * @param token
 * @return std::ostream&
 */
std::ostream &operator<<(std::ostream &out, const Token &token);

/**
 * @brief The text representation of all different Wick token types.
 *
 */
const std::array<std::string, 42> tokenTypeNames{
    "variable", "constant",  "if",      "else",       "for",        "while",
    "or",       "and",       "Boolean", "Identifier", "Number",     "String",
    "begin",    "end",       "{",       "}",          ";",          "(",
    ")",        "==",        "!=",      "<",          ">",          "<=",
    ">=",       "=",         "*",       "/",          "+",          "-",
    "!",        "mod",       ",",       ".",          "Subroutine", "Lambda",
    "return",   "prototype", "from",    "public",     "private",    ":"};