#include "lr_parser.hpp"

#include <stack>

inline const std::string& TLRParser::TState::GetDotSymbol() const {
  return rule_rhs->size() == dot_pos ? TGrammar::kEmptyString
                                     : rule_rhs->at(dot_pos);
}

auto operator<=>(const TLRParser::TState& lhs, const TLRParser::TState& rhs) {
  auto lhs_tie =
      std::tie(lhs.from_symbol, lhs.rule_rhs, lhs.dot_pos, lhs.next_symbol);
  auto rhs_tie =
      std::tie(rhs.from_symbol, rhs.rule_rhs, rhs.dot_pos, rhs.next_symbol);
  return lhs_tie <=> rhs_tie;
}

auto operator==(const TLRParser::TState& lhs, const TLRParser::TState& rhs) {
  auto lhs_tie =
      std::tie(lhs.from_symbol, lhs.rule_rhs, lhs.dot_pos, lhs.next_symbol);
  auto rhs_tie =
      std::tie(rhs.from_symbol, rhs.rule_rhs, rhs.dot_pos, rhs.next_symbol);
  return lhs_tie == rhs_tie;
}

// std::ostream& operator<<(std::ostream& os, const TLRParser::TState& state) {
//   std::cerr << state.from_symbol << " ";
//   for (const auto& s : *state.rule_rhs) {
//     std::cerr << '\'' << s << "\' ";
//   }
//   std::cerr << ";;" << state.dot_pos << std::endl;
//
//   os << "[" << state.from_symbol << " -> ";
//   for (size_t i = 0; i < state.dot_pos; ++i) {
//     os << '\'' << state.rule_rhs->at(i) << "\' ";
//   }
//   os << '.';
//   for (size_t i = state.dot_pos; i < state.rule_rhs->size(); ++i) {
//     os << '\'' << state.rule_rhs->at(i) << "\' ";
//   }
//   os << "; " << state.next_symbol << "]";
//   return os;
// }

auto operator<=>(const TLRParser::AutomatonNode& lhs,
                 const TLRParser::AutomatonNode& rhs) {
  return lhs.states <=> rhs.states;
}

auto operator==(const TLRParser::AutomatonNode& lhs,
                const TLRParser::AutomatonNode& rhs) {
  return lhs.states == rhs.states;
}

// std::ostream& operator<<(std::ostream& os, const TLRParser::AutomatonNode&
// node) {
//   os << "{";
//   for (const auto& state : node.states) {
//     os << state << ", ";
//   }
//   os << "}";
//   return os;
// }

void TLRParser::AutomatonNode::Closure(const TGrammar& grammar) {
  bool changed = true;
  while (changed) {
    changed = false;

    for (const auto& state : states) {
      if (state.visited_closure) {
        continue;
      }

      const auto& dot_symbol = state.GetDotSymbol();
      if (grammar.IsTerminal(dot_symbol)) {
        state.visited_closure = true;
        continue;
      }

      for (const auto& rule_rhs : grammar.GetRuleRhss(dot_symbol)) {
        size_t new_dot_pos = 0;
        while (new_dot_pos < rule_rhs.size() &&
               rule_rhs.at(new_dot_pos).empty()) {
          ++new_dot_pos;
        }

        std::vector<std::string> left_rule = {
            state.rule_rhs->begin() + state.dot_pos + 1, state.rule_rhs->end()};
        if (state.next_symbol != TGrammar::kWordEnd) {
          left_rule.push_back(state.next_symbol);
        }

        const auto& first_symbols = grammar.First(left_rule);
        for (const auto& first_symbol : first_symbols) {
          auto res = states.insert(TState{.from_symbol = dot_symbol,
                                          .rule_rhs = &rule_rhs,
                                          .dot_pos = new_dot_pos,
                                          .next_symbol = first_symbol});
          changed |= res.second;
        }
      }
      state.visited_closure = true;
    }
  }
}

TLRParser::AutomatonNode TLRParser::AutomatonNode::CreateGotoNode(
    const std::string& token, const TGrammar& grammar) const {
  AutomatonNode goto_node;
  for (const auto& state : states) {
    const auto& dot_symbol = state.GetDotSymbol();
    if (dot_symbol != token) {
      continue;
    }

    size_t new_dot_pos = (state.dot_pos == state.rule_rhs->size())
                             ? state.dot_pos
                             : state.dot_pos + 1;
    goto_node.states.insert(TState{.from_symbol = state.from_symbol,
                                   .rule_rhs = state.rule_rhs,
                                   .dot_pos = new_dot_pos,
                                   .next_symbol = state.next_symbol});
  }
  goto_node.Closure(grammar);
  return goto_node;
}

TLRParser::TLRParser(const TGrammar& grammar) : grammar_(grammar) {
  PrepareGrammar();
}

void TLRParser::PrepareGrammar() {
  const auto& start_symbol = grammar_.GetStartSymbol();
  parser_start_symbol_ = "_" + start_symbol;
  grammar_.AddRule(parser_start_symbol_, {start_symbol});

  CreateAutomaton();
}

