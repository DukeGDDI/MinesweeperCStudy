#include <gtest/gtest.h>
#include "board.hpp"  // from lib/

static const string kTestBoard = R"(
5 6 4
. . * . . .
. * . . . .
. . . * . .
. . . . . .
. . . . . *
)";

TEST(Math, Adds) {
    EXPECT_EQ(2+2, 4);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
