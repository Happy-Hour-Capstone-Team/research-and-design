#include "parser.hpp"

Parser::Parser(const Tokens &iTokens, ErrorReporter *const iErrorReporter) :
    tokens{iTokens}, errorReporter{iErrorReporter} {}

Parser::Parser(std::initializer_list<Token> iTokens,
               ErrorReporter *const iErrorReporter) :
    tokens{iTokens}, errorReporter{iErrorReporter} {}

std::vector<Statement::StatementUPtr> Parser::parse() {
  std::vector<Statement::StatementUPtr> statements{};
  while(pos != tokens.size()) statements.push_back(declaration());
  return statements;
}

Parser::ParserException::ParserException() :
    std::runtime_error{"Internal parser exception."} {}

void Parser::synchronize() {
  advance();
  while(pos < tokens.size()) {
    if(tokens[pos - 1].type == Token::Type::Semicolon) return;
    switch(tokens[pos].type) {
      case Token::Type::Subroutine:
      case Token::Type::Variable:
      case Token::Type::If:
      case Token::Type::While: return;
    }
    advance();
  }
}

Statement::StatementUPtr Parser::declaration(bool allowStatements) {
  try {
    if(match({Token::Type::Subroutine})) return functionDeclaration();
    if(match({Token::Type::Prototype})) return prototypeDeclaration();
    if(match({Token::Type::Variable})) return variableDeclaration(false);
    if(match({Token::Type::Constant})) return variableDeclaration(true);
    if(allowStatements)
      return statement();
    else
      error(tokens[pos], "Statement not allowed here.");
  } catch(ParserException e) {
    synchronize();
    return nullptr;
  }
}

Statement::StatementUPtr Parser::functionDeclaration() {
  Token name{expect(Token::Type::Identifier, "Expected a function name.")};
  name.constant = false;
  Expression::ExpressionUPtr definition{lambda()};
  return std::make_unique<Statement::Variable>(name, std::move(definition));
}

Statement::StatementUPtr Parser::prototypeDeclaration() {
  const Token name{
      expect(Token::Type::Identifier, "Expected a prototype name.")};
  Expression::ExpressionUPtr definition{anonymousPrototype()};
  return std::make_unique<Statement::Variable>(name, std::move(definition));
}

Statement::StatementUPtr Parser::variableDeclaration(const bool constant) {
  Token variable{expect(Token::Type::Identifier, "Expected a variable name.")};
  variable.constant = constant;
  Expression::ExpressionUPtr variableInitializer{nullptr};
  if(match({Token::Type::Equal})) variableInitializer = expression();
  expect(Token::Type::Semicolon, "Expected a ';' after variable declaration.");
  return std::make_unique<Statement::Variable>(variable,
                                               std::move(variableInitializer));
}

Statement::StatementUPtr Parser::constantDeclaration() {
  const Token constant{
      expect(Token::Type::Identifier, "Expected a constant name.")};
  expect(Token::Type::Equal, "Expected an initializer for constant value.");
  Expression::ExpressionUPtr constantInitializer{expression()};
  expect(Token::Type::Semicolon, "Expected a ';' after constant declaration.");
  return std::make_unique<Statement::Variable>(constant,
                                               std::move(constantInitializer));
}

Statement::StatementUPtr Parser::statement() {
  if(match({Token::Type::For})) return forStmt();
  if(match({Token::Type::While})) return whileStmt();
  if(match({Token::Type::If})) return ifStmt();
  if(match({Token::Type::LeftCurly})) return scope();
  if(match({Token::Type::Return})) return returnStmt();
  return expressionStatement();
}

Statement::StatementUPtr Parser::forStmt() {
  Statement::StatementUPtr initializer{variableDeclaration(false)};
  Expression::ExpressionUPtr condition{expression()};
  expect(Token::Type::Semicolon, "Expected a ';' after variable declaration.");
  Statement::StatementUPtr update{expressionStatement(false)};
  expect(Token::Type::LeftCurly, "Expected a '{' after for statement.");
  Statement::StatementUPtr body{scope()};
  return std::make_unique<Statement::For>(std::move(initializer),
                                          std::move(condition),
                                          std::move(body),
                                          std::move(update));
}

