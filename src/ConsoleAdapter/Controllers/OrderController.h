#pragma once

#include "IOrderView.h"
#include "ReserveOrderUseCase.h"

namespace SampleOrderSystem {

// Coordinates user flow only: forwards requests to ReserveOrderUseCase and results to IOrderView.
// Holds no owning pointers — non-owning references, lifetime owned by the Composition Root.
class OrderController {
  public:
    OrderController(IOrderView& view, ReserveOrderUseCase& reserveUseCase);

    void Run();

    void HandleReservation(const OrderReservationInput& input);

  private:
    IOrderView& view_;
    ReserveOrderUseCase& reserveUseCase_;
};

}  // namespace SampleOrderSystem
