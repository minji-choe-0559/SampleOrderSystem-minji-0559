#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

#include "ConsoleView.h"

namespace SampleOrderSystem {
namespace {

// std::cin >> 토큰 읽기는 공백 값/EOF 경로를 재현하지 못했다(ConsoleMVC PoC에서 실제로 발견된
// 문제). 순수 파서(ConsoleInputParsingTest)만으로는 재시도 루프 자체가 고쳐졌는지 증명하지
// 못하므로, std::cin을 실제로 리다이렉트해 ConsoleView의 재시도/EOF 처리를 직접 검증한다.
class ConsoleViewTest : public ::testing::Test {
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

TEST_F(ConsoleViewTest, ReadMenuChoiceReturnsZeroOnImmediateEof) {
    FeedInput("");
    ConsoleView view;

    EXPECT_EQ(0, view.ReadMenuChoice());
}

TEST_F(ConsoleViewTest, ReadRegistrationInputRetriesOnInvalidNumberThenSucceeds) {
    // 평균 생산시간에 잘못된 값("abc")을 한 번 준 뒤 올바른 값을 준다.
    FeedInput("S-001\nSample A\nabc\n1.5\n90\n10\n");
    ConsoleView view;

    SampleRegistrationInput input = view.ReadRegistrationInput();

    EXPECT_EQ("S-001", input.sampleCode);
    EXPECT_EQ("Sample A", input.name);
    EXPECT_DOUBLE_EQ(1.5, input.avgProcessTime);
    EXPECT_DOUBLE_EQ(90.0, input.yieldRate);
    EXPECT_EQ(10, input.stock);
}

TEST_F(ConsoleViewTest, ReadRegistrationInputStopsOnEofInsteadOfLoopingForever) {
    // 평균 생산시간을 읽기 전에 스트림이 끝나버리는 상황 — 무한 루프 없이 기본값(0.0)으로
    // 진행되어야 한다(재시도 루프가 EOF에서 종료하는지가 이 테스트의 핵심).
    FeedInput("S-001\nSample A\n");
    ConsoleView view;

    SampleRegistrationInput input = view.ReadRegistrationInput();

    EXPECT_DOUBLE_EQ(0.0, input.avgProcessTime);
}

}  // namespace
}  // namespace SampleOrderSystem
