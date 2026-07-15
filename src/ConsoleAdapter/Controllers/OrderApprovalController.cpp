#include "OrderApprovalController.h"

namespace SampleOrderSystem {

namespace {
constexpr int kMenuBack = 0;
constexpr int kMenuListReserved = 1;
constexpr int kMenuApprove = 2;
constexpr int kMenuReject = 3;
}  // namespace

OrderApprovalController::OrderApprovalController(IOrderApprovalView& view,
                                                 ListReservedOrdersUseCase& listUseCase,
                                                 ApproveOrderUseCase& approveUseCase,
                                                 RejectOrderUseCase& rejectUseCase)
    : view_(view),
      listUseCase_(listUseCase),
      approveUseCase_(approveUseCase),
      rejectUseCase_(rejectUseCase) {}

void OrderApprovalController::Run() {
    bool running = true;
    while (running) {
        view_.ShowMenu();
        switch (view_.ReadMenuChoice()) {
            case kMenuListReserved:
                HandleListReserved();
                break;
            case kMenuApprove:
                HandleApprove(view_.ReadOrderNumber());
                break;
            case kMenuReject:
                HandleReject(view_.ReadOrderNumber());
                break;
            case kMenuBack:
                running = false;
                break;
            default:
                break;
        }
    }
}

void OrderApprovalController::HandleListReserved() {
    view_.ShowReservedOrders(listUseCase_.ListReserved());
}

void OrderApprovalController::HandleApprove(const std::string& orderNumber) {
    view_.ShowApproveResult(approveUseCase_.Approve(orderNumber));
}

void OrderApprovalController::HandleReject(const std::string& orderNumber) {
    view_.ShowRejectResult(rejectUseCase_.Reject(orderNumber));
}

}  // namespace SampleOrderSystem
