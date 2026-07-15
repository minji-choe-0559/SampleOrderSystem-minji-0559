#pragma once

#include "ISampleRepository.h"
#include "SampleRegistrationInput.h"
#include "SampleRegistrationResult.h"

namespace SampleOrderSystem {

// PRD.md 5.2 "시료 등록". Repository가 중복 판정/필드 유효성 검사의 최종 책임을 지므로(CLAUDE.md
// 아키텍처 규칙), 이 UseCase는 Repository의 예외를 Presentation 결과로 변환만 한다.
class RegisterSampleUseCase {
  public:
    explicit RegisterSampleUseCase(ISampleRepository& repository);

    SampleRegistrationResult Register(const SampleRegistrationInput& input);

  private:
    ISampleRepository& repository_;
};

}  // namespace SampleOrderSystem
