#include <getopt.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <memory>

#include "earley_parser.hpp"
#include "grammar.hpp"
#include "lr_parser.hpp"
#include "parser.hpp"

class ArgumentParser {
 public:
  std::string grammar_file;
  std::string word_file;
  std::string parser_type;

  void ParseArguments(int argc, char** argv) {
    int opt;
    int option_index;

    while ((opt = getopt_long(argc, argv, "g:w:p:", kLongOptions,
                              &option_index)) != -1) {
      switch (opt) {
        case 'g':
          grammar_file = optarg;
          break;
        case 'w':
          word_file = optarg;
          break;
        case 'p':
          parser_type = optarg;
          break;
        default:
          PrintUsage(std::cerr);
          throw std::invalid_argument("");
      }
    }
    ValidateArguments(grammar_file, word_file, parser_type);
  }

 private:
  static constexpr option kLongOptions[] = {
      {"grammar-file", required_argument, NULL, 'g'},
      {"word-file", required_argument, NULL, 'w'},
      {"parser-type", required_argument, NULL, 'p'},
      {0, 0, 0, 0}};

  static void PrintUsage(std::ostream& os) {
    static const std::string kUsage =
        "Usage: run [-g <GRAMMAR_FILE>] [-p <PARSER_TYPE: earley>]";
    os << kUsage << std::endl;
  }

  static void ValidateArguments(const std::string& grammar_file,
                                const std::string& word_file,
                                const std::string& parser_type) {
    if (grammar_file.empty()) {
      PrintUsage(std::cerr);
      throw std::invalid_argument("Grammar file is not given");
    }
    if (word_file.empty()) {
      PrintUsage(std::cerr);
      throw std::invalid_argument("Word file is not given");
    }
    if (parser_type.empty()) {
      PrintUsage(std::cerr);
      throw std::invalid_argument("Parser type is not given");
    }
  }
};

int main(int argc, char* argv[]) {
  ArgumentParser argparser;
  argparser.ParseArguments(argc, argv);

  std::ifstream grammar_ifs{argparser.grammar_file};
  std::ifstream word_ifs{argparser.word_file};

  auto grammar = ReadGrammarFromFile(grammar_ifs);
  auto word = ReadWordFromFile(word_ifs);

  std::unique_ptr<Parser> parser;
  if (argparser.parser_type == "earley") {
    parser = std::make_unique<TEarleyParser>(grammar);
  } else if (argparser.parser_type == "lr1") {
    parser = std::make_unique<TLRParser>(grammar);
  } else {
    throw std::invalid_argument("Invalid parser type");
  }

  std::cout << "Is word in grammar? " << std::boolalpha << parser->Parse(word)
            << std::endl;

  return 0;
}
