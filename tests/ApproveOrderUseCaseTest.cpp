#include <gtest/gtest.h>

#include "ApproveOrderUseCase.h"
#include "FakeOrderApprovalRepository.h"
#include "FakeOrderRepository.h"
#include "FakeProductionQueueRepository.h"
#include "FakeSampleRepository.h"

namespace SampleOrderSystem {
namespace {

class ApproveOrderUseCaseTest : public ::testing::Test {
  protected:
    Order SeedOrder(const std::string& sampleCode, int quantity,
                    OrderStatus status = OrderStatus::Reserved) {
        Order order{Guid::NewGuid(), "ORD-00001", sampleCode, "고객", quantity, status};
        orderRepository_.Seed(order);
        return order;
    }

    FakeSampleRepository sampleRepository_;
    FakeOrderRepository orderRepository_;
    FakeProductionQueueRepository productionQueueRepository_;
    FakeOrderApprovalRepository approvalRepository_{sampleRepository_, orderRepository_,
                                                    productionQueueRepository_};
};

TEST_F(ApproveOrderUseCaseTest, ConfirmsImmediatelyAndDeductsStockWhenStockSufficient) {
    sampleRepository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 100});
    SeedOrder("S-001", 60);
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, approvalRepository_);

    ApproveOrderResult result = useCase.Approve("ORD-00001");

    EXPECT_EQ(ApproveOrderOutcome::ConfirmedSufficientStock, result.outcome);
    EXPECT_EQ(OrderStatus::Confirmed, orderRepository_.findByOrderNumber("ORD-00001")->status);
    EXPECT_EQ(40, sampleRepository_.findBySampleCode("S-001")->stock);
    EXPECT_TRUE(productionQueueRepository_.readAll().empty());
}

TEST_F(ApproveOrderUseCaseTest,
       CreatesProductionJobAndTransitionsToProducingWhenStockInsufficient) {
    sampleRepository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 30});
    SeedOrder("S-001", 200);  // 부족분 170, 수율 90% -> ceil(170 / 0.9) = 189
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, approvalRepository_);

    ApproveOrderResult result = useCase.Approve("ORD-00001");

    EXPECT_EQ(ApproveOrderOutcome::ProducingInsufficientStock, result.outcome);
    EXPECT_EQ(OrderStatus::Producing, orderRepository_.findByOrderNumber("ORD-00001")->status);
    ASSERT_EQ(1u, productionQueueRepository_.readAll().size());
    EXPECT_EQ(189, productionQueueRepository_.readAll()[0].quantity);
    EXPECT_EQ("S-001", productionQueueRepository_.readAll()[0].sampleCode);
    // 재고 부족 경로에서는 재고를 차감하지 않는다(생산 완료 시에만 반영, Phase 4).
    EXPECT_EQ(30, sampleRepository_.findBySampleCode("S-001")->stock);
}

TEST_F(ApproveOrderUseCaseTest, ReturnsOrderNotFoundWhenOrderNumberUnknown) {
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, approvalRepository_);

    EXPECT_EQ(ApproveOrderOutcome::OrderNotFound, useCase.Approve("ORD-99999").outcome);
}

TEST_F(ApproveOrderUseCaseTest, ReturnsOrderNotReservedWhenAlreadyProcessed) {
    sampleRepository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 100});
    SeedOrder("S-001", 10, OrderStatus::Confirmed);
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, approvalRepository_);

    EXPECT_EQ(ApproveOrderOutcome::OrderNotReserved, useCase.Approve("ORD-00001").outcome);
}

TEST_F(ApproveOrderUseCaseTest, ReturnsOrderNotReservedOnSecondApproveCall) {
    // PRD.md 5.4가 막으려는 "같은 재고로 중복 승인"이 실제로 재발하지 않는지, 동일 UseCase로
    // 연속 두 번 승인 호출해 검증한다(test-auditor 지적: 재승인 시나리오가 시딩만으로 검증됨).
    sampleRepository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 100});
    SeedOrder("S-001", 60);
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, approvalRepository_);

    ApproveOrderResult first = useCase.Approve("ORD-00001");
    ApproveOrderResult second = useCase.Approve("ORD-00001");

    EXPECT_EQ(ApproveOrderOutcome::ConfirmedSufficientStock, first.outcome);
    EXPECT_EQ(ApproveOrderOutcome::OrderNotReserved, second.outcome);
    // 재고는 처음 승인 때 한 번만 차감되어야 한다(중복 차감 없음).
    EXPECT_EQ(40, sampleRepository_.findBySampleCode("S-001")->stock);
}

TEST_F(ApproveOrderUseCaseTest, ReturnsStorageFailureWhenLinkedSampleMissing) {
    // Order가 참조하는 sampleCode가 SampleRepository에 없는 데이터 불일치 상황(test-auditor 지적).
    SeedOrder("S-MISSING", 10);
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, approvalRepository_);

    EXPECT_EQ(ApproveOrderOutcome::StorageFailure, useCase.Approve("ORD-00001").outcome);
}

TEST_F(ApproveOrderUseCaseTest, ReturnsInvalidYieldRateWhenYieldRateIsZero) {
    // yieldRate == 0이면 ceil(shortfall * 100 / yieldRate)가 0으로 나누기가 되어 정의되지 않은
    // 동작이 될 수 있다(code-reviewer/test-auditor 공통 지적). 계산 전에 명시적으로 거부해야 한다.
    sampleRepository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 0.0, 10});
    SeedOrder("S-001", 100);
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, approvalRepository_);

    ApproveOrderResult result = useCase.Approve("ORD-00001");

    EXPECT_EQ(ApproveOrderOutcome::InvalidYieldRate, result.outcome);
    EXPECT_TRUE(productionQueueRepository_.readAll().empty());
    EXPECT_EQ(OrderStatus::Reserved, orderRepository_.findByOrderNumber("ORD-00001")->status);
}

}  // namespace
}  // namespace SampleOrderSystem
