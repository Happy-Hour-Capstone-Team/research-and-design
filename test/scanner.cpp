#include "scanner.hpp"
#include "doctest.h"

TEST_SUITE("Scanner") {
  Scanner scanner{
      {std::regex{"variable"}, TokenType::Variable},
      {std::regex{"[[:alpha:]][[:alnum:]]*"}, TokenType::Identifier},
      {std::regex{"[[:digit:]]*"}, TokenType::Number},
      {std::regex{"[[:digit:]]*\\.[[:digit:]]*"}, TokenType::Number},
      {std::regex{"\\("}, TokenType::LeftParen},
      {std::regex{"\\)"}, TokenType::RightParen},
      {std::regex{"="}, TokenType::Equals},
      {std::regex{";"}, TokenType::Semicolon},
      {std::regex{"\\+"}, TokenType::Plus},
      {std::regex{"-"}, TokenType::Minus},
      {std::regex{"\\*"}, TokenType::Asterisk},
      {std::regex{"/"}, TokenType::ForwardSlash}};

  TEST_CASE("Single line tokenization.") {
    SUBCASE("One identifier.") {
      auto results = scanner.tokenize("test");
      REQUIRE(results.size() == 1);
      CHECK(results[0] == Token{"test", TokenType::Identifier});
    }

    SUBCASE("Three identifiers.") {
      auto results = scanner.tokenize("test1 test2 test3");
      REQUIRE(results.size() == 3);
      CHECK(results[0] == Token{"test1", TokenType::Identifier});
      CHECK(results[1] == Token{"test2", TokenType::Identifier});
      CHECK(results[2] == Token{"test3", TokenType::Identifier});
    }

    SUBCASE("Complex line.") {
      auto results = scanner.tokenize("variable value = (2.525)(3351);");
      REQUIRE(results.size() == 10);
      std::vector<Token> expected{Token{"variable", TokenType::Variable},
                                  Token{"value", TokenType::Identifier},
                                  Token{"=", TokenType::Equals},
                                  Token{"(", TokenType::LeftParen},
                                  Token{"2.525", TokenType::Number},
                                  Token{")", TokenType::RightParen},
                                  Token{"(", TokenType::LeftParen},
                                  Token{"3351", TokenType::Number},
                                  Token{")", TokenType::RightParen},
                                  Token{";", TokenType::Semicolon}};
      for(int i = 0; i < results.size(); i++) CHECK(results[i] == expected[i]);
    }
  }

  TEST_CASE("Multiple lines.") {
    std::string input{"variable value1 = (2.525)(3351);\n"
                      "variable value2 = 123 + 456;\n"
                      "variable value3 = 360 / 60;"};
    auto results = scanner.tokenize(input);

    std::vector<Token> expected{Token{"variable", TokenType::Variable, 1},
                                Token{"value1", TokenType::Identifier, 1},
                                Token{"=", TokenType::Equals, 1},
                                Token{"(", TokenType::LeftParen, 1},
                                Token{"2.525", TokenType::Number, 1},
                                Token{")", TokenType::RightParen, 1},
                                Token{"(", TokenType::LeftParen, 1},
                                Token{"3351", TokenType::Number, 1},
                                Token{")", TokenType::RightParen, 1},
                                Token{";", TokenType::Semicolon, 1},
                                Token{"variable", TokenType::Variable, 2},
                                Token{"value2", TokenType::Identifier, 2},
                                Token{"=", TokenType::Equals, 2},
                                Token{"123", TokenType::Number, 2},
                                Token{"+", TokenType::Plus, 2},
                                Token{"456", TokenType::Number, 2},
                                Token{";", TokenType::Semicolon, 2},
                                Token{"variable", TokenType::Variable, 3},
                                Token{"value3", TokenType::Identifier, 3},
                                Token{"=", TokenType::Equals, 3},
                                Token{"360", TokenType::Number, 3},
                                Token{"/", TokenType::ForwardSlash, 3},
                                Token{"60", TokenType::Number, 3},
                                Token{";", TokenType::Semicolon, 3}};
    REQUIRE(results.size() == 24);
    for(int i = 0; i < results.size(); i++) CHECK(results[i] == expected[i]);
  }

  TEST_CASE("Comments are ignored.") {
    SUBCASE("Single line comments.") {
      auto results =
          scanner.tokenize("variable value = 2.5 * 4 * anotherValue; // "
                           "This should be ignored.");
      CHECK(results.size() == 9);
      std::string input{"variable value1 = (2.525)(3351); // Ignore me! \n"
                        "variable value2 = 123 + 456; // Ignore me!"};
      results = scanner.tokenize(input);
      CHECK(results.size() == 17);
    }

    SUBCASE("Muli-line comments.") {
      std::string input{"variable value1 = (2.525)(3351);\n"
                        "/: This is a multi-line comment.\n"
                        "This should be ignored.\n"
                        "Here's to hoping... :/\n"
                        "variable value2 = 123 + 456;\n"
                        "variable value3 = 360 / 60;"};
      auto results = scanner.tokenize(input);
      CHECK(results.size() == 24);
    }

    SUBCASE("Edge case multi-line comments.") {
      std::string input{"variable value1 = (2.525)(3351); /: What\n"
                        "if it's weird? :/ variable value2 = 123 + 456;\n"
                        "variable value3 = 360 / 60;"};
      auto results = scanner.tokenize(input);
      CHECK(results.size() == 24);
    }
  }
}