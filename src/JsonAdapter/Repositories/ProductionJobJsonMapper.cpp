#include "ProductionJobJsonMapper.h"

#include <stdexcept>

namespace SampleOrderSystem {

namespace {

std::string ToString(ProductionJobStatus status) {
    switch (status) {
        case ProductionJobStatus::Queued:
            return "QUEUED";
        case ProductionJobStatus::InProgress:
            return "IN_PROGRESS";
        case ProductionJobStatus::Completed:
            return "COMPLETED";
    }
    throw std::invalid_argument("ProductionJobJsonMapper: unknown ProductionJobStatus");
}

ProductionJobStatus ParseStatus(const std::string& text) {
    if (text == "QUEUED") return ProductionJobStatus::Queued;
    if (text == "IN_PROGRESS") return ProductionJobStatus::InProgress;
    if (text == "COMPLETED") return ProductionJobStatus::Completed;
    throw std::runtime_error("ProductionJobJsonMapper: unknown status '" + text + "'");
}

}  // namespace

JsonValue toJson(const ProductionJob& job) {
    auto epochMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(job.requestedAt.time_since_epoch())
            .count();

    std::map<std::string, JsonValue> fields;
    fields["jobId"] = JsonValue(job.jobId.toString());
    fields["orderId"] = JsonValue(job.orderId.toString());
    fields["sampleCode"] = JsonValue(job.sampleCode);
    fields["quantity"] = JsonValue(job.quantity);
    fields["requestedAt"] = JsonValue(static_cast<double>(epochMs));
    fields["status"] = JsonValue(ToString(job.status));
    return JsonValue(fields);
}

ProductionJob fromJson(const JsonValue& value) {
    auto epochMs = static_cast<long long>(value["requestedAt"].asNumber());
    return ProductionJob{
        Guid::Parse(value["jobId"].asString()),
        Guid::Parse(value["orderId"].asString()),
        value["sampleCode"].asString(),
        static_cast<int>(value["quantity"].asNumber()),
        std::chrono::system_clock::time_point(std::chrono::milliseconds(epochMs)),
        ParseStatus(value["status"].asString()),
    };
}

}  // namespace SampleOrderSystem
