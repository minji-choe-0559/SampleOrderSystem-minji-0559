#include "SampleController.h"

namespace SampleOrderSystem {

namespace {
constexpr int kMenuExit = 0;
constexpr int kMenuRegister = 1;
constexpr int kMenuListSamples = 2;
}  // namespace

SampleController::SampleController(IView& view, ISampleService& service)
    : view_(view), service_(service) {}

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
            case kMenuExit:
                running = false;
                break;
            default:
                break;
        }
    }
}

void SampleController::HandleRegistration(const SampleRegistrationInput& input) {
    view_.ShowRegistrationResult(service_.Register(input));
}

void SampleController::HandleListSamples() { view_.ShowSampleList(service_.ListSamples()); }

}  // namespace SampleOrderSystem
