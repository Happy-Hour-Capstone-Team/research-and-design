#include "interpreter.hpp"

Interpreter::Interpreter() {
  global = std::make_shared<Environment>();
  using namespace std::placeholders;
  global->define(Token{"doNothing", Token::Type::Identifier},
                 Callable{0, 0, std::bind(native::doNothing, _1, _2), global});
  global->define(Token{"print", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::print, _1, _2), global});
  global->define(Token{"input", Token::Type::Identifier},
                 Callable{0, 1, std::bind(native::input, _1, _2), global});
  global->define(Token{"time", Token::Type::Identifier},
                 Callable{0, 0, std::bind(native::time, _1, _2), global});

  global->define(Token{"min", Token::Type::Identifier},
                 Callable{2, 2, std::bind(native::min, _1, _2), global});
  global->define(Token{"max", Token::Type::Identifier},
                 Callable{2, 2, std::bind(native::max, _1, _2), global});
  global->define(Token{"abs", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::abs, _1, _2), global});
  global->define(Token{"round", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::round, _1, _2), global});
  global->define(Token{"floor", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::floor, _1, _2), global});
  global->define(Token{"ceil", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::ceil, _1, _2), global});
  global->define(Token{"truncate", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::truncate, _1, _2), global});

  global->define(Token{"pow", Token::Type::Identifier},
                 Callable{2, 2, std::bind(native::pow, _1, _2), global});
  global->define(Token{"exp", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::exp, _1, _2), global});
  global->define(Token{"sqrt", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::sqrt, _1, _2), global});
  global->define(Token{"cbrt", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::cbrt, _1, _2), global});
  global->define(Token{"hypotenuse", Token::Type::Identifier},
                 Callable{2, 3, std::bind(native::hypotenuse, _1, _2), global});
  global->define(Token{"log", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::log, _1, _2), global});
  global->define(Token{"lg", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::lg, _1, _2), global});
  global->define(Token{"ln", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::ln, _1, _2), global});

  global->define(Token{"sin", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::sin, _1, _2), global});
  global->define(Token{"cos", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::cos, _1, _2), global});
  global->define(Token{"tan", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::tan, _1, _2), global});
  global->define(Token{"sinh", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::sinh, _1, _2), global});
  global->define(Token{"cosh", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::cosh, _1, _2), global});
  global->define(Token{"tanh", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::tanh, _1, _2), global});
  global->define(Token{"arcsin", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::arcsin, _1, _2), global});
  global->define(Token{"arccos", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::arccos, _1, _2), global});
  global->define(Token{"arctan", Token::Type::Identifier},
                 Callable{1, 2, std::bind(native::arctan, _1, _2), global});
  global->define(Token{"arcsinh", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::arcsinh, _1, _2), global});
  global->define(Token{"arccosh", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::arccosh, _1, _2), global});
  global->define(Token{"arctanh", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::arctanh, _1, _2), global});

  global->define(Token{"isnan", Token::Type::Identifier},
                 Callable{1, 1, std::bind(native::isnan, _1, _2), global});

  global->define(Token{"PI", Token::Type::Identifier}, native::PI);
  global->define(Token{"E_V", Token::Type::Identifier}, native::E_V);
  global->define(Token{"MIN_VALUE", Token::Type::Identifier},
                 native::MIN_VALUE);
  global->define(Token{"MAX_VALUE", Token::Type::Identifier},
                 native::MAX_VALUE);
  global->define(Token{"NaN", Token::Type::Identifier}, native::NaN);
}

std::optional<std::any> Interpreter::visit(const Expression::Literal &literal,
                                           Environment *env) {
  return literal.value;
}

std::optional<std::any> Interpreter::visit(const Expression::Unary &unary,
                                           Environment *env) {
  std::any rightVal = evaluate(unary.right.get(), env);
  switch(unary.op.type) {
    case Token::Type::Exclamation: return !std::any_cast<bool>(rightVal);
    case Token::Type::Dash: return -std::any_cast<long double>(rightVal);
    default: throw std::runtime_error("Not a supported unary operator");
  }
}

