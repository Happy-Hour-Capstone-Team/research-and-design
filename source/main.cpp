/**
 * Thank you to Robert Nystrom for the Java examples provided in his
 * book found at https://craftinginterpreters.com/.
 *
 */

#include "main.hpp"

/**
 * PROGRAM ->  DECLARATION*
 * DECLARATION -> CONSTANT | VARIABLE | STATEMENT
 * CONSTANT -> constant IDENTIFIER = EXPRESSION
 * VARIABLE -> variable IDENTIFIER ( = EXPRESSION )?
 * STATEMENT -> EXPRESSION_STATEMENT | BLOCK | IF | WHILE
 * EXPRESSION_STATEMENT -> EXPRESSION ;
 * BLOCK -> ( begin DECLARATION* end ) | ( { DECLARATION* } )
 * IF -> if EXPRESSION '{' STATEMENT '}' ( else IF? '{' STATEMENT '}' )?
 * WHILE_STATEMENT -> while '(' EXPRESSION ')' STATEMENT
 * EXPRESSION -> LAMBDA | AND ( or AND )*
 * LAMBDA -> lambda '(' PARAMETERS* DEFAULT_PARAMETERS* ')' '{' STATEMENT* '}'
 * AND -> EQUALITY ( and EQUALITY )*
 * EQUALITY -> COMPARISON ( ( == | != ) COMPARISON )*
 * COMPARISON -> TERM ( ( < | <= | > | >= ) TERM )*
 * TERM -> FACTOR ( ( + | - ) FACTOR )*
 * FACTOR -> UNARY ( ( * | / ) UNARY )*
 * UNARY -> ( ! | - ) PRIMARY | CALL
 * PRIMARY -> true | false | Number | String | Identifier | '( EXPRESSION ')'
 * CALL -> PRIMARY ( "(" ARGUMENTS? ")" )*
 * ARGUMENTS -> EXPRESSION ( "," EXPRESSION )*
 * PARAMETERS -> IDENTIFIER ( ',' IDENTIFIER )*
 * DEFAULT_PARAMETERS -> IDENTIFIER = EXPRESSION ( ',' IDENTIFIER = EXPRESSION
 * )*
 */

#include <any>
#include <fstream>
#include <memory>
#include <optional>
#include <string>

template <typename KeyType, typename ValueType, std::size_t N = 1024>
class PersistentMap {
  public:
  using Entry = std::shared_ptr<std::pair<KeyType, ValueType>>;

  using Table = std::array<std::vector<Entry>, N>;

  PersistentMap() = default;

  PersistentMap(const Table &iTable) : table{iTable} {}

  PersistentMap insert(const KeyType &key, const ValueType &value) {
    Table newTable{table};
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    newTable[hashIndex].push_back(
        std::make_shared<std::pair<KeyType, ValueType>>(
            std::make_pair(key, value)));
    return PersistentMap{newTable};
  }

  std::optional<PersistentMap> assign(const KeyType &key,
                                      const ValueType &value) {
    Table newTable{table};
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    for(std::size_t i{0}; i < newTable[hashIndex].size(); i++) {
      if(newTable[hashIndex][i]->first == key) {
        newTable[hashIndex][i]->second = value;
        return PersistentMap{newTable};
      }
    }
    return {};
  }

  std::optional<ValueType> get(const KeyType &key) const {
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    for(std::size_t i{0}; i < table[hashIndex].size(); i++) {
      if(table[hashIndex][i]->first == key) return table[hashIndex][i]->second;
    }
    return {};
  }

  Entry getEntry(const KeyType &key) const {
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    for(std::size_t i{0}; i < table[hashIndex].size(); i++) {
      if(table[hashIndex][i]->first == key) return table[hashIndex][i];
    }
    return nullptr;
  }

  private:
  Table table;
};

class Environment {
  public:
  using SymbolTable = PersistentMap<Token, std::any>;

  Environment() = default;

  Environment(const Environment &iEnv) : outer{iEnv.outer}, table{iEnv.table} {}

  Environment(Environment *iEnv, const bool persist = false) {
    outer = iEnv;
    if(persist) {
      outer = outer->outer;
      table = iEnv->table;
    }
  }

  void define(const Token &variable, const std::any &value) {
    table = table.insert(variable, value);
  }

