#include <gtest/gtest.h>

#include "FakeSampleRepository.h"
#include "SearchSampleUseCase.h"

namespace SampleOrderSystem {
namespace {

class SearchSampleUseCaseTest : public ::testing::Test {
  protected:
    void SetUp() override {
        repository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Silicon Wafer", 1.0, 90.0, 10});
        repository_.Seed(SampleRecord{Guid::NewGuid(), "S-002", "GaN Epitaxial", 2.0, 80.0, 20});
    }

    FakeSampleRepository repository_;
};

TEST_F(SearchSampleUseCaseTest, MatchesByPartialNameCaseInsensitive) {
    SearchSampleUseCase useCase(repository_);

    std::vector<SampleViewModel> result = useCase.Search("silicon");

    ASSERT_EQ(1u, result.size());
    EXPECT_EQ("S-001", result[0].sampleCode);
}

TEST_F(SearchSampleUseCaseTest, MatchesByPartialSampleCodeCaseInsensitive) {
    SearchSampleUseCase useCase(repository_);

    std::vector<SampleViewModel> result = useCase.Search("s-002");

    ASSERT_EQ(1u, result.size());
    EXPECT_EQ("GaN Epitaxial", result[0].name);
}

TEST_F(SearchSampleUseCaseTest, ReturnsEmptyWhenNoFieldMatches) {
    SearchSampleUseCase useCase(repository_);

    EXPECT_TRUE(useCase.Search("no-such-keyword").empty());
}

}  // namespace
}  // namespace SampleOrderSystem