Statement::StatementUPtr Parser::whileStmt() {
  Expression::ExpressionUPtr condition{expression()};
  expect(Token::Type::LeftCurly, "Expected a '{' after if statement.");
  Statement::StatementUPtr body{scope()};
  return std::make_unique<Statement::For>(
      nullptr, std::move(condition), std::move(body), nullptr);
}

Statement::StatementUPtr Parser::ifStmt() {
  Expression::ExpressionUPtr condition{expression()};
  expect(Token::Type::LeftCurly, "Expected a '{' after if statement.");
  Statement::StatementUPtr thenStmt{scope()};
  Statement::StatementUPtr elseStmt{nullptr};
  if(match({Token::Type::Else})) {
    if(match({Token::Type::If}))
      elseStmt = ifStmt();
    else {
      expect(Token::Type::LeftCurly, "Expected a '{' after else statement.");
      elseStmt = scope();
    }
  }
  return std::make_unique<Statement::If>(
      std::move(condition), std::move(thenStmt), std::move(elseStmt));
}

Statement::StatementUPtr Parser::scope() {
  std::vector<Statement::StatementUPtr> statements{};
  while(!check(Token::Type::RightCurly)) statements.push_back(declaration());
  expect(Token::Type::RightCurly, "Expected a '}' after scope.");
  return std::make_unique<Statement::Scope>(std::move(statements));
}

Statement::StatementUPtr Parser::returnStmt() {
  const Token keyword{tokens[pos - 1]};
  Expression::ExpressionUPtr expr{nullptr};
  if(!check({Token::Type::Semicolon})) expr = expression();
  expect(Token::Type::Semicolon, "Expected a ';' after statement.");
  return std::make_unique<Statement::Return>(keyword, std::move(expr));
}

Statement::StatementUPtr Parser::expressionStatement(bool expectSemicolon) {
  Expression::ExpressionUPtr expr = expression();
  if(expectSemicolon)
    expect(Token::Type::Semicolon, "Expected a ';' after statement.");
  return std::make_unique<Statement::Expression>(std::move(expr));
}

Expression::ExpressionUPtr Parser::expression() {
  Expression::ExpressionUPtr thenExpr{simpleExpression()};
  if(match({Token::Type::If})) {
    Expression::ExpressionUPtr condition{expression()};
    expect(Token::Type::Else, "Expected an \"else\" after ternary condition.");
    Expression::ExpressionUPtr elseExpr{expression()};
    return std::make_unique<Expression::Ternary>(
        std::move(thenExpr), std::move(condition), std::move(elseExpr));
  }
  return std::move(thenExpr);
}

Expression::ExpressionUPtr Parser::simpleExpression() {
  if(match({Token::Type::Lambda})) return lambda();
  if(match({Token::Type::Prototype})) return anonymousPrototype();
  return assignment();
}

Expression::ExpressionUPtr Parser::lambda() {
  expect(Token::Type::LeftParen, "Expected a '(' before parameters.");
  std::vector<Token> params;
  std::vector<std::pair<Token, Expression::ExpressionUPtr>> defaultParams;
  if(!check(Token::Type::RightParen)) {
    bool nowDefaultParams{false};
    do {
      const Token identifier{expect(Token::Type::Identifier,
                                    "Expected an identifier for parameter.")};
      if(match({Token::Type::Equal})) {
        nowDefaultParams = true;
        defaultParams.push_back(std::make_pair(identifier, expression()));
      } else if(!nowDefaultParams)
        params.push_back(identifier);
      else
        error(identifier, "Non-default parameters must come first.");
    } while(match({Token::Type::Comma}));
  }
  expect(Token::Type::RightParen, "Expected a ')' after parameters.");
  expect(Token::Type::LeftCurly, "Expected a '{' before statements.");
  Statement::StatementUPtr body{scope()};
  return std::make_unique<Expression::Lambda>(
      params, std::move(defaultParams), std::move(body));
}

