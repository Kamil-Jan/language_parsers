#include "earley_parser.hpp"

#include <iostream>
#include <map>
#include <set>

TEarleyParser::TEarleyParser(const TGrammar& grammar) : grammar_(grammar) {
  PrepareGrammar();
}

void TEarleyParser::PrepareGrammar() {
  const auto& start_symbol = grammar_.GetStartSymbol();
  parser_start_symbol_ = "_" + start_symbol;
  grammar_.AddRule(parser_start_symbol_, {start_symbol});
}

bool TEarleyParser::Parse(const std::vector<std::string>& str) const {
  const auto& start_symbol = grammar_.GetStartSymbol();
  const auto* start_symbol_rhs =
      &grammar_.GetRuleRhss(parser_start_symbol_).at(0);

  TState init_state{.from_symbol = parser_start_symbol_,
                    .rule_rhs = start_symbol_rhs,
                    .symbols_read_before = 0,
                    .dot_pos = 0};

  std::vector<TStageSet> stages(1);
  stages[0][start_symbol] = {init_state};

  bool changed = true;
  while (changed) {
    changed = false;
    changed |= Complete(stages, stages.back());
    changed |= Predict(0, stages.back());
  }

  stages.reserve(str.size() + 1);
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i].empty()) {
      continue;
    }

    stages.push_back(Scan(stages.back(), str[i]));
    changed = true;
    while (changed) {
      changed = false;
      changed |= Complete(stages, stages.back());
      changed |= Predict(i + 1, stages.back());
    }
  }

  init_state.dot_pos = 1;
  if (!stages.back().contains(TGrammar::kEmptyString)) {
    return false;
  }
  return stages.back().at(TGrammar::kEmptyString).contains(init_state);
}

bool TEarleyParser::InsertToStageSet(const TState& state,
                                     TStageSet& stage_set) {
  const std::string& dot_symbol = state.GetDotSymbol();
  auto& states = stage_set[dot_symbol];
  auto insert_res = states.insert(state);
  return insert_res.second;
}

void TEarleyParser::Scan(const TState& state, const std::string& token,
                         TStageSet& stage_set) const {
  if (state.rule_rhs->size() == state.dot_pos ||
      state.GetDotSymbol() != token) {
    return;
  }

  InsertToStageSet(TState{.from_symbol = state.from_symbol,
                          .rule_rhs = state.rule_rhs,
                          .symbols_read_before = state.symbols_read_before,
                          .dot_pos = state.dot_pos + 1},
                   stage_set);
}

bool TEarleyParser::Predict(const TState& state, size_t symbols_read,
                            TStageSet& stage_set) const {
  if (state.rule_rhs->size() == state.dot_pos ||
      grammar_.IsTerminal(state.GetDotSymbol())) {
    return false;
  }

  std::vector<TState> predicted;
  const auto& non_terminal = state.GetDotSymbol();
  const auto& rule_rhss = grammar_.GetRuleRhss(non_terminal);

  bool added = false;
  for (const auto& rule_rhs : rule_rhss) {
    size_t new_dot_pos = 0;
    while (new_dot_pos < rule_rhs.size() && rule_rhs.at(new_dot_pos).empty()) {
      ++new_dot_pos;
    }

    added |= InsertToStageSet(TState{.from_symbol = non_terminal,
                                     .rule_rhs = &rule_rhs,
                                     .symbols_read_before = symbols_read,
                                     .dot_pos = new_dot_pos},
                              stage_set);
  }
  return added;
}

bool TEarleyParser::Complete(const TState& state,
                             const std::vector<TStageSet>& prev_stage_sets,
                             TStageSet& stage_set) const {
  if (state.rule_rhs->size() != state.dot_pos) {
    return false;
  }

  const auto& non_terminal = state.from_symbol;
  const auto& prev_stage_set = prev_stage_sets.at(state.symbols_read_before);
  if (!prev_stage_set.contains(non_terminal)) {
    return false;
  }

  const auto& prev_states = prev_stage_set.at(non_terminal);

  bool added = false;
  for (const auto& prev_state : prev_states) {
    size_t new_dot_pos = prev_state.dot_pos + 1;
    while (new_dot_pos < prev_state.rule_rhs->size() &&
           prev_state.rule_rhs->at(new_dot_pos).empty()) {
      ++new_dot_pos;
    }

    added |= InsertToStageSet(
        TState{.from_symbol = prev_state.from_symbol,
               .rule_rhs = prev_state.rule_rhs,
               .symbols_read_before = prev_state.symbols_read_before,
               .dot_pos = new_dot_pos},
        stage_set);
  }
  return added;
}

TEarleyParser::TStageSet TEarleyParser::Scan(TStageSet& stage_set,
                                             const std::string& token) const {
  TStageSet new_stage_set;
  for (auto& [dot_symbol, states] : stage_set) {
    for (auto& state : states) {
      Scan(state, token, new_stage_set);
    }
  }
  return new_stage_set;
}

bool TEarleyParser::Predict(size_t symbols_read, TStageSet& stage_set) const {
  bool changed = false;
  for (auto& [dot_symbol, states] : stage_set) {
    for (auto& state : states) {
      changed |= Predict(state, symbols_read, stage_set);
    }
  }
  return changed;
}

bool TEarleyParser::Complete(const std::vector<TStageSet>& prev_stage_sets,
                             TStageSet& stage_set) const {
  bool changed = false;
  for (auto& [dot_symbol, states] : stage_set) {
    for (auto& state : states) {
      changed |= Complete(state, prev_stage_sets, stage_set);
    }
  }
  return changed;
}
