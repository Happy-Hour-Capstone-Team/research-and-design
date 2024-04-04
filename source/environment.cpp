#include "environment.hpp"

Environment::Environment(const SymbolTable &iTable) : table{iTable} {}

Environment::Environment(const Environment &iEnv) :
    outer{iEnv.outer}, table{iEnv.table} {}

Environment::Environment(Environment *iEnv, const bool persist) {
  outer = iEnv;
  if(persist) {
    outer = outer->outer;
    table = iEnv->table;
  }
}

void Environment::define(const Token &variable, const std::any &value) {
  try {
    assign(variable, value);
  } catch(std::runtime_error) {
    table = table.insert(variable, value);
  }
}

void Environment::assign(const Token &variable, const std::any &value) {
  auto entry = table.getEntry(variable);
  if(entry && entry->first.constant)
    throw std::runtime_error{"Can not assign to the constant " +
                             variable.lexeme + "!"};
  if(entry) {
    table = table.assign(variable, value).value();
    return;
  }
  if(outer) {
    outer->assign(variable, value);
    return;
  }
  throw std::runtime_error{"Undefined variable \"" + variable.lexeme + "\"!"};
}

void Environment::defineOrAssign(const Token &variable, const std::any &value) {
  try {
    assign(variable, value);
  } catch(std::runtime_error) {
    define(variable, value);
  }
}

std::any Environment::get(const Token &variable) {
  std::optional<std::any> value{table.get(variable)};
  if(!value && outer) value = outer->get(variable);
  if(value) return value.value();
  throw std::runtime_error{"Undefined variable!"};
}

void Environment::copyOver(Environment *other) {
  table = table.copyOver(other->table);
}

std::shared_ptr<Environment>
    Environment::unionize(const std::vector<Environment *> &envs) {
  std::vector<SymbolTable> tables{};
  for(Environment *env : envs) tables.push_back(env->table);
  return std::make_shared<Environment>(SymbolTable::unionize(tables));
}