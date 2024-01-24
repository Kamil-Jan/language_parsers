#include "grammar.hpp"

#include <sstream>
#include <stdexcept>

#include "util.hpp"

const std::string TGrammar::kEmptyString = "";
const std::string TGrammar::kWordEnd = "$";
const std::string TGrammar::kRuleDelimiter = "->";
const std::string TGrammar::kRhsDelimiter = "|";
const std::string TGrammar::kTokensDelimiter = " ";

TGrammar::TGrammar(const std::vector<std::string>& rules_strings,
                   const std::unordered_set<std::string>& terminal_symbols,
                   const std::unordered_set<std::string>& nonterminal_symbols,
                   const std::string& start_symbol)
    : terminal_symbols_(terminal_symbols),
      nonterminal_symbols_(nonterminal_symbols),
      start_symbol_(start_symbol) {
  for (const auto& rule_str : rules_strings) {
    ParseRuleString(rule_str);
  }
}

void TGrammar::ParseRuleString(const std::string& rule_str) {
  auto splitted_string = SplitString(rule_str, kRuleDelimiter);
  if (splitted_string.size() != 2) {
    throw std::invalid_argument("Invalid rule!");
  }

  auto from_symbol = Trim(splitted_string[0]);
  if (from_symbol.empty()) {
    throw std::invalid_argument("From symbol must not be empty");
  }

  if (from_symbol[0] == kForbiddenPrefix) {
    throw std::invalid_argument("From symbol starts with forbidden prefix!");
  }

  auto rhs_strings = SplitString(splitted_string[1], kRhsDelimiter);
  std::vector<std::vector<std::string>> rhs_rules_parts;
  for (auto& rhs_str : rhs_strings) {
    Trim(rhs_str);

    rhs_rules_parts.push_back({});
    auto tokens = SplitString(rhs_str, kTokensDelimiter);
    for (auto& token : tokens) {
      Trim(token);
      if (!token.empty() && !terminal_symbols_.contains(token)) {
        if (token[0] == kForbiddenPrefix) {
          throw std::invalid_argument("Token starts with forbidden prefix!");
        }
      }
      rhs_rules_parts.back().push_back(token);
    }
  }

  auto& dest_to_merge = rules_map_[from_symbol];
  dest_to_merge.insert(dest_to_merge.end(),
                       std::make_move_iterator(rhs_rules_parts.begin()),
                       std::make_move_iterator(rhs_rules_parts.end()));
}

bool TGrammar::IsTerminal(const std::string& symbol) const {
  if (symbol.empty()) {
    return true;
  }
  return terminal_symbols_.contains(symbol);
}

const std::string& TGrammar::GetStartSymbol() const { return start_symbol_; }

const std::unordered_set<std::string>& TGrammar::GetTerminalSymbols() const {
  return terminal_symbols_;
}

const std::unordered_set<std::string>& TGrammar::GetNonTerminalSymbols() const {
  return nonterminal_symbols_;
}

const std::vector<std::vector<std::string>>& TGrammar::GetRuleRhss(
    const std::string& from_symbol) const {
  return rules_map_.at(from_symbol);
}

void TGrammar::AddRule(const std::string& from_symbol,
                       const std::vector<std::string>& rule_rhs) {
  rules_map_[from_symbol].push_back(rule_rhs);
}

const std::unordered_set<std::string>& TGrammar::TokenFirstSymbols(
    const std::string& token, std::unordered_set<std::string>& path) const {
  auto iter = first_.find(token);
  if (iter != first_.end()) {
    return iter->second;
  }

  if (token == kWordEnd) {
    first_[token] = {kWordEnd};
    return first_[token];
  }

  if (IsTerminal(token)) {
    if (token == kEmptyString) {
      first_[token] = {kWordEnd};
    } else {
      first_[token] = {token};
    }
    return first_[token];
  }

  std::unordered_set<std::string> first_symbols;
  path.insert(token);
  for (const auto& rhs : GetRuleRhss(token)) {
    for (const auto& rhs_token : rhs) {
      if (path.contains(rhs_token)) {
        break;
      }
      const auto& dfs_first_symbols = TokenFirstSymbols(rhs_token, path);
      for (const auto& first_symbol : dfs_first_symbols) {
        first_symbols.insert(first_symbol);
      }
      if (!dfs_first_symbols.contains(kWordEnd)) {
        break;
      }
    }
  }
  first_[token] = first_symbols;
  return first_[token];
}

std::unordered_set<std::string> TGrammar::First(
    const std::vector<std::string>& tokens) const {
  if (tokens.empty()) {
    return {kWordEnd};
  }

  std::unordered_set<std::string> first_symbols;
  for (const auto& token : tokens) {
    std::unordered_set<std::string> path;
    const auto& dfs_first_symbols = TokenFirstSymbols(token, path);
    for (const auto& first_symbol : dfs_first_symbols) {
      first_symbols.insert(first_symbol);
    }
    if (!dfs_first_symbols.contains(kWordEnd)) {
      break;
    }
  }
  return first_symbols;
}

TGrammar ReadGrammarFromFile(std::istream& is) {
  if (!is.good()) {
    throw std::invalid_argument("Invalid file\n");
  }

  std::string line;
  int input_type = 0;

  std::vector<std::string> rules_strings;
  std::unordered_set<std::string> terminal_symbols;
  std::unordered_set<std::string> nonterminal_symbols;
  std::string start_symbol;
  while (std::getline(is, line)) {
    if (line.empty()) {
      ++input_type;
      if (input_type == 4) {
        throw std::invalid_argument("Invalid file format\n");
      }
    }
    if (input_type == 0) {
      rules_strings.push_back(line);
    } else if (input_type == 1) {
      std::istringstream iss(line);
      std::string terminal_symbol;
      while (iss >> terminal_symbol) {
        terminal_symbols.insert(terminal_symbol);
      }
    } else if (input_type == 2) {
      std::istringstream iss(line);
      std::string terminal_symbol;
      while (iss >> terminal_symbol) {
        nonterminal_symbols.insert(terminal_symbol);
      }
    } else {
      start_symbol = line;
    }
  }
  return TGrammar(rules_strings, terminal_symbols, nonterminal_symbols,
                  start_symbol);
}

std::vector<std::string> ReadWordFromFile(std::istream& is) {
  if (!is.good()) {
    throw std::invalid_argument("Invalid file\n");
  }

  std::string word;
  std::getline(is, word);
  return SplitString(word, TGrammar::kTokensDelimiter);
}
