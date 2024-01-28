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
};

struct Literal : Expression {
  Literal(const Token &iToken) : token{iToken} {}
  const Token token;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

struct Unary : Expression {
  Unary(const Token &iOp, std::shared_ptr<Expression> iRight) :
      op{iOp},
      right{std::move(iRight)} {}
  const Token op;
  const std::shared_ptr<Expression> right;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

struct Binary : Expression {
  Binary(std::shared_ptr<Expression> iLeft,
         const Token &iOp,
         std::shared_ptr<Expression> iRight) :
      left{std::move(iLeft)},
      op{iOp},
      right{std::move(iRight)} {}
  const std::shared_ptr<Expression> left;
  const Token op;
  const std::shared_ptr<Expression> right;

  std::any accept(Visitor *visitor) override {
    return visitor->visit(*this);
  }
};

struct Group : Expression {
  const std::shared_ptr<Expression> expression;

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
  private:
  Tokens tokens;
  int pos{0};
};

void test() {
  std::shared_ptr<Expression> test = std::make_shared<Binary>(
      std::make_shared<Unary>(
          Token{"!", Token::Type::Exclamation},
          std::make_shared<Literal>(Token{"true", Token::Type::Boolean})),
      Token{"or", Token::Type::Or},
      std::make_shared<Literal>(Token{"true", Token::Type::Boolean}));
  std::shared_ptr<Expression::Visitor> testVisitor =
      std::make_shared<PrintVisitor>();
  std::cout << std::any_cast<std::string>(test->accept(testVisitor.get()));
}

int main() {
  test();
  return 0;
}
