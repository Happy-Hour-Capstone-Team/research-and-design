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

/**
 * @brief Contains the grammar rules for Wick expressions.
 *
 */
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

/**
 * @brief Parent class of all expressions. Enforces accept method.
 *
 */
struct Expression {
  /**
   * @brief Declares an interface for visitors over Wick expressions.
   *
   */
  class Visitor {
    public:
    /**
     * @brief Visits a literal expression.
     *
     * @param literal
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Literal &literal,
                                          Environment *env) = 0;

    /**
     * @brief Visits a unary expression.
     *
     * @param unary
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Unary &unary,
                                          Environment *env) = 0;

    /**
     * @brief Visits a binary expression.
     *
     * @param binary
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Binary &binary,
                                          Environment *env) = 0;

    /**
     * @brief Visits a grouped expression (expressions within a set of
     * parentheses).
     *
     * @param group
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Group &group,
                                          Environment *env) = 0;

    /**
     * @brief Visits a ternary expression (<then result> if <condition> else
     * <otherwise result>).
     *
     * @param ternary
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Ternary &ternary,
                                          Environment *env) = 0;

    /**
     * @brief Visits a variable expression.
     *
     * @param variable
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Variable &variable,
                                          Environment *env) = 0;

    /**
     * @brief Visits an assignment expression.
     *
     * @param assignment
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Assignment &assignment,
                                          Environment *env) = 0;

    /**
     * @brief Visits a call expression.
     *
     * @param call
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Call &call,
                                          Environment *env) = 0;

    /**
     * @brief Visits a lambda expression.
     *
     * @param lambdaStmt
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Lambda &lambdaStmt,
                                          Environment *env) = 0;

    /**
     * @brief Visits a prototype expression.
     *
     * @param prototype
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Prototype &prototype,
                                          Environment *env) = 0;

    /**
     * @brief Visits a set expression (as in setting a prototype property to
     * something else).
     *
     * @param set
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Set &set, Environment *env) = 0;

    /**
     * @brief Visits a get expression (as in getting a prototype property).
     *
     * @param get
     * @param env
     * @return std::optional<std::any>
     */
    virtual std::optional<std::any> visit(const Get &get, Environment *env) = 0;
  };

  /**
   * @brief The accept method for all Wick expressions.
   *
   * @param visitor
   * @param env
   * @return std::optional<std::any>
   */
  virtual std::optional<std::any> accept(Visitor *visitor,
                                         Environment *env) = 0;

  // Needed to free memory in children.
  virtual ~Expression() = default;
};

using ExpressionUPtr = std::unique_ptr<Expression>;

/**
 * @brief A literal expression (just a value by itself).
 *
 */
struct Literal : Expression {
  /**
   * @brief Constructs a new literal expression.
   *
   * @param iValue
   */
  explicit Literal(const std::any &iValue);

  const std::any value;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A unary expression like !x or -x.
 *
 */
struct Unary : Expression {
  /**
   * @brief Constructs a new unary expression.
   *
   * @param iOp
   * @param iRight
   */
  Unary(const Token &iOp, ExpressionUPtr iRight);

  const Token op;
  const ExpressionUPtr right;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A binary expression like a + b or x * y.
 *
 */
struct Binary : Expression {
  /**
   * @brief Constructs a new binary expression.
   *
   * @param iLeft
   * @param iOp
   * @param iRight
   */
  Binary(ExpressionUPtr iLeft, const Token &iOp, ExpressionUPtr iRight);

  const ExpressionUPtr left;
  const Token op;
  const ExpressionUPtr right;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A group expression like ((x)) or ((a * a) + (b * b)).
 *
 */
struct Group : Expression {
  /**
   * @brief Constructs a new group expression.
   *
   * @param iExpr
   */
  explicit Group(ExpressionUPtr iExpr);

  const ExpressionUPtr expr;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A ternary expression like -1 if x < y else 1.
 *
 */
struct Ternary : Expression {
  /**
   * @brief Constructs a new ternary expression.
   *
   * @param iThenExpr
   * @param iCondition
   * @param iElseExpr
   */
  Ternary(ExpressionUPtr iThenExpr,
          ExpressionUPtr iCondition,
          ExpressionUPtr iElseExpr);

  const ExpressionUPtr thenExpr;
  const ExpressionUPtr condition;
  const ExpressionUPtr elseExpr;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A variable expression like x or y.
 *
 */
struct Variable : Expression {
  /**
   * @brief Constructs a new variable expression.
   *
   * @param iVariable
   */
  Variable(const Token &iVariable);

  const Token variable;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief An assignment expression like a = 1.
 *
 */
struct Assignment : Expression {
  /**
   * @brief Constructs a new assignment expression.
   *
   * @param iVariable
   * @param iValue
   */
  Assignment(const Token &iVariable, ExpressionUPtr iValue);

  const Token variable;
  const ExpressionUPtr value;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A call expression like fib(3) or addTwoNumbers(x, y).
 *
 */
struct Call : Expression {
  /**
   * @brief Constructs a new call expression.
   *
   * @param iCallee
   * @param iArgs
   * @param iClosingParen
   */
  Call(ExpressionUPtr iCallee,
       std::vector<ExpressionUPtr> iArgs,
       const Token &iClosingParen);

  const ExpressionUPtr callee;
  const std::vector<ExpressionUPtr> args;
  const Token closingParen;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A lambda expression like lambda (x, y) { return x + y }.
 *
 */
struct Lambda : Expression {
  /**
   * @brief Constructs a new lambda expression.
   *
   * @param iParams
   * @param iDefaultParams
   * @param iBody
   */
  Lambda(const std::vector<Token> &iParams,
         std::vector<std::pair<Token, ExpressionUPtr>> iDefaultParams,
         Statement::StatementUPtr iBody);

  const std::vector<Token> params;
  const std::vector<std::pair<Token, ExpressionUPtr>> defaultParams;
  const Statement::StatementUPtr body;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A prototype expression like prototype { public: subroutine
 * doSomething() {} }.
 *
 */
struct Prototype : Expression {
  /**
   * @brief Constructs a new prototype expression.
   *
   * @param iConstructor
   * @param iParent
   * @param iPublicProperties
   * @param iPrivateProperties
   */
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

/**
 * @brief A set expression like person.name = "Lisset".
 *
 */
struct Set : Expression {
  /**
   * @brief Constructs a new set expression.
   *
   * @param iObject
   * @param iProperty
   * @param iValue
   */
  Set(ExpressionUPtr iObject, const Token &iProperty, ExpressionUPtr iValue);

  const ExpressionUPtr object;
  const Token property;
  const ExpressionUPtr value;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

/**
 * @brief A get expression like person.name.
 *
 */
struct Get : Expression {
  /**
   * @brief Constructs a new get expression.
   *
   * @param iObject
   * @param iProperty
   */
  Get(ExpressionUPtr iObject, const Token &iProperty);

  ExpressionUPtr object; // Not constant to support conversion to Set.
  const Token property;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override;
};

} // namespace Expression