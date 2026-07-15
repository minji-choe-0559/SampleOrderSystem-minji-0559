#pragma once

#include <string>
#include <vector>

#include "ApproveOrderResult.h"
#include "OrderViewModel.h"
#include "RejectOrderResult.h"

namespace SampleOrderSystem {

// Passive View contract: only Presentation types cross this boundary.
class IOrderApprovalView {
  public:
    virtual ~IOrderApprovalView() = default;

    virtual void ShowMenu() = 0;
    virtual int ReadMenuChoice() = 0;
    virtual void ShowReservedOrders(const std::vector<OrderViewModel>& orders) = 0;
    virtual std::string ReadOrderNumber() = 0;
    virtual void ShowApproveResult(const ApproveOrderResult& result) = 0;
    virtual void ShowRejectResult(const RejectOrderResult& result) = 0;
};

}  // namespace SampleOrderSystem
