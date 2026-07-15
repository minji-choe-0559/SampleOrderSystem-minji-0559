#include <gtest/gtest.h>

#include "FakeSampleRepository.h"
#include "RegisterSampleUseCase.h"

namespace SampleOrderSystem {
namespace {

SampleRegistrationInput MakeInput(std::string sampleCode, std::string name = "Sample A") {
    SampleRegistrationInput input;
    input.sampleCode = std::move(sampleCode);
    input.name = std::move(name);
    input.avgProcessTime = 1.0;
    input.yieldRate = 90.0;
    input.stock = 0;
    return input;
}

TEST(RegisterSampleUseCaseTest, ReturnsSuccessWhenSampleCodeIsNew) {
    FakeSampleRepository repository;
    RegisterSampleUseCase useCase(repository);

    SampleRegistrationResult result = useCase.Register(MakeInput("S-001"));

    EXPECT_EQ(SampleRegistrationOutcome::Success, result.outcome);
    EXPECT_EQ(1u, repository.readAll().size());
}

TEST(RegisterSampleUseCaseTest, ReturnsDuplicateSampleCodeWhenAlreadyRegistered) {
    FakeSampleRepository repository;
    RegisterSampleUseCase useCase(repository);
    useCase.Register(MakeInput("S-001"));

    SampleRegistrationResult result = useCase.Register(MakeInput("s-001", "Sample B"));

    EXPECT_EQ(SampleRegistrationOutcome::DuplicateSampleCode, result.outcome);
}

TEST(RegisterSampleUseCaseTest, ReturnsInvalidInputWhenNameIsBlank) {
    FakeSampleRepository repository;
    RegisterSampleUseCase useCase(repository);

    SampleRegistrationResult result = useCase.Register(MakeInput("S-001", ""));

    EXPECT_EQ(SampleRegistrationOutcome::InvalidInput, result.outcome);
}

TEST(RegisterSampleUseCaseTest, ReturnsInvalidInputWhenAvgProcessTimeIsNegative) {
    FakeSampleRepository repository;
    RegisterSampleUseCase useCase(repository);
    SampleRegistrationInput input = MakeInput("S-001");
    input.avgProcessTime = -1.0;

    EXPECT_EQ(SampleRegistrationOutcome::InvalidInput, useCase.Register(input).outcome);
}

TEST(RegisterSampleUseCaseTest, ReturnsInvalidInputWhenYieldRateIsOutOfRange) {
    FakeSampleRepository repository;
    RegisterSampleUseCase useCase(repository);
    SampleRegistrationInput negative = MakeInput("S-001");
    negative.yieldRate = -1.0;
    SampleRegistrationInput tooHigh = MakeInput("S-002");
    tooHigh.yieldRate = 100.1;

    EXPECT_EQ(SampleRegistrationOutcome::InvalidInput, useCase.Register(negative).outcome);
    EXPECT_EQ(SampleRegistrationOutcome::InvalidInput, useCase.Register(tooHigh).outcome);
}

TEST(RegisterSampleUseCaseTest, ReturnsInvalidInputWhenStockIsNegative) {
    FakeSampleRepository repository;
    RegisterSampleUseCase useCase(repository);
    SampleRegistrationInput input = MakeInput("S-001");
    input.stock = -1;

    EXPECT_EQ(SampleRegistrationOutcome::InvalidInput, useCase.Register(input).outcome);
}

}  // namespace
}  // namespace SampleOrderSystem
