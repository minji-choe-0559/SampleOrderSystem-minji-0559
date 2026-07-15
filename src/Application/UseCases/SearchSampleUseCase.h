#pragma once

#include <string>
#include <vector>

#include "ISampleRepository.h"
#include "SampleViewModel.h"

namespace SampleOrderSystem {

// PRD.md 5.2 "시료 검색"(확정: 이름 또는 시료 ID, 대소문자 무시 부분일치).
class SearchSampleUseCase {
  public:
    explicit SearchSampleUseCase(const ISampleRepository& repository);

    std::vector<SampleViewModel> Search(const std::string& keyword) const;

  private:
    const ISampleRepository& repository_;
};

}  // namespace SampleOrderSystem
