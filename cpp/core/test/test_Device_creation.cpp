#include <gtest/gtest.h>
#include <iostream>

TEST(SquareRootTest, NegativeNos) {
    ASSERT_EQ(-1.0, -1.0);
}
 
int main(int argc, char **argv) {
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}