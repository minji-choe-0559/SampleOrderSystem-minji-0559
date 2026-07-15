#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

#include "ConsoleOrderApprovalView.h"

namespace SampleOrderSystem {
namespace {

// ConsoleViewTest/ConsoleOrderViewTest와 동일한 이유(test-auditor 지적): 순수 파서만으로는
// EOF/입력 처리 경로가 실제로 고쳐졌는지 증명하지 못하므로, std::cin을 실제로 리다이렉트해
// 검증한다.
class ConsoleOrderApprovalViewTest : public ::testing::Test {
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

TEST_F(ConsoleOrderApprovalViewTest, ReadMenuChoiceReturnsZeroOnImmediateEof) {
    FeedInput("");
    ConsoleOrderApprovalView view;

    EXPECT_EQ(0, view.ReadMenuChoice());
}

TEST_F(ConsoleOrderApprovalViewTest, ReadOrderNumberReturnsEnteredLine) {
    FeedInput("ORD-00001\n");
    ConsoleOrderApprovalView view;

    EXPECT_EQ("ORD-00001", view.ReadOrderNumber());
}

TEST_F(ConsoleOrderApprovalViewTest, ReadOrderNumberReturnsEmptyStringOnImmediateEof) {
    // ReadOrderNumber는 getline 실패 여부를 별도로 확인하지 않는다(test-auditor 지적) — EOF 시
    // 빈 문자열을 반환하며 무한 루프에 빠지지 않는지만 검증한다.
    FeedInput("");
    ConsoleOrderApprovalView view;

    EXPECT_EQ("", view.ReadOrderNumber());
}

}  // namespace
}  // namespace SampleOrderSystem
