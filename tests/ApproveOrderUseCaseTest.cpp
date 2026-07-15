#include <gtest/gtest.h>

#include "ApproveOrderUseCase.h"
#include "FakeOrderRepository.h"
#include "FakeProductionQueueRepository.h"
#include "FakeSampleRepository.h"

namespace SampleOrderSystem {
namespace {

class ApproveOrderUseCaseTest : public ::testing::Test {
  protected:
    Order SeedOrder(int quantity, OrderStatus status = OrderStatus::Reserved) {
        Order order{Guid::NewGuid(), "ORD-00001", "S-001", "고객", quantity, status};
        orderRepository_.Seed(order);
        return order;
    }

    FakeSampleRepository sampleRepository_;
    FakeOrderRepository orderRepository_;
    FakeProductionQueueRepository productionQueueRepository_;
};

TEST_F(ApproveOrderUseCaseTest, ConfirmsImmediatelyAndDeductsStockWhenStockSufficient) {
    sampleRepository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 100});
    SeedOrder(60);
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, productionQueueRepository_);

    ApproveOrderResult result = useCase.Approve("ORD-00001");

    EXPECT_EQ(ApproveOrderOutcome::ConfirmedSufficientStock, result.outcome);
    EXPECT_EQ(OrderStatus::Confirmed, orderRepository_.findByOrderNumber("ORD-00001")->status);
    EXPECT_EQ(40, sampleRepository_.findBySampleCode("S-001")->stock);
    EXPECT_TRUE(productionQueueRepository_.readAll().empty());
}

TEST_F(ApproveOrderUseCaseTest,
       CreatesProductionJobAndTransitionsToProducingWhenStockInsufficient) {
    sampleRepository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 30});
    SeedOrder(200);  // 부족분 170, 수율 90% -> ceil(170 / 0.9) = 189
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, productionQueueRepository_);

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
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, productionQueueRepository_);

    EXPECT_EQ(ApproveOrderOutcome::OrderNotFound, useCase.Approve("ORD-99999").outcome);
}

TEST_F(ApproveOrderUseCaseTest, ReturnsOrderNotReservedWhenAlreadyProcessed) {
    sampleRepository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 100});
    SeedOrder(10, OrderStatus::Confirmed);
    ApproveOrderUseCase useCase(orderRepository_, sampleRepository_, productionQueueRepository_);

    EXPECT_EQ(ApproveOrderOutcome::OrderNotReserved, useCase.Approve("ORD-00001").outcome);
}

}  // namespace
}  // namespace SampleOrderSystem
