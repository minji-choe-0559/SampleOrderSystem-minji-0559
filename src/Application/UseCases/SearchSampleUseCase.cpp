#include "SearchSampleUseCase.h"

#include "SampleViewModelMapper.h"
#include "TextMatch.h"

namespace SampleOrderSystem {

SearchSampleUseCase::SearchSampleUseCase(const ISampleRepository& repository)
    : repository_(repository) {}

std::vector<SampleViewModel> SearchSampleUseCase::Search(const std::string& keyword) const {
    std::vector<SampleViewModel> result;
    for (const SampleRecord& record : repository_.readAll()) {
        if (ContainsIgnoreCase(record.name, keyword) ||
            ContainsIgnoreCase(record.sampleCode, keyword)) {
            result.push_back(ToViewModel(record));
        }
    }
    return result;
}

}  // namespace SampleOrderSystem
