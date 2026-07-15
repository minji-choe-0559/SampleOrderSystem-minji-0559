#pragma once

#include "IOrderApprovalView.h"

namespace SampleOrderSystem {

// Concrete Passive View: reads raw console input via std::getline and prints ViewModels/Results.
class ConsoleOrderApprovalView : public IOrderApprovalView {
  public:
    void ShowMenu() override;
    int ReadMenuChoice() override;
    void ShowReservedOrders(const std::vector<OrderViewModel>& orders) override;
    std::string ReadOrderNumber() override;
    void ShowApproveResult(const ApproveOrderResult& result) override;
    void ShowRejectResult(const RejectOrderResult& result) override;
};

}  // namespace SampleOrderSystem
