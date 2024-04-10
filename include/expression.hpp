#pragma once

#include "environment.hpp"
#include "statement.hpp"
#include <any>
#include <optional>

// Forward declaration in order to implement functions/classes.
namespace Statement {
struct Statement;
struct Variable;
using StatementUPtr = std::unique_ptr<Statement>;
} // namespace Statement

namespace Expression {
// Forward declares in order to declare visit methods.
struct Literal;
struct Unary;
struct Binary;
struct Group;
struct Ternary;
struct Variable;
struct Assignment;
struct Call;
struct Lambda;
struct Prototype;
struct Set;
struct Get;

struct Expression {
  class Visitor {
    public:
    virtual std::optional<std::any> visit(const Literal &literal,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Unary &unary,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Binary &binary,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Group &group,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Ternary &ternary,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Variable &variable,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Assignment &assignment,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Call &call,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Lambda &lambdaStmt,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Prototype &prototype,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Set &set, Environment *env) = 0;
    virtual std::optional<std::any> visit(const Get &get, Environment *env) = 0;
  };

  virtual std::optional<std::any> accept(Visitor *visitor,
                                         Environment *env) = 0;

  // Needed to free memory in children.
  virtual ~Expression() = default;
};

using ExpressionUPtr = std::unique_ptr<Expression>;

struct Literal : Expression {
  explicit Literal(const std::any &iValue);

  const std::any value;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Unary : Expression {
  Unary(const Token &iOp, ExpressionUPtr iRight);

  const Token op;
  const ExpressionUPtr right;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Binary : Expression {
  Binary(ExpressionUPtr iLeft, const Token &iOp, ExpressionUPtr iRight);

  const ExpressionUPtr left;
  const Token op;
  const ExpressionUPtr right;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Group : Expression {
  explicit Group(ExpressionUPtr iExpr);

  const ExpressionUPtr expr;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Ternary : Expression {
  Ternary(ExpressionUPtr iThenExpr,
          ExpressionUPtr iCondition,
          ExpressionUPtr iElseExpr);

  const ExpressionUPtr thenExpr;
  const ExpressionUPtr condition;
  const ExpressionUPtr elseExpr;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Variable : Expression {
  Variable(const Token &iVariable);

  const Token variable;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Assignment : Expression {
  Assignment(const Token &iVariable, ExpressionUPtr iValue);

  const Token variable;
  const ExpressionUPtr value;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Call : Expression {
  Call(ExpressionUPtr iCallee,
       std::vector<ExpressionUPtr> iArgs,
       const Token &iClosingParen);

  const ExpressionUPtr callee;
  const std::vector<ExpressionUPtr> args;
  const Token closingParen;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Lambda : Expression {
  Lambda(const std::vector<Token> &iParams,
         std::vector<std::pair<Token, ExpressionUPtr>> iDefaultParams,
         Statement::StatementUPtr iBody);

  const std::vector<Token> params;
  const std::vector<std::pair<Token, ExpressionUPtr>> defaultParams;
  const Statement::StatementUPtr body;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Prototype : Expression {
  Prototype(ExpressionUPtr iConstructor,
            const std::optional<Token> &iParent,
            std::vector<Statement::StatementUPtr> iPublicProperties,
            std::vector<Statement::StatementUPtr> iPrivateProperties);

  const ExpressionUPtr constructor;
  const std::optional<Token> parent;
  const std::vector<Statement::StatementUPtr> publicProperties;
  const std::vector<Statement::StatementUPtr> privateProperties;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Set : Expression {
  Set(ExpressionUPtr iObject, const Token &iProperty, ExpressionUPtr iValue);

  const ExpressionUPtr object;
  const Token property;
  const ExpressionUPtr value;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

struct Get : Expression {
  Get(ExpressionUPtr iObject, const Token &iProperty);

  ExpressionUPtr object; // Not constant to support conversion to Set.
  const Token property;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

} // namespace Expression