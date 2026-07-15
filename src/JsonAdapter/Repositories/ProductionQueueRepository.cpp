#include "ProductionQueueRepository.h"

#include <stdexcept>

#include "ProductionJobDocument.h"
#include "SharedDocument.h"

namespace SampleOrderSystem {

ProductionQueueRepository::ProductionQueueRepository(std::string path) : path_(std::move(path)) {}

std::vector<ProductionJob> ProductionQueueRepository::readAll() const {
    JsonValue document = ReadSharedDocument(path_);
    return ProductionJobDocument::fromJson(GetArrayField(document, "productionJobs"));
}

void ProductionQueueRepository::writeAll(const std::vector<ProductionJob>& jobs) const {
    JsonValue document = ReadSharedDocument(path_);
    JsonValue updated =
        WithArrayField(document, "productionJobs", ProductionJobDocument::toJson(jobs));
    WriteSharedDocument(path_, updated);
}

ProductionJob ProductionQueueRepository::create(const Guid& orderId, const std::string& sampleCode,
                                                int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("ProductionQueueRepository: quantity must be > 0");
    }

    std::vector<ProductionJob> jobs = readAll();
    ProductionJob job{Guid::NewGuid(),
                      orderId,
                      sampleCode,
                      quantity,
                      std::chrono::system_clock::now(),
                      ProductionJobStatus::Queued};
    jobs.push_back(job);
    writeAll(jobs);
    return job;
}

}  // namespace SampleOrderSystem
