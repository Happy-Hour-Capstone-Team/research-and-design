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

// Forward declares in order to declare visit methods.
#include <any>
#include <memory>

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

using ExpressionUPtr = ExpressionUPtr;

struct Literal : Expression {
  explicit Literal(const Token &iToken) : token{iToken} {}
  const Token token;

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
  const ExpressionUPtr expression;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

class PrintVisitor : public Expression::Visitor {
  public:
  std::any visit(const Literal &literal) override {
    return literal.token.lexeme;
  }

  std::any visit(const Unary &unary) override {
    return parenthesize(unary.op.lexeme, {unary.right.get()});
  }

  std::any visit(const Binary &binary) override {
    return parenthesize(binary.op.lexeme,
                        {binary.left.get(), binary.right.get()});
  }

  std::any visit(const Group &group) override {
    return parenthesize("group", {group.expression.get()});
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

struct Parser {
  public:
  explicit Parser(std::initializer_list<Token> iTokens) : tokens{iTokens} {}

  ExpressionUPtr parse() {
    return expression();
  }

  private:
  ExpressionUPtr expression() {
    return std::move(equality());
  }

  ExpressionUPtr equality() {
    ExpressionUPtr left{comparison()};
    while(match({Token::Type::NotEqualTo, Token::Type::EqualTo})) {
      const Token op{tokens[pos - 1]};
      ExpressionUPtr right{comparison};
      left = std::make_unique<Binary>(left, op, right);
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
      left = std::make_unique<Binary>(left, op, right);
    }
    return std::move(left);
  }

  ExpressionUPtr term() {
    ExpressionUPtr left{factor()};
    while(match({Token::Type::LessThan,
                 Token::Type::LessThanOrEqualTo,
                 Token::Type::GreaterThan,
                 Token::Type::GreaterThanOrEqualTo})) {
      const Token op{tokens[pos - 1]};
      ExpressionUPtr right{factor()};
      left = std::make_unique<Binary>(left, op, right);
    }
    return std::move(left);
  }

  ExpressionUPtr factor() {
    ExpressionUPtr left{unary()};
    while(match({Token::Type::LessThan,
                 Token::Type::LessThanOrEqualTo,
                 Token::Type::GreaterThan,
                 Token::Type::GreaterThanOrEqualTo})) {
      const Token op{tokens[pos - 1]};
      ExpressionUPtr right{unary()};
      left = std::make_unique<Binary>(left, op, right);
    }
    return std::move(left);
  }

  ExpressionUPtr unary() {
    if(match({Token::Type::Exclamation, Token::Type::Dash})) {
      const Token op{tokens[pos - 1]};
      ExpressionUPtr right{primary()};
      return std::make_unique<Unary>(op, right);
    }
    return std::move(primary());
  }

  ExpressionUPtr primary() {
    switch(tokens[pos++].type) {
      case Token::Type::Boolean:
        return std::make_unique<Literal>(tokens[pos].lexeme == "true" ? true :
                                                                        false);
      case Token::Type::Number:
        return std::make_unique<Literal>(std::stold(tokens[pos].lexeme));
      case Token::Type::String:
        return std::make_unique<Literal>(tokens[pos].lexeme);
      case Token::Type::LeftParen:
        // DEAL WITH LEFT PARENTHESIS ISSUE
    }
  }

  bool match(const std::vector<Token::Type> &types) {
    for(Token::Type type : types) {
      if(tokens[pos].type == type) {
        pos++;
        return true;
      }
    }
    return false;
  }

  Tokens tokens;
  int pos{0};
};

int main() {
  ExpressionUPtr test = std::make_unique<Binary>(
      std::make_unique<Unary>(
          Token{"!", Token::Type::Exclamation},
          std::make_unique<Literal>(Token{"true", Token::Type::Boolean})),
      Token{"or", Token::Type::Or},
      std::make_unique<Literal>(Token{"true", Token::Type::Boolean}));
  std::unique_ptr<Expression::Visitor> testVisitor =
      std::make_unique<PrintVisitor>();
  std::cout << std::any_cast<std::string>(test->accept(testVisitor.get()));
  return 0;
}
