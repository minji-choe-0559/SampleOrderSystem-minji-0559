#include <gtest/gtest.h>

#include "FakeSampleRepository.h"
#include "ListSamplesUseCase.h"

namespace SampleOrderSystem {
namespace {

TEST(ListSamplesUseCaseTest, ReturnsEmptyWhenNoSamplesRegistered) {
    FakeSampleRepository repository;
    ListSamplesUseCase useCase(repository);

    EXPECT_TRUE(useCase.ListSamples().empty());
}

TEST(ListSamplesUseCaseTest, ReturnsViewModelForEachStoredRecord) {
    FakeSampleRepository repository;
    repository.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 10});
    repository.Seed(SampleRecord{Guid::NewGuid(), "S-002", "Sample B", 2.0, 80.0, 20});
    ListSamplesUseCase useCase(repository);

    std::vector<SampleViewModel> result = useCase.ListSamples();

    ASSERT_EQ(2u, result.size());
    EXPECT_EQ("S-001", result[0].sampleCode);
    EXPECT_EQ("Sample A", result[0].name);
    EXPECT_EQ(10, result[0].stock);
    EXPECT_EQ("S-002", result[1].sampleCode);
}

}  // namespace
}  // namespace SampleOrderSystem
