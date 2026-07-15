#include "SampleJsonMapper.h"

namespace SampleOrderSystem {

JsonValue toJson(const SampleRecord& record) {
    std::map<std::string, JsonValue> fields;
    fields["id"] = JsonValue(record.id.toString());
    fields["sampleCode"] = JsonValue(record.sampleCode);
    fields["name"] = JsonValue(record.name);
    fields["avgProcessTime"] = JsonValue(record.avgProcessTime);
    fields["yield"] = JsonValue(record.yieldRate);
    fields["stock"] = JsonValue(record.stock);
    return JsonValue(fields);
}

SampleRecord fromJson(const JsonValue& value) {
    return SampleRecord{
        Guid::Parse(value["id"].asString()),
        value["sampleCode"].asString(),
        value["name"].asString(),
        value["avgProcessTime"].asNumber(),
        value["yield"].asNumber(),
        static_cast<int>(value["stock"].asNumber()),
    };
}

}  // namespace SampleOrderSystem
