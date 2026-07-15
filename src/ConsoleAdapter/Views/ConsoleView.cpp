#include "ConsoleView.h"

#include <iostream>

namespace SampleOrderSystem {

void ConsoleView::ShowMenu() {
    std::cout << "\n=== 반도체 시료 생산주문관리 시스템 (MVC PoC) ===\n"
              << "[1] 시료 등록\n"
              << "[2] 시료 목록 조회\n"
              << "[0] 종료\n"
              << "선택 > ";
}

int ConsoleView::ReadMenuChoice() {
    int choice = -1;
    if (!(std::cin >> choice)) {
        // EOF or non-numeric input: treat as "종료" instead of looping
        // forever (operator>> leaves choice untouched once the stream is
        // already in a fail state, so the caller must check the result).
        return 0;
    }
    return choice;
}

SampleRegistrationInput ConsoleView::ReadRegistrationInput() {
    SampleRegistrationInput input;
    std::cout << "시료 코드 > ";
    std::cin >> input.sampleCode;
    std::cout << "이름 > ";
    std::cin >> input.name;
    std::cout << "평균 생산시간 > ";
    std::cin >> input.avgProcessTime;
    std::cout << "수율(0~100) > ";
    std::cin >> input.yieldRate;
    std::cout << "재고 수량 > ";
    std::cin >> input.stock;
    return input;
}

void ConsoleView::ShowRegistrationResult(const SampleRegistrationResult& result) {
    std::cout << (result.outcome == SampleRegistrationOutcome::Success ? "[성공] " : "[실패] ")
              << result.message << "\n";
}

void ConsoleView::ShowSampleList(const std::vector<SampleViewModel>& samples) {
    std::cout << "\n등록된 시료 목록 (" << samples.size() << "건)\n";
    for (const SampleViewModel& sample : samples) {
        std::cout << "  " << sample.sampleCode << " | " << sample.name << " | 생산시간 "
                  << sample.avgProcessTime << " | 수율 " << sample.yieldRate << " | 재고 "
                  << sample.stock << "\n";
    }
}

}  // namespace SampleOrderSystem