std::optional<std::any> Interpreter::visit(const Expression::Binary &binary,
                                           Environment *env) {
  std::any leftVal = evaluate(binary.left.get(), env);
  std::any rightVal = evaluate(binary.right.get(), env);
  try {
    if(leftVal.type() == typeid(std::string))
      return stringOperation(std::any_cast<std::string>(leftVal),
                             binary.op.type,
                             std::any_cast<std::string>(rightVal));
    else if(leftVal.type() == typeid(bool))
      return booleanOperation(std::any_cast<bool>(leftVal),
                              binary.op.type,
                              std::any_cast<bool>(rightVal));
    else
      return numericOperation(std::any_cast<long double>(leftVal),
                              binary.op.type,
                              std::any_cast<long double>(rightVal));
  } catch(std::bad_any_cast) {
    throw std::runtime_error("Type mismatch between operator!");
  }
}

std::optional<std::any> Interpreter::visit(const Expression::Group &group,
                                           Environment *env) {
  return evaluate(group.expr.get(), env);
}

std::optional<std::any> Interpreter::visit(const Expression::Ternary &ternary,
                                           Environment *env) {
  if(isTrue(evaluate(ternary.condition.get(), env)))
    return evaluate(ternary.thenExpr.get(), env);
  return evaluate(ternary.elseExpr.get(), env);
}

std::optional<std::any> Interpreter::visit(const Expression::Variable &variable,
                                           Environment *env) {
  return env->get(variable.variable);
}

std::optional<std::any>
    Interpreter::visit(const Expression::Assignment &assignment,
                       Environment *env) {
  std::any value = evaluate(assignment.value.get(), env);
  env->assign(assignment.variable, value);
  return value;
}

std::optional<std::any> Interpreter::visit(const Expression::Call &call,
                                           Environment *env) {
  std::any callee{evaluate(call.callee.get(), env)};
  std::vector<std::any> args{};
  for(std::size_t i{0}; i < call.args.size(); i++)
    args.push_back(evaluate(call.args[i].get(), env));
  try {
    Callable callable{std::any_cast<Callable>(callee)};
    if(args.size() < callable.minArity || args.size() > callable.maxArity)
      throw std::runtime_error{
          "Method expected at least " + std::to_string(callable.minArity) +
          " arguments, at most " + std::to_string(callable.maxArity) +
          " arguments, and received " + std::to_string(args.size()) +
          " arguments."};
    return callable.procedure(args, callable.fnEnv.get());
  } catch(std::bad_any_cast) {
    try {
      Prototypable prototype{std::any_cast<Prototypable>(callee)};
      Prototypable newPrototype{prototype.copy()};
      if(args.size() < newPrototype.constructor.minArity ||
         args.size() > newPrototype.constructor.maxArity)
        throw std::runtime_error{
            "Constructor expected at least " +
            std::to_string(newPrototype.constructor.minArity) +
            " arguments, at most " +
            std::to_string(newPrototype.constructor.maxArity) +
            " arguments, and received " + std::to_string(args.size()) +
            " arguments."};
      newPrototype.constructor.procedure(args, newPrototype.methodEnv.get());
      return newPrototype;
    } catch(std::bad_any_cast) {
      throw std::runtime_error{"Only functions and prototypes may be called."};
    }
  }
}

std::optional<std::any> Interpreter::visit(const Expression::Lambda &lambda,
                                           Environment *env) {
  Procedure lambdaFn = [&lambda, this](const std::vector<std::any> &args,
                                       Environment *fnEnv) {
    std::unique_ptr<Environment> scopedEnv{
        std::make_unique<Environment>(fnEnv)};
    for(std::size_t i{0};
        i < lambda.params.size() + lambda.defaultParams.size();
        i++) {
      if(i < lambda.params.size())
        scopedEnv->define(lambda.params[i], args[i]);
      else if(i < args.size())
        scopedEnv->define(lambda.defaultParams[i - lambda.params.size()].first,
                          args[i]);
      else
        scopedEnv->define(
            lambda.defaultParams[i - lambda.params.size()].first,
            evaluate(
                lambda.defaultParams[i - lambda.params.size()].second.get(),
                scopedEnv.get()));
    }
    try {
      execute(lambda.body.get(), scopedEnv.get());
    } catch(std::optional<std::any> value) {
      return value;
    }
    return std::make_optional<std::any>({});
  };
  return Callable{lambda.params.size(),
                  lambda.params.size() + lambda.defaultParams.size(),
                  lambdaFn,
                  std::make_shared<Environment>(env, true)};
}

