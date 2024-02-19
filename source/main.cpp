/**
 * Thank you to Robert Nystrom for the Java examples provided in his
 * book found at https://craftinginterpreters.com/.
 *
 */

#include "main.hpp"

/**
 * PROGRAM ->  DECLARATION*
 * DECLARATION -> VARIABLE | STATEMENT
 * STATEMENT -> EXPRESSION_STATEMENT | BLOCK | IF | WHILE
 * EXPRESSION_STATEMENT -> EXPRESSION ;
 * BLOCK -> ( begin DECLARATION* end ) | ( { DECLARATION* } )
 * IF -> if '(' EXPRESSION ')' STATEMENT ( else STATEMENT )?
 * WHILE_STATEMENT -> while '(' EXPRESSION ')' STATEMENT
 * EXPRESSION -> AND ( or AND )* // Will probably want to separate OR later.
 * AND -> EQUALITY ( and EQUALITY )*
 * EQUALITY -> COMPARISON ( ( == | != ) COMPARISON )*
 * COMPARISON -> TERM ( ( < | <= | > | >= ) TERM )*
 * TERM -> FACTOR ( ( + | - ) FACTOR )*
 * FACTOR -> UNARY ( ( * | / ) UNARY )*
 * UNARY -> ( ! | - ) PRIMARY
 * PRIMARY -> true | false | Number | String | Identifier | '( EXPRESSION ')'
 * unary -> ( "!" | "-" ) unary | call ;
 * CALL -> primary ( "(" arguments? ")" )* ;
 * ARGUMENTS -> expression ( "," expression )* ;
 */

#include <any>
#include <fstream>
#include <memory>
#include <string>

namespace Expression {

// Forward declares in order to declare visit methods.
struct Literal;
struct Unary;
struct Binary;
struct Group;
struct Variable;

struct Expression {
  class Visitor {
    public:
    virtual std::any visit(const Literal &literal) = 0;
    virtual std::any visit(const Unary &unary) = 0;
    virtual std::any visit(const Binary &binary) = 0;
    virtual std::any visit(const Group &group) = 0;
    virtual std::any visit(const Variable &token) = 0;
  };

  virtual std::any accept(Visitor *visitor) = 0;

  // Need to free memory in children.
  virtual ~Expression() = default;
};

using ExpressionUPtr = std::unique_ptr<Expression>;

struct Literal : Expression {
  explicit Literal(const std::any &iValue) : value{iValue} {}
  const std::any value;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

struct Unary : Expression {
  Unary(const Token &iOp, ExpressionUPtr iRight) :
      op{iOp}, right{std::move(iRight)} {}
  const Token op;
  const ExpressionUPtr right;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

struct Binary : Expression {
  Binary(ExpressionUPtr iLeft, const Token &iOp, ExpressionUPtr iRight) :
      left{std::move(iLeft)}, op{iOp}, right{std::move(iRight)} {}
  const ExpressionUPtr left;
  const Token op;
  const ExpressionUPtr right;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

struct Group : Expression {
  const ExpressionUPtr expr;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

struct Variable : Expression {
  Variable(const Token &iVariable) : variable{iVariable} {}

  const Token variable;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

} // namespace Expression

namespace Statement {

// Forward declares in order to declare visit methods.
struct Expression;
struct Variable;

struct Statement {
  class Visitor {
    public:
    virtual void visit(const Expression &expr) = 0;
    virtual void visit(const Variable &var) = 0;
  };

  virtual void accept(Visitor *visitor) = 0;

  // Need to free memory in children.
  virtual ~Statement() = default;
};

using StatementUPtr = std::unique_ptr<Statement>;

struct Expression : Statement {
  explicit Expression(::Expression::ExpressionUPtr iExpr) :
      expr{std::move(iExpr)} {}
  const ::Expression::ExpressionUPtr expr;

  void accept(Visitor *visitor) override {
    visitor->visit(*this);
  }
};

struct Variable : Statement {
  Variable(const Token &iVariable, ::Expression::ExpressionUPtr iInitializer) :
      variable{iVariable}, initializer{std::move(iInitializer)} {}
  const Token variable;
  const ::Expression::ExpressionUPtr initializer;

