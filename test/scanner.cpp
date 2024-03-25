#include "scanner.hpp"
#include "doctest.h"

void sameAs(const Tokens &results, const Tokens &expected) {
  REQUIRE(results.size() == expected.size());
  for(int i = 0; i < results.size(); i++) {
    CHECK(results[i].type == expected[i].type);
    CHECK(results[i].lexeme == expected[i].lexeme);
    CHECK(results[i].line == expected[i].line);
    CHECK(results[i].col == expected[i].col);
  }
}

TEST_SUITE("Scanner") {
  TEST_CASE("Single character tokens.") {
    Tokens results = Scanner{"()=;+-*/"}.tokenize();
    CHECK(results[0] == Token{"(", Token::Type::LeftParen, true, 1, 1});
    CHECK(results[1] == Token{")", Token::Type::RightParen, true, 1, 2});
    CHECK(results[2] == Token{"=", Token::Type::Equal, true, 1, 3});
    CHECK(results[3] == Token{";", Token::Type::Semicolon, true, 1, 4});
    CHECK(results[4] == Token{"+", Token::Type::Plus, true, 1, 5});
    CHECK(results[5] == Token{"-", Token::Type::Dash, true, 1, 6});
    CHECK(results[6] == Token{"*", Token::Type::Asterisk, true, 1, 7});
    CHECK(results[7] == Token{"/", Token::Type::ForwardSlash, true, 1, 8});
  }

  TEST_CASE("Single line tokenization.") {
    SUBCASE("One identifier.") {
      Tokens results = Scanner{"test"}.tokenize();
      REQUIRE(results.size() == 1);
      CHECK(results[0] == Token{"test", Token::Type::Identifier, true, 1, 1});
    }

    SUBCASE("Three identifiers.") {
      Tokens results = Scanner{"test1 test2 test3"}.tokenize();
      REQUIRE(results.size() == 3);
      CHECK(results[0] == Token{"test1", Token::Type::Identifier, true, 1, 1});
      CHECK(results[1] == Token{"test2", Token::Type::Identifier, true, 1, 7});
      CHECK(results[2] == Token{"test3", Token::Type::Identifier, true, 1, 13});
    }

    SUBCASE("Complex line.") {
      Tokens results = Scanner{"variable value = (2.525)(3351);"}.tokenize();
      REQUIRE(results.size() == 10);
      Tokens expected{{"variable", Token::Type::Variable, true, 1, 1},
                      {"value", Token::Type::Identifier, true, 1, 10},
                      {"=", Token::Type::Equal, true, 1, 16},
                      {"(", Token::Type::LeftParen, true, 1, 18},
                      {"2.525", Token::Type::Number, true, 1, 19},
                      {")", Token::Type::RightParen, true, 1, 24},
                      {"(", Token::Type::LeftParen, true, 1, 25},
                      {"3351", Token::Type::Number, true, 1, 26},
                      {")", Token::Type::RightParen, true, 1, 30},
                      {";", Token::Type::Semicolon, true, 1, 31}};
      sameAs(results, expected);
    }

    SUBCASE("Strings.") {
      Tokens results = Scanner{"variable word = \"John Doe\";"}.tokenize();
      Tokens expected{{"variable", Token::Type::Variable, true, 1, 1},
                      {"word", Token::Type::Identifier, true, 1, 10},
                      {"=", Token::Type::Equal, true, 1, 15},
                      {"John Doe", Token::Type::String, true, 1, 17},
                      {";", Token::Type::Semicolon, true, 1, 27}};
      sameAs(results, expected);
    }
  }
  TEST_CASE("Multiple lines.") {
    std::string input{"variable value1 = (2.525)(3351);\n"
                      "variable value2 = 123 + 456;\n"
                      "variable value3 = 360 / 60;"};
    Tokens results = Scanner{input}.tokenize();
    Tokens expected{{"variable", Token::Type::Variable, true, 1, 1},
                    {"value1", Token::Type::Identifier, true, 1, 10},
                    {"=", Token::Type::Equal, true, 1, 17},
                    {"(", Token::Type::LeftParen, true, 1, 19},
                    {"2.525", Token::Type::Number, true, 1, 20},
                    {")", Token::Type::RightParen, true, 1, 25},
                    {"(", Token::Type::LeftParen, true, 1, 26},
                    {"3351", Token::Type::Number, true, 1, 27},
                    {")", Token::Type::RightParen, true, 1, 31},
                    {";", Token::Type::Semicolon, true, 1, 32},
                    {"variable", Token::Type::Variable, true, 2, 1},
                    {"value2", Token::Type::Identifier, true, 2, 10},
                    {"=", Token::Type::Equal, true, 2, 17},
                    {"123", Token::Type::Number, true, 2, 19},
                    {"+", Token::Type::Plus, true, 2, 23},
                    {"456", Token::Type::Number, true, 2, 25},
                    {";", Token::Type::Semicolon, true, 2, 28},
                    {"variable", Token::Type::Variable, true, 3, 1},
                    {"value3", Token::Type::Identifier, true, 3, 10},
                    {"=", Token::Type::Equal, true, 3, 17},
                    {"360", Token::Type::Number, true, 3, 19},
                    {"/", Token::Type::ForwardSlash, true, 3, 23},
                    {"60", Token::Type::Number, true, 3, 25},
                    {";", Token::Type::Semicolon, true, 3, 27}};
    sameAs(results, expected);
  }

  TEST_CASE("Comments are ignored.") {
    SUBCASE("Single line comments.") {
      Tokens results = Scanner{"variable value = 2.5 * 4 * anotherValue; // "
                               "This should be ignored."}
                           .tokenize();
      Tokens expected{{"variable", Token::Type::Variable, true, 1, 1},
                      {"value", Token::Type::Identifier, true, 1, 10},
                      {"=", Token::Type::Equal, true, 1, 16},
                      {"2.5", Token::Type::Number, true, 1, 18},
                      {"*", Token::Type::Asterisk, true, 1, 22},
                      {"4", Token::Type::Number, true, 1, 24},
                      {"*", Token::Type::Asterisk, true, 1, 26},
                      {"anotherValue", Token::Type::Identifier, true, 1, 28},
                      {";", Token::Type::Semicolon, true, 1, 40}};
      sameAs(results, expected);
      std::string input{"variable value1 = (2.525)(3351); // Ignore me! \n"
                        "variable value2 = 123 + 456; // Ignore me!"};
      results = Scanner{input}.tokenize();
      expected = {{"variable", Token::Type::Variable, true, 1, 1},
                  {"value1", Token::Type::Identifier, true, 1, 10},
                  {"=", Token::Type::Equal, true, 1, 17},
                  {"(", Token::Type::LeftParen, true, 1, 19},
                  {"2.525", Token::Type::Number, true, 1, 20},
                  {")", Token::Type::RightParen, true, 1, 25},
                  {"(", Token::Type::LeftParen, true, 1, 26},
                  {"3351", Token::Type::Number, true, 1, 27},
                  {")", Token::Type::RightParen, true, 1, 31},
                  {";", Token::Type::Semicolon, true, 1, 32},
                  {"variable", Token::Type::Variable, true, 2, 1},
                  {"value2", Token::Type::Identifier, true, 2, 10},
                  {"=", Token::Type::Equal, true, 2, 17},
                  {"123", Token::Type::Number, true, 2, 19},
                  {"+", Token::Type::Plus, true, 2, 23},
                  {"456", Token::Type::Number, true, 2, 25},
                  {";", Token::Type::Semicolon, true, 2, 28}};
      sameAs(results, expected);
    }

    SUBCASE("Muli-line comments.") {
      std::string input{"variable value1 = (2.525)(3351);\n"
                        "/: This is a multi-line comment.\n"
                        "This should be ignored.\n"
                        "Here's to hoping... :/\n"
                        "variable value2 = 123 + 456;\n"
                        "variable value3 = 360 / 60;"};
      Tokens results = Scanner{input}.tokenize();
      Tokens expected = {{"variable", Token::Type::Variable, true, 1, 1},
                         {"value1", Token::Type::Identifier, true, 1, 10},
                         {"=", Token::Type::Equal, true, 1, 17},
                         {"(", Token::Type::LeftParen, true, 1, 19},
                         {"2.525", Token::Type::Number, true, 1, 20},
                         {")", Token::Type::RightParen, true, 1, 25},
                         {"(", Token::Type::LeftParen, true, 1, 26},
                         {"3351", Token::Type::Number, true, 1, 27},
                         {")", Token::Type::RightParen, true, 1, 31},
                         {";", Token::Type::Semicolon, true, 1, 32},
                         {"variable", Token::Type::Variable, true, 5, 1},
                         {"value2", Token::Type::Identifier, true, 5, 10},
                         {"=", Token::Type::Equal, true, 5, 17},
                         {"123", Token::Type::Number, true, 5, 19},
                         {"+", Token::Type::Plus, true, 5, 23},
                         {"456", Token::Type::Number, true, 5, 25},
                         {";", Token::Type::Semicolon, true, 5, 28},
                         {"variable", Token::Type::Variable, true, 6, 1},
                         {"value3", Token::Type::Identifier, true, 6, 10},
                         {"=", Token::Type::Equal, true, 6, 17},
                         {"360", Token::Type::Number, true, 6, 19},
                         {"/", Token::Type::ForwardSlash, true, 6, 23},
                         {"60", Token::Type::Number, true, 6, 25},
                         {";", Token::Type::Semicolon, true, 6, 27}};
      sameAs(results, expected);
    }

    SUBCASE("Edge case multi-line comments.") {
      std::string input{"variable value1 = (2.525)(3351); /: What\n"
                        "if it's weird? :/ variable value2 = 123 + 456;\n"
                        "variable value3 = 360 / 60; /: What about this /:"};
      Tokens results = Scanner{input}.tokenize();
      Tokens expected = {{"variable", Token::Type::Variable, true, 1, 1},
                         {"value1", Token::Type::Identifier, true, 1, 10},
                         {"=", Token::Type::Equal, true, 1, 17},
                         {"(", Token::Type::LeftParen, true, 1, 19},
                         {"2.525", Token::Type::Number, true, 1, 20},
                         {")", Token::Type::RightParen, true, 1, 25},
                         {"(", Token::Type::LeftParen, true, 1, 26},
                         {"3351", Token::Type::Number, true, 1, 27},
                         {")", Token::Type::RightParen, true, 1, 31},
                         {";", Token::Type::Semicolon, true, 1, 32},
                         {"variable", Token::Type::Variable, true, 2, 19},
                         {"value2", Token::Type::Identifier, true, 2, 28},
                         {"=", Token::Type::Equal, true, 2, 35},
                         {"123", Token::Type::Number, true, 2, 37},
                         {"+", Token::Type::Plus, true, 2, 41},
                         {"456", Token::Type::Number, true, 2, 43},
                         {";", Token::Type::Semicolon, true, 2, 46},
                         {"variable", Token::Type::Variable, true, 3, 1},
                         {"value3", Token::Type::Identifier, true, 3, 10},
                         {"=", Token::Type::Equal, true, 3, 17},
                         {"360", Token::Type::Number, true, 3, 19},
                         {"/", Token::Type::ForwardSlash, true, 3, 23},
                         {"60", Token::Type::Number, true, 3, 25},
                         {";", Token::Type::Semicolon, true, 3, 27}};
      sameAs(results, expected);
    }
  }

  TEST_CASE("Every token type can be detected.") {
    std::string input{
        "variable constant if else while\n"
        "or and true false thisIsATest1\n"
        "123 1.23 \"Testing here!\" begin\n"
        "end { } ; ( ) == != < > <= >= =\n"
        "* / + - ! % , ^ mod function lambda private public class"};
    Tokens results = Scanner{input}.tokenize();
    Tokens expected = {{"variable", Token::Type::Variable, true, 1, 1},
                       {"constant", Token::Type::Constant, true, 1, 10},
                       {"if", Token::Type::If, true, 1, 19},
                       {"else", Token::Type::Else, true, 1, 22},
                       {"while", Token::Type::While, true, 1, 27},
                       {"or", Token::Type::Or, true, 2, 1},
                       {"and", Token::Type::And, true, 2, 4},
                       {"true", Token::Type::Boolean, true, 2, 8},
                       {"false", Token::Type::Boolean, true, 2, 13},
                       {"thisIsATest1", Token::Type::Identifier, true, 2, 19},
                       {"123", Token::Type::Number, true, 3, 1},
                       {"1.23", Token::Type::Number, true, 3, 5},
                       {"Testing here!", Token::Type::String, true, 3, 10},
                       {"begin", Token::Type::Begin, true, 3, 26},
                       {"end", Token::Type::End, true, 4, 1},
                       {"{", Token::Type::LeftCurly, true, 4, 5},
                       {"}", Token::Type::RightCurly, true, 4, 7},
                       {";", Token::Type::Semicolon, true, 4, 9},
                       {"(", Token::Type::LeftParen, true, 4, 11},
                       {")", Token::Type::RightParen, true, 4, 13},
                       {"==", Token::Type::EqualTo, true, 4, 15},
                       {"!=", Token::Type::NotEqualTo, true, 4, 18},
                       {"<", Token::Type::LessThan, true, 4, 21},
                       {">", Token::Type::GreaterThan, true, 4, 23},
                       {"<=", Token::Type::LessThanOrEqualTo, true, 4, 25},
                       {">=", Token::Type::GreaterThanOrEqualTo, true, 4, 28},
                       {"=", Token::Type::Equal, true, 4, 31},
                       {"*", Token::Type::Asterisk, true, 5, 1},
                       {"/", Token::Type::ForwardSlash, true, 5, 3},
                       {"+", Token::Type::Plus, true, 5, 5},
                       {"-", Token::Type::Dash, true, 5, 7},
                       {"!", Token::Type::Exclamation, true, 5, 9},
                       {",", Token::Type::Comma, true, 5, 13},
                       {"^", Token::Type::Caret, true, 5, 15},
                       {"mod", Token::Type::Modulus, true, 5, 17},
                       {"function", Token::Type::Function, true, 5, 21},
                       {"lambda", Token::Type::Lambda, true, 5, 30},
                       {"private", Token::Type::Private, true, 5, 37},
                       {"public", Token::Type::Public, true, 5, 45},
                       {"class", Token::Type::Class, true, 5, 52}};
    Scanner::printTokens(results);
    sameAs(results, expected);
  }
}