std::optional<std::any>
    Interpreter::visit(const Expression::Prototype &prototype,
                       Environment *env) {
  std::shared_ptr<Environment> surroundingEnv{
      std::make_shared<Environment>(env, true)};
  std::shared_ptr<Environment> publicEnv{std::make_shared<Environment>()};
  std::shared_ptr<Environment> privateEnv{std::make_shared<Environment>()};
  if(prototype.parent) {
    try {
      std::any parent{env->get(prototype.parent.value())};
      Prototypable parentPrototype{std::any_cast<Prototypable>(parent)};
      privateEnv->copyOver(parentPrototype.privateEnv.get());
      publicEnv->copyOver(parentPrototype.publicEnv.get());
      surroundingEnv->define(Token{"parent", Token::Type::Identifier, true},
                             parentPrototype);
    } catch(...) {
      throw std::runtime_error{"Can only inherit from other prototypes."};
    }
  }
  for(std::size_t i{0}; i < prototype.publicProperties.size(); i++)
    execute(prototype.publicProperties[i].get(), publicEnv.get());
  for(std::size_t i{0}; i < prototype.privateProperties.size(); i++)
    execute(prototype.privateProperties[i].get(), privateEnv.get());
  Callable constructor{0, 0, native::doNothing, surroundingEnv};
  Prototypable anonymousPrototype{
      constructor, surroundingEnv, publicEnv, privateEnv, nullptr};
  anonymousPrototype.methodEnv = Environment::unionize(
      {surroundingEnv.get(), publicEnv.get(), privateEnv.get()});
  if(prototype.constructor)
    anonymousPrototype.constructor = std::any_cast<Callable>(evaluate(
        prototype.constructor.get(), anonymousPrototype.methodEnv.get()));
  anonymousPrototype.methodEnv->define(
      Token{"this", Token::Type::Identifier, true}, anonymousPrototype);
  return anonymousPrototype;
}

std::optional<std::any> Interpreter::visit(const Expression::Set &set,
                                           Environment *env) {
  std::any object{evaluate(set.object.get(), env)};
  try {
    Prototypable prototype{std::any_cast<Prototypable>(object)};
    try {
      prototype.publicEnv->assign(set.property, evaluate(set.value.get(), env));
    } catch(std::runtime_error) {
      try {
        prototype.privateEnv->get(set.property);
        throw std::runtime_error{"Requested property is private."};
      } catch(std::runtime_error) {
        throw std::runtime_error{"Property not found in prototype."};
      }
    }
  } catch(std::bad_any_cast) {
    throw std::runtime_error{"Can only set properties of prototypes."};
  }
  return {};
}

std::optional<std::any> Interpreter::visit(const Expression::Get &get,
                                           Environment *env) {
  std::any object{evaluate(get.object.get(), env)};
  std::any value;
  try {
    Prototypable prototype{std::any_cast<Prototypable>(object)};
    try {
      value = prototype.publicEnv->get(get.property);
      try {
        Callable callable{std::any_cast<Callable>(value)};
        callable.fnEnv = prototype.methodEnv;
        return callable;
      } catch(std::bad_any_cast) {
        return value;
      }
    } catch(std::runtime_error) {
      try {
        throw prototype.privateEnv->get(get.property);
      } catch(std::runtime_error) {
        throw std::runtime_error{"Property not found in prototype."};
      } catch(std::any) {
        throw std::runtime_error{"Requested property is private."};
      }
    }
  } catch(std::bad_any_cast) {
    throw std::runtime_error{"Can only receive properties from prototypes."};
  }
}

void Interpreter::visit(const Statement::Expression &expr, Environment *env) {
  optEvaluate(expr.expr.get(), env);
}

void Interpreter::visit(const Statement::Variable &variable, Environment *env) {
  std::any value;
  if(variable.initializer) value = evaluate(variable.initializer.get(), env);
  // Necessary so functions can call themselves recursively.
  if(value.type() == typeid(Callable))
    std::any_cast<Callable>(value).fnEnv->define(variable.variable, value);
  env->define(variable.variable, value);
}

void Interpreter::visit(const Statement::Scope &scope, Environment *env) {
  std::unique_ptr<Environment> scopedEnv{std::make_unique<Environment>(env)};
  for(std::size_t i{0}; i < scope.statements.size(); i++)
    execute(scope.statements[i].get(), scopedEnv.get());
}

