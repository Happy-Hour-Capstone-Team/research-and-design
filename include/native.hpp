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

struct Callable {
  std::size_t minArity;
  std::size_t maxArity;
  Procedure procedure;
  std::shared_ptr<Environment> fnEnv;
};

struct Prototypable {
  Callable constructor;
  std::shared_ptr<Environment> surroundingEnv;
  std::shared_ptr<Environment> publicEnv;
  std::shared_ptr<Environment> privateEnv;
  std::shared_ptr<Environment> methodEnv;
  Prototypable copy();
};

namespace native {
std::optional<std::any> doNothing(const std::vector<std::any> &args,
                                  Environment *fnEnv);

std::optional<std::any> print(const std::vector<std::any> &args,
                              Environment *fnEnv);

std::optional<std::any> input(const std::vector<std::any> &args,
                              Environment *fnEnv);

std::optional<std::any> time(const std::vector<std::any> &args,
                             Environment *fnEnv);

std::any min(const std::vector<std::any> &args, Environment *fnEnv);

std::any max(const std::vector<std::any> &args, Environment *fnEnv);

std::optional<std::any> abs(const std::vector<std::any> &args,
                            Environment *fnEnv);

std::optional<std::any> round(const std::vector<std::any> &args,
                              Environment *fnEnv);

std::optional<std::any> floor(const std::vector<std::any> &args,
                              Environment *fnEnv);

std::optional<std::any> ceil(const std::vector<std::any> &args,
                             Environment *fnEnv);

std::optional<std::any> truncate(const std::vector<std::any> &args,
                                 Environment *fnEnv);

std::optional<std::any> pow(const std::vector<std::any> &args,
                            Environment *fnEnv);

std::optional<std::any> exp(const std::vector<std::any> &args,
                            Environment *fnEnv);

std::optional<std::any> sqrt(const std::vector<std::any> &args,
                             Environment *fnEnv);

std::optional<std::any> cbrt(const std::vector<std::any> &args,
                             Environment *fnEnv);

std::optional<std::any> hypotenuse(const std::vector<std::any> &args,
                                   Environment *fnEnv);

std::optional<std::any> log(const std::vector<std::any> &args,
                            Environment *fnEnv);

std::optional<std::any> lg(const std::vector<std::any> &args,
                           Environment *fnEnv);

std::optional<std::any> ln(const std::vector<std::any> &args,
                           Environment *fnEnv);

std::optional<std::any> sin(const std::vector<std::any> &args,
                            Environment *fnEnv);

std::optional<std::any> cos(const std::vector<std::any> &args,
                            Environment *fnEnv);

std::optional<std::any> tan(const std::vector<std::any> &args,
                            Environment *fnEnv);

std::optional<std::any> sinh(const std::vector<std::any> &args,
                             Environment *fnEnv);

std::optional<std::any> cosh(const std::vector<std::any> &args,
                             Environment *fnEnv);

std::optional<std::any> tanh(const std::vector<std::any> &args,
                             Environment *fnEnv);

std::optional<std::any> arcsin(const std::vector<std::any> &args,
                               Environment *fnEnv);

std::optional<std::any> arccos(const std::vector<std::any> &args,
                               Environment *fnEnv);

std::optional<std::any> arctan(const std::vector<std::any> &args,
                               Environment *fnEnv);

std::optional<std::any> arcsinh(const std::vector<std::any> &args,
                                Environment *fnEnv);

std::optional<std::any> arccosh(const std::vector<std::any> &args,
                                Environment *fnEnv);

std::optional<std::any> arctanh(const std::vector<std::any> &args,
                                Environment *fnEnv);
;
std::optional<std::any> isnan(const std::vector<std::any> &args,
                              Environment *fnEnv);

constexpr long double PI{M_PI};
constexpr long double E_V{M_E};
constexpr long double MIN_VALUE{std::numeric_limits<long double>::min()};
constexpr long double MAX_VALUE{std::numeric_limits<long double>::max()};
constexpr long double NaN{std::numeric_limits<long double>::quiet_NaN()};
} // namespace native