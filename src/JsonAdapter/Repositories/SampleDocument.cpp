#include "SampleDocument.h"

#include <stdexcept>

#include "SampleJsonMapper.h"

namespace SampleOrderSystem {

namespace SampleDocument {

JsonValue toJson(const std::vector<SampleRecord>& records) {
    std::vector<JsonValue> items;
    items.reserve(records.size());
    for (const auto& record : records) {
        items.push_back(SampleOrderSystem::toJson(record));
    }
    std::map<std::string, JsonValue> fields;
    fields["schemaVersion"] = JsonValue(kCurrentSchemaVersion);
    fields["records"] = JsonValue(items);
    return JsonValue(fields);
}

std::vector<SampleRecord> fromJson(const JsonValue& document) {
    if (!document.isObject()) {
        throw std::runtime_error(
            "SampleDocument: unsupported root — expected a versioned object document "
            "{schemaVersion, records}; legacy bare-array JSON_CRUD files are not supported");
    }

    const auto& fields = document.asObject();

    auto versionIt = fields.find("schemaVersion");
    if (versionIt == fields.end() || !versionIt->second.isNumber()) {
        throw std::runtime_error("SampleDocument: missing or invalid schemaVersion");
    }
    int version = static_cast<int>(versionIt->second.asNumber());
    if (version != kCurrentSchemaVersion) {
        throw std::runtime_error("SampleDocument: unsupported schemaVersion " +
                                 std::to_string(version) +
                                 " (supported: " + std::to_string(kCurrentSchemaVersion) + ")");
    }

    auto recordsIt = fields.find("records");
    if (recordsIt == fields.end() || !recordsIt->second.isArray()) {
        throw std::runtime_error("SampleDocument: missing or invalid records field");
    }

    std::vector<SampleRecord> records;
    records.reserve(recordsIt->second.asArray().size());
    for (const auto& item : recordsIt->second.asArray()) {
        records.push_back(SampleOrderSystem::fromJson(item));
    }
    return records;
}

}  // namespace SampleDocument

}  // namespace SampleOrderSystem
