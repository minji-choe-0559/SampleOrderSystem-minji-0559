#include "RegisterSampleUseCase.h"

#include <stdexcept>
#include <string>

#include "TextMatch.h"

namespace SampleOrderSystem {

RegisterSampleUseCase::RegisterSampleUseCase(ISampleRepository& repository)
    : repository_(repository) {}

SampleRegistrationResult RegisterSampleUseCase::Register(const SampleRegistrationInput& input) {
    try {
        repository_.create(input.sampleCode, input.name, input.avgProcessTime, input.yieldRate,
                           static_cast<int>(input.stock));
        return SampleRegistrationResult{SampleRegistrationOutcome::Success, "등록되었습니다."};
    } catch (const std::invalid_argument& e) {
        std::string message = e.what();
        SampleRegistrationOutcome outcome = ContainsIgnoreCase(message, "duplicate")
                                                ? SampleRegistrationOutcome::DuplicateSampleCode
                                                : SampleRegistrationOutcome::InvalidInput;
        return SampleRegistrationResult{outcome, message};
    } catch (const std::exception& e) {
        return SampleRegistrationResult{SampleRegistrationOutcome::StorageFailure, e.what()};
    }
}

}  // namespace SampleOrderSystem
