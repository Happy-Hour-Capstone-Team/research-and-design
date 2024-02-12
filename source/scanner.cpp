#include "scanner.hpp"

Scanner::Scanner(const std::string &iText,
                 ErrorReporter *const iErrorReporter) :
    text{iText}, errorReporter{iErrorReporter} {}

Tokens Scanner::tokenize() {
  tokens.clear();
  line = col = 1;
  const std::size_t length{text.size()};
  for(pos = 0; pos < length; pos++, col++) scanToken();
  return tokens;
}

void Scanner::scanToken() {
  switch(text[pos]) {
    case ' ':
    case '\r':
    case '\t': break;
    case '{': addToken("{", Token::Type::LeftCurly); break;
    case '}': addToken("}", Token::Type::RightCurly); break;
    case ';': addToken(";", Token::Type::Semicolon); break;
    case '(': addToken("(", Token::Type::LeftParen); break;
    case ')': addToken(")", Token::Type::RightParen); break;
    case '*': addToken("*", Token::Type::Asterisk); break;
    case '+': addToken("+", Token::Type::Plus); break;
    case '-': addToken("-", Token::Type::Dash); break;
    case '!':
      if(text[pos + 1] == '=')
        addToken("!=", Token::Type::NotEqualTo);
      else
        addToken("!", Token::Type::Exclamation);
      break;
    case '=':
      if(text[pos + 1] == '=')
        addToken("==", Token::Type::EqualTo);
      else
        addToken("=", Token::Type::Equal);
      break;
    case '<':
      if(text[pos + 1] == '=')
        addToken("<=", Token::Type::LessThanOrEqualTo);
      else
        addToken("<", Token::Type::LessThan);
      break;
    case '>':
      if(text[pos + 1] == '=')
        addToken(">=", Token::Type::GreaterThanOrEqualTo);
      else
        addToken(">", Token::Type::GreaterThan);
      break;
    case '\n': newLine(); break;
    case '/': forwardSlash(); break;
    case '"': string(); break;
    default: longTokens(); break;
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
  int i;
  for(i = 1; pos + i < text.length() && text[pos + i] != '"'; i++)
    lexeme += text[pos + i];
  if(pos + i >= text.length() && errorReporter)
    errorReporter->report(line, pos, "Unterminated quote.");
  addToken(lexeme, Token::Type::String);
  incPosCol(2); // Account for two skipped quotes.
}

void Scanner::longTokens() {
  if(std::isdigit(text[pos]))
    number();
  else if(std::isalpha(text[pos]))
    identifier();
  else
    addToken(std::string{text[pos]}, Token::Type::Error);
}

void Scanner::number() {
  std::string lexeme{""};
  int i;
  for(i = 0; std::isdigit(text[pos + i]); i++) lexeme += text[pos + i];
  if(text[pos + i] == '.') {
    lexeme += text[pos + i];
    for(i++; std::isdigit(text[pos + i]); i++) lexeme += text[pos + i];
  }
  addToken(lexeme, Token::Type::Number);
}

void Scanner::identifier() {
  std::string lexeme{""};
  for(int i{0}; std::isalnum(text[pos + i]); i++) lexeme += text[pos + i];
  if(auto search = keywords.find(lexeme); search != keywords.end())
    addToken(lexeme, search->second);
  else
    addToken(lexeme, Token::Type::Identifier);
}

void Scanner::addToken(const std::string &lexeme, Token::Type type) {
  if(type == Token::Type::Error && errorReporter)
    errorReporter->report({lexeme, type, line, col}, "Unrecognized token.");
  tokens.push_back({lexeme, type, line, col});
  // Subtract one to account for for-loop increment.
  incPosCol(lexeme.length() - 1);
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