#pragma once

#include <optional>
#include <string>
#include <vector>

#include "SampleRecord.h"

namespace SampleOrderSystem {

// Application이 정의하는 Port. JsonAdapter의 SampleRepository가 이를 구현한다 — Application/
// Domain은 구체 Adapter가 아니라 이 인터페이스에만 의존한다(CLAUDE.md 아키텍처 규칙).
class ISampleRepository {
  public:
    virtual ~ISampleRepository() = default;

    virtual SampleRecord create(const std::string& sampleCode, const std::string& name,
                                double avgProcessTime, double yieldRate, int stock) = 0;

    virtual std::vector<SampleRecord> readAll() const = 0;

    virtual std::optional<SampleRecord> findBySampleCode(const std::string& sampleCode) const = 0;
};

}  // namespace SampleOrderSystem
