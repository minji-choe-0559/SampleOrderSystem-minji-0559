#include <gtest/gtest.h>

#include "FakeSampleRepository.h"
#include "FakeView.h"
#include "SampleController.h"

namespace SampleOrderSystem {
namespace {

class SampleControllerTest : public ::testing::Test {
  protected:
    FakeSampleRepository repository_;
    FakeView view_;
    RegisterSampleUseCase registerUseCase_{repository_};
    ListSamplesUseCase listUseCase_{repository_};
    SearchSampleUseCase searchUseCase_{repository_};
    SampleController controller_{view_, registerUseCase_, listUseCase_, searchUseCase_};
};

TEST_F(SampleControllerTest, RunExitsImmediatelyOnMenuChoiceZero) {
    view_.menuChoiceSequence = {0};

    controller_.Run();

    EXPECT_EQ(1, view_.showMenuCallCount);
}

TEST_F(SampleControllerTest, RunRegistersThenExits) {
    view_.menuChoiceSequence = {1, 0};
    view_.nextRegistrationInput.sampleCode = "S-001";
    view_.nextRegistrationInput.name = "Sample A";

    controller_.Run();

    EXPECT_EQ(1, view_.registrationResultCallCount);
    EXPECT_EQ(SampleRegistrationOutcome::Success, view_.lastResult.outcome);
}

TEST_F(SampleControllerTest, RunPropagatesDuplicateRegistrationFailureToView) {
    repository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Existing", 1.0, 90.0, 0});
    view_.menuChoiceSequence = {1, 0};
    view_.nextRegistrationInput.sampleCode = "S-001";
    view_.nextRegistrationInput.name = "Sample A";

    controller_.Run();

    EXPECT_EQ(1, view_.registrationResultCallCount);
    EXPECT_EQ(SampleRegistrationOutcome::DuplicateSampleCode, view_.lastResult.outcome);
}

TEST_F(SampleControllerTest, RunListsThenExits) {
    repository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 10});
    view_.menuChoiceSequence = {2, 0};

    controller_.Run();

    EXPECT_EQ(1, view_.sampleListCallCount);
    EXPECT_EQ(1u, view_.lastSampleList.size());
}

TEST_F(SampleControllerTest, RunSearchesThenExits) {
    repository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Silicon Wafer", 1.0, 90.0, 10});
    view_.menuChoiceSequence = {3, 0};
    view_.nextSearchKeyword = "silicon";

    controller_.Run();

    EXPECT_EQ(1, view_.sampleListCallCount);
    EXPECT_EQ(1u, view_.lastSampleList.size());
}

TEST_F(SampleControllerTest, RunIgnoresInvalidChoiceAndContinues) {
    view_.menuChoiceSequence = {-1, 99, 0};

    controller_.Run();

    EXPECT_EQ(3, view_.showMenuCallCount);
    EXPECT_EQ(0, view_.registrationResultCallCount);
    EXPECT_EQ(0, view_.sampleListCallCount);
}

}  // namespace
}  // namespace SampleOrderSystem
