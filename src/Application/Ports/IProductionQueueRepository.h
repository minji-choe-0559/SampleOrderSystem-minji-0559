#pragma once

#include <vector>

#include "Guid.h"
#include "ProductionJob.h"

namespace SampleOrderSystem {

// Application이 정의하는 Port. JsonAdapter의 ProductionQueueRepository가 이를 구현한다. Phase 3
// 범위는 최소 CRUD만 — FIFO 체이닝/정산은 Phase 4에서 추가한다.
class IProductionQueueRepository {
  public:
    virtual ~IProductionQueueRepository() = default;

    // quantity <= 0이면 std::invalid_argument를 던진다(PRD.md 5.5.7).
    virtual ProductionJob create(const Guid& orderId, const std::string& sampleCode,
                                 int quantity) = 0;

    virtual std::vector<ProductionJob> readAll() const = 0;
};

}  // namespace SampleOrderSystem
