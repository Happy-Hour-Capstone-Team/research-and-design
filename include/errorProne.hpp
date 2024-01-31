#include "token.hpp"

class ErrorProne {
  public:
  virtual void report(const Token &token, const std::string &msg);
  virtual void report(const int line, const int col, const std::string &msg);
  virtual bool hadError() const;

  protected:
  bool error{false};
};