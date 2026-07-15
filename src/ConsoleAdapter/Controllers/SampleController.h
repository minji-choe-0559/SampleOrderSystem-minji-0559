#pragma once

#include "ISampleService.h"
#include "IView.h"
#include "SampleRegistrationInput.h"

namespace SampleOrderSystem {

// Coordinates user flow only: forwards requests to ISampleService and
// results to IView. Deliberately has no ISampleRepository dependency
// (POC_SPEC.md 8~9절) and no owning pointers (CLAUDE.md RAII 규칙) —
// it holds its collaborators as non-owning references, whose lifetime
// is owned by the Composition Root (main).
class SampleController {
  public:
    SampleController(IView& view, ISampleService& service);

    // Owns the console menu loop: reads a choice from the View, dispatches
    // to Handle*, and repeats until the user exits.
    void Run();

    void HandleRegistration(const SampleRegistrationInput& input);
    void HandleListSamples();

  private:
    IView& view_;
    ISampleService& service_;
};

}  // namespace SampleOrderSystem
