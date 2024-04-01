#include "statement.hpp"

namespace Statement {
Expression::Expression(::Expression::ExpressionUPtr iExpr) :
    expr{std::move(iExpr)} {}

void Expression::accept(Visitor *visitor, Environment *env) {
  visitor->visit(*this, env);
}

Variable::Variable(const Token &iVariable,
                   ::Expression::ExpressionUPtr iInitializer) :
    variable{iVariable}, initializer{std::move(iInitializer)} {}

void Variable::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Scope::Scope(std::vector<StatementUPtr> iStatements) :
    statements{std::move(iStatements)} {}

void Scope::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

If::If(::Expression::ExpressionUPtr iCondition,
       StatementUPtr iThen,
       StatementUPtr iElse) :
    condition{std::move(iCondition)},
    thenStmt{std::move(iThen)},
    elseStmt{std::move(iElse)} {}

void If::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

For::For(StatementUPtr iInitializer,
         ::Expression::ExpressionUPtr iCondition,
         StatementUPtr iBody,
         StatementUPtr iUpdate) :
    initializer{std::move(iInitializer)},
    condition{std::move(iCondition)},
    body{std::move(iBody)},
    update{std::move(iUpdate)} {}

void For::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Return::Return(const Token &iKeyword, ::Expression::ExpressionUPtr iExpr) :
    keyword{iKeyword}, expr{std::move(iExpr)} {}

void Return::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}
} // namespace Statement