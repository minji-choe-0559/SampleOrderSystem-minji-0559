#pragma once

#include <string>
#include <vector>

#include "IProductionQueueRepository.h"

namespace SampleOrderSystem {

// ProductionJob CRUD 전담 계층. Sample/Order와 같은 공유 JSON 문서를 사용한다(PRD.md 5.5.5) —
// SharedDocument로 문서를 읽고 "productionJobs" 필드만 갱신한다.
class ProductionQueueRepository : public IProductionQueueRepository {
  public:
    explicit ProductionQueueRepository(std::string path);

    // quantity <= 0이면 std::invalid_argument를 던진다.
    ProductionJob create(const Guid& orderId, const std::string& sampleCode, int quantity) override;

    std::vector<ProductionJob> readAll() const override;

  private:
    void writeAll(const std::vector<ProductionJob>& jobs) const;

    std::string path_;
};

}  // namespace SampleOrderSystem
