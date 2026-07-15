#pragma once

#include <cstddef>

#include "IOrderApprovalView.h"

namespace SampleOrderSystem {

// Test double for IOrderApprovalView (FakeView/FakeOrderView와 동일한 패턴).
class FakeOrderApprovalView : public IOrderApprovalView {
  public:
    void ShowMenu() override { ++showMenuCallCount; }

    int ReadMenuChoice() override {
        if (menuChoiceIndex < menuChoiceSequence.size()) {
            return menuChoiceSequence[menuChoiceIndex++];
        }
        return nextMenuChoice;
    }

    void ShowReservedOrders(const std::vector<OrderViewModel>& orders) override {
        lastReservedOrders = orders;
        ++showReservedOrdersCallCount;
    }

    std::string ReadOrderNumber() override {
        if (orderNumberIndex < orderNumberSequence.size()) {
            return orderNumberSequence[orderNumberIndex++];
        }
        return nextOrderNumber;
    }

    void ShowApproveResult(const ApproveOrderResult& result) override {
        lastApproveResult = result;
        ++approveResultCallCount;
    }

    void ShowRejectResult(const RejectOrderResult& result) override {
        lastRejectResult = result;
        ++rejectResultCallCount;
    }

    int nextMenuChoice = 0;
    std::vector<int> menuChoiceSequence;
    std::size_t menuChoiceIndex = 0;
    std::string nextOrderNumber;
    std::vector<std::string> orderNumberSequence;
    std::size_t orderNumberIndex = 0;
    std::vector<OrderViewModel> lastReservedOrders;
    ApproveOrderResult lastApproveResult{};
    RejectOrderResult lastRejectResult{};
    int showMenuCallCount = 0;
    int showReservedOrdersCallCount = 0;
    int approveResultCallCount = 0;
    int rejectResultCallCount = 0;
};

}  // namespace SampleOrderSystem
