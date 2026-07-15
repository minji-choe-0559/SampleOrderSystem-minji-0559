#pragma once

#include <string>

namespace SampleOrderSystem {

enum class ApproveOrderOutcome {
    ConfirmedSufficientStock,
    ProducingInsufficientStock,
    OrderNotFound,
    OrderNotReserved,
    StorageFailure,
};

// Presentation-layer result: ApproveOrderUseCase가 재고 판정 결과를 이 값으로 변환한 뒤
// Controller가 View에 전달한다.
struct ApproveOrderResult {
    ApproveOrderOutcome outcome = ApproveOrderOutcome::ConfirmedSufficientStock;
    std::string message;
};

}  // namespace SampleOrderSystem
