#pragma once

#include <string>

namespace SampleOrderSystem {

// Presentation-layer input DTO: View packs raw console input into this
// single value instead of passing primitives separately (POC_SPEC.md 10.2).
struct SampleRegistrationInput {
    std::string sampleCode;
    std::string name;
    double avgProcessTime = 0.0;
    double yieldRate = 0.0;
    long stock = 0;
};

}  // namespace SampleOrderSystem
