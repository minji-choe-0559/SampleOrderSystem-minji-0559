#pragma once

#include "IView.h"

namespace SampleOrderSystem {

// Concrete Passive View: reads raw console input and prints ViewModels.
// Holds no business logic and never references SampleRecord (POC_SPEC.md 7절).
class ConsoleView : public IView {
  public:
    void ShowMenu() override;
    int ReadMenuChoice() override;
    SampleRegistrationInput ReadRegistrationInput() override;
    void ShowRegistrationResult(const SampleRegistrationResult& result) override;
    void ShowSampleList(const std::vector<SampleViewModel>& samples) override;
};

}  // namespace SampleOrderSystem
