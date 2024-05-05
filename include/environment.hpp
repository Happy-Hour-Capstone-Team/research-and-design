#pragma once

#include "persistentMap.hpp"
#include "token.hpp"
#include <any>

/**
 * @brief This class deals with memory environments; where variables, constants, and subroutines are 
 * associated with information. This is based off a persistent implementation of a hash table in order 
 * to save memory and deal with scope issues. 
 */
class Environment {
  public:
  /**
   * @brief The underlying data structure for environments. A persistent implementation of a hash map.
   * 
   */
  using SymbolTable = PersistentMap<Token, std::any>;

  /**
   * @brief Constructs a new environment. 
   * 
   */
  Environment() = default;

  /**
   * @brief Constructs a new environment with the given symbol table. 
   * 
   * @param iTable 
   */
  Environment(const SymbolTable &iTable);

  /**
   * @brief Constructs a new environment based on the given environments outer environment and table. 
   * 
   * @param iEnv 
   */
  Environment(const Environment &iEnv);

  /**
   * @brief Constructs a new environment with the given environment as its outer scope if persist is false;
   * copies over the surrounding environment and table otherwise (creates a persistent copy of the given environment). 
   * 
   * @param iEnv 
   * @param persist 
   */
  Environment(Environment *iEnv, const bool persist = false);

  /**
   * @brief Defines a variable in the environment with a value. Throws error if variable doesn't exist and 
   * allowAssign is false. 
   * 
   * @param variable 
   * @param value 
   */
  virtual void define(const Token &variable, const std::any &value);

  /**
   * @brief Assigns a value to a variable in the environment. Throws error if token is tagged constant.
   * 
   * @param variable 
   * @param value 
   */
  virtual void assign(const Token &variable, const std::any &value);

  /**
   * @brief Gets the value associated with the given token. Throws error if undefined. 
   * 
   * @param variable 
   * @return std::any 
   */
  virtual std::any get(const Token &variable);

  /**
   * @brief Copies the contents of another environments table into this environments table. 
   * 
   * @param other 
   */
  virtual void copyOver(Environment *other);

  /**
   * @brief Changes if the environment allows assignment when defining. 
   * 
   * @param iAllowAssign 
   */
  void defineOrAssign(const bool iAllowAssign);

  /**
   * @brief Creates a new environment that combines the symbol tables of the given environments. 
   * 
   * @param envs 
   * @return std::shared_ptr<Environment> 
   */
  static std::shared_ptr<Environment>
      unionize(const std::vector<Environment *> &envs);

  private:
  Environment *outer{nullptr};
  SymbolTable table{};
  bool allowAssign{false};
};