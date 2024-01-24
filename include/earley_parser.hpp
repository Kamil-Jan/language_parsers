#pragma once

#include <optional>
#include <set>
#include <tuple>

#include "grammar.hpp"
#include "parser.hpp"

class TEarleyParser : public Parser {
  struct TState {
    std::string from_symbol;
    const std::vector<std::string>* rule_rhs;
    size_t symbols_read_before;
    size_t dot_pos;

    inline const std::string& GetDotSymbol() const {
      return rule_rhs->size() == dot_pos ? TGrammar::kEmptyString
                                         : rule_rhs->at(dot_pos);
    }

    friend auto operator<=>(const TState& lhs, const TState& rhs) {
      auto lhs_tie = std::tie(lhs.from_symbol, lhs.rule_rhs,
                              lhs.symbols_read_before, lhs.dot_pos);
      auto rhs_tie = std::tie(rhs.from_symbol, rhs.rule_rhs,
                              rhs.symbols_read_before, rhs.dot_pos);
      return lhs_tie <=> rhs_tie;
    }
  };

  using TStageSet = std::unordered_map<std::string, std::set<TState>>;

 public:
  TEarleyParser(const TGrammar& grammar);
  bool Parse(const std::vector<std::string>& tokens) const override;

 private:
  TGrammar grammar_;
  std::string parser_start_symbol_;

  void PrepareGrammar() override;

  static bool InsertToStageSet(const TState& state, TStageSet& stage_set);

  TStageSet Scan(TStageSet& stage_set, const std::string& token) const;

  bool Predict(size_t symbols_read, TStageSet& stage_set) const;

  bool Complete(const std::vector<TStageSet>& prev_stage_sets,
                TStageSet& stage_set) const;

  void Scan(const TState& state, const std::string& token,
            TStageSet& stage_set) const;

  bool Predict(const TState& state, size_t symbols_read,
               TStageSet& stage_set) const;

  bool Complete(const TState& state,
                const std::vector<TStageSet>& prev_stage_sets,
                TStageSet& stage_set) const;
};
