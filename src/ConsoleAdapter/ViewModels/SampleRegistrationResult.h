#pragma once

#include <string>

namespace SampleOrderSystem {

enum class SampleRegistrationOutcome {
    Success,
    DuplicateSampleCode,
    InvalidInput,
    StorageFailure,
};

// Presentation-layer result: Controller translates ISampleService's
// outcome into this before handing it to the View (POC_SPEC.md 10.2).
struct SampleRegistrationResult {
    SampleRegistrationOutcome outcome = SampleRegistrationOutcome::Success;
    std::string message;
};

}  // namespace SampleOrderSystem
