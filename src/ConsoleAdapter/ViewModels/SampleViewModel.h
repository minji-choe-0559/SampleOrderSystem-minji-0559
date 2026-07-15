#pragma once

#include <string>

namespace SampleOrderSystem {

// Presentation-layer read model: View displays this instead of the
// domain type SampleRecord, so it never needs to depend on it
// (POC_SPEC.md 10.2, MVC-NFR-003).
struct SampleViewModel {
    std::string sampleCode;
    std::string name;
    double avgProcessTime = 0.0;
    double yieldRate = 0.0;
    long stock = 0;
};

}  // namespace SampleOrderSystem
