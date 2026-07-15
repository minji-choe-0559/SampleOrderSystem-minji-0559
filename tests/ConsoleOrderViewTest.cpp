#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

#include "ConsoleOrderView.h"

namespace SampleOrderSystem {
namespace {

// ConsoleViewTest와 동일한 이유: 순수 파서(ConsoleInputParsingTest)만으로는 재시도 루프 자체가
// 고쳐졌는지 증명하지 못하므로, std::cin을 실제로 리다이렉트해 검증한다(test-auditor 지적).
class ConsoleOrderViewTest : public ::testing::Test {
  protected:
    void SetUp() override { originalCinBuffer_ = std::cin.rdbuf(); }

    void TearDown() override { std::cin.rdbuf(originalCinBuffer_); }

    void FeedInput(const std::string& text) {
        input_.str(text);
        input_.clear();
        std::cin.rdbuf(input_.rdbuf());
    }

  private:
    std::istringstream input_;
    std::streambuf* originalCinBuffer_ = nullptr;
};

TEST_F(ConsoleOrderViewTest, ReadMenuChoiceReturnsZeroOnImmediateEof) {
    FeedInput("");
    ConsoleOrderView view;

    EXPECT_EQ(0, view.ReadMenuChoice());
}

TEST_F(ConsoleOrderViewTest, ReadReservationInputRetriesOnInvalidQuantityThenSucceeds) {
    FeedInput("S-001\n삼성전자 파운드리\nabc\n200\n");
    ConsoleOrderView view;

    OrderReservationInput input = view.ReadReservationInput();

    EXPECT_EQ("S-001", input.sampleCode);
    EXPECT_EQ("삼성전자 파운드리", input.customerName);
    EXPECT_EQ(200, input.quantity);
}

TEST_F(ConsoleOrderViewTest, ReadReservationInputStopsOnEofInsteadOfLoopingForever) {
    // 주문 수량을 읽기 전에 스트림이 끝나버리는 상황 — 무한 루프 없이 기본값(0)으로 진행되어야
    // 한다(재시도 루프가 EOF에서 종료하는지가 이 테스트의 핵심).
    FeedInput("S-001\n삼성전자 파운드리\n");
    ConsoleOrderView view;

    OrderReservationInput input = view.ReadReservationInput();

    EXPECT_EQ(0, input.quantity);
}

}  // namespace
}  // namespace SampleOrderSystem
