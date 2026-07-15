#include <gtest/gtest.h>

#include "ConsoleInputParsing.h"

namespace SampleOrderSystem {
namespace {

TEST(ParseMenuChoiceTest, ParsesValidInteger) { EXPECT_EQ(1, ParseMenuChoice("1").value()); }

TEST(ParseMenuChoiceTest, ReturnsNulloptForBlankLine) {
    // std::cin >> 로는 재현되지 않던 공백 값 경로(CLAUDE.md 재사용 원칙).
    EXPECT_FALSE(ParseMenuChoice("").has_value());
    EXPECT_FALSE(ParseMenuChoice("   ").has_value());
}

TEST(ParseMenuChoiceTest, ReturnsNulloptForNonNumericText) {
    EXPECT_FALSE(ParseMenuChoice("abc").has_value());
}

TEST(ParseMenuChoiceTest, ReturnsNulloptForTrailingGarbage) {
    EXPECT_FALSE(ParseMenuChoice("3abc").has_value());
}

TEST(ParseMenuChoiceTest, IgnoresTrailingWhitespace) {
    EXPECT_EQ(2, ParseMenuChoice("2   ").value());
}

TEST(ParseDoubleTest, ParsesValidDecimal) { EXPECT_DOUBLE_EQ(12.5, ParseDouble("12.5").value()); }

TEST(ParseDoubleTest, ReturnsNulloptForBlankLine) { EXPECT_FALSE(ParseDouble("").has_value()); }

TEST(ParseDoubleTest, ReturnsNulloptForNonNumericText) {
    EXPECT_FALSE(ParseDouble("abc").has_value());
}

}  // namespace
}  // namespace SampleOrderSystem
