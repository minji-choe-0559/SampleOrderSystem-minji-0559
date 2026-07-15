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

    // sampleCode를 찾지 못하면 std::nullopt를 반환한다(예외 아님). delta 적용 후 재고가 음수가
    // 되면 std::invalid_argument를 던진다(PRD.md 5.4 — 재고 충분 판정 후에만 호출되므로 정상
    // 흐름에서는 도달하지 않지만, Repository가 최종 방어선을 유지한다).
    virtual std::optional<SampleRecord> adjustStock(const std::string& sampleCode, int delta) = 0;
};

}  // namespace SampleOrderSystem
