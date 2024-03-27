#pragma once

#include "persistentMap.hpp"
#include "token.hpp"
#include <any>

class Environment {
  public:
  using SymbolTable = PersistentMap<Token, std::any>;

  Environment() = default;

  Environment(const SymbolTable &iTable);

  Environment(const Environment &iEnv);

  Environment(Environment *iEnv, const bool persist = false);

  virtual void define(const Token &variable, const std::any &value);

  virtual void assign(const Token &variable, const std::any &value);

  virtual void defineOrAssign(const Token &variable, const std::any &value);

  virtual std::any get(const Token &variable);

  virtual void copyOver(Environment *other);

  static std::shared_ptr<Environment>
      unionize(const std::vector<Environment *> &envs);

  private:
  Environment *outer{nullptr};
  SymbolTable table{};
};