#include "gtest/gtest.h"

#include "nr.h"


TEST(IsPower2, Power2) {
    ASSERT_TRUE(nr::is_power2(16));
    ASSERT_TRUE(nr::is_power2(8));
    ASSERT_TRUE(nr::is_power2(2));
    ASSERT_FALSE(nr::is_power2(6));
}

TEST(RealFT, TestVec1) {
    std::vector<nr::DP> x(16, 0.0);
    x[0] = 1.0;
    nr::realft(x, 1);
    ASSERT_EQ(1.0, x[0]);
}

TEST(RealFT, TestVec2) {
    std::vector<nr::DP> x(8, 1.0);
    nr::realft(x, 1);
    ASSERT_EQ(8.0, x[0]);
}