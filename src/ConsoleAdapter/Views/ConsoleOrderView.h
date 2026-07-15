#pragma once

#include "IOrderView.h"

namespace SampleOrderSystem {

// Concrete Passive View: reads raw console input via std::getline and prints ViewModels/Results.
class ConsoleOrderView : public IOrderView {
  public:
    void ShowMenu() override;
    int ReadMenuChoice() override;
    OrderReservationInput ReadReservationInput() override;
    void ShowReservationResult(const OrderReservationResult& result) override;
};

}  // namespace SampleOrderSystem
