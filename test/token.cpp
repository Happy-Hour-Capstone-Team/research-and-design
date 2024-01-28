#include "doctest.h"
#include "token.hpp"

TEST_SUITE("Token") {
  TEST_CASE("Type equality works.") {
    Tokens input{{"variable", Token::Type::Variable},
                 {"constant", Token::Type::Constant},
                 {"if", Token::Type::If},
                 {"else", Token::Type::Else},
                 {"or", Token::Type::Or},
                 {"and", Token::Type::And},
                 {"true", Token::Type::Boolean},
                 {"test", Token::Type::Identifier},
                 {"1234", Token::Type::Number},
                 {"Testing!", Token::Type::String},
                 {"begin", Token::Type::Begin},
                 {"end", Token::Type::End}};
    std::vector<Token::Type> expected{Token::Type::Variable,
                                      Token::Type::Constant,
                                      Token::Type::If,
                                      Token::Type::Else,
                                      Token::Type::Or,
                                      Token::Type::And,
                                      Token::Type::Boolean,
                                      Token::Type::Identifier,
                                      Token::Type::Number,
                                      Token::Type::String,
                                      Token::Type::Begin,
                                      Token::Type::End};
    REQUIRE(input.size() == expected.size());
    for(int i = 0; i < input.size(); i++) CHECK(input[i] == expected[i]);
  }
}