Expression::ExpressionUPtr Parser::anonymousPrototype() {
  std::optional<Token> parent;
  if(match({Token::Type::From}))
    parent = expect(Token::Type::Identifier,
                    "Expected a prototype to inherit from.");
  expect(Token::Type::LeftCurly, "Expected a '{' after prototype declaration!");
  Expression::ExpressionUPtr constructor;
  if(match({Token::Type::Identifier})) {
    if(tokens[pos - 1].lexeme != "constructor")
      error(tokens[pos - 1], "Constructor must be named \"constructor\".");
    else
      constructor = lambda();
  }
  std::vector<Statement::StatementUPtr> publicProperties{};
  if(match({Token::Type::Public})) {
    expect(Token::Type::Colon, "Expected a ':' after \"public\".");
    while(!check(Token::Type::Private) && !check(Token::Type::RightCurly)) {
      publicProperties.push_back(declaration(false));
    }
  }

  std::vector<Statement::StatementUPtr> privateProperties{};
  if(match({Token::Type::Private})) {
    expect(Token::Type::Colon, "Expected a ':' after \"private\".");
    while(!check(Token::Type::RightCurly))
      privateProperties.push_back(declaration(false));
  }
  expect(Token::Type::RightCurly, "Expected a '}' after prototype definition.");
  return std::make_unique<Expression::Prototype>(std::move(constructor),
                                                 parent,
                                                 std::move(publicProperties),
                                                 std::move(privateProperties));
}

Expression::ExpressionUPtr Parser::assignment() {
  Expression::ExpressionUPtr expr = equality();
  if(match({Token::Type::Equal})) {
    const Token equal{tokens[pos - 1]};
    Expression::ExpressionUPtr value{assignment()};
    try {
      const Token variable{
          static_cast<Expression::Variable *>(expr.get())->variable};
      return std::make_unique<Expression::Assignment>(variable,
                                                      std::move(value));
    } catch(std::bad_cast) {
      try {
        Expression::Get *get{static_cast<Expression::Get *>(expr.get())};
        return std::make_unique<Expression::Set>(
            std::move(get->object), get->property, std::move(value));
      } catch(std::bad_cast) {
        error(equal, "Can not assign to this token.");
      }
    }
  }
  return std::move(expr);
}

Expression::ExpressionUPtr Parser::equality() {
  Expression::ExpressionUPtr left{andExpr()};
  while(match({Token::Type::NotEqualTo, Token::Type::EqualTo})) {
    const Token op{tokens[pos - 1]};
    Expression::ExpressionUPtr right{andExpr()};
    left = std::make_unique<Expression::Binary>(
        std::move(left), op, std::move(right));
  }
  return std::move(left);
}

Expression::ExpressionUPtr Parser::andExpr() {
  Expression::ExpressionUPtr left{orExpr()};
  while(match({Token::Type::And})) {
    const Token op{tokens[pos - 1]};
    Expression::ExpressionUPtr right{orExpr()};
    left = std::make_unique<Expression::Binary>(
        std::move(left), op, std::move(right));
  }
  return std::move(left);
}

Expression::ExpressionUPtr Parser::orExpr() {
  Expression::ExpressionUPtr left{comparison()};
  while(match({Token::Type::Or})) {
    const Token op{tokens[pos - 1]};
    Expression::ExpressionUPtr right{comparison()};
    left = std::make_unique<Expression::Binary>(
        std::move(left), op, std::move(right));
  }
  return std::move(left);
}

