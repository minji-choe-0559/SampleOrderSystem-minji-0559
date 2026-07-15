#pragma once

#include <string>

#include "ApproveOrderResult.h"
#include "IOrderRepository.h"
#include "IProductionQueueRepository.h"
#include "ISampleRepository.h"

namespace SampleOrderSystem {

// PRD.md 5.4 "주문 승인" — 재고 판정에 따라 자동 분기한다.
// 재고 충분: 시료 재고를 즉시 차감하고 주문을 CONFIRMED로 전환한다(PRD.md 5.4 확정).
// 재고 부족: ProductionJob을 생성해 큐에 등록하고 주문을 PRODUCING으로 전환한다.
class ApproveOrderUseCase {
  public:
    ApproveOrderUseCase(IOrderRepository& orderRepository, ISampleRepository& sampleRepository,
                        IProductionQueueRepository& productionQueueRepository);

    ApproveOrderResult Approve(const std::string& orderNumber);

  private:
    IOrderRepository& orderRepository_;
    ISampleRepository& sampleRepository_;
    IProductionQueueRepository& productionQueueRepository_;
};

}  // namespace SampleOrderSystem
