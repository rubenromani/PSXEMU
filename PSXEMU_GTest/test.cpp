#include "pch.h"

GTEST_API_ int main(int argc, char** argv)
{
	printf("Running main()");
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(TestCaseName, TestName) {
  EXPECT_EQ(10, 10);

  EXPECT_TRUE(true);
}