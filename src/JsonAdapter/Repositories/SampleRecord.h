#pragma once

#include <string>

#include "Guid.h"
#include "JsonValue.h"

namespace SampleOrderSystem {

// POC_SPEC.md 7장에 확정된 6개 필드. Id는 시스템이 발급하며(Guid::NewGuid), 사용자 입력 경로가
// 없다 — Repository는 어떤 CRUD 경로로도 기존 레코드의 id 필드를 덮어쓰지 않는다.
struct SampleRecord {
    Guid id;
    std::string sampleCode;
    std::string name;
    double avgProcessTime = 0.0;  // 분(minutes) 단위, 0 이상
    double yieldRate = 0.0;       // 0~100 백분율. JSON 필드명은 "yield"
    int stock = 0;                // 0 이상의 정수
};

JsonValue toJson(const SampleRecord& record);
SampleRecord fromJson(const JsonValue& value);

}  // namespace SampleOrderSystem
