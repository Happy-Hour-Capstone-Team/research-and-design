#pragma once

#include "environment.hpp"
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>

using Procedure =
    std::function<std::optional<std::any>(const std::vector<std::any> &args,
                                          Environment *fnEnv)>;

/**
 * @brief The structure for callable objects in Wick. Has a minimum and maximum
 * arity (the number of allowed parameters) as well as the environment at the
 * point it was defined.
 *
 */
struct Callable {
  std::size_t minArity;
  std::size_t maxArity;
  Procedure procedure;
  std::shared_ptr<Environment> fnEnv;
};

/**
 * @brief The structure of the prototypes in Wick. Contains the appropriate
 * environments and constructor.
 *
 */
struct Prototypable {
  Callable constructor;
  std::shared_ptr<Environment> surroundingEnv;
  std::shared_ptr<Environment> publicEnv;
  std::shared_ptr<Environment> privateEnv;
  std::shared_ptr<Environment> methodEnv;
  Prototypable copy();
};

namespace native {
/**
 * @brief Does nothing. Used mostly as a helper internally, could also be used
 * to represent unimplemented code.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> doNothing(const std::vector<std::any> &args,
                                  Environment *fnEnv);

/**
 * @brief Prints a message.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> print(const std::vector<std::any> &args,
                              Environment *fnEnv);

/**
 * @brief Gets input from the terminal and prints out a message if one is
 * provided.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> input(const std::vector<std::any> &args,
                              Environment *fnEnv);

/**
 * @brief Prints out the current time based on the C epoch.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> time(const std::vector<std::any> &args,
                             Environment *fnEnv);

/**
 * @brief Gets the minimum of two values.
 *
 * @param args
 * @param fnEnv
 * @return std::any
 */
std::any min(const std::vector<std::any> &args, Environment *fnEnv);

/**
 * @brief Gets the maximum of two values.
 *
 * @param args
 * @param fnEnv
 * @return std::any
 */
std::any max(const std::vector<std::any> &args, Environment *fnEnv);

/**
 * @brief Gets the absolute value of a number.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> abs(const std::vector<std::any> &args,
                            Environment *fnEnv);

/**
 * @brief Rounds a number to the closest integer.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> round(const std::vector<std::any> &args,
                              Environment *fnEnv);

/**
 * @brief Rounds a number down.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> floor(const std::vector<std::any> &args,
                              Environment *fnEnv);

/**
 * @brief Rounds a number up.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> ceil(const std::vector<std::any> &args,
                             Environment *fnEnv);

/**
 * @brief Rounds a number toward zero.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> truncate(const std::vector<std::any> &args,
                                 Environment *fnEnv);

/**
 * @brief Raises a number to an exponent.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> pow(const std::vector<std::any> &args,
                            Environment *fnEnv);

/**
 * @brief Raises a number to the power of e.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> exp(const std::vector<std::any> &args,
                            Environment *fnEnv);

/**
 * @brief Takes the square root of a number.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> sqrt(const std::vector<std::any> &args,
                             Environment *fnEnv);

/**
 * @brief Takes the cube root of a number.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> cbrt(const std::vector<std::any> &args,
                             Environment *fnEnv);

/**
 * @brief Gets the hypotenuse of two sides of a right triangle.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> hypotenuse(const std::vector<std::any> &args,
                                   Environment *fnEnv);

/**
 * @brief Gets the base 10 log of a number.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> log(const std::vector<std::any> &args,
                            Environment *fnEnv);

/**
 * @brief Gets the base 2 log of a number.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> lg(const std::vector<std::any> &args,
                           Environment *fnEnv);

/**
 * @brief Gets the natural log of a number.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> ln(const std::vector<std::any> &args,
                           Environment *fnEnv);

/**
 * @brief Gets the sine.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> sin(const std::vector<std::any> &args,
                            Environment *fnEnv);

/**
 * @brief Gets the cosine.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> cos(const std::vector<std::any> &args,
                            Environment *fnEnv);

/**
 * @brief Gets the tangent.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> tan(const std::vector<std::any> &args,
                            Environment *fnEnv);

/**
 * @brief Gets the hyperbolic sine.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> sinh(const std::vector<std::any> &args,
                             Environment *fnEnv);

/**
 * @brief Gets the hyperbolic cosine.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> cosh(const std::vector<std::any> &args,
                             Environment *fnEnv);

/**
 * @brief Gets the hyperbolic tangent.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> tanh(const std::vector<std::any> &args,
                             Environment *fnEnv);

/**
 * @brief Gets the inverse sine.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> arcsin(const std::vector<std::any> &args,
                               Environment *fnEnv);

/**
 * @brief Gets the inverse cosine.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> arccos(const std::vector<std::any> &args,
                               Environment *fnEnv);

/**
 * @brief Gets the inverse tangent.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> arctan(const std::vector<std::any> &args,
                               Environment *fnEnv);

/**
 * @brief Gets the inverse hyperbolic sine.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> arcsinh(const std::vector<std::any> &args,
                                Environment *fnEnv);

/**
 * @brief Gets the inverse hyperbolic cosine.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> arccosh(const std::vector<std::any> &args,
                                Environment *fnEnv);

/**
 * @brief Gets the inverse hyperbolic tangent.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> arctanh(const std::vector<std::any> &args,
                                Environment *fnEnv);

/**
 * @brief Determines if the value is not-a-number.
 *
 * @param args
 * @param fnEnv
 * @return std::optional<std::any>
 */
std::optional<std::any> isnan(const std::vector<std::any> &args,
                              Environment *fnEnv);

// Provided native value for PI.
constexpr long double PI{M_PI};

// Provided native value for e.
constexpr long double E_V{M_E};

// Minimum supported value for a number.
constexpr long double MIN_VALUE{std::numeric_limits<long double>::min()};

// Maximum supported value for a number.
constexpr long double MAX_VALUE{std::numeric_limits<long double>::max()};

// Value representing not-a-number.
constexpr long double NaN{std::numeric_limits<long double>::quiet_NaN()};
} // namespace native