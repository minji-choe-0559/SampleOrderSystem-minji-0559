#pragma once

#include "FakeOrderRepository.h"
#include "FakeProductionQueueRepository.h"
#include "FakeSampleRepository.h"
#include "IOrderApprovalRepository.h"

namespace SampleOrderSystem {

// 실제 파일 I/O 없이 ApproveOrderUseCase를 검증하기 위한 인메모리 Fake. 기존 3개 Fake
// Repository에 위임해, 실제 OrderApprovalRepository가 하나의 파일 쓰기로 묶는 것과 같은 결과
// (재고/주문 상태/생산 큐가 함께 갱신됨)를 테스트에서 관찰할 수 있게 한다.
class FakeOrderApprovalRepository : public IOrderApprovalRepository {
  public:
    FakeOrderApprovalRepository(FakeSampleRepository& sampleRepository,
                                FakeOrderRepository& orderRepository,
                                FakeProductionQueueRepository& productionQueueRepository)
        : sampleRepository_(sampleRepository),
          orderRepository_(orderRepository),
          productionQueueRepository_(productionQueueRepository) {}

    bool ConfirmWithStockDeduction(const std::string& orderNumber, const std::string& sampleCode,
                                   int quantity) override {
        if (!sampleRepository_.findBySampleCode(sampleCode).has_value()) {
            return false;
        }
        if (!orderRepository_.findByOrderNumber(orderNumber).has_value()) {
            return false;
        }
        sampleRepository_.adjustStock(sampleCode, -quantity);
        orderRepository_.updateStatus(orderNumber, OrderStatus::Confirmed);
        return true;
    }

    bool TransitionToProducing(const std::string& orderNumber, const Guid& orderId,
                               const std::string& sampleCode, int productionQuantity) override {
        if (!orderRepository_.findByOrderNumber(orderNumber).has_value()) {
            return false;
        }
        productionQueueRepository_.create(orderId, sampleCode, productionQuantity);
        orderRepository_.updateStatus(orderNumber, OrderStatus::Producing);
        return true;
    }

  private:
    FakeSampleRepository& sampleRepository_;
    FakeOrderRepository& orderRepository_;
    FakeProductionQueueRepository& productionQueueRepository_;
};

}  // namespace SampleOrderSystem
