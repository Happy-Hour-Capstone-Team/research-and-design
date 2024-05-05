#pragma once

#include "errorReporter.hpp"
#include "expression.hpp"

/**
 * @brief The class responsible for parsing the tokens from the scanner into a
 * parse tree. This also verifies that the provided Wick program has correct
 * grammar and upholds some semantic rules.
 *
 */
class Parser {
  public:
  /**
   * @brief Constructs a parser with the provided tokens and an error reporter.
   *
   * @param iTokens
   * @param iErrorReporter
   */
  Parser(const Tokens &iTokens, ErrorReporter *const iErrorReporter = nullptr);

  /**
   * @brief Constructs a parser witht he provided tokens and an error reporter.
   *
   * @param iTokens
   * @param iErrorReporter
   */
  Parser(std::initializer_list<Token> iTokens,
         ErrorReporter *const iErrorReporter = nullptr);

  /**
   * @brief Parses the list of tokens provided in the parsers constructor based
   * on Wick's grammar rules.
   *
   * @return std::vector<Statement::StatementUPtr>
   */
  std::vector<Statement::StatementUPtr> parse();

  private:
  class ParserException : public std::runtime_error {
    public:
    ParserException();
  };

  void synchronize();

  Statement::StatementUPtr declaration(bool allowStatements = true);

  Statement::StatementUPtr functionDeclaration();

  Statement::StatementUPtr prototypeDeclaration();

  Statement::StatementUPtr variableDeclaration(const bool constant);

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

  Expression::ExpressionUPtr andExpr();

  Expression::ExpressionUPtr orExpr();

  Expression::ExpressionUPtr equality();

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