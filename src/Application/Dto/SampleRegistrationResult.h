#pragma once

#include <string>

namespace SampleOrderSystem {

enum class SampleRegistrationOutcome {
    Success,
    DuplicateSampleCode,
    InvalidInput,
    StorageFailure,
};

// Presentation-layer result: RegisterSampleUseCase translates
// ISampleRepository's exception into this before the Controller hands it
// to the View.
struct SampleRegistrationResult {
    SampleRegistrationOutcome outcome = SampleRegistrationOutcome::Success;
    std::string message;
};

}  // namespace SampleOrderSystem
