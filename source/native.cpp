#include "native.hpp"

Prototypable Prototypable::copy() {
  Callable newConstructor = constructor;
  std::shared_ptr<Environment> newSurroundingEnv{
      std::make_shared<Environment>()};
  newSurroundingEnv->copyOver(surroundingEnv.get());
  std::shared_ptr<Environment> newPublicEnv{std::make_shared<Environment>()};
  newPublicEnv->copyOver(publicEnv.get());
  std::shared_ptr<Environment> newPrivateEnv{std::make_shared<Environment>()};
  newPrivateEnv->copyOver(privateEnv.get());
  return Prototypable{
      constructor,
      newSurroundingEnv,
      newPublicEnv,
      newPrivateEnv,
      Environment::unionize(
          {newSurroundingEnv.get(), newPublicEnv.get(), newPrivateEnv.get()})};
}

namespace native {

std::optional<std::any> doNothing(const std::vector<std::any> &args,
                                  Environment *fnEnv) {
  return {};
}

std::optional<std::any> print(const std::vector<std::any> &args,
                              Environment *fnEnv) {
  std::any toPrint{args[0]};
  if(toPrint.type() == typeid(std::string))
    std::cout << std::any_cast<std::string>(toPrint) << '\n';
  else if(toPrint.type() == typeid(bool))
    std::cout << (std::any_cast<bool>(toPrint) ? "true" : "false") << '\n';
  else if(toPrint.type() == typeid(long double))
    std::cout << std::any_cast<long double>(toPrint) << '\n';
  return {};
}

std::optional<std::any> input(const std::vector<std::any> &args,
                              Environment *fnEnv) {
  if(args.size()) print(args, fnEnv);
  std::string line;
  std::getline(std::cin, line);
  return line;
}

std::optional<std::any> time(const std::vector<std::any> &args,
                             Environment *fnEnv) {
  const std::chrono::time_point currentTime{std::chrono::system_clock::now()};
  return static_cast<long double>(
      std::chrono::system_clock::to_time_t(currentTime));
}

std::any min(const std::vector<std::any> &args, Environment *fnEnv) {
  const long double valueA{std::any_cast<long double>(args[0])};
  const long double valueB{std::any_cast<long double>(args[1])};
  return std::min(valueA, valueB);
}

std::any max(const std::vector<std::any> &args, Environment *fnEnv) {
  const long double valueA{std::any_cast<long double>(args[0])};
  const long double valueB{std::any_cast<long double>(args[1])};
  return std::max(valueA, valueB);
}

std::optional<std::any> abs(const std::vector<std::any> &args,
                            Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::abs(value);
}

std::optional<std::any> round(const std::vector<std::any> &args,
                              Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::round(value);
}

std::optional<std::any> floor(const std::vector<std::any> &args,
                              Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::floor(value);
}

std::optional<std::any> ceil(const std::vector<std::any> &args,
                             Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::ceil(value);
}

std::optional<std::any> truncate(const std::vector<std::any> &args,
                                 Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::trunc(value);
}

std::optional<std::any> pow(const std::vector<std::any> &args,
                            Environment *fnEnv) {
  const long double base{std::any_cast<long double>(args[0])};
  const long double power{std::any_cast<long double>(args[1])};
  return std::pow(base, power);
}

std::optional<std::any> exp(const std::vector<std::any> &args,
                            Environment *fnEnv) {
  const long double power{std::any_cast<long double>(args[0])};
  return std::exp(power);
}

std::optional<std::any> sqrt(const std::vector<std::any> &args,
                             Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::sqrt(value);
}

std::optional<std::any> cbrt(const std::vector<std::any> &args,
                             Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::cbrt(value);
}

std::optional<std::any> hypotenuse(const std::vector<std::any> &args,
                                   Environment *fnEnv) {
  const long double a{std::any_cast<long double>(args[0])};
  const long double b{std::any_cast<long double>(args[1])};
  if(args.size() == 2) return std::hypot(a, b);
  const long double c{std::any_cast<long double>(args[2])};
  return std::hypot(a, b, c);
}

std::optional<std::any> log(const std::vector<std::any> &args,
                            Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::log10(value);
}

std::optional<std::any> lg(const std::vector<std::any> &args,
                           Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::log2(value);
}

std::optional<std::any> ln(const std::vector<std::any> &args,
                           Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::log(value);
}

std::optional<std::any> sin(const std::vector<std::any> &args,
                            Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::sin(value);
}

std::optional<std::any> cos(const std::vector<std::any> &args,
                            Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::cos(value);
}

std::optional<std::any> tan(const std::vector<std::any> &args,
                            Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::tan(value);
}

std::optional<std::any> sinh(const std::vector<std::any> &args,
                             Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::sinh(value);
}

std::optional<std::any> cosh(const std::vector<std::any> &args,
                             Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::cosh(value);
}

std::optional<std::any> tanh(const std::vector<std::any> &args,
                             Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::tanh(value);
}

std::optional<std::any> arcsin(const std::vector<std::any> &args,
                               Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::asin(value);
}

std::optional<std::any> arccos(const std::vector<std::any> &args,
                               Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::acos(value);
}

std::optional<std::any> arctan(const std::vector<std::any> &args,
                               Environment *fnEnv) {
  const long double y{std::any_cast<long double>(args[0])};
  if(args.size() == 1) return std::atan(y);
  const long double x{std::any_cast<long double>(args[1])};
  return std::atan2(y, x);
}

std::optional<std::any> arcsinh(const std::vector<std::any> &args,
                                Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::asinh(value);
}

std::optional<std::any> arccosh(const std::vector<std::any> &args,
                                Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::acosh(value);
}

std::optional<std::any> arctanh(const std::vector<std::any> &args,
                                Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::atanh(value);
}

std::optional<std::any> isnan(const std::vector<std::any> &args,
                              Environment *fnEnv) {
  const long double value{std::any_cast<long double>(args[0])};
  return std::isnan(value);
}

} // namespace native