void Interpreter::visit(const Statement::If &ifStmt, Environment *env) {
  if(isTrue(evaluate(ifStmt.condition.get(), env)))
    execute(ifStmt.thenStmt.get(), env);
  else if(ifStmt.elseStmt)
    execute(ifStmt.elseStmt.get(), env);
}

void Interpreter::visit(const Statement::For &forStmt, Environment *env) {
  std::unique_ptr<Environment> forEnv{std::make_unique<Environment>(env)};
  if(forStmt.initializer) execute(forStmt.initializer.get(), forEnv.get());
  while(isTrue(evaluate(forStmt.condition.get(), forEnv.get()))) {
    if(forStmt.body) execute(forStmt.body.get(), forEnv.get());
    if(forStmt.update) execute(forStmt.update.get(), forEnv.get());
  }
}

void Interpreter::visit(const Statement::Return &returnStmt, Environment *env) {
  if(returnStmt.expr) throw optEvaluate(returnStmt.expr.get(), env);
  throw std::make_optional<std::any>({});
}

void Interpreter::interpret(
    const std::vector<Statement::StatementUPtr> &statements) {
  try {
    for(std::size_t i{0}; i < statements.size(); i++)
      execute(statements[i].get(), global.get());
  } catch(std::runtime_error e) {
    std::cout << e.what() << '\n'; // Add proper error logging here later...
  }
}

std::any Interpreter::evaluate(Expression::Expression *expr, Environment *env) {
  std::optional<std::any> optValue{expr->accept(this, env)};
  if(!optValue.has_value())
    throw std::runtime_error{"Expected a non-null value!"};
  return optValue.value();
}

std::optional<std::any> Interpreter::optEvaluate(Expression::Expression *expr,
                                                 Environment *env) {
  return expr->accept(this, env);
}

void Interpreter::execute(Statement::Statement *statement, Environment *env) {
  statement->accept(this, env);
}

bool Interpreter::isTrue(const std::any &value) {
  bool truth{false};
  if(value.type() == typeid(bool) && std::any_cast<bool>(value))
    truth = true;
  else if(value.type() == typeid(long double) &&
          std::any_cast<long double>(value) != 0.0)
    truth = true;
  else if(value.type() == typeid(std::string) &&
          std::any_cast<std::string>(value) != "")
    truth = true;
  return truth;
}

std::any Interpreter::stringOperation(const std::string &left,
                                      const Token::Type op,
                                      const std::string &right) {
  switch(op) {
    case Token::Type::Plus: return left + right;
    case Token::Type::EqualTo: return left == right;
    case Token::Type::NotEqualTo: return left != right;
    case Token::Type::LessThan: return left < right;
    case Token::Type::GreaterThan: return left > right;
    case Token::Type::LessThanOrEqualTo: return left <= right;
    case Token::Type::GreaterThanOrEqualTo: return left >= right;
    default: throw std::runtime_error("Not a supported string operator.");
  }
}

std::any Interpreter::booleanOperation(const bool left,
                                       const Token::Type op,
                                       const bool right) {
  switch(op) {
    case Token::Type::And: return left && right;
    case Token::Type::Or: return left || right;
    case Token::Type::EqualTo: return left == right;
    case Token::Type::NotEqualTo: return left != right;
    default: throw std::runtime_error("Not a supported boolean operator.");
  }
}

std::any Interpreter::numericOperation(const long double left,
                                       const Token::Type op,
                                       const long double right) {
  switch(op) {
    case Token::Type::NotEqualTo: return left != right;
    case Token::Type::EqualTo: return left == right;
    case Token::Type::LessThan: return left < right;
    case Token::Type::LessThanOrEqualTo: return left <= right;
    case Token::Type::GreaterThan: return left > right;
    case Token::Type::GreaterThanOrEqualTo: return left >= right;
    case Token::Type::Asterisk: return left * right;
    case Token::Type::Plus: return left + right;
    case Token::Type::Dash: return left - right;
    case Token::Type::ForwardSlash:
      if(right == 0) throw std::runtime_error{"Attempted to divide by zero!"};
      return left / right;
    case Token::Type::Modulus:
      if(right == 0)
        throw std::runtime_error{
            "Attempted to take remainder of division by zero!"};
      return static_cast<long double>(fmod(left, right));
    default: throw std::runtime_error("Not a supported binary operator.");
  }
}