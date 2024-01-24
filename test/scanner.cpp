#include "scanner.hpp"
#include "doctest.h"
#include <iostream>

TEST_SUITE("Scanner") {
  TEST_CASE("Scanner construction.") {
    Scanner scanner{{"[[:alpha:]][[:alnum:]]*", TokenType::Identifier},
                    {"#", TokenType::Integer},
                    {"#*.#*", TokenType::Real},
                    {"(", TokenType::LeftParenthesis},
                    {")", TokenType::RightParenthesis},
                    {"=", TokenType::Equals},
                    {";", TokenType::StatementEnd}};
  }

  TEST_CASE("Single line tokenization.") {
    Scanner scanner{{"[[:alpha:]][[:alnum:]]*", TokenType::Identifier},
                    {"[[:digit:]]*", TokenType::Integer},
                    {"[[:digit:]]*\\.[[:digit:]]*", TokenType::Real},
                    {"\\(", TokenType::LeftParenthesis},
                    {"\\)", TokenType::RightParenthesis},
                    {"=", TokenType::Equals},
                    {";", TokenType::StatementEnd}};
    auto results = scanner.tokenize("test");
    REQUIRE(results.size() == 1);
    CHECK(results[0].lexeme == "test");
    CHECK(results[0].type == TokenType::Identifier);
  }
}