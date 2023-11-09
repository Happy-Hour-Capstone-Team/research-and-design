#include <cctype>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

enum class TokenType {
  Identifier,
  Integer,
  Real,
  LeftParenthesis,
  RightParenthesis,
  Equals,
  StatementEnd,
  Custom
};

struct Token {
  std::string lexeme;
  TokenType type;
};

struct TokenRule {
  std::regex pattern;
  TokenType type;
};

class Tokenizer {
public:
  Tokenizer() {}

  void addRule(const std::string &pattern, TokenType type) {
    customRules_.push_back({std::regex(pattern), type});
  }

  std::vector<Token> tokenize(const std::string &input) {
    std::vector<Token> tokens;
    std::string currentToken;
    bool inComment = false;

    for (size_t i = 0; i < input.length(); i++) {
      char c = input[i];

      if (inComment) {
        if (c == '\n') {
          inComment = false;
        }
        continue;
      }

      if (std::isspace(c) || c == '\n') {
        if (!currentToken.empty()) {
          TokenType tokenType = TokenType::Identifier; // Default to Identifier
          for (const auto &rule : customRules_) {
            if (std::regex_match(currentToken, rule.pattern)) {
              tokenType = rule.type;
              break;
            }
          }
          tokens.push_back({currentToken, tokenType});
          currentToken.clear();
        }
      } else if (c == '/') {
        if (i + 1 < input.length() && input[i + 1] == '/') {
          inComment = true; // Handle single-line comments
          i++;
        }
      } else {
        currentToken += c;
      }
    }

    if (!currentToken.empty()) {
      TokenType tokenType = TokenType::Identifier; // Default to Identifier
      for (const auto &rule : customRules_) {
        if (std::regex_match(currentToken, rule.pattern)) {
          tokenType = rule.type;
          break;
        }
      }
      tokens.push_back({currentToken, tokenType});
    }

    return tokens;
  }

private:
  std::vector<TokenRule> customRules_;
};
