#pragma once

#include <cstddef>

#include "IOrderView.h"

namespace SampleOrderSystem {

// Test double for IOrderView (FakeView와 동일한 패턴).
class FakeOrderView : public IOrderView {
  public:
    void ShowMenu() override { ++showMenuCallCount; }

    int ReadMenuChoice() override {
        if (menuChoiceIndex < menuChoiceSequence.size()) {
            return menuChoiceSequence[menuChoiceIndex++];
        }
        return nextMenuChoice;
    }

    OrderReservationInput ReadReservationInput() override { return nextReservationInput; }

    void ShowReservationResult(const OrderReservationResult& result) override {
        lastResult = result;
        ++reservationResultCallCount;
    }

    int nextMenuChoice = 0;
    std::vector<int> menuChoiceSequence;
    std::size_t menuChoiceIndex = 0;
    OrderReservationInput nextReservationInput{};
    OrderReservationResult lastResult{};
    int showMenuCallCount = 0;
    int reservationResultCallCount = 0;
};

}  // namespace SampleOrderSystem
