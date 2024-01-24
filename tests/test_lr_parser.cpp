#include "test_lr_parser.hpp"

#include "lr_parser.hpp"

TEST_F(LRParserTest, TestCorrectBracketSeq) {
  TGrammar grammar({"S -> ( S ) S | "}, {"(", ")"}, {"S"}, "S");

  TLRParser parser(grammar);
  ASSERT_TRUE(parser.Parse({"(", ")"}));
  ASSERT_TRUE(parser.Parse({"(", "(", ")", ")", "(", ")"}));
  ASSERT_TRUE(parser.Parse({"(", "(", ")", "(", "(", ")", ")", ")"}));
  ASSERT_TRUE(parser.Parse({""}));

  ASSERT_FALSE(parser.Parse({"("}));
  ASSERT_FALSE(parser.Parse({"(", ")", ")"}));
  ASSERT_FALSE(parser.Parse({"(", "(", "("}));
  ASSERT_FALSE(parser.Parse({"(", "(", "(", ")"}));
}

TEST_F(LRParserTest, TestRandomGrammar1) {
  TGrammar grammar({"S -> S a S b |"}, {"a", "b"}, {"S"}, "S");

  TLRParser parser(grammar);
  ASSERT_TRUE(parser.Parse({"a", "b", "a", "a", "b", "b"}));
  ASSERT_TRUE(parser.Parse({"a", "b", "a", "b"}));
  ASSERT_TRUE(parser.Parse({"a", "b"}));
  ASSERT_TRUE(parser.Parse({""}));

  ASSERT_FALSE(parser.Parse({"a", "a", "b"}));
  ASSERT_FALSE(parser.Parse({"b", "a", "b"}));
  ASSERT_FALSE(parser.Parse({"b"}));
}

TEST_F(LRParserTest, TestRandomGrammar2) {
  TGrammar grammar({"S -> S S|a|b"}, {"a", "b"}, {"S"}, "S");

  TLRParser parser(grammar);
  ASSERT_TRUE(parser.Parse({"a", "b", "a", "a", "b", "b"}));
  ASSERT_TRUE(parser.Parse({"a", "b", "a", "b"}));
  ASSERT_TRUE(parser.Parse({"a", "b"}));
  ASSERT_TRUE(parser.Parse({"b"}));
  ASSERT_TRUE(parser.Parse({"a"}));

  ASSERT_FALSE(parser.Parse({""}));
}
