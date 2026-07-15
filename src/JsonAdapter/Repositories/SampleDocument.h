#pragma once

#include <vector>

#include "JsonValue.h"
#include "SampleRecord.h"

namespace SampleOrderSystem {

// SampleRecord 배열 <-> JsonValue 배열 변환만 전담한다. 문서 루트/schemaVersion 검증은
// SharedDocument로 옮겨졌다 — Sample/Order/ProductionJob이 하나의 공유 문서를 쓰기 때문
// (PRD.md 5.5.5).
namespace SampleDocument {

JsonValue toJson(const std::vector<SampleRecord>& records);

std::vector<SampleRecord> fromJson(const std::vector<JsonValue>& items);

}  // namespace SampleDocument

}  // namespace SampleOrderSystem
