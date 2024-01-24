#include "test_grammar.hpp"

#include "grammar.hpp"

TEST_F(GrammarTest, TestConstruction) {
  TGrammar grammar({"S -> ( S ) S | "}, {"(", ")", ""}, {"S"}, "S");

  ASSERT_EQ(grammar.GetStartSymbol(), "S");

  auto rule_rhs = grammar.GetRuleRhss("S");
  ASSERT_TRUE(rule_rhs.size() == 2);
  ASSERT_EQ(rule_rhs[0], std::vector<std::string>({"(", "S", ")", "S"}));
  ASSERT_EQ(rule_rhs[1], std::vector<std::string>({""}));

  ASSERT_THROW(TGrammar({"_S -> a"}, {"a"}, {"_S"}, "_S"),
               std::invalid_argument);
}

TEST_F(GrammarTest, InvalidArguments) {
  ASSERT_THROW(TGrammar({"_S -> ( _S   ) _S | "}, {"(", ")", ""}, {"S"}, "_S"),
               std::invalid_argument);
  ASSERT_THROW(TGrammar({"S -> ( S -> ) S | "}, {"(", ")", ""}, {"S"}, "S"),
               std::invalid_argument);
  ASSERT_THROW(TGrammar({"-> ( S ) S | "}, {"(", ")", ""}, {"S"}, "S"),
               std::invalid_argument);
  ASSERT_THROW(TGrammar({"S -> ( S ) _S | "}, {"(", ")", ""}, {"S"}, "S"),
               std::invalid_argument);
}

TEST_F(GrammarTest, TestReadGrammarFromFile) {
  std::stringstream ss;
  ss << "S -> ( S ) S | \n\n( ) \n\nS\n\nS";

  TGrammar grammar = ReadGrammarFromFile(ss);
  ASSERT_EQ(grammar.GetStartSymbol(), "S");

  auto rule_rhs = grammar.GetRuleRhss("S");
  ASSERT_TRUE(rule_rhs.size() == 2);
  ASSERT_EQ(rule_rhs[0], std::vector<std::string>({"(", "S", ")", "S"}));
  ASSERT_EQ(rule_rhs[1], std::vector<std::string>({""}));

  ASSERT_THROW(TGrammar({"_S -> a"}, {"a"}, {"_S"}, "_S"),
               std::invalid_argument);
}

TEST_F(GrammarTest, TestReadWordFromFile) {
  std::stringstream ss;
  ss << "( ( ( ) ) )";

  auto word = ReadWordFromFile(ss);
  ASSERT_EQ(word, std::vector<std::string>({"(", "(", "(", ")", ")", ")"}));
}
