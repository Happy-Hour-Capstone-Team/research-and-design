#include "doctest.h"
#include "scanner.hpp"

void sameAs(const Token &token,
            const std::string &lexeme,
            const TokenType type,
            const int line,
            const int col) {
  CHECK(token.type == type);
  CHECK(token.lexeme == lexeme);
  CHECK(token.line == line);
  CHECK(token.col == col);
}

void sameAs(const Token &token, const Token &expected) {
  sameAs(token, expected.lexeme, expected.type, expected.line, expected.col);
}

void sameAs(const std::vector<Token> &results,
            const std::vector<Token> &expected) {
  REQUIRE(results.size() == expected.size());
  for(int i = 0; i < results.size(); i++) sameAs(results[i], expected[i]);
}

TEST_SUITE("Scanner") {
  Scanner scanner{};

  TEST_CASE("Single character tokens.") {
    auto results = scanner.tokenize("()=;+-*/");
    sameAs(results[0], "(", TokenType::LeftParen, 1, 1);
    sameAs(results[1], ")", TokenType::RightParen, 1, 2);
    sameAs(results[2], "=", TokenType::Equal, 1, 3);
    sameAs(results[3], ";", TokenType::Semicolon, 1, 4);
    sameAs(results[4], "+", TokenType::Plus, 1, 5);
    sameAs(results[5], "-", TokenType::Dash, 1, 6);
    sameAs(results[6], "*", TokenType::Asterisk, 1, 7);
    sameAs(results[7], "/", TokenType::ForwardSlash, 1, 8);
  }

  TEST_CASE("Single line tokenization.") {
    SUBCASE("One identifier.") {
      auto results = scanner.tokenize("test");
      REQUIRE(results.size() == 1);
      sameAs(results[0], "test", TokenType::Identifier, 1, 1);
    }

    SUBCASE("Three identifiers.") {
      auto results = scanner.tokenize("test1 test2 test3");
      REQUIRE(results.size() == 3);
      sameAs(results[0], "test1", TokenType::Identifier, 1, 1);
      sameAs(results[1], "test2", TokenType::Identifier, 1, 7);
      sameAs(results[2], "test3", TokenType::Identifier, 1, 13);
    }

    SUBCASE("Complex line.") {
      auto results = scanner.tokenize("variable value = (2.525)(3351);");
      REQUIRE(results.size() == 10);
      std::vector<Token> expected{Token{"variable", TokenType::Variable, 1, 1},
                                  Token{"value", TokenType::Identifier, 1, 10},
                                  Token{"=", TokenType::Equal, 1, 16},
                                  Token{"(", TokenType::LeftParen, 1, 18},
                                  Token{"2.525", TokenType::Number, 1, 19},
                                  Token{")", TokenType::RightParen, 1, 24},
                                  Token{"(", TokenType::LeftParen, 1, 25},
                                  Token{"3351", TokenType::Number, 1, 26},
                                  Token{")", TokenType::RightParen, 1, 30},
                                  Token{";", TokenType::Semicolon, 1, 31}};
      sameAs(results, expected);
    }

    SUBCASE("Strings.") {
      auto results = scanner.tokenize("variable word = \"John Doe\";");
      std::vector<Token> expected{Token{"variable", TokenType::Variable, 1, 1},
                                  Token{"word", TokenType::Identifier, 1, 10},
                                  Token{"=", TokenType::Equal, 1, 15},
                                  Token{"John Doe", TokenType::String, 1, 17},
                                  Token{";", TokenType::Semicolon, 1, 27}};
      sameAs(results, expected);
    }
  }
  TEST_CASE("Multiple lines.") {
    std::string input{"variable value1 = (2.525)(3351);\n"
                      "variable value2 = 123 + 456;\n"
                      "variable value3 = 360 / 60;"};
    auto results = scanner.tokenize(input);
    std::vector<Token> expected{Token{"variable", TokenType::Variable, 1, 1},
                                Token{"value1", TokenType::Identifier, 1, 10},
                                Token{"=", TokenType::Equal, 1, 17},
                                Token{"(", TokenType::LeftParen, 1, 19},
                                Token{"2.525", TokenType::Number, 1, 20},
                                Token{")", TokenType::RightParen, 1, 25},
                                Token{"(", TokenType::LeftParen, 1, 26},
                                Token{"3351", TokenType::Number, 1, 27},
                                Token{")", TokenType::RightParen, 1, 31},
                                Token{";", TokenType::Semicolon, 1, 32},
                                Token{"variable", TokenType::Variable, 2, 1},
                                Token{"value2", TokenType::Identifier, 2, 10},
                                Token{"=", TokenType::Equal, 2, 17},
                                Token{"123", TokenType::Number, 2, 19},
                                Token{"+", TokenType::Plus, 2, 23},
                                Token{"456", TokenType::Number, 2, 25},
                                Token{";", TokenType::Semicolon, 2, 28},
                                Token{"variable", TokenType::Variable, 3, 1},
                                Token{"value3", TokenType::Identifier, 3, 10},
                                Token{"=", TokenType::Equal, 3, 17},
                                Token{"360", TokenType::Number, 3, 19},
                                Token{"/", TokenType::ForwardSlash, 3, 23},
                                Token{"60", TokenType::Number, 3, 25},
                                Token{";", TokenType::Semicolon, 3, 27}};
    sameAs(results, expected);
  }

  TEST_CASE("Comments are ignored.") {
    SUBCASE("Single line comments.") {
      auto results =
          scanner.tokenize("variable value = 2.5 * 4 * anotherValue; // "
                           "This should be ignored.");
      std::vector<Token> expected{
          Token{"variable", TokenType::Variable, 1, 1},
          Token{"value", TokenType::Identifier, 1, 10},
          Token{"=", TokenType::Equal, 1, 16},
          Token{"2.5", TokenType::Number, 1, 18},
          Token{"*", TokenType::Asterisk, 1, 22},
          Token{"4", TokenType::Number, 1, 24},
          Token{"*", TokenType::Asterisk, 1, 26},
          Token{"anotherValue", TokenType::Identifier, 1, 28},
          Token{";", TokenType::Semicolon, 1, 40}};
      sameAs(results, expected);
      std::string input{"variable value1 = (2.525)(3351); // Ignore me! \n"
                        "variable value2 = 123 + 456; // Ignore me!"};
      results = scanner.tokenize(input);
      expected = {Token{"variable", TokenType::Variable, 1, 1},
                  Token{"value1", TokenType::Identifier, 1, 10},
                  Token{"=", TokenType::Equal, 1, 17},
                  Token{"(", TokenType::LeftParen, 1, 19},
                  Token{"2.525", TokenType::Number, 1, 20},
                  Token{")", TokenType::RightParen, 1, 25},
                  Token{"(", TokenType::LeftParen, 1, 26},
                  Token{"3351", TokenType::Number, 1, 27},
                  Token{")", TokenType::RightParen, 1, 31},
                  Token{";", TokenType::Semicolon, 1, 32},
                  Token{"variable", TokenType::Variable, 2, 1},
                  Token{"value2", TokenType::Identifier, 2, 10},
                  Token{"=", TokenType::Equal, 2, 17},
                  Token{"123", TokenType::Number, 2, 19},
                  Token{"+", TokenType::Plus, 2, 23},
                  Token{"456", TokenType::Number, 2, 25},
                  Token{";", TokenType::Semicolon, 2, 28}};
      sameAs(results, expected);
    }

    SUBCASE("Muli-line comments.") {
      std::string input{"variable value1 = (2.525)(3351);\n"
                        "/: This is a multi-line comment.\n"
                        "This should be ignored.\n"
                        "Here's to hoping... :/\n"
                        "variable value2 = 123 + 456;\n"
                        "variable value3 = 360 / 60;"};
      auto results = scanner.tokenize(input);
      auto expected = {Token{"variable", TokenType::Variable, 1, 1},
                       Token{"value1", TokenType::Identifier, 1, 10},
                       Token{"=", TokenType::Equal, 1, 17},
                       Token{"(", TokenType::LeftParen, 1, 19},
                       Token{"2.525", TokenType::Number, 1, 20},
                       Token{")", TokenType::RightParen, 1, 25},
                       Token{"(", TokenType::LeftParen, 1, 26},
                       Token{"3351", TokenType::Number, 1, 27},
                       Token{")", TokenType::RightParen, 1, 31},
                       Token{";", TokenType::Semicolon, 1, 32},
                       Token{"variable", TokenType::Variable, 5, 1},
                       Token{"value2", TokenType::Identifier, 5, 10},
                       Token{"=", TokenType::Equal, 5, 17},
                       Token{"123", TokenType::Number, 5, 19},
                       Token{"+", TokenType::Plus, 5, 23},
                       Token{"456", TokenType::Number, 5, 25},
                       Token{";", TokenType::Semicolon, 5, 28},
                       Token{"variable", TokenType::Variable, 6, 1},
                       Token{"value3", TokenType::Identifier, 6, 10},
                       Token{"=", TokenType::Equal, 6, 17},
                       Token{"360", TokenType::Number, 6, 19},
                       Token{"/", TokenType::ForwardSlash, 6, 23},
                       Token{"60", TokenType::Number, 6, 25},
                       Token{";", TokenType::Semicolon, 6, 27}};
      sameAs(results, expected);
    }

    SUBCASE("Edge case multi-line comments.") {
      std::string input{"variable value1 = (2.525)(3351); /: What\n"
                        "if it's weird? :/ variable value2 = 123 + 456;\n"
                        "variable value3 = 360 / 60; /: What about this /:"};
      auto results = scanner.tokenize(input);
      auto expected = {Token{"variable", TokenType::Variable, 1, 1},
                       Token{"value1", TokenType::Identifier, 1, 10},
                       Token{"=", TokenType::Equal, 1, 17},
                       Token{"(", TokenType::LeftParen, 1, 19},
                       Token{"2.525", TokenType::Number, 1, 20},
                       Token{")", TokenType::RightParen, 1, 25},
                       Token{"(", TokenType::LeftParen, 1, 26},
                       Token{"3351", TokenType::Number, 1, 27},
                       Token{")", TokenType::RightParen, 1, 31},
                       Token{";", TokenType::Semicolon, 1, 32},
                       Token{"variable", TokenType::Variable, 2, 19},
                       Token{"value2", TokenType::Identifier, 2, 28},
                       Token{"=", TokenType::Equal, 2, 35},
                       Token{"123", TokenType::Number, 2, 37},
                       Token{"+", TokenType::Plus, 2, 41},
                       Token{"456", TokenType::Number, 2, 43},
                       Token{";", TokenType::Semicolon, 2, 46},
                       Token{"variable", TokenType::Variable, 3, 1},
                       Token{"value3", TokenType::Identifier, 3, 10},
                       Token{"=", TokenType::Equal, 3, 17},
                       Token{"360", TokenType::Number, 3, 19},
                       Token{"/", TokenType::ForwardSlash, 3, 23},
                       Token{"60", TokenType::Number, 3, 25},
                       Token{";", TokenType::Semicolon, 3, 27}};
      sameAs(results, expected);
    }
  }
  TEST_CASE("Every token type can be detected.") {
    std::string input{"variable constant if else while\n"
                      "or and true false thisIsATest1\n"
                      "123 1.23 \"Testing here!\" begin\n"
                      "end { } ; ( ) == != < > <= >= =\n"
                      "* / + - !"};
    auto results = scanner.tokenize(input);
    auto expected = {Token{"variable", TokenType::Variable, 1, 1},
                     Token{"constant", TokenType::Constant, 1, 10},
                     Token{"if", TokenType::If, 1, 19},
                     Token{"else", TokenType::Else, 1, 22},
                     Token{"while", TokenType::While, 1, 27},
                     Token{"or", TokenType::Or, 2, 1},
                     Token{"and", TokenType::And, 2, 4},
                     Token{"true", TokenType::Boolean, 2, 8},
                     Token{"false", TokenType::Boolean, 2, 13},
                     Token{"thisIsATest1", TokenType::Identifier, 2, 19},
                     Token{"123", TokenType::Number, 3, 1},
                     Token{"1.23", TokenType::Number, 3, 5},
                     Token{"Testing here!", TokenType::String, 3, 10},
                     Token{"begin", TokenType::Begin, 3, 26},
                     Token{"end", TokenType::End, 4, 1},
                     Token{"{", TokenType::LeftCurly, 4, 5},
                     Token{"}", TokenType::RightCurly, 4, 7},
                     Token{";", TokenType::Semicolon, 4, 9},
                     Token{"(", TokenType::LeftParen, 4, 11},
                     Token{")", TokenType::RightParen, 4, 13},
                     Token{"==", TokenType::EqualTo, 4, 15},
                     Token{"!=", TokenType::NotEqualTo, 4, 18},
                     Token{"<", TokenType::LessThan, 4, 21},
                     Token{">", TokenType::GreaterThan, 4, 23},
                     Token{"<=", TokenType::LessThanOrEqualTo, 4, 25},
                     Token{">=", TokenType::GreaterThanOrEqualTo, 4, 28},
                     Token{"=", TokenType::Equal, 4, 31},
                     Token{"*", TokenType::Asterisk, 5, 1},
                     Token{"/", TokenType::ForwardSlash, 5, 3},
                     Token{"+", TokenType::Plus, 5, 5},
                     Token{"-", TokenType::Dash, 5, 7},
                     Token{"!", TokenType::Exclamation, 5, 9}};
    sameAs(results, expected);
  }
}