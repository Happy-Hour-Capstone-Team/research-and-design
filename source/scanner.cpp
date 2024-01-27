#include "scanner.hpp"

/*
TODO:
- Read from file
- Error handle unrecognized tokens
*/

std::ostream &operator<<(std::ostream &out, const TokenType tokenType) {
  const int value{static_cast<int>(tokenType)};
  if(value >= tokenTypeNames.size()) return out << "Error";
  return out << tokenTypeNames[value];
}

std::ostream &operator<<(std::ostream &out, const Token &token) {
  out << token.type << " (" << token.lexeme << ") on line " << token.line
      << ", column " << token.col;
  return out;
}

Scanner::Scanner() {}

std::vector<Token> Scanner::tokenize(const std::string &input) {
  text = input;
  tokens.clear();
  line = col = 1;
  const std::size_t length{text.size()};
  for(pos = 0; pos < length; pos++, col++) scanToken();
  printTokens(tokens);
  return tokens;
}

void Scanner::scanToken() {
  if(singleCharacter()) return;
  switch(text[pos]) {
    case '\n':
      line++;
      col = 0;
      break;
    case '/': forwardSlash(); break;
    case '"': string(); break;
    default: longTokens(); break;
  };
}

bool Scanner::singleCharacter() {
  switch(text[pos]) {
    case ' ':
    case '\r':
    case '\t': return true;
    case '{': addToken("{", TokenType::LeftCurly); return true;
    case '}': addToken("}", TokenType::RightCurly); return true;
    case '(': addToken("(", TokenType::LeftParen); return true;
    case ')': addToken(")", TokenType::RightParen); return true;
    case '=': addToken("=", TokenType::Equals); return true;
    case ';': addToken(";", TokenType::Semicolon); return true;
    case '+': addToken("+", TokenType::Plus); return true;
    case '-': addToken("-", TokenType::Dash); return true;
    case '*': addToken("*", TokenType::Asterisk); return true;
    default: return false;
  };
}

void Scanner::forwardSlash() {
  switch(text[pos + 1]) {
    case '/':
      while(text[pos + 1] != '\n') pos++;
      break;
    case ':':
      while(pos + 2 < text.length() &&
            (text[pos + 1] != ':' || text[pos + 2] != '/')) {
        if(text[pos] == '\n') {
          line++;
          col = 0;
        }
        pos++;
        col++;
      }
      pos += 2;
      col += 2;
      break;
    default: addToken("/", TokenType::ForwardSlash); break;
  };
}

void Scanner::string() {
  std::string lexeme{""};
  while(pos + 1 < text.length() && text[pos + 1] != '"') {
    lexeme += text[++pos];
  }
  pos++;
  addToken(lexeme, TokenType::String);
  col += lexeme.length() + 1;
}

void Scanner::longTokens() {
  std::string lexeme{text[pos]};
  if(std::isdigit(text[pos]))
    number(lexeme);
  else if(std::isalpha(text[pos]))
    identifier(lexeme);
  else
    tokens.push_back({lexeme, TokenType::Error, line, col});
  col += lexeme.size() - 1;
}

void Scanner::number(std::string &lexeme) {
  while(std::isdigit(text[pos + 1])) {
    pos++;
    lexeme += text[pos];
  }
  if(text[pos + 1] == '.') {
    do {
      pos++;
      lexeme += text[pos];
    } while(std::isdigit(text[pos + 1]));
  }
  addToken(lexeme, TokenType::Number);
}

void Scanner::identifier(std::string &lexeme) {
  while(std::isalnum(text[pos + 1])) {
    pos++;
    lexeme += text[pos];
  }
  if(lexeme == "variable")
    addToken(lexeme, TokenType::Variable);
  else
    addToken(lexeme, TokenType::Identifier);
}

void Scanner::addToken(const std::string &lexeme, TokenType type) {
  tokens.push_back({lexeme, type, line, col});
}

void Scanner::printTokens(const std::vector<Token> &tokens) {
  std::cout << "TOKENS:\n";
  for(const Token &token : tokens) std::cout << "\t" << token << '\n';
}