  void assign(const Token &variable, const std::any &value) {
    auto entry = table.getEntry(variable);
    if(entry && entry->first.constant)
      throw std::runtime_error{"Can not assign to the constant " +
                               variable.lexeme + "!"};
    if(entry) {
      table = table.assign(variable, value).value();
      return;
    }
    if(outer) {
      outer->assign(variable, value);
      return;
    }
    throw std::runtime_error{"Undefined variable \"" + variable.lexeme + "\"!"};
  }

  std::any get(const Token &variable) {
    std::optional<std::any> value{table.get(variable)};
    if(!value && outer) value = outer->get(variable);
    if(value) return value.value();
    throw std::runtime_error{"Undefined variable!"};
  }

  private:
  Environment *outer{nullptr};
  SymbolTable table{};
};

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
struct Variable;
struct Assignment;
struct Call;
struct Lambda;
struct Class;

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
    virtual std::optional<std::any> visit(const Variable &variable,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Assignment &assignment,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Call &call,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Lambda &lambdaStmt,
                                          Environment *env) = 0;
    virtual std::optional<std::any> visit(const Class &classStmt,
                                          Environment *env) = 0;
  };

  virtual std::optional<std::any> accept(Visitor *visitor,
                                         Environment *env) = 0;

  // Need to free memory in children.
  virtual ~Expression() = default;
};

using ExpressionUPtr = std::unique_ptr<Expression>;

