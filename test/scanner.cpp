#include "scanner.hpp"
#include "doctest.h"

TEST_SUITE("Scanner") {
  Scanner scanner{{{"variable", TokenType::Variable},
                   {"[[:alpha:]][[:alnum:]]*", TokenType::Identifier},
                   {"[[:digit:]]*", TokenType::Integer},
                   {"[[:digit:]]*\\.[[:digit:]]*", TokenType::Real},
                   {"\\(", TokenType::LeftParenthesis},
                   {"\\)", TokenType::RightParenthesis},
                   {"=", TokenType::Equals},
                   {";", TokenType::Semicolon},
                   {"\\+", TokenType::Plus},
                   {"-", TokenType::Minus},
                   {"\\*", TokenType::Asterisk},
                   {"/", TokenType::ForwardSlash}}};

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
                                  Token{"(", TokenType::LeftParenthesis},
                                  Token{"2.525", TokenType::Real},
                                  Token{")", TokenType::RightParenthesis},
                                  Token{"(", TokenType::LeftParenthesis},
                                  Token{"3351", TokenType::Integer},
                                  Token{")", TokenType::RightParenthesis},
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
                                Token{"(", TokenType::LeftParenthesis, 1},
                                Token{"2.525", TokenType::Real, 1},
                                Token{")", TokenType::RightParenthesis, 1},
                                Token{"(", TokenType::LeftParenthesis, 1},
                                Token{"3351", TokenType::Integer, 1},
                                Token{")", TokenType::RightParenthesis, 1},
                                Token{";", TokenType::Semicolon, 1},
                                Token{"variable", TokenType::Variable, 2},
                                Token{"value2", TokenType::Identifier, 2},
                                Token{"=", TokenType::Equals, 2},
                                Token{"123", TokenType::Integer, 2},
                                Token{"+", TokenType::Plus, 2},
                                Token{"456", TokenType::Integer, 2},
                                Token{";", TokenType::Semicolon, 2},
                                Token{"variable", TokenType::Variable, 3},
                                Token{"value3", TokenType::Identifier, 3},
                                Token{"=", TokenType::Equals, 3},
                                Token{"360", TokenType::Integer, 3},
                                Token{"/", TokenType::ForwardSlash, 3},
                                Token{"60", TokenType::Integer, 3},
                                Token{";", TokenType::Semicolon, 3}};
    REQUIRE(results.size() == 24);
    for(int i = 0; i < results.size(); i++) CHECK(results[i] == expected[i]);
  } /*

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
                         "//: This is a multi-line comment.\n"
                         "This should be ignored.\n"
                         "Here's to hoping... ://\n"
                         "variable value2 = 123 + 456;\n"
                         "variable value3 = 360 / 60;"};
       auto results = scanner.tokenize(input);
       CHECK(results.size() == 24);
     }

     SUBCASE("Edge case multi-line comments.") {
       std::string input{"variable value1 = (2.525)(3351); //: What\n"
                         "if it's weird? :// variable value2 = 123 + 456;\n"
                         "variable value3 = 360 / 60;"};
       auto results = scanner.tokenize(input);
       CHECK(results.size() == 24);
     }
   }*/
}