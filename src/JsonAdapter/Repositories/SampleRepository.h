#pragma once

#include <optional>
#include <string>
#include <vector>

#include "ISampleRepository.h"
#include "SampleRecord.h"

namespace SampleOrderSystem {

// SampleRecord CRUD 전담 계층. 파일 경로/파일 연산은 JsonStore에, 스키마 검증은 SampleDocument에
// 위임하고, 이 클래스는 CRUD 오케스트레이션·중복 검사·필드 유효성 검사·오류 변환만 책임진다.
// 콘솔 코드는 이 클래스를 통해서만 데이터에 접근하며 JsonValue/JsonStore를 직접 다루지 않는다.
class SampleRepository : public ISampleRepository {
  public:
    explicit SampleRepository(std::string path);

    // sampleCode 중복(대소문자 무시, trim 후 비교) 시 std::invalid_argument를 던진다.
    // 필드 유효성 위반 시에도 std::invalid_argument를 던진다.
    SampleRecord create(const std::string& sampleCode, const std::string& name,
                        double avgProcessTime, double yieldRate, int stock) override;

    std::vector<SampleRecord> readAll() const override;

    std::optional<SampleRecord> findBySampleCode(const std::string& sampleCode) const override;

    // currentSampleCode로 대상을 찾지 못하면 std::nullopt를 반환한다(예외 아님).
    // newSampleCode가 자기 자신을 제외한 다른 레코드와 중복되면 std::invalid_argument를 던진다.
    // Phase 1(시료 관리)에는 수정 기능이 없어 ISampleRepository Port에는 노출하지 않는다 — 필요한
    // Phase에서 실제로 쓰일 때 Port에 추가하고 테스트를 붙인다(YAGNI, test-auditor 지적 반영).
    std::optional<SampleRecord> update(const std::string& currentSampleCode,
                                       const std::string& newSampleCode, const std::string& name,
                                       double avgProcessTime, double yieldRate, int stock);

    // 대상을 찾지 못하면 false를 반환한다(예외 아님). update와 마찬가지로 Port에는 없다.
    bool remove(const std::string& sampleCode);

  private:
    void writeAll(const std::vector<SampleRecord>& records) const;

    std::string path_;
};

}  // namespace SampleOrderSystem
