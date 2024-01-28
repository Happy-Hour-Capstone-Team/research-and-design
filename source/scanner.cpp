#include "scanner.hpp"

Scanner::Scanner(const std::string &input) : text{input} {}

Tokens Scanner::tokenize() {
  tokens.clear();
  line = col = 1;
  const std::size_t length{text.size()};
  for(pos = 0; pos < length; pos++, col++) scanToken();
  return tokens;
}

void Scanner::scanToken() {
  if(shortTokens()) return;
  switch(text[pos]) {
    case '\n': newLine(); break;
    case '/': forwardSlash(); break;
    case '"': string(); break;
    default: longTokens(); break;
  };
}

bool Scanner::shortTokens() {
  switch(text[pos]) {
    case ' ':
    case '\r':
    case '\t': return true;
    case '{': addToken("{", Token::Type::LeftCurly); return true;
    case '}': addToken("}", Token::Type::RightCurly); return true;
    case ';': addToken(";", Token::Type::Semicolon); return true;
    case '(': addToken("(", Token::Type::LeftParen); return true;
    case ')': addToken(")", Token::Type::RightParen); return true;
    case '=':
      if(text[pos + 1] == '=') {
        addToken("==", Token::Type::EqualTo);
        incPosCol();
      } else
        addToken("=", Token::Type::Equal);
      return true;
    case '<':
      if(text[pos + 1] == '=') {
        addToken("<=", Token::Type::LessThanOrEqualTo);
        incPosCol();
      } else
        addToken("<", Token::Type::LessThan);
      return true;
    case '>':
      if(text[pos + 1] == '=') {
        addToken(">=", Token::Type::GreaterThanOrEqualTo);
        incPosCol();
      } else
        addToken(">", Token::Type::GreaterThan);
      return true;
    case '*': addToken("*", Token::Type::Asterisk); return true;
    case '+': addToken("+", Token::Type::Plus); return true;
    case '-': addToken("-", Token::Type::Dash); return true;
    case '!':
      if(text[pos + 1] == '=') {
        addToken("!=", Token::Type::NotEqualTo);
        incPosCol();
      } else
        addToken("!", Token::Type::Exclamation);
      return true;
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
        if(text[pos] == '\n') newLine();
        incPosCol();
      }
      incPosCol(2);
      break;
    default: addToken("/", Token::Type::ForwardSlash); break;
  };
}

void Scanner::string() {
  std::string lexeme{""};
  while(pos + 1 < text.length() && text[pos + 1] != '"') {
    lexeme += text[++pos];
  }
  pos++;
  addToken(lexeme, Token::Type::String);
  col += lexeme.length() + 1;
}

void Scanner::longTokens() {
  std::string lexeme{text[pos]};
  if(std::isdigit(text[pos]))
    number(lexeme);
  else if(std::isalpha(text[pos]))
    identifier(lexeme);
  else
    addToken(lexeme, Token::Type::Error);
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
  addToken(lexeme, Token::Type::Number);
}

void Scanner::identifier(std::string &lexeme) {
  while(std::isalnum(text[pos + 1])) {
    pos++;
    lexeme += text[pos];
  }
  if(auto search = keywords.find(lexeme); search != keywords.end())
    addToken(lexeme, search->second);
  else
    addToken(lexeme, Token::Type::Identifier);
}

void Scanner::addToken(const std::string &lexeme, Token::Type type) {
  if(type == Token::Type::Error) std::cout << "Add logging here...\n";
  tokens.push_back({lexeme, type, line, col});
}

void Scanner::newLine() {
  line++;
  col = 0;
}

void Scanner::incPosCol(int i) {
  pos += i;
  col += i;
}

void Scanner::printTokens(const Tokens &tokens) {
  std::cout << "TOKENS:\n";
  for(const Token &token : tokens) std::cout << "\t" << token << '\n';
}