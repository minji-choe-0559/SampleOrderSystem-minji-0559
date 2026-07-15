#pragma once

#include <vector>

#include "ISampleRepository.h"
#include "SampleViewModel.h"

namespace SampleOrderSystem {

// PRD.md 5.2 "시료 조회".
class ListSamplesUseCase {
  public:
    explicit ListSamplesUseCase(const ISampleRepository& repository);

    std::vector<SampleViewModel> ListSamples() const;

  private:
    const ISampleRepository& repository_;
};

}  // namespace SampleOrderSystem
