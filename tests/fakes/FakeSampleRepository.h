#pragma once

#include <algorithm>
#include <cctype>
#include <stdexcept>

#include "ISampleRepository.h"

namespace SampleOrderSystem {

// 실제 파일 I/O 없이 UseCase를 검증하기 위한 인메모리 Fake. 실제 SampleRepository와 동일하게
// sampleCode 중복(대소문자 무시)·빈 값·필드 유효성 경계값(avgProcessTime/yieldRate/stock)을
// 거부해, UseCase가 그 예외를 올바르게 변환하는지 테스트할 수 있게 한다(test-auditor 지적:
// 두 구현의 검증 로직이 갈라지면 Fake로 통과한 테스트가 실제 동작을 보장하지 못한다).
class FakeSampleRepository : public ISampleRepository {
  public:
    SampleRecord create(const std::string& sampleCode, const std::string& name,
                        double avgProcessTime, double yieldRate, int stock) override {
        if (sampleCode.empty()) {
            throw std::invalid_argument("FakeSampleRepository: sampleCode must not be blank");
        }
        if (name.empty()) {
            throw std::invalid_argument("FakeSampleRepository: name must not be blank");
        }
        if (avgProcessTime < 0.0) {
            throw std::invalid_argument("FakeSampleRepository: avgProcessTime must be >= 0");
        }
        if (yieldRate < 0.0 || yieldRate > 100.0) {
            throw std::invalid_argument("FakeSampleRepository: yieldRate must be within [0, 100]");
        }
        if (stock < 0) {
            throw std::invalid_argument("FakeSampleRepository: stock must be >= 0");
        }
        for (const auto& existing : records_) {
            if (EqualsIgnoreCase(existing.sampleCode, sampleCode)) {
                throw std::invalid_argument("FakeSampleRepository: duplicate sampleCode '" +
                                            sampleCode + "'");
            }
        }
        SampleRecord record{Guid::NewGuid(), sampleCode, name, avgProcessTime, yieldRate, stock};
        records_.push_back(record);
        return record;
    }

    std::vector<SampleRecord> readAll() const override { return records_; }

    std::optional<SampleRecord> findBySampleCode(const std::string& sampleCode) const override {
        auto it = std::find_if(records_.begin(), records_.end(), [&](const SampleRecord& record) {
            return record.sampleCode == sampleCode;
        });
        if (it == records_.end()) {
            return std::nullopt;
        }
        return *it;
    }

    void Seed(const SampleRecord& record) { records_.push_back(record); }

  private:
    static bool EqualsIgnoreCase(const std::string& a, const std::string& b) {
        if (a.size() != b.size()) {
            return false;
        }
        return std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
            return std::tolower(static_cast<unsigned char>(x)) ==
                   std::tolower(static_cast<unsigned char>(y));
        });
    }

    std::vector<SampleRecord> records_;
};

}  // namespace SampleOrderSystem
