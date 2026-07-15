#include "SampleDocument.h"

#include "SampleJsonMapper.h"

namespace SampleOrderSystem {

namespace SampleDocument {

JsonValue toJson(const std::vector<SampleRecord>& records) {
    std::vector<JsonValue> items;
    items.reserve(records.size());
    for (const auto& record : records) {
        items.push_back(SampleOrderSystem::toJson(record));
    }
    return JsonValue(items);
}

std::vector<SampleRecord> fromJson(const std::vector<JsonValue>& items) {
    std::vector<SampleRecord> records;
    records.reserve(items.size());
    for (const auto& item : items) {
        records.push_back(SampleOrderSystem::fromJson(item));
    }
    return records;
}

}  // namespace SampleDocument

}  // namespace SampleOrderSystem
