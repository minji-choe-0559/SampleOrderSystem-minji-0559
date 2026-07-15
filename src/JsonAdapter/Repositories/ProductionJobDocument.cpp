#include "ProductionJobDocument.h"

#include "ProductionJobJsonMapper.h"

namespace SampleOrderSystem {

namespace ProductionJobDocument {

JsonValue toJson(const std::vector<ProductionJob>& jobs) {
    std::vector<JsonValue> items;
    items.reserve(jobs.size());
    for (const auto& job : jobs) {
        items.push_back(SampleOrderSystem::toJson(job));
    }
    return JsonValue(items);
}

std::vector<ProductionJob> fromJson(const std::vector<JsonValue>& items) {
    std::vector<ProductionJob> jobs;
    jobs.reserve(items.size());
    for (const auto& item : items) {
        jobs.push_back(SampleOrderSystem::fromJson(item));
    }
    return jobs;
}

}  // namespace ProductionJobDocument

}  // namespace SampleOrderSystem
