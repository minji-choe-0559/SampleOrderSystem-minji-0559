#include "ListSamplesUseCase.h"

#include "SampleViewModelMapper.h"

namespace SampleOrderSystem {

ListSamplesUseCase::ListSamplesUseCase(const ISampleRepository& repository)
    : repository_(repository) {}

std::vector<SampleViewModel> ListSamplesUseCase::ListSamples() const {
    std::vector<SampleViewModel> result;
    for (const SampleRecord& record : repository_.readAll()) {
        result.push_back(ToViewModel(record));
    }
    return result;
}

}  // namespace SampleOrderSystem
