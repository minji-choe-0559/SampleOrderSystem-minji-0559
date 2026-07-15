#pragma once

#include <optional>
#include <string>
#include <vector>

#include "SampleRecord.h"

namespace SampleOrderSystem {

// SampleRecord CRUD 전담 계층. 파일 경로/파일 연산은 JsonStore에, 스키마 검증은 SampleDocument에
// 위임하고, 이 클래스는 CRUD 오케스트레이션·중복 검사·필드 유효성 검사·오류 변환만 책임진다.
// 콘솔 코드는 이 클래스를 통해서만 데이터에 접근하며 JsonValue/JsonStore를 직접 다루지 않는다.
class SampleRepository {
  public:
    explicit SampleRepository(std::string path);

    // sampleCode 중복(대소문자 무시, trim 후 비교) 시 std::invalid_argument를 던진다.
    // 필드 유효성 위반 시에도 std::invalid_argument를 던진다.
    SampleRecord create(const std::string& sampleCode, const std::string& name,
                        double avgProcessTime, double yieldRate, int stock);

    std::vector<SampleRecord> readAll() const;

    std::optional<SampleRecord> findBySampleCode(const std::string& sampleCode) const;

    // currentSampleCode로 대상을 찾지 못하면 std::nullopt를 반환한다(예외 아님).
    // newSampleCode가 자기 자신을 제외한 다른 레코드와 중복되면 std::invalid_argument를 던진다.
    std::optional<SampleRecord> update(const std::string& currentSampleCode,
                                       const std::string& newSampleCode, const std::string& name,
                                       double avgProcessTime, double yieldRate, int stock);

    // 대상을 찾지 못하면 false를 반환한다(예외 아님).
    bool remove(const std::string& sampleCode);

  private:
    void writeAll(const std::vector<SampleRecord>& records) const;

    std::string path_;
};

}  // namespace SampleOrderSystem