  void accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

} // namespace Statement

class PrintVisitor : public Expression::Expression::Visitor {
  public:
  std::any visit(const Expression::Literal &literal) override {
    std::string str{""};
    if(literal.value.type() == typeid(long double)) {
      str = std::to_string(std::any_cast<long double>(literal.value));
    } else if(literal.value.type() == typeid(bool)) {
      str = std::to_string(std::any_cast<bool>(literal.value));
    } else {
      str = std::any_cast<std::string>(literal.value);
    }
    return str;
  }

  std::any visit(const Expression::Unary &unary) override {
    return parenthesize(unary.op.lexeme, {unary.right.get()});
  }

  std::any visit(const Expression::Binary &binary) override {
    return parenthesize(binary.op.lexeme,
                        {binary.left.get(), binary.right.get()});
  }

  std::any visit(const Expression::Group &group) override {
    return parenthesize("group", {group.expr.get()});
  }

  std::any visit(const Expression::Variable &token) override {
    return nullptr;
  }

  private:
  std::string
      parenthesize(const std::string &lexeme,
                   const std::vector<Expression::Expression *> &expressions) {
    std::string output{"(" + lexeme};
    for(Expression::Expression *expression : expressions)
      output += ", " + std::any_cast<std::string>(expression->accept(this));
    return output + ")";
  }
};

class Parser {
  public:
  Parser(const Tokens &iTokens, ErrorReporter *const iErrorReporter = nullptr) :
      tokens{iTokens}, errorReporter{iErrorReporter} {}
  Parser(std::initializer_list<Token> iTokens,
         ErrorReporter *const iErrorReporter = nullptr) :
      tokens{iTokens}, errorReporter{iErrorReporter} {}

  std::vector<Statement::StatementUPtr> parse() {
    std::vector<Statement::StatementUPtr> statements{};
    while(pos != tokens.size()) statements.push_back(declaration());
    return statements;
  }

  private:
  class ParserException : public std::runtime_error {
    public:
    ParserException() : std::runtime_error{"Internal parser exception."} {}
  };

  void synchronize() {
    advance();
    while(pos < tokens.size()) {
      if(tokens[pos - 1].type == Token::Type::Semicolon) return;
      switch(tokens[pos].type) {
        case Token::Type::Function:
        case Token::Type::Variable:
        case Token::Type::If:
        case Token::Type::While: return;
      }
      advance();
    }
  }

  Statement::StatementUPtr declaration() {
    try {
      if(match({Token::Type::Variable})) return variableDeclaration();
      return statement();
    } catch(ParserException e) {
      synchronize();
      return nullptr;
    }
  }

  Statement::StatementUPtr variableDeclaration() {
    const Token variable{
        expect(Token::Type::Identifier, "Expected a variable name.")};
    Expression::ExpressionUPtr variableInitializer{nullptr};
    if(match({Token::Type::Equal})) variableInitializer = expression();
    expect(Token::Type::Semicolon,
           "Expected a ';' after variable declaration.");
    return std::make_unique<Statement::Variable>(
        variable, std::move(variableInitializer));
  }

  Statement::StatementUPtr statement() {
    return expressionStatement();
  }

  Statement::StatementUPtr expressionStatement() {
    Expression::ExpressionUPtr expr = expression();
    expect(Token::Type::Semicolon, "Expected a ';' after statement.");
    return std::make_unique<Statement::Expression>(std::move(expr));
  }

  Expression::ExpressionUPtr expression() {
    return equality();
  }

  Expression::ExpressionUPtr equality() {
    Expression::ExpressionUPtr left{comparison()};
    while(match({Token::Type::NotEqualTo, Token::Type::EqualTo})) {
      const Token op{tokens[pos - 1]};
      Expression::ExpressionUPtr right{comparison()};
      left = std::make_unique<Expression::Binary>(
          std::move(left), op, std::move(right));
    }
    return std::move(left);
  }

  Expression::ExpressionUPtr comparison() {
    Expression::ExpressionUPtr left{term()};
    while(match({Token::Type::LessThan,
                 Token::Type::LessThanOrEqualTo,
                 Token::Type::GreaterThan,
                 Token::Type::GreaterThanOrEqualTo})) {
      const Token op{tokens[pos - 1]};
      Expression::ExpressionUPtr right{term()};
      left = std::make_unique<Expression::Binary>(
          std::move(left), op, std::move(right));
    }
    return std::move(left);
  }

