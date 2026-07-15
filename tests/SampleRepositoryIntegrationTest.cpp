#include <gtest/gtest.h>

#include <atomic>
#include <filesystem>

#include "ListSamplesUseCase.h"
#include "RegisterSampleUseCase.h"
#include "SampleRepository.h"
#include "SearchSampleUseCase.h"

namespace SampleOrderSystem {
namespace {

std::atomic<int> g_counter{0};

class SampleRepositoryIntegrationTest : public ::testing::Test {
  protected:
    void SetUp() override {
        auto dir = std::filesystem::temp_directory_path() / "sos_sample_repo_tests";
        std::filesystem::create_directories(dir);
        path_ = (dir / ("repo_" + std::to_string(++g_counter) + ".json")).string();
        std::filesystem::remove(path_);
    }

    void TearDown() override { std::filesystem::remove(path_); }

    std::string path_;
};

TEST_F(SampleRepositoryIntegrationTest, CreateThenReadAllReturnsRecord) {
    SampleRepository repo(path_);
    SampleRecord created = repo.create("S-001", "Sample A", 12.5, 97.3, 100);

    std::vector<SampleRecord> all = repo.readAll();
    ASSERT_EQ(1u, all.size());
    EXPECT_EQ(created.id, all[0].id);
    EXPECT_EQ("S-001", all[0].sampleCode);
}

TEST_F(SampleRepositoryIntegrationTest, CreateRejectsDuplicateSampleCodeCaseInsensitive) {
    SampleRepository repo(path_);
    repo.create("S-001", "Sample A", 1.0, 50.0, 0);
    EXPECT_THROW(repo.create("s-001", "Another", 1.0, 50.0, 0), std::invalid_argument);
}

TEST_F(SampleRepositoryIntegrationTest, CreateRejectsNegativeAvgProcessTime) {
    SampleRepository repo(path_);
    EXPECT_THROW(repo.create("S-001", "Sample A", -1.0, 50.0, 0), std::invalid_argument);
}

TEST_F(SampleRepositoryIntegrationTest, CreateRejectsYieldRateOutOfRange) {
    SampleRepository repo(path_);
    EXPECT_THROW(repo.create("S-001", "Sample A", 1.0, -1.0, 0), std::invalid_argument);
    EXPECT_THROW(repo.create("S-002", "Sample B", 1.0, 100.1, 0), std::invalid_argument);
}

TEST_F(SampleRepositoryIntegrationTest, CreateRejectsNegativeStock) {
    SampleRepository repo(path_);
    EXPECT_THROW(repo.create("S-001", "Sample A", 1.0, 50.0, -1), std::invalid_argument);
}

TEST_F(SampleRepositoryIntegrationTest, FindBySampleCodeReturnsNulloptWhenMissing) {
    SampleRepository repo(path_);
    EXPECT_FALSE(repo.findBySampleCode("MISSING").has_value());
}

TEST_F(SampleRepositoryIntegrationTest, DataSurvivesRepositoryRestart) {
    // 프로세스 재실행과 동등한 시나리오: 새 SampleRepository 인스턴스로 같은 파일을 다시 읽는다.
    {
        SampleRepository firstInstance(path_);
        firstInstance.create("S-001", "Sample A", 12.5, 97.3, 100);
    }

    SampleRepository secondInstance(path_);
    std::optional<SampleRecord> found = secondInstance.findBySampleCode("S-001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ("Sample A", found->name);
    EXPECT_DOUBLE_EQ(12.5, found->avgProcessTime);
    EXPECT_DOUBLE_EQ(97.3, found->yieldRate);
    EXPECT_EQ(100, found->stock);
}

TEST_F(SampleRepositoryIntegrationTest, RegisterListSearchRoundTripThroughUseCases) {
    // Phase 1의 3개 UseCase가 실제 파일 기반 Repository와 함께 동작하는지 확인하는 통합 테스트.
    SampleRepository repo(path_);
    RegisterSampleUseCase registerUseCase(repo);
    ListSamplesUseCase listUseCase(repo);
    SearchSampleUseCase searchUseCase(repo);

    SampleRegistrationInput input;
    input.sampleCode = "S-001";
    input.name = "Silicon Wafer";
    input.avgProcessTime = 0.5;
    input.yieldRate = 92.0;
    input.stock = 480;
    ASSERT_EQ(SampleRegistrationOutcome::Success, registerUseCase.Register(input).outcome);

    EXPECT_EQ(1u, listUseCase.ListSamples().size());
    EXPECT_EQ(1u, searchUseCase.Search("silicon").size());
    EXPECT_TRUE(searchUseCase.Search("no-match").empty());
}

TEST_F(SampleRepositoryIntegrationTest, AdjustStockPersistsAcrossRestart) {
    // test-auditor 지적: Phase 3에서 추가된 adjustStock의 실물 통합 테스트가 없었다.
    SampleRepository repo(path_);
    repo.create("S-001", "Sample A", 1.0, 90.0, 100);

    std::optional<SampleRecord> updated = repo.adjustStock("S-001", -60);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(40, updated->stock);

    SampleRepository reloaded(path_);
    EXPECT_EQ(40, reloaded.findBySampleCode("S-001")->stock);
}

TEST_F(SampleRepositoryIntegrationTest, AdjustStockRejectsNegativeResult) {
    SampleRepository repo(path_);
    repo.create("S-001", "Sample A", 1.0, 90.0, 10);

    EXPECT_THROW(repo.adjustStock("S-001", -20), std::invalid_argument);
}

TEST_F(SampleRepositoryIntegrationTest, AdjustStockReturnsNulloptWhenSampleCodeUnknown) {
    SampleRepository repo(path_);
    EXPECT_FALSE(repo.adjustStock("S-999", -1).has_value());
}

}  // namespace
}  // namespace SampleOrderSystem
