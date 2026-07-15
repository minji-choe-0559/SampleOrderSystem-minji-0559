#include "ConsoleView.h"

#include <iostream>

#include "ConsoleInputParsing.h"

namespace SampleOrderSystem {

namespace {

// std::cin >> 토큰 읽기는 공백 값과 EOF를 올바르게 처리하지 못했다(ConsoleMVC PoC에서 실제로
// 발견된 문제, CLAUDE.md 재사용 원칙) — 모든 입력은 std::getline으로 원시 줄을 읽은 뒤 파싱한다.
std::string ReadLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);  // EOF/실패 시 line은 빈 문자열로 남는다.
    return line;
}

double ReadDoubleWithRetry(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return 0.0;  // EOF: 더 입력을 받을 수 없으므로 무한 루프 대신 기본값으로 진행한다.
        }
        if (auto parsed = ParseDouble(line)) {
            return *parsed;
        }
        std::cout << "숫자를 입력해주세요.\n";
    }
}

long ReadStockWithRetry(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return 0;
        }
        if (auto parsed = ParseDouble(line)) {
            return static_cast<long>(*parsed);
        }
        std::cout << "숫자를 입력해주세요.\n";
    }
}

}  // namespace

void ConsoleView::ShowMenu() {
    std::cout << "\n=== 시료 관리 ===\n"
              << "[1] 시료 등록\n"
              << "[2] 시료 목록 조회\n"
              << "[3] 시료 검색\n"
              << "[0] 뒤로\n"
              << "선택 > ";
}

int ConsoleView::ReadMenuChoice() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return 0;  // EOF/실패: 뒤로가기로 취급해 무한 루프에 빠지지 않는다.
    }
    return ParseMenuChoice(line).value_or(-1);  // 파싱 실패는 -1(무효 선택)로 취급.
}

SampleRegistrationInput ConsoleView::ReadRegistrationInput() {
    SampleRegistrationInput input;
    input.sampleCode = ReadLine("시료 ID > ");
    input.name = ReadLine("이름 > ");
    input.avgProcessTime = ReadDoubleWithRetry("평균 생산시간(분) > ");
    input.yieldRate = ReadDoubleWithRetry("수율(0~100) > ");
    input.stock = ReadStockWithRetry("재고 수량 > ");
    return input;
}

std::string ConsoleView::ReadSearchKeyword() { return ReadLine("검색어(이름 또는 시료 ID) > "); }

void ConsoleView::ShowRegistrationResult(const SampleRegistrationResult& result) {
    std::cout << (result.outcome == SampleRegistrationOutcome::Success ? "[성공] " : "[실패] ")
              << result.message << "\n";
}

void ConsoleView::ShowSampleList(const std::vector<SampleViewModel>& samples) {
    if (samples.empty()) {
        std::cout << "\n결과가 없습니다.\n";
        return;
    }
    std::cout << "\n시료 목록 (" << samples.size() << "건)\n";
    for (const SampleViewModel& sample : samples) {
        std::cout << "  " << sample.sampleCode << " | " << sample.name << " | 생산시간 "
                  << sample.avgProcessTime << " | 수율 " << sample.yieldRate << " | 재고 "
                  << sample.stock << "\n";
    }
}

}  // namespace SampleOrderSystem
