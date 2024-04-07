#include "expression.hpp"

namespace Expression {
Literal::Literal(const std::any &iValue) : value{iValue} {}

std::optional<std::any> Literal::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Unary::Unary(const ::Token &iOp, ExpressionUPtr iRight) :
    op{iOp}, right{std::move(iRight)} {}

std::optional<std::any> Unary::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Binary::Binary(ExpressionUPtr iLeft,
               const ::Token &iOp,
               ExpressionUPtr iRight) :
    left{std::move(iLeft)}, op{iOp}, right{std::move(iRight)} {}

std::optional<std::any> Binary::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Group::Group(ExpressionUPtr iExpr) : expr{std::move(iExpr)} {}

std::optional<std::any> Group::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Ternary::Ternary(ExpressionUPtr iThenExpr,
                 ExpressionUPtr iCondition,
                 ExpressionUPtr iElseExpr) :
    thenExpr{std::move(iThenExpr)},
    condition{std::move(iCondition)},
    elseExpr{std::move(iElseExpr)} {}

std::optional<std::any> Ternary::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Variable::Variable(const ::Token &iVariable) : variable{iVariable} {}

std::optional<std::any> Variable::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Assignment::Assignment(const ::Token &iVariable, ExpressionUPtr iValue) :
    variable{iVariable}, value{std::move(iValue)} {}

std::optional<std::any> Assignment::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Call::Call(ExpressionUPtr iCallee,
           std::vector<ExpressionUPtr> iArgs,
           const ::Token &iClosingParen) :
    callee{std::move(iCallee)},
    args{std::move(iArgs)},
    closingParen{iClosingParen} {}

std::optional<std::any> Call::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Lambda::Lambda(const std::vector<::Token> &iParams,
               std::vector<std::pair<::Token, ExpressionUPtr>> iDefaultParams,
               ::Statement::StatementUPtr iBody) :
    params{iParams},
    defaultParams{std::move(iDefaultParams)},
    body{std::move(iBody)} {}

std::optional<std::any> Lambda::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Prototype::Prototype(
    const std::optional<::Token> &iParent,
    std::vector<::Statement::StatementUPtr> iPublicProperties,
    std::vector<::Statement::StatementUPtr> iPrivateProperties) :
    parent{iParent},
    publicProperties{std::move(iPublicProperties)},
    privateProperties{std::move(iPrivateProperties)} {}

std::optional<std::any> Prototype::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Set::Set(ExpressionUPtr iObject,
         const ::Token &iProperty,
         ExpressionUPtr iValue) :
    object{std::move(iObject)}, property{iProperty}, value{std::move(iValue)} {}

std::optional<std::any> Set::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

Get::Get(ExpressionUPtr iObject, const ::Token &iProperty) :
    object{std::move(iObject)}, property{iProperty} {}

std::optional<std::any> Get::accept(Visitor *visitor, Environment *env) {
  return visitor->visit(*this, env);
}

} // namespace Expression