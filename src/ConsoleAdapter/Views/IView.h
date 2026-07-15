#pragma once

#include <vector>

#include "SampleRegistrationInput.h"
#include "SampleRegistrationResult.h"
#include "SampleViewModel.h"

namespace SampleOrderSystem {

// Passive View contract: only Presentation types cross this boundary,
// never SampleRecord (POC_SPEC.md 7절, 9절). Controller decides the flow;
// View only performs raw console I/O (read raw input, print ViewModels).
class IView {
  public:
    virtual ~IView() = default;

    virtual void ShowMenu() = 0;
    virtual int ReadMenuChoice() = 0;
    virtual SampleRegistrationInput ReadRegistrationInput() = 0;
    virtual void ShowRegistrationResult(const SampleRegistrationResult& result) = 0;
    virtual void ShowSampleList(const std::vector<SampleViewModel>& samples) = 0;
};

}  // namespace SampleOrderSystem
