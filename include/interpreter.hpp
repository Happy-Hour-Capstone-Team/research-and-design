#pragma once

#include "expression.hpp"
#include "native.hpp"
#include <optional>

/**
 * @brief Class responsible for traversing the tree produced by the parser and
 * executing the associated behavior with the expression and statement nodes.
 * Implements the visitor interface for the expression and statement parent
 * nodes.
 *
 */
class Interpreter :
    public Expression::Expression::Visitor,
    public Statement::Statement::Visitor {
  public:
  /**
   * @brief Constructs the interpreter and initialized the global environment
   * with the appropriate native subroutines and constants.
   *
   */
  Interpreter();

  /**
   * @brief Evaluates a literal expression.
   *
   * @param literal
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Literal &literal,
                                Environment *env) override;

  /**
   * @brief Evaluates a unary expression according to its appropriate operator.
   *
   * @param unary
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Unary &unary,
                                Environment *env) override;

  /**
   * @brief Evaluates a binary expression according to its appropriate operator.
   *
   * @param binary
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Binary &binary,
                                Environment *env) override;

  /**
   * @brief Evaluates a group.
   *
   * @param group
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Group &group,
                                Environment *env) override;

  /**
   * @brief Evaluates a ternary expression.
   *
   * @param ternary
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Ternary &ternary,
                                Environment *env) override;

  /**
   * @brief Evaluates a variable expression.
   *
   * @param variable
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Variable &variable,
                                Environment *env) override;

  /**
   * @brief Evaluates an assignment and places the result in the appropriate
   * environment.
   *
   * @param assignment
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Assignment &assignment,
                                Environment *env) override;

  /**
   * @brief Evaluates a call expression. If the callee is callable it is treated
   * as a subroutine. If the callee is a prototypable it is treated as a call to
   * its constructor and a new prototype is created.
   *
   * @param call
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Call &call,
                                Environment *env) override;

  /**
   * @brief Evaluates a lambda expression and creates the appropriate callable
   * object.
   *
   * @param lambda
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Lambda &lambda,
                                Environment *env) override;

  /**
   * @brief Evaluates a prototype expression and creates the appropriate
   * prototypable object.
   *
   * @param prototype
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Prototype &prototype,
                                Environment *env) override;

  /**
   * @brief Evaluates a set expression and assigns the property to the value
   * provided.
   *
   * @param set
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Set &set,
                                Environment *env) override;

  /**
   * @brief Evaluates a get expression and returns the requested property.
   *
   * @param get
   * @param env
   * @return std::optional<std::any>
   */
  std::optional<std::any> visit(const Expression::Get &get,
                                Environment *env) override;

  /**
   * @brief Executes an expression statement.
   *
   * @param expr
   * @param env
   */
  void visit(const Statement::Expression &expr, Environment *env) override;

  /**
   * @brief Executes a variable (or constant) declaration.
   *
   * @param variable
   * @param env
   */
  void visit(const Statement::Variable &variable, Environment *env) override;

  /**
   * @brief Executes the statements within a scope.
   *
   * @param scope
   * @param env
   */
  void visit(const Statement::Scope &scope, Environment *env) override;

  /**
   * @brief Executes a series of if, else-if, and else statements according to
   * the truth of its conditions.
   *
   * @param ifStmt
   * @param env
   */
  void visit(const Statement::If &ifStmt, Environment *env) override;

  /**
   * @brief Executes a for statement body based on its initializer, condition,
   * and update statement.
   *
   * @param forStmt
   * @param env
   */
  void visit(const Statement::For &forStmt, Environment *env) override;

  /**
   * @brief Returns an expression from within a subroutine.
   *
   * @param returnStmt
   * @param env
   */
  void visit(const Statement::Return &returnStmt, Environment *env) override;

  /**
   * @brief Interprets a series of statements.
   *
   * @param statements
   */
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
