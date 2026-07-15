#include "ConsoleOrderView.h"

#include <iostream>

#include "ConsoleInputParsing.h"

namespace SampleOrderSystem {

namespace {

std::string ReadLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);  // EOF/실패 시 line은 빈 문자열로 남는다.
    return line;
}

int ReadQuantityWithRetry(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return 0;  // EOF: 무한 루프 대신 기본값으로 진행한다.
        }
        if (auto parsed = ParseInt(line)) {
            return *parsed;
        }
        std::cout << "숫자를 입력해주세요.\n";
    }
}

}  // namespace

void ConsoleOrderView::ShowMenu() {
    std::cout << "\n=== 시료 주문 ===\n"
              << "[1] 주문 접수\n"
              << "[0] 뒤로\n"
              << "선택 > ";
}

int ConsoleOrderView::ReadMenuChoice() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return 0;  // EOF/실패: 뒤로가기로 취급해 무한 루프에 빠지지 않는다.
    }
    return ParseMenuChoice(line).value_or(-1);
}

OrderReservationInput ConsoleOrderView::ReadReservationInput() {
    OrderReservationInput input;
    input.sampleCode = ReadLine("시료 ID > ");
    input.customerName = ReadLine("고객명 > ");
    input.quantity = ReadQuantityWithRetry("주문 수량 > ");
    return input;
}

void ConsoleOrderView::ShowReservationResult(const OrderReservationResult& result) {
    if (result.outcome == OrderReservationOutcome::Success) {
        std::cout << "[성공] " << result.message << " 주문번호: " << result.orderNumber << "\n";
    } else {
        std::cout << "[실패] " << result.message << "\n";
    }
}

}  // namespace SampleOrderSystem
