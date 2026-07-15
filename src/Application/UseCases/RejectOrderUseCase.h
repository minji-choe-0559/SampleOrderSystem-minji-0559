#pragma once

#include <string>

#include "IOrderRepository.h"
#include "RejectOrderResult.h"

namespace SampleOrderSystem {

// PRD.md 5.4 "주문 거절" — RESERVED 주문을 즉시 REJECTED로 전환한다.
class RejectOrderUseCase {
  public:
    explicit RejectOrderUseCase(IOrderRepository& orderRepository);

    RejectOrderResult Reject(const std::string& orderNumber);

  private:
    IOrderRepository& orderRepository_;
};

}  // namespace SampleOrderSystem
