#include "ApproveOrderUseCase.h"

#include <cmath>
#include <stdexcept>

namespace SampleOrderSystem {

ApproveOrderUseCase::ApproveOrderUseCase(IOrderRepository& orderRepository,
                                         ISampleRepository& sampleRepository,
                                         IProductionQueueRepository& productionQueueRepository)
    : orderRepository_(orderRepository),
      sampleRepository_(sampleRepository),
      productionQueueRepository_(productionQueueRepository) {}

ApproveOrderResult ApproveOrderUseCase::Approve(const std::string& orderNumber) {
    auto orderOpt = orderRepository_.findByOrderNumber(orderNumber);
    if (!orderOpt.has_value()) {
        return ApproveOrderResult{ApproveOrderOutcome::OrderNotFound,
                                  "주문을 찾을 수 없습니다: " + orderNumber};
    }
    if (orderOpt->status != OrderStatus::Reserved) {
        return ApproveOrderResult{ApproveOrderOutcome::OrderNotReserved,
                                  "RESERVED 상태의 주문만 승인할 수 있습니다."};
    }

    auto sampleOpt = sampleRepository_.findBySampleCode(orderOpt->sampleCode);
    if (!sampleOpt.has_value()) {
        // ReserveOrderUseCase가 생성 시점에 시료 존재를 이미 확인했으므로, 여기서 못 찾으면
        // 저장소가 손상된 예외 상황이다.
        return ApproveOrderResult{ApproveOrderOutcome::StorageFailure,
                                  "연결된 시료를 찾을 수 없습니다: " + orderOpt->sampleCode};
    }

    try {
        if (sampleOpt->stock >= orderOpt->quantity) {
            sampleRepository_.adjustStock(orderOpt->sampleCode, -orderOpt->quantity);
            orderRepository_.updateStatus(orderNumber, OrderStatus::Confirmed);
            return ApproveOrderResult{ApproveOrderOutcome::ConfirmedSufficientStock,
                                      "재고 충분 — 즉시 출고 대기(CONFIRMED) 처리되었습니다."};
        }

        int shortfall = orderOpt->quantity - sampleOpt->stock;
        int actualQuantity = static_cast<int>(std::ceil(shortfall * 100.0 / sampleOpt->yieldRate));
        productionQueueRepository_.create(orderOpt->orderId, orderOpt->sampleCode, actualQuantity);
        orderRepository_.updateStatus(orderNumber, OrderStatus::Producing);
        return ApproveOrderResult{
            ApproveOrderOutcome::ProducingInsufficientStock,
            "재고 부족 — 생산 큐에 등록되어 생산 중(PRODUCING) 처리되었습니다."};
    } catch (const std::exception& e) {
        return ApproveOrderResult{ApproveOrderOutcome::StorageFailure, e.what()};
    }
}

}  // namespace SampleOrderSystem
