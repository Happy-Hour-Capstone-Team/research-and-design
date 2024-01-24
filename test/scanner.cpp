#include "scanner.hpp"
#include "doctest.h"

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

    SUBCASE("One identifier.") {
      auto results = scanner.tokenize("test");
      REQUIRE(results.size() == 1);
      CHECK(results[0].lexeme == "test");
      CHECK(results[0].type == TokenType::Identifier);
    }

    SUBCASE("Three identifiers.") {
      auto results = scanner.tokenize("test1 test2 test3");
      REQUIRE(results.size() == 3);
      CHECK(results[0].lexeme == "test1");
      CHECK(results[0].type == TokenType::Identifier);
      CHECK(results[1].lexeme == "test2");
      CHECK(results[1].type == TokenType::Identifier);
      CHECK(results[2].lexeme == "test3");
      CHECK(results[2].type == TokenType::Identifier);
    }

    SUBCASE("Complex line.") {
      auto results = scanner.tokenize("value = (2.525)(3351);");
      REQUIRE(results.size() == 9);
      std::vector<Token> expected{
          Token{"value", TokenType::Identifier},
          Token{"=", TokenType::Equals},
          Token{"(", TokenType::LeftParenthesis},
          Token{"2.525", TokenType::Real},
          Token{")", TokenType::RightParenthesis},
          Token{"(", TokenType::LeftParenthesis},
          Token{"3351", TokenType::Integer},
          Token{")", TokenType::RightParenthesis},
          Token{";", TokenType::StatementEnd}
      };
      Scanner::printTokens(results);
      for(int i = 0; i < results.size(); i++) {
        CHECK(results[i].lexeme == expected[i].lexeme);
        CHECK(results[i].type == expected[i].type);
        CHECK(results[i].line == 1);
      }
    }
  }
}