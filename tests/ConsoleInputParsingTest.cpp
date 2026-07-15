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

TEST(ParseIntTest, ParsesValidInteger) { EXPECT_EQ(10, ParseInt("10").value()); }

TEST(ParseIntTest, ReturnsNulloptForBlankLine) { EXPECT_FALSE(ParseInt("").has_value()); }

TEST(ParseIntTest, ReturnsNulloptForDecimalText) {
    // "10.5"는 정수 필드(주문 수량)에 유효하지 않은 값으로 취급되어야 한다.
    EXPECT_FALSE(ParseInt("10.5").has_value());
}

TEST(ParseIntTest, ParsesNegativeInteger) {
    // 메뉴 선택과 달리 주문 수량 입력에서는 "-1" 같은 음수도 일단 파싱은 되어야
    // OrderRepository의 quantity <= 0 검증으로 넘겨 거부할 수 있다.
    EXPECT_EQ(-5, ParseInt("-5").value());
}

}  // namespace
}  // namespace SampleOrderSystem
