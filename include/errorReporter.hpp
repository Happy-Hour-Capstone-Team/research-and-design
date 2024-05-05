#pragma once

#include "token.hpp"

/**
 * @brief Provides support for error reporting in all the components of the Wick
 * interpreter.
 *
 */
class ErrorReporter {
  public:
  /**
   * @brief Reports an error with given information from the token and message
   * given. Sets error to true.
   *
   * @param token
   * @param msg
   */
  virtual void report(const Token &token, const std::string &msg);

  /**
   * @brief Reports an error with the given line and column number with the
   * message. Sets error to true.
   *
   * @param token
   * @param msg
   */
  virtual void report(const int line, const int col, const std::string &msg);

  /**
   * @brief Returns if an error has been detected.
   *
   * @return true
   * @return false
   */
  bool hadError() const;

  private:
  static bool error;
};