#pragma once

#include <string>

namespace SampleOrderSystem {

enum class ApproveOrderOutcome {
    ConfirmedSufficientStock,
    ProducingInsufficientStock,
    OrderNotFound,
    OrderNotReserved,
    // 시료 수율이 0 이하라 실 생산량(ceil(부족분 * 100 / 수율))을 계산할 수 없는 경우
    // (test-auditor 지적: yieldRate == 0이면 0으로 나누기가 되어 정의되지 않은 동작이 됨).
    InvalidYieldRate,
    StorageFailure,
};

// Presentation-layer result: ApproveOrderUseCase가 재고 판정 결과를 이 값으로 변환한 뒤
// Controller가 View에 전달한다.
struct ApproveOrderResult {
    ApproveOrderOutcome outcome = ApproveOrderOutcome::ConfirmedSufficientStock;
    std::string message;
};

}  // namespace SampleOrderSystem
