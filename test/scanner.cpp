#include "scanner.hpp"
#include "doctest.h"

TEST_SUITE("Scanner") {
  TEST_CASE("Scanner construction.") {
    Scanner scanner{{"variable", TokenType::Variable},
                    {"[[:alpha:]][[:alnum:]]*", TokenType::Identifier},
                    {"[[:digit:]]*", TokenType::Integer},
                    {"[[:digit:]]*\\.[[:digit:]]*", TokenType::Real},
                    {"\\(", TokenType::LeftParenthesis},
                    {"\\)", TokenType::RightParenthesis},
                    {"=", TokenType::Equals},
                    {";", TokenType::Semicolon},
                    {"+", TokenType::Plus},
                    {"-", TokenType::Minus},
                    {"*", TokenType::Asterisk},
                    {"/", TokenType::ForwardSlash}};
  }

  TEST_CASE("Single line tokenization.") {
    Scanner scanner{{"variable", TokenType::Variable},
                    {"[[:alpha:]][[:alnum:]]*", TokenType::Identifier},
                    {"[[:digit:]]*", TokenType::Integer},
                    {"[[:digit:]]*\\.[[:digit:]]*", TokenType::Real},
                    {"\\(", TokenType::LeftParenthesis},
                    {"\\)", TokenType::RightParenthesis},
                    {"=", TokenType::Equals},
                    {";", TokenType::Semicolon},
                    {"+", TokenType::Plus},
                    {"-", TokenType::Minus},
                    {"*", TokenType::Asterisk},
                    {"/", TokenType::ForwardSlash}};

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
      auto results = scanner.tokenize("variable value = (2.525)(3351);");
      REQUIRE(results.size() == 9);
      std::vector<Token> expected{Token{"variable", TokenType::Variable},
                                  Token{"value", TokenType::Identifier},
                                  Token{"=", TokenType::Equals},
                                  Token{"(", TokenType::LeftParenthesis},
                                  Token{"2.525", TokenType::Real},
                                  Token{")", TokenType::RightParenthesis},
                                  Token{"(", TokenType::LeftParenthesis},
                                  Token{"3351", TokenType::Integer},
                                  Token{")", TokenType::RightParenthesis},
                                  Token{";", TokenType::Semicolon}};
      Scanner::printTokens(results);
      for(int i = 0; i < results.size(); i++) {
        CHECK(results[i].lexeme == expected[i].lexeme);
        CHECK(results[i].type == expected[i].type);
        CHECK(results[i].line == 1);
      }
    }
  }

  TEST_CASE("Multiple lines.") {
    Scanner scanner{{"variable", TokenType::Variable},
                    {"[[:alpha:]][[:alnum:]]*", TokenType::Identifier},
                    {"[[:digit:]]*", TokenType::Integer},
                    {"[[:digit:]]*\\.[[:digit:]]*", TokenType::Real},
                    {"\\(", TokenType::LeftParenthesis},
                    {"\\)", TokenType::RightParenthesis},
                    {"=", TokenType::Equals},
                    {";", TokenType::Semicolon},
                    {"+", TokenType::Plus},
                    {"-", TokenType::Minus},
                    {"*", TokenType::Asterisk},
                    {"/", TokenType::ForwardSlash}};

    std::string input{"variable value1 = (2.525)(3351);\n"
                      "variable value2 = 123 + 456;\n"
                      "variable value3 = 360 / 60;"};
    std::vector<Token> expected{Token{"variable", TokenType::Variable},
                                Token{"value1", TokenType::Identifier},
                                Token{"=", TokenType::Equals},
                                Token{"(", TokenType::LeftParenthesis},
                                Token{"2.525", TokenType::Real},
                                Token{")", TokenType::RightParenthesis},
                                Token{"(", TokenType::LeftParenthesis},
                                Token{"3351", TokenType::Integer},
                                Token{")", TokenType::RightParenthesis},
                                Token{";", TokenType::Semicolon},
                                Token{"variable", TokenType::Variable},
                                Token{"value2", TokenType::Identifier},
                                Token{"=", TokenType::Equals},
                                Token{"123", TokenType::Integer},
                                Token{"+", TokenType::Plus},
                                Token{"456", TokenType::Integer},
                                Token{";", TokenType::Semicolon},
                                Token{"variable", TokenType::Variable},
                                Token{"value3", TokenType::Identifier},
                                Token{"=", TokenType::Equals},
                                Token{"360", TokenType::Integer},
                                Token{"/", TokenType::Plus},
                                Token{"60", TokenType::Integer},
                                Token{";", TokenType::Semicolon}};
  }
}