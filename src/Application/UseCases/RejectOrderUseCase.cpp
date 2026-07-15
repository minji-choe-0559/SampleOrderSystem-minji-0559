#include "RejectOrderUseCase.h"

namespace SampleOrderSystem {

RejectOrderUseCase::RejectOrderUseCase(IOrderRepository& orderRepository)
    : orderRepository_(orderRepository) {}

RejectOrderResult RejectOrderUseCase::Reject(const std::string& orderNumber) {
    auto orderOpt = orderRepository_.findByOrderNumber(orderNumber);
    if (!orderOpt.has_value()) {
        return RejectOrderResult{RejectOrderOutcome::OrderNotFound,
                                 "주문을 찾을 수 없습니다: " + orderNumber};
    }
    if (orderOpt->status != OrderStatus::Reserved) {
        return RejectOrderResult{RejectOrderOutcome::OrderNotReserved,
                                 "RESERVED 상태의 주문만 거절할 수 있습니다."};
    }

    orderRepository_.updateStatus(orderNumber, OrderStatus::Rejected);
    return RejectOrderResult{RejectOrderOutcome::Success, "주문이 거절되었습니다."};
}

}  // namespace SampleOrderSystem