  Expression::ExpressionUPtr term() {
    Expression::ExpressionUPtr left{factor()};
    while(match({Token::Type::Plus, Token::Type::Dash})) {
      const Token op{tokens[pos - 1]};
      Expression::ExpressionUPtr right{factor()};
      left = std::make_unique<Expression::Binary>(
          std::move(left), op, std::move(right));
    }
    return std::move(left);
  }

  Expression::ExpressionUPtr factor() {
    Expression::ExpressionUPtr left{unary()};
    while(match({Token::Type::Asterisk, Token::Type::ForwardSlash})) {
      const Token op{tokens[pos - 1]};
      Expression::ExpressionUPtr right{unary()};
      left = std::make_unique<Expression::Binary>(
          std::move(left), op, std::move(right));
    }
    return std::move(left);
  }

  Expression::ExpressionUPtr unary() {
    if(match({Token::Type::Exclamation, Token::Type::Dash})) {
      const Token op{tokens[pos - 1]};
      Expression::ExpressionUPtr right{primary()};
      return std::make_unique<Expression::Unary>(op, std::move(right));
    }
    return primary();
  }

  Expression::ExpressionUPtr primary() {
    if(match({Token::Type::Boolean}))
      return std::make_unique<Expression::Literal>(
          tokens[pos - 1].lexeme == "true" ? true : false);
    if(match({Token::Type::Number}))
      return std::make_unique<Expression::Literal>(
          std::stold(tokens[pos - 1].lexeme));
    if(match({Token::Type::String}))
      return std::make_unique<Expression::Literal>(tokens[pos - 1].lexeme);
    if(match({Token::Type::Identifier}))
      return std::make_unique<Expression::Variable>(tokens[pos - 1]);
    if(match({Token::Type::LeftParen})) {
      Expression::ExpressionUPtr expr{expression()};
      expect(Token::Type::RightParen, "Expected a ')' after expression.");
      return std::move(expr);
    }
    throw error(tokens[pos - 1], "Unexpected token.");
  }

  bool match(const std::vector<Token::Type> &types) {
    for(Token::Type type : types) {
      if(check(type)) {
        advance();
        return true;
      }
    }
    return false;
  }

  const Token &expect(const Token::Type type, const std::string &msg) {
    if(check(type)) return advance();
    throw error(tokens[pos - 1], msg);
  }

  bool check(const Token::Type type) {
    return pos != tokens.size() && tokens[pos] == type;
  }

  const Token &advance() {
    if(pos != tokens.size()) pos++;
    return tokens[pos - 1];
  }

  ParserException error(const Token &token, const std::string &msg) {
    if(errorReporter) errorReporter->report(token, msg);
    return ParserException{};
  }

  Tokens tokens;
  ErrorReporter *const errorReporter;
  int pos{0};
};

class Environment {
  public:
  using SymbolTable = std::unordered_map<Token, std::any>;

  void define(const Token &variable, const std::any &value) {
    values.insert(make_pair(variable, value));
  }

  std::any get(const Token &variable) {
    if(auto search = values.find(variable); search != values.end())
      return search->second;
    throw std::runtime_error{"Undefined variable!"}; // Make this better later.
  }

