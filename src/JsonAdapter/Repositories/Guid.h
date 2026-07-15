#pragma once

#include <string>

namespace SampleOrderSystem {

// SampleRecord.Id를 위한 값 타입. C++에는 C#의 System.Guid가 없어 최소한의 RFC 4122 v4 UUID
// 문자열 래퍼로 직접 구현한다(외부 라이브러리 미사용 원칙 유지). 항상 NewGuid()/Parse()로만
// 생성하며 기본 생성자를 두지 않아, 값이 없는 상태의 Guid가 실수로 만들어지지 않게 한다.
class Guid {
  public:
    static Guid NewGuid();
    static Guid Parse(const std::string& text);

    const std::string& toString() const;

    bool operator==(const Guid& other) const;
    bool operator!=(const Guid& other) const;

  private:
    explicit Guid(std::string value);
    std::string value_;
};

}  // namespace SampleOrderSystem
