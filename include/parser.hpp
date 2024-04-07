#pragma once

#include "errorReporter.hpp"
#include "expression.hpp"

class Parser {
  public:
  Parser(const Tokens &iTokens, ErrorReporter *const iErrorReporter = nullptr);

  Parser(std::initializer_list<Token> iTokens,
         ErrorReporter *const iErrorReporter = nullptr);

  std::vector<Statement::StatementUPtr> parse();

  private:
  class ParserException : public std::runtime_error {
    public:
    ParserException();
  };

  void synchronize();

  Statement::StatementUPtr declaration(bool allowStatements = true);

  Statement::StatementUPtr functionDeclaration();

  Statement::StatementUPtr variableDeclaration();

  Statement::StatementUPtr constantDeclaration();

  Statement::StatementUPtr statement();

  Statement::StatementUPtr forStmt();

  Statement::StatementUPtr whileStmt();

  Statement::StatementUPtr ifStmt();

  Statement::StatementUPtr scope();

  Statement::StatementUPtr returnStmt();

  Statement::StatementUPtr expressionStatement(bool expectSemicolon = true);

  Expression::ExpressionUPtr expression();

  Expression::ExpressionUPtr simpleExpression();

  Expression::ExpressionUPtr lambda();

  Expression::ExpressionUPtr anonymousPrototype();

  Expression::ExpressionUPtr assignment();

  Expression::ExpressionUPtr equality();

  Expression::ExpressionUPtr andExpr();

  Expression::ExpressionUPtr orExpr();

  Expression::ExpressionUPtr comparison();

  Expression::ExpressionUPtr term();

  Expression::ExpressionUPtr factor();

  Expression::ExpressionUPtr unary();

  Expression::ExpressionUPtr call();

  Expression::ExpressionUPtr primary();

  bool match(const std::vector<Token::Type> &types);

  const Token &expect(const Token::Type type, const std::string &msg);
  bool check(const Token::Type type);
  const Token &advance();

  ParserException error(const Token &token, const std::string &msg);

  Tokens tokens;
  ErrorReporter *const errorReporter;
  int pos{0};
};