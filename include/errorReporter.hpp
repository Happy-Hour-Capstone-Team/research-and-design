#pragma once

#include "token.hpp"

class ErrorReporter {
  public:
  virtual void report(const Token &token, const std::string &msg);
  virtual void report(const int line, const int col, const std::string &msg);
  bool hadError() const;

  private:
  static bool error;
};