#include "main.hpp"

/**
 * PROGRAM ->  DECLARATION*
 * DECLARATION -> VARIABLE | STATEMENT
 * STATEMENT -> EXPRESSION_STATEMENT | BLOCK | IF | WHILE  
 * EXPRESSION_STATEMENT -> EXPRESSION ; 
 * BLOCK -> ( begin DECLARATION* end ) | ( { DECLARATION* } )
 * IF -> if ( EXPRESSION ) STATEMENT ( else STATEMENT )?
 * WHILE_STATEMENT -> while ( EXPRESSION ) STATEMENT
 * EXPRESSION -> 
 */

// equality -> expression
class Expression {

};

struct Equality {
  Comparison comparison;
};
class Parser {
  public:
};

int main() {
  return 0;
}
