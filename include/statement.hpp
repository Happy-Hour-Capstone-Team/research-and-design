#pragma once

#include "expression.hpp"

// Forward declaration in order to implement functions/classes.
namespace Expression {
struct Expression;
using ExpressionUPtr = std::unique_ptr<Expression>;
} // namespace Expression

/**
 * @brief Contains the grammar rules for Wick statements.
 *
 */
namespace Statement {
// Forward declares in order to declare visit methods.
struct Expression;
struct Variable;
struct Scope;
struct If;
struct For;
struct Return;

/**
 * @brief Parent class of all statements. Enforces accept method.
 *
 */
struct Statement {
  /**
   * @brief Declares an interface for visitors over Wick statements.
   *
   */
  class Visitor {
    public:
    /**
     * @brief Visits an expression statement.
     *
     * @param expr
     * @param env
     */
    virtual void visit(const Expression &expr, Environment *env) = 0;

    /**
     * @brief Visits a variable declaration/definition.
     *
     * @param var
     * @param env
     */
    virtual void visit(const Variable &var, Environment *env) = 0;

    /**
     * @brief Visits a scope (code block surrounded by '{' and '}').
     *
     * @param scope
     * @param env
     */
    virtual void visit(const Scope &scope, Environment *env) = 0;

    /**
     * @brief Visits an if-then statement.
     *
     * @param ifStmt
     * @param env
     */
    virtual void visit(const If &ifStmt, Environment *env) = 0;

    /**
     * @brief Visits a for loop statement.
     *
     * @param forStmt
     * @param env
     */
    virtual void visit(const For &forStmt, Environment *env) = 0;

    /**
     * @brief Visits a return statement.
     *
     * @param returnStmt
     * @param env
     */
    virtual void visit(const Return &returnStmt, Environment *env) = 0;
  };

  /**
   * @brief The accept method for all Wick statements.
   *
   * @param visitor
   * @param env
   * @return std::optional<std::any>
   */
  virtual void accept(Visitor *visitor, Environment *env) = 0;

  // Needed to free memory in children.
  virtual ~Statement() = default;
};

using StatementUPtr = std::unique_ptr<Statement>;

/**
 * @brief An expression statement (think calling a subroutine like
 * doSomething()).
 *
 */
struct Expression : Statement {
  /**
   * @brief Constructs a new expression statement.
   *
   * @param iExpr
   */
  explicit Expression(::Expression::ExpressionUPtr iExpr);

  const ::Expression::ExpressionUPtr expr;

  void accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A variable declaration/definition like variable a = 2.
 *
 */
struct Variable : Statement {
  /**
   * @brief Constructs a new variable statement.
   *
   * @param iVariable
   * @param iInitializer
   */
  Variable(const Token &iVariable, ::Expression::ExpressionUPtr iInitializer);

  const Token variable;
  const ::Expression::ExpressionUPtr initializer;

  void accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief Visits a scope statement like { doSomething(); doSomethinElse(); }.
 *
 */
struct Scope : Statement {
  /**
   * @brief Constructs a new scope statement.
   *
   * @param iStatements
   */
  Scope(std::vector<StatementUPtr> iStatements);

  const std::vector<StatementUPtr> statements;

  void accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief An if-else statement like if isTrue() { doSomething(); } else {
 * doSomethingElse(); }.
 *
 */
struct If : Statement {
  /**
   * @brief Constructs a new if statement.
   *
   * @param iCondition
   * @param iThen
   * @param iElse
   */
  If(::Expression::ExpressionUPtr iCondition,
     StatementUPtr iThen,
     StatementUPtr iElse);

  const ::Expression::ExpressionUPtr condition;
  const StatementUPtr thenStmt;
  const StatementUPtr elseStmt;

  void accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A for loop statement (also used for while loops) like for i = 0; i <
 * 10; i++ { doSomething(); }.
 *
 */
struct For : Statement {
  /**
   * @brief Constructs a new for statement. 
   * 
   * @param iInitializer 
   * @param iCondition 
   * @param iBody 
   * @param iUpdate 
   */
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

/**
 * @brief A return statement in a subroutine like return true.
 *
 */
struct Return : Statement {
  /**
   * @brief Constructs a new return statement.
   *
   * @param iKeyword
   * @param iExpr
   */
  Return(const Token &iKeyword, ::Expression::ExpressionUPtr iExpr);

  const Token keyword; // Used for providing better error messages.
  const ::Expression::ExpressionUPtr expr;

  void accept(Visitor *visitor, Environment *env) override;
};

} // namespace Statement