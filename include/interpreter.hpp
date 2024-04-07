#pragma once

#include "expression.hpp"
#include "native.hpp"
#include <optional>

class Interpreter :
    public Expression::Expression::Visitor,
    public Statement::Statement::Visitor {
  public:
  Interpreter();

  std::optional<std::any> visit(const Expression::Literal &literal,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Unary &unary,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Binary &binary,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Group &group,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Ternary &ternary,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Variable &variable,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Assignment &assignment,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Call &call,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Lambda &lambda,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Prototype &prototype,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Set &set,
                                Environment *env) override;

  std::optional<std::any> visit(const Expression::Get &get,
                                Environment *env) override;

  void visit(const Statement::Expression &expr, Environment *env) override;

  void visit(const Statement::Variable &variable, Environment *env) override;

  void visit(const Statement::Scope &scope, Environment *env) override;

  void visit(const Statement::If &ifStmt, Environment *env) override;

  void visit(const Statement::For &forStmt, Environment *env) override;

  void visit(const Statement::Return &returnStmt, Environment *env) override;

  void interpret(const std::vector<Statement::StatementUPtr> &statements);

  private:
  std::shared_ptr<Environment> global;

  std::any evaluate(Expression::Expression *expr, Environment *env);

  std::optional<std::any> optEvaluate(Expression::Expression *expr,
                                      Environment *env);

  void execute(Statement::Statement *statement, Environment *env);

  bool isTrue(const std::any &value);

  std::any stringOperation(const std::string &left,
                           const Token::Type op,
                           const std::string &right);

  std::any
      booleanOperation(const bool left, const Token::Type op, const bool right);

  std::any numericOperation(const long double left,
                            const Token::Type op,
                            const long double right);
};
