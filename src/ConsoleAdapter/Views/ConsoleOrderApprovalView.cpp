#include "ConsoleOrderApprovalView.h"

#include <iostream>

#include "ConsoleInputParsing.h"

namespace SampleOrderSystem {

namespace {

std::string ReadLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

}  // namespace

void ConsoleOrderApprovalView::ShowMenu() {
    std::cout << "\n=== 주문 승인/거절 ===\n"
              << "[1] 접수된 주문(RESERVED) 목록 조회\n"
              << "[2] 주문 승인\n"
              << "[3] 주문 거절\n"
              << "[0] 뒤로\n"
              << "선택 > ";
}

int ConsoleOrderApprovalView::ReadMenuChoice() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return 0;  // EOF/실패: 뒤로가기로 취급해 무한 루프에 빠지지 않는다.
    }
    return ParseMenuChoice(line).value_or(-1);
}

void ConsoleOrderApprovalView::ShowReservedOrders(const std::vector<OrderViewModel>& orders) {
    if (orders.empty()) {
        std::cout << "\n접수된 주문이 없습니다.\n";
        return;
    }
    std::cout << "\n접수된 주문 목록 (" << orders.size() << "건)\n";
    for (const OrderViewModel& order : orders) {
        std::cout << "  " << order.orderNumber << " | " << order.sampleCode << " | "
                  << order.customerName << " | 수량 " << order.quantity << " | " << order.status
                  << "\n";
    }
}

std::string ConsoleOrderApprovalView::ReadOrderNumber() { return ReadLine("주문번호 > "); }

void ConsoleOrderApprovalView::ShowApproveResult(const ApproveOrderResult& result) {
    bool success = result.outcome == ApproveOrderOutcome::ConfirmedSufficientStock ||
                   result.outcome == ApproveOrderOutcome::ProducingInsufficientStock;
    std::cout << (success ? "[성공] " : "[실패] ") << result.message << "\n";
}

void ConsoleOrderApprovalView::ShowRejectResult(const RejectOrderResult& result) {
    std::cout << (result.outcome == RejectOrderOutcome::Success ? "[성공] " : "[실패] ")
              << result.message << "\n";
}

}  // namespace SampleOrderSystem
