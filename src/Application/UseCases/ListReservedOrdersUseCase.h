#pragma once

#include <vector>

#include "IOrderRepository.h"
#include "OrderViewModel.h"

namespace SampleOrderSystem {

// PRD.md 5.4 "접수된 주문 목록" — RESERVED 상태 주문만 표시한다.
class ListReservedOrdersUseCase {
  public:
    explicit ListReservedOrdersUseCase(const IOrderRepository& orderRepository);

    std::vector<OrderViewModel> ListReserved() const;

  private:
    const IOrderRepository& orderRepository_;
};

}  // namespace SampleOrderSystem
