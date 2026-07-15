#include <gtest/gtest.h>

#include <atomic>
#include <filesystem>

#include "OrderApprovalRepository.h"
#include "OrderRepository.h"
#include "SampleRepository.h"

namespace SampleOrderSystem {
namespace {

std::atomic<int> g_counter{0};

class OrderApprovalRepositoryIntegrationTest : public ::testing::Test {
  protected:
    void SetUp() override {
        auto dir = std::filesystem::temp_directory_path() / "sos_order_approval_repo_tests";
        std::filesystem::create_directories(dir);
        path_ = (dir / ("repo_" + std::to_string(++g_counter) + ".json")).string();
        std::filesystem::remove(path_);
    }

    void TearDown() override { std::filesystem::remove(path_); }

    std::string path_;
};

// OrderApprovalRepository의 "찾지 못하면 false" 분기는 ApproveOrderUseCase가 승인 전 존재를
// 먼저 확인하기 때문에 UseCase 경로로는 도달하지 않는다 — Repository 자체를 직접 호출해
// 검증한다(test-auditor 지적: 이번에 추가된 Repository 메서드의 실물 테스트 공백).

TEST_F(OrderApprovalRepositoryIntegrationTest,
       ConfirmWithStockDeductionReturnsFalseWhenSampleMissing) {
    OrderRepository orderRepo(path_);
    orderRepo.create("S-001", "Customer", 10);
    OrderApprovalRepository approvalRepo(path_);

    EXPECT_FALSE(approvalRepo.ConfirmWithStockDeduction("ORD-00001", "S-MISSING", 10));
}

TEST_F(OrderApprovalRepositoryIntegrationTest,
       ConfirmWithStockDeductionReturnsFalseWhenOrderMissing) {
    SampleRepository sampleRepo(path_);
    sampleRepo.create("S-001", "Sample A", 1.0, 90.0, 100);
    OrderApprovalRepository approvalRepo(path_);

    EXPECT_FALSE(approvalRepo.ConfirmWithStockDeduction("ORD-99999", "S-001", 10));
}

TEST_F(OrderApprovalRepositoryIntegrationTest, TransitionToProducingReturnsFalseWhenOrderMissing) {
    OrderApprovalRepository approvalRepo(path_);

    EXPECT_FALSE(approvalRepo.TransitionToProducing("ORD-99999", Guid::NewGuid(), "S-001", 10));
}

}  // namespace
}  // namespace SampleOrderSystem
