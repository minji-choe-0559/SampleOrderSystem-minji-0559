#pragma once

#include "IOrderRepository.h"
#include "ISampleRepository.h"
#include "OrderReservationInput.h"
#include "OrderReservationResult.h"

namespace SampleOrderSystem {

// PRD.md 5.3 "시료 주문(예약)". 시료 존재 여부는 ISampleRepository로 확인해(존재하지 않는 시료를
// 참조하는 주문이 생기지 않도록), 확인 후에만 IOrderRepository에 실제 생성/채번/유효성 검사를
// 위임한다.
class ReserveOrderUseCase {
  public:
    ReserveOrderUseCase(ISampleRepository& sampleRepository, IOrderRepository& orderRepository);

    OrderReservationResult Reserve(const OrderReservationInput& input);

  private:
    ISampleRepository& sampleRepository_;
    IOrderRepository& orderRepository_;
};

}  // namespace SampleOrderSystem
