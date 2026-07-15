#pragma once

#include <chrono>
#include <string>

#include "Guid.h"

namespace SampleOrderSystem {

// PRD.md 4.2 확정 상태 머신.
enum class ProductionJobStatus {
    Queued,
    InProgress,
    Completed,
};

// PRD.md 5.5.1 최소 데이터 구조(Phase 3 범위). scheduledStartAt/scheduledCompletionAt을 실제로
// 계산하는 FIFO 스케줄링 로직과 ProductionSchedule.Reconcile은 Phase 4에서 추가한다(PLAN.md
// Phase 3/4 범위 분리 — 이 구조체에는 아직 그 두 필드가 없다).
struct ProductionJob {
    Guid jobId;
    Guid orderId;
    std::string sampleCode;
    int quantity = 0;
    std::chrono::system_clock::time_point requestedAt;
    ProductionJobStatus status = ProductionJobStatus::Queued;
};

}  // namespace SampleOrderSystem
