#pragma once

#include "OrderReservationInput.h"
#include "OrderReservationResult.h"

namespace SampleOrderSystem {

// Passive View contract: only Presentation types cross this boundary. Controller decides the
// flow; View only performs raw console I/O.
class IOrderView {
  public:
    virtual ~IOrderView() = default;

    virtual void ShowMenu() = 0;
    virtual int ReadMenuChoice() = 0;
    virtual OrderReservationInput ReadReservationInput() = 0;
    virtual void ShowReservationResult(const OrderReservationResult& result) = 0;
};

}  // namespace SampleOrderSystem
