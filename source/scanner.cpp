#include "scanner.hpp"

/*
TODO:
- Handle comments (see outline below)
- Read from file
- Error handle unrecognized tokens
- Error handle lone multiLineCommentEnd
*/

std::ostream &operator<<(std::ostream &out, const TokenType tokenType) {
  const int value{static_cast<int>(tokenType)};
  if(value >= tokenTypeNames.size()) return out << "Error";
  return out << tokenTypeNames[value];
}

Scanner::Scanner(std::initializer_list<TokenRule> rules) : rules{rules} {}

std::vector<Token> Scanner::tokenize(std::string input) {
  std::vector<Token> tokens;
  int line{1};
  for(int i{0}; i < input.size(); i++) {
    switch(input[i]) {
      case '\n': line++; break;
      case '(': tokens.push_back({"(", TokenType::LeftParen, line}); break;
      case ')': tokens.push_back({")", TokenType::RightParen, line}); break;
      case '=': tokens.push_back({"=", TokenType::Equals, line}); break;
      case ';': tokens.push_back({";", TokenType::Semicolon, line}); break;
      case '+': tokens.push_back({"+", TokenType::Plus, line}); break;
      case '-': tokens.push_back({"-", TokenType::Minus, line}); break;
      case '/':
        switch(input[i + 1]) {
          case '/':
            while(input[i + 1] != '\n') i++;
            break;
          case ':':
            while(input[i + 1] != ':' && input[i + 2] != '/') i += 2;
            break;
          default:
            tokens.push_back({"/", TokenType::ForwardSlash, line});
            break;
        };
        break;
      case '*': tokens.push_back({"*", TokenType::Asterisk, line}); break;
    };
  }
  return tokens;
}
/*
for(char c : input)
  const std::string newLexeme{lexeme + c};
  if(isspace(c)) {
    if(!lexeme.empty()) addToken(tokens, lexeme, "", lineNumber);
    if(c == '\n') lineNumber++;
    continue;
  }
  if(getTokenTypeMatches(newLexeme).size())
    lexeme = newLexeme;
  else
    addToken(tokens, lexeme, std::string(1, c), lineNumber);
}
if(!lexeme.empty()) addToken(tokens, lexeme, lexeme, lineNumber);
Scanner::printTokens(tokens) {
  return tokens;
}*/

void Scanner::printTokens(const std::vector<Token> &tokens) {
  std::cout << "TOKENS:\n";
  for(const Token &token : tokens)
    std::cout << "\t" << token.type << ", " << token.lexeme << '\n';
}

std::vector<TokenType> Scanner::getTokenTypeMatches(const std::string &lexeme) {
  std::vector<TokenType> tokenTypeMatches{};
  for(TokenRule rule : rules) {
    if(std::regex_match(lexeme, rule.first))
      tokenTypeMatches.push_back(rule.second);
  }
  return tokenTypeMatches;
}

void Scanner::addToken(std::vector<Token> &tokens,
                       std::string &lexeme,
                       const std::string &newLexeme,
                       int lineNumber) {
  const TokenType type{getTokenTypeMatches(lexeme)[0]};
  tokens.push_back({lexeme, type, lineNumber});
  lexeme = newLexeme;
}