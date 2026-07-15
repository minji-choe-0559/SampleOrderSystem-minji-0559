#pragma once

#include <string>
#include <vector>

#include "JsonValue.h"

namespace SampleOrderSystem {

inline constexpr int kSharedDocumentSchemaVersion = 1;

// Sample/Order/ProductionJob이 공유하는 하나의 JSON 문서(PRD.md 5.5.5)의 루트를 읽는다. 파일이
// 없으면 빈 문서({schemaVersion})를 반환한다. object 루트가 아니거나 schemaVersion이 없거나 지원
// 범위를 벗어나면 std::runtime_error를 던진다(구형 bare array 포함).
JsonValue ReadSharedDocument(const std::string& path);

// 원자적으로 파일에 문서를 저장한다(상위 디렉터리 생성 포함, JsonStore::write에 위임).
void WriteSharedDocument(const std::string& path, const JsonValue& document);

// document에서 field로 지정된 배열만 arrayValue로 교체한 새 문서를 반환한다. 다른 필드(다른
// 애그리게잇의 배열)는 그대로 보존된다 — 여러 Repository가 같은 파일을 공유해도 서로의 데이터를
// 덮어쓰지 않기 위한 핵심 규칙.
JsonValue WithArrayField(const JsonValue& document, const std::string& field, JsonValue arrayValue);

// field에 해당하는 배열을 꺼낸다. 없으면 빈 배열을 반환한다(그 애그리게잇 데이터가 아직 없는 문서).
std::vector<JsonValue> GetArrayField(const JsonValue& document, const std::string& field);

}  // namespace SampleOrderSystem
