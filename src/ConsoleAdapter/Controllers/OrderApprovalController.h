#pragma once

#include "ApproveOrderUseCase.h"
#include "IOrderApprovalView.h"
#include "ListReservedOrdersUseCase.h"
#include "RejectOrderUseCase.h"

namespace SampleOrderSystem {

// Coordinates user flow only: forwards requests to the three UseCases and results to
// IOrderApprovalView. Non-owning references, lifetime owned by the Composition Root.
class OrderApprovalController {
  public:
    OrderApprovalController(IOrderApprovalView& view, ListReservedOrdersUseCase& listUseCase,
                            ApproveOrderUseCase& approveUseCase, RejectOrderUseCase& rejectUseCase);

    void Run();

    void HandleListReserved();
    void HandleApprove(const std::string& orderNumber);
    void HandleReject(const std::string& orderNumber);

  private:
    IOrderApprovalView& view_;
    ListReservedOrdersUseCase& listUseCase_;
    ApproveOrderUseCase& approveUseCase_;
    RejectOrderUseCase& rejectUseCase_;
};

}  // namespace SampleOrderSystem
