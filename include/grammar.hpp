#pragma once

#include <functional>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

class TGrammar {
  using TRulesMap =
      std::unordered_map<std::string, std::vector<std::vector<std::string>>>;

 public:
  static const std::string kEmptyString;
  static const std::string kWordEnd;
  static const std::string kRuleDelimiter;
  static const std::string kRhsDelimiter;
  static const std::string kTokensDelimiter;
  static const char kForbiddenPrefix = '_';

  TGrammar(const std::vector<std::string>& rules_strings,
           const std::unordered_set<std::string>& terminal_symbols,
           const std::unordered_set<std::string>& nonterminal_symbols,
           const std::string& start_symbol);

  bool IsTerminal(const std::string& symbol) const;

  const std::string& GetStartSymbol() const;

  const std::unordered_set<std::string>& GetTerminalSymbols() const;

  const std::unordered_set<std::string>& GetNonTerminalSymbols() const;

  const std::vector<std::vector<std::string>>& GetRuleRhss(
      const std::string& from_symbol) const;

  void AddRule(const std::string& from_symbol,
               const std::vector<std::string>& rule_rhs);

  std::unordered_set<std::string> First(
      const std::vector<std::string>& tokens) const;

 private:
  void ParseRuleString(const std::string& rule_str);
  const std::unordered_set<std::string>& TokenFirstSymbols(
      const std::string& token, std::unordered_set<std::string>& path) const;

  std::unordered_set<std::string> terminal_symbols_;
  std::unordered_set<std::string> nonterminal_symbols_;
  std::string start_symbol_;
  TRulesMap rules_map_;

  mutable std::unordered_map<std::string, std::unordered_set<std::string>>
      first_;
};

TGrammar ReadGrammarFromFile(std::istream& is);

std::vector<std::string> ReadWordFromFile(std::istream& is);
