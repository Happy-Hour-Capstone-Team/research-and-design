/**
 * Thank you to Robert Nystrom for the Java examples provided in his
 * book found at https://craftinginterpreters.com/.
 *
 */

#include "main.hpp"

int main(int argc, char *argv[]) {
  std::cout << std::setprecision(20);
  if(argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <file>\n";
    return 1;
  }
  std::ifstream file{argv[1]}; // Open the file specified in the CLI.
  if(!file.is_open()) {
    std::cerr << "Error opening file: " << argv[1] << "\n";
    return 1;
  }
  std::string expression{std::istreambuf_iterator<char>(file),
                         std::istreambuf_iterator<char>()};
  const std::unique_ptr<ErrorReporter> errorReporter{
      std::make_unique<ErrorReporter>()};
  Scanner scanner{expression, errorReporter.get()};
  Parser parser{scanner.tokenize(), errorReporter.get()};
  const std::vector<Statement::StatementUPtr> statements{parser.parse()};
  if(errorReporter->hadError()) return 1;
  Interpreter interpreter{};
  interpreter.interpret(statements);
  return 0;
}