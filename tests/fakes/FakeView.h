#pragma once

#include <cstddef>

#include "IView.h"

namespace SampleOrderSystem {

// Test double for IView. Deliberately never includes SampleRecord.h —
// proving the View boundary depends only on Presentation types.
class FakeView : public IView {
  public:
    void ShowMenu() override { ++showMenuCallCount; }

    // Consumes menuChoiceSequence in order (for Run() loop tests); falls
    // back to nextMenuChoice when the sequence is empty or exhausted.
    int ReadMenuChoice() override {
        if (menuChoiceIndex < menuChoiceSequence.size()) {
            return menuChoiceSequence[menuChoiceIndex++];
        }
        return nextMenuChoice;
    }

    SampleRegistrationInput ReadRegistrationInput() override { return nextRegistrationInput; }

    std::string ReadSearchKeyword() override { return nextSearchKeyword; }

    void ShowRegistrationResult(const SampleRegistrationResult& result) override {
        lastResult = result;
        ++registrationResultCallCount;
    }

    void ShowSampleList(const std::vector<SampleViewModel>& samples) override {
        lastSampleList = samples;
        ++sampleListCallCount;
    }

    int nextMenuChoice = 0;
    std::vector<int> menuChoiceSequence;
    std::size_t menuChoiceIndex = 0;
    SampleRegistrationInput nextRegistrationInput{};
    std::string nextSearchKeyword;
    SampleRegistrationResult lastResult{};
    std::vector<SampleViewModel> lastSampleList;
    int showMenuCallCount = 0;
    int registrationResultCallCount = 0;
    int sampleListCallCount = 0;
};

}  // namespace SampleOrderSystem
