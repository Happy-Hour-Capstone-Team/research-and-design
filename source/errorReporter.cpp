#include "errorReporter.hpp"

void ErrorReporter::report(const Token &token, const std::string &msg) {
  std::cerr << "On line " << token.line << ", column " << token.col << " ["
            << token.lexeme << "]: " << msg << '\n';
  error = true;
}

void ErrorReporter::report(const int line, const int col, const std::string &msg) {
  std::cerr << "On line " << line << ", column " << col << ": " << msg << '\n';
  error = true;
}

bool ErrorReporter::hadError() const {
  return error;
}

bool ErrorReporter::error{false};