struct Literal : Expression {
  explicit Literal(const std::any &iValue) : value{iValue} {}
  const std::any value;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Unary : Expression {
  Unary(const Token &iOp, ExpressionUPtr iRight) :
      op{iOp}, right{std::move(iRight)} {}
  const Token op;
  const ExpressionUPtr right;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Binary : Expression {
  Binary(ExpressionUPtr iLeft, const Token &iOp, ExpressionUPtr iRight) :
      left{std::move(iLeft)}, op{iOp}, right{std::move(iRight)} {}
  const ExpressionUPtr left;
  const Token op;
  const ExpressionUPtr right;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Group : Expression {
  const ExpressionUPtr expr;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Variable : Expression {
  Variable(const Token &iVariable) : variable{iVariable} {}

  const Token variable;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Assignment : Expression {
  Assignment(const Token &iVariable, ExpressionUPtr iValue) :
      variable{iVariable}, value{std::move(iValue)} {}

  const Token variable;
  const ExpressionUPtr value;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Call : Expression {
  Call(ExpressionUPtr iCallee,
       std::vector<ExpressionUPtr> iArgs,
       const Token &iClosingParen) :
      callee{std::move(iCallee)},
      args{std::move(iArgs)},
      closingParen{iClosingParen} {}

  const ExpressionUPtr callee;
  const std::vector<ExpressionUPtr> args;
  const Token closingParen;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Lambda : Expression {
  Lambda(const std::vector<Token> &iParams,
         std::vector<std::pair<Token, ExpressionUPtr>> iDefaultParams,
         ::Statement::StatementUPtr iBody) :
      params{iParams},
      defaultParams{std::move(iDefaultParams)},
      body{std::move(iBody)} {}

  const std::vector<Token> params;
  const std::vector<std::pair<Token, ExpressionUPtr>> defaultParams;
  const ::Statement::StatementUPtr body;

  std::optional<std::any> accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Class : Expression {
  Class(
      std::unique_ptr<::Statement::Variable> iParent,
      std::vector<std::unique_ptr<::Statement::Variable>> iPublicProperties,
      std::vector<std::unique_ptr<::Statement::Variable>> iPrivateProperties) :
      parent{std::move(iParent)},
      publicProperties{std::move(iPublicProperties)},
      privateProperties{std::move(iPrivateProperties)} {}

  const Token name;
  const std::unique_ptr<::Statement::Variable> parent;
  const std::vector<std::unique_ptr<::Statement::Variable>> publicProperties;
  const std::vector<std::unique_ptr<::Statement::Variable>> privateProperties;
};

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

  // Need to free memory in children.
  virtual ~Statement() = default;
};

using StatementUPtr = std::unique_ptr<Statement>;

struct Expression : Statement {
  explicit Expression(::Expression::ExpressionUPtr iExpr) :
      expr{std::move(iExpr)} {}
  const ::Expression::ExpressionUPtr expr;

  void accept(Visitor *visitor, Environment *env) override {
    visitor->visit(*this, env);
  }
};

struct Variable : Statement {
  Variable(const Token &iVariable, ::Expression::ExpressionUPtr iInitializer) :
      variable{iVariable}, initializer{std::move(iInitializer)} {}
  const Token variable;
  const ::Expression::ExpressionUPtr initializer;

  void accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Scope : Statement {
  Scope(std::vector<StatementUPtr> iStatements) :
      statements{std::move(iStatements)} {}
  const std::vector<StatementUPtr> statements;

  void accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct If : Statement {
  If(::Expression::ExpressionUPtr iCondition,
     StatementUPtr iThen,
     StatementUPtr iElse) :
      condition{std::move(iCondition)},
      thenStmt{std::move(iThen)},
      elseStmt{std::move(iElse)} {}

  const ::Expression::ExpressionUPtr condition;
  const StatementUPtr thenStmt;
  const StatementUPtr elseStmt;

  void accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct For : Statement {
  For(StatementUPtr iInitializer,
      ::Expression::ExpressionUPtr iCondition,
      StatementUPtr iBody,
      StatementUPtr iUpdate) :
      initializer{std::move(iInitializer)},
      condition{std::move(iCondition)},
      body{std::move(iBody)},
      update{std::move(iUpdate)} {}

  const StatementUPtr initializer;
  const ::Expression::ExpressionUPtr condition;
  const StatementUPtr body;
  const StatementUPtr update;

  void accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

struct Return : Statement {
  Return(const Token &iKeyword, ::Expression::ExpressionUPtr iExpr) :
      keyword{iKeyword}, expr{std::move(iExpr)} {}

  const Token keyword;
  const ::Expression::ExpressionUPtr expr;

  void accept(Visitor *visitor, Environment *env) override {
    return visitor->visit(*this, env);
  }
};

} // namespace Statement

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
        case Token::Type::Subroutine:
        case Token::Type::Variable:
        case Token::Type::If:
        case Token::Type::While: return;
      }
      advance();
    }
  }

  Statement::StatementUPtr declaration() {
    try {
      if(match({Token::Type::Subroutine})) return functionDeclaration();
      if(match({Token::Type::Variable})) return variableDeclaration();
      if(match({Token::Type::Constant})) return constantDeclaration();
      return statement();
    } catch(ParserException e) {
      synchronize();
      return nullptr;
    }
  }

  Statement::StatementUPtr functionDeclaration() {
    const Token name{
        expect(Token::Type::Identifier, "Expected a function name.")};
    Expression::ExpressionUPtr definition{lambda()};
    return std::make_unique<Statement::Variable>(name, std::move(definition));
  }

  Statement::StatementUPtr variableDeclaration() {
    Token variable{
        expect(Token::Type::Identifier, "Expected a variable name.")};
    variable.constant = false;
    Expression::ExpressionUPtr variableInitializer{nullptr};
    if(match({Token::Type::Equal})) variableInitializer = expression();
    expect(Token::Type::Semicolon,
           "Expected a ';' after variable declaration.");
    return std::make_unique<Statement::Variable>(
        variable, std::move(variableInitializer));
  }

  Statement::StatementUPtr constantDeclaration() {
    const Token constant{
        expect(Token::Type::Identifier, "Expected a constant name.")};
    expect(Token::Type::Equal, "Expected an initializer for constant value.");
    Expression::ExpressionUPtr constantInitializer{expression()};
    expect(Token::Type::Semicolon,
           "Expected a ';' after constant declaration.");
    return std::make_unique<Statement::Variable>(
        constant, std::move(constantInitializer));
  }

  Statement::StatementUPtr statement() {
    if(match({Token::Type::For})) return forStmt();
    if(match({Token::Type::While})) return whileStmt();
    if(match({Token::Type::If})) return ifStmt();
    if(match({Token::Type::LeftCurly})) return scope();
    if(match({Token::Type::Return})) return returnStmt();
    return expressionStatement();
  }

  Statement::StatementUPtr forStmt() {
    Statement::StatementUPtr initializer{variableDeclaration()};
    Expression::ExpressionUPtr condition{expression()};
    expect(Token::Type::Semicolon,
           "Expected a ';' after variable declaration.");
    Statement::StatementUPtr update{expressionStatement(false)};
    expect(Token::Type::LeftCurly, "Expected a '{' after for statment.");
    Statement::StatementUPtr body{scope()};
    return std::make_unique<Statement::For>(std::move(initializer),
                                            std::move(condition),
                                            std::move(body),
                                            std::move(update));
  }

  Statement::StatementUPtr whileStmt() {
    Expression::ExpressionUPtr condition{expression()};
    expect(Token::Type::LeftCurly, "Expected a '{' after if statment.");
    Statement::StatementUPtr body{scope()};
    return std::make_unique<Statement::For>(
        nullptr, std::move(condition), std::move(body), nullptr);
  }

  Statement::StatementUPtr ifStmt() {
    Expression::ExpressionUPtr condition{expression()};
    expect(Token::Type::LeftCurly, "Expected a '{' after if statment.");
    Statement::StatementUPtr thenStmt{scope()};
    Statement::StatementUPtr elseStmt{nullptr};
    if(match({Token::Type::Else})) {
      if(match({Token::Type::If}))
        elseStmt = ifStmt();
      else {
        expect(Token::Type::LeftCurly, "Expected a '{' after else statment.");
        elseStmt = scope();
      }
    }
    return std::make_unique<Statement::If>(
        std::move(condition), std::move(thenStmt), std::move(elseStmt));
  }

  Statement::StatementUPtr scope() {
    std::vector<Statement::StatementUPtr> statements{};
    while(!check(Token::Type::RightCurly)) {
      statements.push_back(declaration());
    }
    expect(Token::Type::RightCurly, "Expected a '}' after scope.");
    return std::make_unique<Statement::Scope>(std::move(statements));
  }

  Statement::StatementUPtr returnStmt() {
    const Token keyword{tokens[pos - 1]};
    Expression::ExpressionUPtr expr{nullptr};
    if(!check({Token::Type::Semicolon})) expr = expression();
    expect(Token::Type::Semicolon, "Expected a ';' after statement.");
    return std::make_unique<Statement::Return>(keyword, std::move(expr));
  }

  Statement::StatementUPtr expressionStatement(bool expectSemicolon = true) {
    Expression::ExpressionUPtr expr = expression();
    if(expectSemicolon)
      expect(Token::Type::Semicolon, "Expected a ';' after statement.");
    return std::make_unique<Statement::Expression>(std::move(expr));
  }

  Expression::ExpressionUPtr expression() {
    if(match({Token::Type::Lambda})) return lambda();
    if(match({Token::Type::Class})) return anonymousClass();
    return assignment();
  }

  Expression::ExpressionUPtr lambda() {
    expect(Token::Type::LeftParen, "Expected a '(' before parameters.");
    std::vector<Token> params;
    std::vector<std::pair<Token, Expression::ExpressionUPtr>> defaultParams;
    if(!check(Token::Type::RightParen)) {
      bool nowDefaultParams{false};
      do {
        const Token identifier{expect(Token::Type::Identifier,
                                      "Expected an identifier for parameter.")};
        if(match({Token::Type::Equal})) {
          nowDefaultParams = true;
          defaultParams.push_back(std::make_pair(identifier, expression()));
        } else if(!nowDefaultParams)
          params.push_back(identifier);
        else
          error(identifier, "Non-default parameters must come first!");
      } while(match({Token::Type::Comma}));
    }
    expect(Token::Type::RightParen, "Expected a ')' after parameters.");
    expect(Token::Type::LeftCurly, "Expected a '{' before statements.");
    Statement::StatementUPtr body{scope()};
    return std::make_unique<Expression::Lambda>(
        params, std::move(defaultParams), std::move(body));
  }

  /*
  Class(std::unique_ptr<::Statement::Variable> iParent,
        std::vector<std::unique_ptr<::Statement::Variable>> iPublicProperties,
        std::vector<std::unique_ptr<::Statement::Variable>> iPrivateProperties)
  : name{iName}, parent{std::move(iParent)},
        publicProperties{std::move(iPublicProperties)},
        privateProperties{std::move(iPrivateProperties)} {}
        */
  Expression::ExpressionUPtr anonymousClass() {
    std::unique_ptr<Statement::Variable> parent;
    if(match({Token::Type::From})) {
      const Token parentName{
          expect(Token::Type::Identifier, "Expected a class to inherit from!")};
      parent = std::make_unique<Statement::Variable>(parentName, nullptr);
    }
    if(match({Token::Type::Public})) {
      while(!check(Token::Type::Private) ) {
        statements.push_back(declaration());
      }
    }
  }

  Expression::ExpressionUPtr assignment() {
    Expression::ExpressionUPtr expr = equality();
    if(match({Token::Type::Equal})) {
      const Token equal{tokens[pos - 1]};
      Expression::ExpressionUPtr value{assignment()};
      try {
        const Token variable{
            static_cast<Expression::Variable *>(expr.get())->variable};
        return std::make_unique<Expression::Assignment>(variable,
                                                        std::move(value));
      } catch(...) {
        error(equal, "Can not assign to this token!");
      }
    }
    return std::move(expr);
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
    return call();
  }

  Expression::ExpressionUPtr call() {
    Expression::ExpressionUPtr expr{primary()};
    while(match({Token::Type::LeftParen})) {
      std::vector<Expression::ExpressionUPtr> args{};
      if(!check({Token::Type::RightParen})) {
        do args.push_back(expression());
        while(match({Token::Type::Comma}));
      }
      const Token closingParen{expect(Token::Type::RightParen,
                                      "Expected a ')' after call arguments.")};
      expr = std::make_unique<Expression::Call>(
          std::move(expr), std::move(args), closingParen);
    }
    return std::move(expr);
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

using Procedure =
    std::function<std::optional<std::any>(const std::vector<std::any> &args,
                                          Environment *fnEnv)>;

struct Callable {
  const std::size_t minArity;
  const std::size_t maxArity;
  const Procedure procedure;
  const std::shared_ptr<Environment> fnEnv;
};

namespace native {
std::optional<std::any> print(const std::vector<std::any> &args,
                              Environment *fnEnv) {
  std::any toPrint{args[0]};
  if(toPrint.type() == typeid(std::string))
    std::cout << std::any_cast<std::string>(toPrint) << '\n';
  else if(toPrint.type() == typeid(bool))
    std::cout << (std::any_cast<bool>(toPrint) ? "true" : "false") << '\n';
  else if(toPrint.type() == typeid(long double))
    std::cout << std::any_cast<long double>(toPrint) << '\n';
  return {};
}

std::optional<std::any> input(const std::vector<std::any> &args,
                              Environment *fnEnv) {
  if(args.size()) print(args, fnEnv);
  std::string line;
  std::getline(std::cin, line);
  return line;
}

std::optional<std::any> time(const std::vector<std::any> &args,
                             Environment *fnEnv) {
  const std::chrono::time_point currentTime{std::chrono::system_clock::now()};
  return static_cast<long double>(
      std::chrono::system_clock::to_time_t(currentTime));
}
} // namespace native

class Interpreter :
    public Expression::Expression::Visitor,
    public Statement::Statement::Visitor {
  public:
  Interpreter() {
    global = std::make_shared<Environment>();
    using namespace std::placeholders;
    global->define(Token{"print", Token::Type::Identifier},
                   Callable{1, 1, std::bind(native::print, _1, _2), global});
    global->define(Token{"input", Token::Type::Identifier},
                   Callable{1, 1, std::bind(native::input, _1, _2), global});
    global->define(Token{"time", Token::Type::Identifier},
                   Callable{0, 0, std::bind(native::time, _1, _2), global});
  }

  std::optional<std::any> visit(const Expression::Literal &literal,
                                Environment *env) override {
    return literal.value;
  }

  std::optional<std::any> visit(const Expression::Unary &unary,
                                Environment *env) override {
    std::any rightVal = evaluate(unary.right.get(), env);
    switch(unary.op.type) {
      case Token::Type::Exclamation: return !std::any_cast<bool>(rightVal);
      case Token::Type::Dash: return -std::any_cast<long double>(rightVal);
      default: throw std::runtime_error("Not a supported unary operator");
    }
  }

  std::optional<std::any> visit(const Expression::Binary &binary,
                                Environment *env) override {
    std::any leftVal = evaluate(binary.left.get(), env);
    std::any rightVal = evaluate(binary.right.get(), env);
    try {
      if(leftVal.type() == typeid(std::string))
        return stringOperation(std::any_cast<std::string>(leftVal),
                               binary.op.type,
                               std::any_cast<std::string>(rightVal));
      else if(leftVal.type() == typeid(bool))
        return booleanOperation(std::any_cast<bool>(leftVal),
                                binary.op.type,
                                std::any_cast<bool>(rightVal));
      else if(leftVal.type() == typeid(long double))
        return numericOperation(std::any_cast<long double>(leftVal),
                                binary.op.type,
                                std::any_cast<long double>(rightVal));
    } catch(std::bad_any_cast) {
      throw std::runtime_error("Type mismatch between operator!");
    }
  }

  std::optional<std::any> visit(const Expression::Group &group,
                                Environment *env) override {
    return evaluate(group.expr.get(), env);
  }

  std::optional<std::any> visit(const Expression::Variable &variable,
                                Environment *env) override {
    return env->get(variable.variable);
  }

  std::optional<std::any> visit(const Expression::Assignment &assignment,
                                Environment *env) override {
    std::any value = evaluate(assignment.value.get(), env);
    env->assign(assignment.variable, value);
    return value;
  }

  std::optional<std::any> visit(const Expression::Call &call,
                                Environment *env) override {
    std::any callee{evaluate(call.callee.get(), env)};
    std::vector<std::any> args{};
    for(std::size_t i{0}; i < call.args.size(); i++)
      args.push_back(evaluate(call.args[i].get(), env));
    try {
      Callable callable{std::any_cast<Callable>(callee)};
      if(args.size() < callable.minArity || args.size() > callable.maxArity)
        throw std::runtime_error{
            "Method expected at least " + std::to_string(callable.minArity) +
            " arguments, at most " + std::to_string(callable.maxArity) +
            " arguments, and received " + std::to_string(args.size()) +
            " arguments."};
      return callable.procedure(args, callable.fnEnv.get());
    } catch(std::bad_any_cast) {
      throw std::runtime_error{"Only functions and objects may be called."};
    }
  }

  std::optional<std::any> visit(const Expression::Lambda &lambda,
                                Environment *env) override {
    Procedure lambdaFn = [&lambda, this](const std::vector<std::any> &args,
                                         Environment *fnEnv) {
      std::unique_ptr<Environment> scopedEnv{
          std::make_unique<Environment>(fnEnv)};
      for(std::size_t i{0};
          i < lambda.params.size() + lambda.defaultParams.size();
          i++) {
        if(i < lambda.params.size())
          scopedEnv->define(lambda.params[i], args[i]);
        else if(i < args.size())
          scopedEnv->define(
              lambda.defaultParams[i - lambda.params.size()].first, args[i]);
        else
          scopedEnv->define(
              lambda.defaultParams[i - lambda.params.size()].first,
              evaluate(
                  lambda.defaultParams[i - lambda.params.size()].second.get(),
                  scopedEnv.get()));
      }
      try {
        execute(lambda.body.get(), scopedEnv.get());
      } catch(std::optional<std::any> value) {
        return value;
      }
      return std::make_optional<std::any>({});
    };
    return Callable{lambda.params.size(),
                    lambda.params.size() + lambda.defaultParams.size(),
                    lambdaFn,
                    std::make_shared<Environment>(env, true)};
  }

  std::optional<std::any> visit(const Expression::Class &classStmt,
                                Environment *env) override {
    return {};
  }

  void visit(const Statement::Expression &expr, Environment *env) override {
    optEvaluate(expr.expr.get(), env);
  }

  void visit(const Statement::Variable &variable, Environment *env) override {
    std::any value;
    if(variable.initializer) value = evaluate(variable.initializer.get(), env);
    // Necessary so functions can call themselves recursively.
    if(value.type() == typeid(Callable))
      std::any_cast<Callable>(value).fnEnv->define(variable.variable, value);
    env->define(variable.variable, value);
  }

  void visit(const Statement::Scope &scope, Environment *env) override {
    std::unique_ptr<Environment> scopedEnv{std::make_unique<Environment>(env)};
    for(std::size_t i{0}; i < scope.statements.size(); i++)
      execute(scope.statements[i].get(), scopedEnv.get());
  }

  void visit(const Statement::If &ifStmt, Environment *env) override {
    if(isTrue(evaluate(ifStmt.condition.get(), env)))
      execute(ifStmt.thenStmt.get(), env);
    else if(ifStmt.elseStmt)
      execute(ifStmt.elseStmt.get(), env);
  }

  void visit(const Statement::For &forStmt, Environment *env) override {
    std::unique_ptr<Environment> forEnv{std::make_unique<Environment>(env)};
    if(forStmt.initializer) execute(forStmt.initializer.get(), forEnv.get());
    while(isTrue(evaluate(forStmt.condition.get(), forEnv.get()))) {
      if(forStmt.body) execute(forStmt.body.get(), forEnv.get());
      if(forStmt.update) execute(forStmt.update.get(), forEnv.get());
    }
  }

  void visit(const Statement::Return &returnStmt, Environment *env) override {
    if(returnStmt.expr) throw optEvaluate(returnStmt.expr.get(), env);
    throw std::make_optional<std::any>({});
  }

  void interpret(const std::vector<Statement::StatementUPtr> &statements) {
    try {
      for(std::size_t i{0}; i < statements.size(); i++)
        execute(statements[i].get(), global.get());
    } catch(std::runtime_error e) {
      std::cout << e.what() << '\n'; // Add proper error logging here later...
    }
  }

  private:
  std::shared_ptr<Environment> global;

  std::any evaluate(Expression::Expression *expr, Environment *env) {
    std::optional<std::any> optValue{expr->accept(this, env)};
    if(!optValue.has_value())
      throw std::runtime_error{"Expected a non-null value!"};
    return optValue.value();
  }

  std::optional<std::any> optEvaluate(Expression::Expression *expr,
                                      Environment *env) {
    return expr->accept(this, env);
  }

  void execute(Statement::Statement *statement, Environment *env) {
    statement->accept(this, env);
  }

  bool isTrue(const std::any &value) {
    bool truth{false};
    if(value.type() == typeid(bool) && std::any_cast<bool>(value))
      truth = true;
    else if(value.type() == typeid(long double) &&
            std::any_cast<long double>(value) != 0.0)
      truth = true;
    else if(value.type() == typeid(std::string) &&
            std::any_cast<std::string>(value) != "")
      truth = true;
    return truth;
  }

  std::string stringOperation(const std::string &left,
                              const Token::Type op,
                              const std::string &right) {
    // + to concatenate strings
    // == to compare strings character by character
    // != to be opposite of above
    // < alphanumerically lower a < b or aardvark < zoology
    // >, <=, >= would be similar to the above
  }

  bool booleanOperation(const bool left,
                        const Token::Type op,
                        const bool right) {
    // and, or, not, ==, !=
  }

  long double numericOperation(const long double left,
                               const Token::Type op,
                               const long double right) {
    switch(op) {
      case Token::Type::NotEqualTo:
        return std::any_cast<long double>(left) !=
               std::any_cast<long double>(right);
      case Token::Type::EqualTo:
        return std::any_cast<long double>(left) ==
               std::any_cast<long double>(right);
      case Token::Type::LessThan:
        return std::any_cast<long double>(left) <
               std::any_cast<long double>(right);
      case Token::Type::LessThanOrEqualTo:
        return std::any_cast<long double>(left) <=
               std::any_cast<long double>(right);
      case Token::Type::GreaterThan:
        return std::any_cast<long double>(left) >
               std::any_cast<long double>(right);
      case Token::Type::GreaterThanOrEqualTo:
        return std::any_cast<long double>(left) >=
               std::any_cast<long double>(right);
      case Token::Type::Asterisk:
        return std::any_cast<long double>(left) *
               std::any_cast<long double>(right);
      case Token::Type::Plus:
        return std::any_cast<long double>(left) +
               std::any_cast<long double>(right);
      case Token::Type::Dash:
        return std::any_cast<long double>(left) -
               std::any_cast<long double>(right);
      case Token::Type::ForwardSlash:
        return std::any_cast<long double>(left) /
               std::any_cast<long double>(right);
      default: throw std::runtime_error("Not a supported binary operator");
    }
  }
};

int main(int argc, char *argv[]) {
  std::cout << std::setprecision(20);
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
  Parser parser{scanner.tokenize(), errorReporter.get()};
  const std::vector<Statement::StatementUPtr> statements{parser.parse()};
  if(errorReporter->hadError()) return 1;
  Interpreter interpreter{};
  interpreter.interpret(statements);
  return 0;
}