#pragma once

#include <vector>

#include "JsonValue.h"
#include "SampleRecord.h"

namespace SampleOrderSystem {

// docs/DATA_CONTRACT.md v1 계약({schemaVersion, records})의 스키마 검증 책임을 전담한다.
// JsonStore/JsonParser/JsonWriter(엔진 계층)는 그대로 두고, 이 계층에서만 schemaVersion 검사와
// bare array(구형 JSON_CRUD 포맷) 거부를 수행한다.
namespace SampleDocument {

inline constexpr int kCurrentSchemaVersion = 1;

JsonValue toJson(const std::vector<SampleRecord>& records);

// document가 object 루트가 아니거나(구형 bare array 포함), schemaVersion이 없거나 지원 범위를
// 벗어나거나, records 필드가 없거나 배열이 아니면 std::runtime_error를 던진다.
std::vector<SampleRecord> fromJson(const JsonValue& document);

}  // namespace SampleDocument

}  // namespace SampleOrderSystem
