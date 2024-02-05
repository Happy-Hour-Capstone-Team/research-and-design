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
 */

#include <any>
#include <memory>
#include <string>

// Forward declares in order to declare visit methods.
struct Literal;
struct Unary;
struct Binary;
struct Group;

struct Expression {
  class Visitor {
    public:
    virtual std::any visit(const Literal &literal) = 0;
    virtual std::any visit(const Unary &unary) = 0;
    virtual std::any visit(const Binary &binary) = 0;
    virtual std::any visit(const Group &group) = 0;
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
      op{iOp},
      right{std::move(iRight)} {}
  const Token op;
  const ExpressionUPtr right;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

struct Binary : Expression {
  Binary(ExpressionUPtr iLeft, const Token &iOp, ExpressionUPtr iRight) :
      left{std::move(iLeft)},
      op{iOp},
      right{std::move(iRight)} {}
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

class PrintVisitor : public Expression::Visitor {
  public:
  std::any visit(const Literal &literal) override {
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

  std::any visit(const Unary &unary) override {
    return parenthesize(unary.op.lexeme, {unary.right.get()});
  }

  std::any visit(const Binary &binary) override {
    return parenthesize(binary.op.lexeme,
                        {binary.left.get(), binary.right.get()});
  }

  std::any visit(const Group &group) override {
    return parenthesize("group", {group.expr.get()});
  }

  private:
  std::string parenthesize(const std::string &lexeme,
                           const std::vector<Expression *> &expressions) {
    std::string output{"(" + lexeme};
    for(Expression *expression : expressions)
      output += ", " + std::any_cast<std::string>(expression->accept(this));
    return output + ")";
  }
};

class Parser {
  public:
  Parser(const Tokens &iTokens, ErrorReporter *const iErrorReporter = nullptr) :
      tokens{iTokens},
      errorReporter{iErrorReporter} {}
  Parser(std::initializer_list<Token> iTokens,
         ErrorReporter *const iErrorReporter = nullptr) :
      tokens{iTokens},
      errorReporter{iErrorReporter} {}

  ExpressionUPtr parse() {
    return expression();
  }

  private:
  class ParserException : public std::runtime_error {
    public:
    ParserException() : std::runtime_error{"Internal parser exception."} {}
  };

  ExpressionUPtr expression() {
    return std::move(equality());
  }

  ExpressionUPtr equality() {
    ExpressionUPtr left{comparison()};
    while(match({Token::Type::NotEqualTo, Token::Type::EqualTo})) {
      const Token op{tokens[pos - 1]};
      ExpressionUPtr right{comparison()};
      left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }
    return std::move(left);
  }

  ExpressionUPtr comparison() {
    ExpressionUPtr left{term()};
    while(match({Token::Type::LessThan,
                 Token::Type::LessThanOrEqualTo,
                 Token::Type::GreaterThan,
                 Token::Type::GreaterThanOrEqualTo})) {
      const Token op{tokens[pos - 1]};
      ExpressionUPtr right{term()};
      left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }
    return std::move(left);
  }

  ExpressionUPtr term() {
    ExpressionUPtr left{factor()};
    while(match({Token::Type::Asterisk, Token::Type::ForwardSlash})) {
      const Token op{tokens[pos - 1]};
      ExpressionUPtr right{factor()};
      left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }
    return std::move(left);
  }

  ExpressionUPtr factor() {
    ExpressionUPtr left{unary()};
    while(match({Token::Type::Plus, Token::Type::Dash})) {
      const Token op{tokens[pos - 1]};
      ExpressionUPtr right{unary()};
      left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }
    return std::move(left);
  }

  ExpressionUPtr unary() {
    if(match({Token::Type::Exclamation, Token::Type::Dash})) {
      const Token op{tokens[pos - 1]};
      ExpressionUPtr right{primary()};
      return std::make_unique<Unary>(op, std::move(right));
    }
    return std::move(primary());
  }

  ExpressionUPtr primary() {
    switch(tokens[incPos()].type) {
      case Token::Type::Boolean:
        return std::make_unique<Literal>(
            tokens[pos - 1].lexeme == "true" ? true : false);
      case Token::Type::Number:
        return std::make_unique<Literal>(std::stold(tokens[pos - 1].lexeme));
      case Token::Type::String:
        return std::make_unique<Literal>(tokens[pos - 1].lexeme);
      case Token::Type::LeftParen:
        ExpressionUPtr expr{expression()};
        expect(Token::Type::RightParen, "Expected ')' after expression.");
        return std::move(expr);
    }
    throw error(tokens[pos - 1], "Unexpected token.");
  }

  bool match(const std::vector<Token::Type> &types) {
    for(Token::Type type : types) {
      if(pos < tokens.size() && tokens[pos] == type) {
        incPos();
        return true;
      }
    }
    return false;
  }

  Token expect(const Token::Type type, const std::string &msg) {
    if(tokens[pos].type == type) return tokens[incPos()];
    throw error(tokens[pos], msg);
  }

  int incPos() {
    if(pos < tokens.size()) pos++;
    return pos - 1;
  }

  ParserException error(const Token &token, const std::string &msg) {
    if(errorReporter) errorReporter->report(token, msg);
    return ParserException{};
  }

  Tokens tokens;
  ErrorReporter *const errorReporter;
  int pos{0};
};

int main() {
  try {
    const std::unique_ptr<ErrorReporter> errorReporter =
        std::make_unique<ErrorReporter>();
    Scanner scanner{"(2 + 2) * (4.25 - 1 / 3)(", errorReporter.get()};
    Parser parser{scanner.tokenize(), errorReporter.get()};
    std::unique_ptr<Expression::Visitor> testVisitor =
        std::make_unique<PrintVisitor>();
    std::cout << std::any_cast<std::string>(
        parser.parse()->accept(testVisitor.get()));
  } catch(...) {
    std::cout << "BRO\n";
  }
  return 0;
}