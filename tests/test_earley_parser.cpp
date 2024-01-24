#include "test_earley_parser.hpp"

#include "earley_parser.hpp"

TEST_F(EarleyParserTest, TestCorrectBracketSeq) {
  TGrammar grammar({"S -> ( S ) S | "}, {"(", ")", ""}, {"S"}, "S");

  TEarleyParser parser(grammar);
  ASSERT_TRUE(parser.Parse({"(", ")"}));
  ASSERT_TRUE(parser.Parse({"(", "(", ")", ")", "(", ")"}));
  ASSERT_TRUE(parser.Parse({"(", "(", ")", "(", "(", ")", ")", ")"}));
  ASSERT_TRUE(parser.Parse({""}));

  ASSERT_FALSE(parser.Parse({"("}));
  ASSERT_FALSE(parser.Parse({"(", ")", ")"}));
  ASSERT_FALSE(parser.Parse({"(", "(", "("}));
  ASSERT_FALSE(parser.Parse({"(", "(", "(", ")"}));
}

TEST_F(EarleyParserTest, TestEnglishGrammar) {
  TGrammar grammar(
      {"S -> NP VP", "VP -> VP PP | V NP | V", "PP -> P NP",
       "NP -> Det N | N | Pn | Det A N | A NP", "A -> Adv A | A A",
       "Adv -> too | very | quite", "Pn -> she | he",
       "A -> fresh | tasty | silver", "N -> fish | fork | apple", "V -> eats",
       "Det -> a | an | the", "P -> with"},
      {"too", "very", "quite", "she", "he", "fresh", "tasty", "silver", "fish",
       "fork", "apple", "eats", "a", "an", "the", "with"},
      {"S", "NP", "VP", "PP", "V", "P", "Det", "N", "Pn", "A", "Adv"}, "S");

  TEarleyParser parser(grammar);
  ASSERT_TRUE(parser.Parse({"she", "eats"}));
  ASSERT_TRUE(parser.Parse({"she", "eats", "an", "apple"}));
  ASSERT_TRUE(parser.Parse({"she", "eats", "a", "fish"}));
  ASSERT_TRUE(parser.Parse({"she", "eats", "a", "quite", "fresh", "fish",
                            "with", "a", "silver", "fork"}));

  ASSERT_FALSE(parser.Parse({"eats", "she", "fork"}));
  ASSERT_FALSE(parser.Parse({"fork", "silver", "she"}));
}
