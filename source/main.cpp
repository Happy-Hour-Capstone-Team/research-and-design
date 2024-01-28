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

class Parser {
  public:
  private:
  Tokens tokens;
  int pos{0};
};

int main() {
  return 0;
}
