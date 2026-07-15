#pragma once

#include <stdexcept>

#include "IProductionQueueRepository.h"

namespace SampleOrderSystem {

// 실제 파일 I/O 없이 ApproveOrderUseCase를 검증하기 위한 인메모리 Fake.
class FakeProductionQueueRepository : public IProductionQueueRepository {
  public:
    ProductionJob create(const Guid& orderId, const std::string& sampleCode,
                         int quantity) override {
        if (quantity <= 0) {
            throw std::invalid_argument("FakeProductionQueueRepository: quantity must be > 0");
        }
        ProductionJob job{Guid::NewGuid(),
                          orderId,
                          sampleCode,
                          quantity,
                          std::chrono::system_clock::now(),
                          ProductionJobStatus::Queued};
        jobs_.push_back(job);
        return job;
    }

    std::vector<ProductionJob> readAll() const override { return jobs_; }

  private:
    std::vector<ProductionJob> jobs_;
};

}  // namespace SampleOrderSystem