void TLRParser::CreateAutomaton() {
  const auto& start_symbol = grammar_.GetStartSymbol();
  const auto* start_symbol_rhs =
      &grammar_.GetRuleRhss(parser_start_symbol_).at(0);

  TState init_state{.from_symbol = parser_start_symbol_,
                    .rule_rhs = start_symbol_rhs,
                    .dot_pos = 0,
                    .next_symbol = TGrammar::kWordEnd};
  AutomatonNode start_node{.states = {init_state}};
  start_node.Closure(grammar_);

  nodes_.emplace_back(std::move(start_node));

  CreateAutomatonDfs(0);
  CreateLRTable();
}

void TLRParser::CreateAutomatonDfs(size_t node_idx) {
  for (const auto& token : grammar_.GetNonTerminalSymbols()) {
    auto new_node = nodes_[node_idx].CreateGotoNode(token, grammar_);
    if (new_node.states.empty()) {
      continue;
    }

    auto iter = std::find(nodes_.begin(), nodes_.end(), new_node);
    if (iter != nodes_.end()) {
      nodes_[node_idx].goto_map[token] = iter - nodes_.begin();
    } else {
      size_t new_node_idx = nodes_.size();
      nodes_[node_idx].goto_map[token] = new_node_idx;
      nodes_.emplace_back(std::move(new_node));
      CreateAutomatonDfs(new_node_idx);
    }
  }

  for (const auto& token : grammar_.GetTerminalSymbols()) {
    auto new_node = nodes_[node_idx].CreateGotoNode(token, grammar_);
    if (new_node.states.empty()) {
      continue;
    }

    auto iter = std::find(nodes_.begin(), nodes_.end(), new_node);
    if (iter != nodes_.end()) {
      nodes_[node_idx].goto_map[token] = iter - nodes_.begin();
    } else {
      size_t new_node_idx = nodes_.size();
      nodes_[node_idx].goto_map[token] = new_node_idx;
      nodes_.emplace_back(std::move(new_node));
      CreateAutomatonDfs(new_node_idx);
    }
  }
}

void TLRParser::CreateLRTable() {
  const auto& start_symbol = grammar_.GetStartSymbol();
  const auto* start_symbol_rhs =
      &grammar_.GetRuleRhss(parser_start_symbol_).at(0);

  TState accept_state{.from_symbol = parser_start_symbol_,
                      .rule_rhs = start_symbol_rhs,
                      .dot_pos = 1,
                      .next_symbol = TGrammar::kWordEnd};

  auto terminals = grammar_.GetTerminalSymbols();
  terminals.insert(TGrammar::kWordEnd);

  for (size_t i = 0; i < nodes_.size(); ++i) {
    const auto& node = nodes_[i];
    for (const auto& token : terminals) {
      for (const auto& state : node.states) {
        if (state.GetDotSymbol() == token && node.goto_map.contains(token)) {
          action_table_[i][token] = Shift{node.goto_map.at(token)};
        }

        if (state.rule_rhs->size() == state.dot_pos &&
            state.next_symbol == token) {
          if (action_table_.contains(i) && action_table_[i].contains(token)) {
            throw std::invalid_argument("Grammar is not valid for LR(1)");
          }
          action_table_[i][token] = Reduce{state.from_symbol, state.rule_rhs};
        }

        if (state == accept_state && token == TGrammar::kWordEnd) {
          action_table_[i][token] = Accept{};
        }
      }
    }
  }
}

bool TLRParser::Parse(const std::vector<std::string>& tokens) const {
  size_t pos = 0;
  std::stack<std::variant<size_t, std::string>> lr_stack;
  lr_stack.push(size_t(0));

  while (pos <= tokens.size()) {
    while (pos < tokens.size() && tokens[pos] == TGrammar::kEmptyString) {
      ++pos;
    }

    const auto& token =
        (pos < tokens.size()) ? tokens.at(pos) : TGrammar::kWordEnd;
    size_t node_idx = std::get<size_t>(lr_stack.top());

    if (!action_table_.contains(node_idx) ||
        !action_table_.at(node_idx).contains(token)) {
      return false;
    }

    const auto& action = action_table_.at(node_idx).at(token);
    switch (action.index()) {
      case 0:  // Shift
      {
        lr_stack.push(token);
        ++pos;
        lr_stack.push(std::get<Shift>(action).idx);
        break;
      }
      case 1:  // Reduce
      {
        const auto& reduce = std::get<Reduce>(action);
        if (reduce.rule_rhs->size() != 1 ||
            reduce.rule_rhs->at(0) != TGrammar::kEmptyString) {
          for (size_t i = 0; i < 2 * reduce.rule_rhs->size(); ++i) {
            lr_stack.pop();
          }
        }
        size_t new_node_idx = std::get<size_t>(lr_stack.top());
        const auto& goto_token = reduce.from_symbol;
        if (!nodes_[new_node_idx].goto_map.contains(goto_token)) {
          return false;
        }
        lr_stack.push(goto_token);
        lr_stack.push(nodes_[new_node_idx].goto_map.at(goto_token));
        break;
      }
      case 2:
        return true;
    }
  }
  return false;
}
