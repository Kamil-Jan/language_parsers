#include "test_util.hpp"

#include "util.hpp"

TEST_F(UtilTest, SplitStringTest) {
  std::vector<std::string> correct_vec;
  std::vector<std::string> vec;

  vec = SplitString("a,b,c,d,e,f,g", ",");
  correct_vec = {"a", "b", "c", "d", "e", "f", "g"};
  ASSERT_EQ(vec, correct_vec);

  vec = SplitString("aaaaaaaa,b,c,d,eeeee,f,g", ",");
  correct_vec = {"aaaaaaaa", "b", "c", "d", "eeeee", "f", "g"};
  ASSERT_EQ(vec, correct_vec);

  vec = SplitString("aaaaaaaa -> b -> c -> d -> eeeee -> f -> g", " -> ");
  correct_vec = {"aaaaaaaa", "b", "c", "d", "eeeee", "f", "g"};
  ASSERT_EQ(vec, correct_vec);

  vec = SplitString("", " ");
  correct_vec = {""};
  ASSERT_EQ(vec, correct_vec);

  vec = SplitString("|a", "|");
  correct_vec = {"", "a"};
  ASSERT_EQ(vec, correct_vec);
}

TEST_F(UtilTest, TrimTest) {
  std::string str;
  std::string correct_str;

  str = "   \t       ab  cd";
  correct_str = "ab  cd";
  ASSERT_EQ(LTrim(str), correct_str);

  str = "ab  cd     \t     ";
  correct_str = "ab  cd";
  ASSERT_EQ(RTrim(str), correct_str);

  str = "   \t       ab  cd     \t     ";
  correct_str = "ab  cd";
  ASSERT_EQ(Trim(str), correct_str);

  str = "ab  cd";
  correct_str = "ab  cd";
  ASSERT_EQ(Trim(str), correct_str);

  str = "";
  correct_str = "";
  ASSERT_EQ(Trim(str), correct_str);

  str = "     ";
  correct_str = "";
  ASSERT_EQ(Trim(str), correct_str);
}
