#include "SampleController.h"

namespace SampleOrderSystem {

namespace {
constexpr int kMenuExit = 0;
constexpr int kMenuRegister = 1;
constexpr int kMenuListSamples = 2;
constexpr int kMenuSearch = 3;
}  // namespace

SampleController::SampleController(IView& view, RegisterSampleUseCase& registerUseCase,
                                   ListSamplesUseCase& listUseCase,
                                   SearchSampleUseCase& searchUseCase)
    : view_(view),
      registerUseCase_(registerUseCase),
      listUseCase_(listUseCase),
      searchUseCase_(searchUseCase) {}

void SampleController::Run() {
    bool running = true;
    while (running) {
        view_.ShowMenu();
        switch (view_.ReadMenuChoice()) {
            case kMenuRegister:
                HandleRegistration(view_.ReadRegistrationInput());
                break;
            case kMenuListSamples:
                HandleListSamples();
                break;
            case kMenuSearch:
                HandleSearch(view_.ReadSearchKeyword());
                break;
            case kMenuExit:
                running = false;
                break;
            default:
                break;
        }
    }
}

void SampleController::HandleRegistration(const SampleRegistrationInput& input) {
    view_.ShowRegistrationResult(registerUseCase_.Register(input));
}

void SampleController::HandleListSamples() { view_.ShowSampleList(listUseCase_.ListSamples()); }

void SampleController::HandleSearch(const std::string& keyword) {
    view_.ShowSampleList(searchUseCase_.Search(keyword));
}

}  // namespace SampleOrderSystem
