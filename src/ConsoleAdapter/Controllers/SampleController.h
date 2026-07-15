#pragma once

#include "IView.h"
#include "ListSamplesUseCase.h"
#include "RegisterSampleUseCase.h"
#include "SearchSampleUseCase.h"

namespace SampleOrderSystem {

// Coordinates user flow only: forwards requests to the three UseCases and
// results to IView. Holds no owning pointers (CLAUDE.md RAII 규칙) — it
// holds its collaborators as non-owning references, whose lifetime is
// owned by the Composition Root (main).
class SampleController {
  public:
    SampleController(IView& view, RegisterSampleUseCase& registerUseCase,
                     ListSamplesUseCase& listUseCase, SearchSampleUseCase& searchUseCase);

    // Owns the console menu loop: reads a choice from the View, dispatches
    // to Handle*, and repeats until the user exits.
    void Run();

    void HandleRegistration(const SampleRegistrationInput& input);
    void HandleListSamples();
    void HandleSearch(const std::string& keyword);

  private:
    IView& view_;
    RegisterSampleUseCase& registerUseCase_;
    ListSamplesUseCase& listUseCase_;
    SearchSampleUseCase& searchUseCase_;
};

}  // namespace SampleOrderSystem
