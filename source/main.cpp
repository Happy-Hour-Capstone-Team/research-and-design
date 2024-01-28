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
#include <memory>

struct Literal;
struct Unary;
struct Binary;
struct Group;

struct Expression {
  class Visitor {
    public:
    virtual Token visit(const Literal &literal) = 0;
    virtual Token visit(const Unary &unary) = 0;
    virtual Token visit(const Binary &binary) = 0;
    virtual Token visit(const Group &group) = 0;
  };

  virtual Token accept(const std::unique_ptr<Visitor> visitor) = 0;
};

struct Literal : Expression {
  Literal(const Token &iToken) : token{iToken} {}
  const Token token;

  Token accept(const std::unique_ptr<Visitor> visitor) override {
    return visitor->visit(*this);
  }
};

struct Unary : Expression {
  Unary(const Token &iOp, std::unique_ptr<Expression> iRight) :
      op{iOp}, right{std::move(iRight)} {}
  const Token op;
  const std::unique_ptr<Expression> right;

  Token accept(const std::unique_ptr<Visitor> visitor) override {
    return visitor->visit(*this);
  }
};

struct Binary : Expression {
  Binary(std::unique_ptr<Expression> iLeft,
         const Token &iOp,
         std::unique_ptr<Expression> iRight) :
      left{std::move(iLeft)}, op{iOp}, right{std::move(iRight)} {}
  const std::unique_ptr<Expression> left;
  const Token op;
  const std::unique_ptr<Expression> right;

  Token accept(const std::unique_ptr<Visitor> visitor) override {
    return visitor->visit(*this);
  }
};

struct Group : Expression {
  const std::unique_ptr<Expression> expression;

  Token accept(const std::unique_ptr<Visitor> visitor) override {
    return visitor->visit(*this);
  }
};

struct Parser {
  public:
  private:
  Tokens tokens;
  int pos{0};
};

int main() {
  std::unique_ptr<Expression> test = std::make_unique<Binary>(
      std::make_unique<Unary>(
          Token{"!", Token::Type::Exclamation},
          std::make_unique<Literal>(Token{"true", Token::Type::Boolean})),
      Token{"or", Token::Type::Or},
      std::make_unique<Literal>(Token{"true", Token::Type::Boolean}));

  return 0;
}