Expression::ExpressionUPtr Parser::comparison() {
  Expression::ExpressionUPtr left{term()};
  while(match({Token::Type::LessThan,
               Token::Type::LessThanOrEqualTo,
               Token::Type::GreaterThan,
               Token::Type::GreaterThanOrEqualTo})) {
    const Token op{tokens[pos - 1]};
    Expression::ExpressionUPtr right{term()};
    left = std::make_unique<Expression::Binary>(
        std::move(left), op, std::move(right));
  }
  return std::move(left);
}

Expression::ExpressionUPtr Parser::term() {
  Expression::ExpressionUPtr left{factor()};
  while(match({Token::Type::Plus, Token::Type::Dash})) {
    const Token op{tokens[pos - 1]};
    Expression::ExpressionUPtr right{factor()};
    left = std::make_unique<Expression::Binary>(
        std::move(left), op, std::move(right));
  }
  return std::move(left);
}

Expression::ExpressionUPtr Parser::factor() {
  Expression::ExpressionUPtr left{unary()};
  while(match({Token::Type::Asterisk,
               Token::Type::ForwardSlash,
               Token::Type::Modulus})) {
    const Token op{tokens[pos - 1]};
    Expression::ExpressionUPtr right{unary()};
    left = std::make_unique<Expression::Binary>(
        std::move(left), op, std::move(right));
  }
  return std::move(left);
}

Expression::ExpressionUPtr Parser::unary() {
  if(match({Token::Type::Exclamation, Token::Type::Dash})) {
    const Token op{tokens[pos - 1]};
    Expression::ExpressionUPtr right{primary()};
    return std::make_unique<Expression::Unary>(op, std::move(right));
  }
  return call();
}

Expression::ExpressionUPtr Parser::call() {
  Expression::ExpressionUPtr expr{primary()};
  while(true) {
    if(match({Token::Type::LeftParen})) {
      std::vector<Expression::ExpressionUPtr> args{};
      if(!check({Token::Type::RightParen})) {
        do args.push_back(expression());
        while(match({Token::Type::Comma}));
      }
      const Token closingParen{expect(Token::Type::RightParen,
                                      "Expected a ')' after call arguments.")};
      expr = std::make_unique<Expression::Call>(
          std::move(expr), std::move(args), closingParen);
    } else if(match({Token::Type::Dot})) {
      const Token property{expect(Token::Type::Identifier,
                                  "Expected a property name after '.'.")};
      expr = std::make_unique<Expression::Get>(std::move(expr), property);
    } else
      break;
  }
  return std::move(expr);
}

Expression::ExpressionUPtr Parser::primary() {
  if(match({Token::Type::Boolean}))
    return std::make_unique<Expression::Literal>(
        tokens[pos - 1].lexeme == "true" ? true : false);
  if(match({Token::Type::Number}))
    return std::make_unique<Expression::Literal>(
        std::stold(tokens[pos - 1].lexeme));
  if(match({Token::Type::String}))
    return std::make_unique<Expression::Literal>(tokens[pos - 1].lexeme);
  if(match({Token::Type::Identifier}))
    return std::make_unique<Expression::Variable>(tokens[pos - 1]);
  if(match({Token::Type::LeftParen})) {
    Expression::ExpressionUPtr expr{expression()};
    expect(Token::Type::RightParen, "Expected a ')' after expression.");
    return std::move(expr);
  }
  throw error(tokens[pos - 1], "Unexpected token.");
}

bool Parser::match(const std::vector<Token::Type> &types) {
  for(Token::Type type : types) {
    if(check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

const Token &Parser::expect(const Token::Type type, const std::string &msg) {
  if(check(type)) return advance();
  throw error(tokens[pos - 1], msg);
}

bool Parser::check(const Token::Type type) {
  return pos != tokens.size() && tokens[pos] == type;
}

const Token &Parser::advance() {
  if(pos != tokens.size()) pos++;
  return tokens[pos - 1];
}

Parser::ParserException Parser::error(const Token &token,
                                      const std::string &msg) {
  if(errorReporter) errorReporter->report(token, msg);
  return ParserException{};
}