  private:
  SymbolTable values;
};

class Interpreter :
    public Expression::Expression::Visitor,
    public Statement::Statement::Visitor {
  public:
  std::any visit(const Expression::Literal &literal) override {
    return literal.value;
  }

  std::any visit(const Expression::Unary &unary) override {
    std::any rightVal = unary.right->accept(this);
    switch(unary.op.type) {
      case Token::Type::Exclamation: return !std::any_cast<bool>(rightVal);
      case Token::Type::Dash: return -std::any_cast<long double>(rightVal);
      default: throw std::runtime_error("Not a supported unary operator");
    }
  }

  std::any visit(const Expression::Binary &binary) override {
    std::any leftVal = binary.left->accept(this);
    std::any rightVal = binary.right->accept(this);
    const long double leftNumber{std::any_cast<long double>(leftVal)};
    const long double rightNumber{std::any_cast<long double>(rightVal)};
    switch(binary.op.type) {
      case Token::Type::NotEqualTo:
        return std::any_cast<long double>(leftVal) !=
               std::any_cast<long double>(rightVal);
      case Token::Type::EqualTo:
        return std::any_cast<long double>(leftVal) ==
               std::any_cast<long double>(rightVal);
      case Token::Type::LessThan:
        return std::any_cast<long double>(leftVal) <
               std::any_cast<long double>(rightVal);
      case Token::Type::LessThanOrEqualTo:
        return std::any_cast<long double>(leftVal) <=
               std::any_cast<long double>(rightVal);
      case Token::Type::GreaterThan:
        return std::any_cast<long double>(leftVal) >
               std::any_cast<long double>(rightVal);
      case Token::Type::GreaterThanOrEqualTo:
        return std::any_cast<long double>(leftVal) >=
               std::any_cast<long double>(rightVal);
      case Token::Type::Asterisk:
        try {
          return std::any_cast<long double>(leftVal) *
                 std::any_cast<long double>(rightVal);
        } catch(const std::bad_any_cast) {
          throw std::runtime_error("Error");
        }
      case Token::Type::Plus:
        try {
          return std::any_cast<long double>(leftNumber) +
                 std::any_cast<long double>(rightNumber);
        } catch(const std::bad_any_cast) {
          throw std::runtime_error("Error");
        }
      case Token::Type::Dash:
        try {
          return std::any_cast<long double>(leftVal) -
                 std::any_cast<long double>(rightVal);
        } catch(const std::bad_any_cast) {
          throw std::runtime_error("Error");
        }
      case Token::Type::ForwardSlash:
        try {
          return std::any_cast<long double>(leftVal) /
                 std::any_cast<long double>(rightVal);
        } catch(const std::bad_any_cast &) {
          throw std::runtime_error("Error");
        }
      default: throw std::runtime_error("Not a supproted binary operator");
    }
  }

  std::any visit(const Expression::Group &group) override {
    return group.expr->accept(this);
  }

  std::any visit(const Expression::Variable &variable) override {
    return environment.get(variable.variable);
  }

  void visit(const Statement::Expression &expr) override {
    evaluate(expr.expr.get());
  }

  void visit(const Statement::Variable &variable) override {
    std::any value;
    if(variable.initializer) value = evaluate(variable.initializer.get());
    environment.define(variable.variable, value);
  }

  void interpret(const std::vector<Statement::StatementUPtr> &statements) {
    try {
      for(std::size_t i{0}; i < statements.size(); i++)
        execute(statements[i].get());
    } catch(std::runtime_error e) {
      std::cerr << e.what() << '\n'; // Add proper error logging here later...
    }
  }

  // THIS SHOULD BE PRIVATE LATER!!!
  // private:
  Environment environment;

  std::any evaluate(Expression::Expression *expr) {
    return expr->accept(this);
  }

  void execute(Statement::Statement *statement) {
    statement->accept(this);
  }
};

int main(int argc, char *argv[]) {
  if(argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <file>\n";
    return 1;
  }
  std::ifstream file{argv[1]}; // Open the file specified in the CLI.
  if(!file.is_open()) {
    std::cerr << "Error opening file: " << argv[1] << "\n";
    return 1;
  }
  std::string expression{std::istreambuf_iterator<char>(file),
                         std::istreambuf_iterator<char>()};
  const std::unique_ptr<ErrorReporter> errorReporter{
      std::make_unique<ErrorReporter>()};
  Scanner scanner{expression, errorReporter.get()};
  Scanner::printTokens(scanner.tokenize());
  Parser parser{scanner.tokenize(), errorReporter.get()};
  const std::vector<Statement::StatementUPtr> statements{parser.parse()};
  if(errorReporter->hadError()) return 1;
  Interpreter interpreter{};
  interpreter.interpret(statements);
  std::cout << std::any_cast<bool>(
      interpreter.environment.get(Token{"a", Token::Type::Identifier})) << '\n';
  std::cout << std::any_cast<bool>(
      interpreter.environment.get(Token{"b", Token::Type::Identifier})) << '\n';
  return 0;
}