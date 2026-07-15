#include "OrderController.h"

namespace SampleOrderSystem {

namespace {
constexpr int kMenuBack = 0;
constexpr int kMenuReserve = 1;
}  // namespace

OrderController::OrderController(IOrderView& view, ReserveOrderUseCase& reserveUseCase)
    : view_(view), reserveUseCase_(reserveUseCase) {}

void OrderController::Run() {
    bool running = true;
    while (running) {
        view_.ShowMenu();
        switch (view_.ReadMenuChoice()) {
            case kMenuReserve:
                HandleReservation(view_.ReadReservationInput());
                break;
            case kMenuBack:
                running = false;
                break;
            default:
                break;
        }
    }
}

void OrderController::HandleReservation(const OrderReservationInput& input) {
    view_.ShowReservationResult(reserveUseCase_.Reserve(input));
}

}  // namespace SampleOrderSystem
