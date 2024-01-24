#pragma once

#include <iostream>
#include <optional>
#include <set>
#include <variant>

#include "grammar.hpp"
#include "parser.hpp"

class TLRParser : public Parser {
  struct Shift {
    size_t idx;
  };

  struct Reduce {
    std::string from_symbol;
    const std::vector<std::string>* rule_rhs;
  };

  struct Accept {};

  using Action = std::variant<Shift, Reduce, Accept>;

  struct TState {
    std::string from_symbol;
    const std::vector<std::string>* rule_rhs;
    size_t dot_pos;
    std::string next_symbol;
    mutable bool visited_closure = false;

    inline const std::string& GetDotSymbol() const;

    friend auto operator<=>(const TState& lhs, const TState& rhs);
    friend auto operator==(const TState& lhs, const TState& rhs);
    friend std::ostream& operator<<(std::ostream& os, const TState& state);
  };

  struct AutomatonNode {
    std::set<TState> states;
    std::unordered_map<std::string, size_t> goto_map;

    void Closure(const TGrammar& grammar);
    AutomatonNode CreateGotoNode(const std::string& token,
                                 const TGrammar& grammar) const;

    friend auto operator<=>(const AutomatonNode& lhs, const AutomatonNode& rhs);
    friend auto operator==(const AutomatonNode& lhs, const AutomatonNode& rhs);
    friend std::ostream& operator<<(std::ostream& os,
                                    const AutomatonNode& node);
  };

  friend auto operator<=>(const TState& lhs, const TState& rhs);
  friend auto operator==(const TState& lhs, const TState& rhs);
  friend std::ostream& operator<<(std::ostream& os, const TState& state);

  friend auto operator<=>(const AutomatonNode& lhs, const AutomatonNode& rhs);
  friend auto operator==(const AutomatonNode& lhs, const AutomatonNode& rhs);
  friend std::ostream& operator<<(std::ostream& os, const AutomatonNode& node);

 public:
  TLRParser(const TGrammar& grammar);
  bool Parse(const std::vector<std::string>& tokens) const override;

 private:
  void PrepareGrammar() override;
  void CreateAutomaton();
  void CreateAutomatonDfs(size_t node_idx);
  void CreateLRTable();

  TGrammar grammar_;
  std::string parser_start_symbol_;
  std::vector<AutomatonNode> nodes_;
  std::unordered_map<size_t, std::unordered_map<std::string, Action>>
      action_table_;
};
