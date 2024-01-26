#include "scanner.hpp"

std::ostream &operator<<(std::ostream &out, const TokenType tokenType) {
  return out << tokenTypeNames[static_cast<int>(tokenType)];
}

Scanner::Scanner(std::initializer_list<TokenRule> rules) : rules{rules} {}

std::vector<Token> Scanner::tokenize(std::string input) {
  std::vector<Token> tokens;
  int lineNumber{1};
  std::string lexeme{""};
  for(char c : input) {
    if(isspace(c)) {
      if(!lexeme.empty()) addToken(tokens, lexeme, "", lineNumber);
      if(c == '\n') lineNumber++;
      continue;
    }
    const std::string newLexeme{lexeme + c};
    if(getTokenTypeMatches(newLexeme).size() == 0)
      addToken(tokens, lexeme, std::string{1, c}, lineNumber);
    else
      lexeme = newLexeme;
  }
  if(!lexeme.empty()) addToken(tokens, lexeme, lexeme, lineNumber);
  Scanner::printTokens(tokens);
  return tokens;
} 

void Scanner::printTokens(const std::vector<Token> &tokens) {
  std::cout << "TOKENS:\n";
  for(const Token &token : tokens)
    std::cout << "\t" << token.type << ", " << token.lexeme << '\n';
}

std::vector<TokenType> Scanner::getTokenTypeMatches(const std::string &lexeme) {
  std::vector<TokenType> tokenTypeMatches{};
  for(TokenRule rule : rules) {
    if(std::regex_match(lexeme, std::regex{rule.first}))
      tokenTypeMatches.push_back(rule.second);
  }
  return tokenTypeMatches;
}

void Scanner::addToken(std::vector<Token> &tokens,
                       std::string &lexeme,
                       const std::string &newLexeme,
                       int lineNumber) {
  tokens.push_back({lexeme, getTokenTypeMatches(lexeme)[0], lineNumber});
  lexeme = newLexeme;
}