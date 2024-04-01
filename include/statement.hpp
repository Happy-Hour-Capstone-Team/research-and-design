#pragma once

#include "expression.hpp"

// Forward declaration in order to implement functions/classes.
namespace Expression {
struct Expression;
using ExpressionUPtr = std::unique_ptr<Expression>;
} // namespace Expression

namespace Statement {
// Forward declares in order to declare visit methods.
struct Expression;
struct Variable;
struct Scope;
struct If;
struct For;
struct Return;

struct Statement {
  class Visitor {
    public:
    virtual void visit(const Expression &expr, Environment *env) = 0;
    virtual void visit(const Variable &var, Environment *env) = 0;
    virtual void visit(const Scope &scope, Environment *env) = 0;
    virtual void visit(const If &ifStmt, Environment *env) = 0;
    virtual void visit(const For &forStmt, Environment *env) = 0;
    virtual void visit(const Return &returnStmt, Environment *env) = 0;
  };

  virtual void accept(Visitor *visitor, Environment *env) = 0;

  // Needed to free memory in children.
  virtual ~Statement() = default;
};

using StatementUPtr = std::unique_ptr<Statement>;

struct Expression : Statement {
  explicit Expression(::Expression::ExpressionUPtr iExpr);

  const ::Expression::ExpressionUPtr expr;

  void accept(Visitor *visitor, Environment *env) override;
};

struct Variable : Statement {
  Variable(const Token &iVariable, ::Expression::ExpressionUPtr iInitializer);

  const Token variable;
  const ::Expression::ExpressionUPtr initializer;

  void accept(Visitor *visitor, Environment *env) override;
};

struct Scope : Statement {
  Scope(std::vector<StatementUPtr> iStatements);

  const std::vector<StatementUPtr> statements;

  void accept(Visitor *visitor, Environment *env) override;
};

struct If : Statement {
  If(::Expression::ExpressionUPtr iCondition,
     StatementUPtr iThen,
     StatementUPtr iElse);

  const ::Expression::ExpressionUPtr condition;
  const StatementUPtr thenStmt;
  const StatementUPtr elseStmt;

  void accept(Visitor *visitor, Environment *env) override;
};

struct For : Statement {
  For(StatementUPtr iInitializer,
      ::Expression::ExpressionUPtr iCondition,
      StatementUPtr iBody,
      StatementUPtr iUpdate);

  const StatementUPtr initializer;
  const ::Expression::ExpressionUPtr condition;
  const StatementUPtr body;
  const StatementUPtr update;

  void accept(Visitor *visitor, Environment *env) override;
};

struct Return : Statement {
  Return(const Token &iKeyword, ::Expression::ExpressionUPtr iExpr);

  const Token keyword;
  const ::Expression::ExpressionUPtr expr;

  void accept(Visitor *visitor, Environment *env) override;
};

} // namespace Statement