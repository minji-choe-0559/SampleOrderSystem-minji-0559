#include <gtest/gtest.h>

#include "FakeOrderApprovalRepository.h"
#include "FakeOrderApprovalView.h"
#include "FakeOrderRepository.h"
#include "FakeProductionQueueRepository.h"
#include "FakeSampleRepository.h"
#include "OrderApprovalController.h"

namespace SampleOrderSystem {
namespace {

class OrderApprovalControllerTest : public ::testing::Test {
  protected:
    void SetUp() override {
        sampleRepository_.Seed(SampleRecord{Guid::NewGuid(), "S-001", "Sample A", 1.0, 90.0, 100});
        orderRepository_.Seed(
            Order{Guid::NewGuid(), "ORD-00001", "S-001", "고객", 10, OrderStatus::Reserved});
    }

    FakeSampleRepository sampleRepository_;
    FakeOrderRepository orderRepository_;
    FakeProductionQueueRepository productionQueueRepository_;
    FakeOrderApprovalRepository approvalRepository_{sampleRepository_, orderRepository_,
                                                    productionQueueRepository_};
    FakeOrderApprovalView view_;
    ListReservedOrdersUseCase listUseCase_{orderRepository_};
    ApproveOrderUseCase approveUseCase_{orderRepository_, sampleRepository_, approvalRepository_};
    RejectOrderUseCase rejectUseCase_{orderRepository_};
    OrderApprovalController controller_{view_, listUseCase_, approveUseCase_, rejectUseCase_};
};

TEST_F(OrderApprovalControllerTest, RunExitsImmediatelyOnMenuChoiceZero) {
    view_.menuChoiceSequence = {0};

    controller_.Run();

    EXPECT_EQ(1, view_.showMenuCallCount);
}

TEST_F(OrderApprovalControllerTest, RunListsReservedOrdersThenExits) {
    view_.menuChoiceSequence = {1, 0};

    controller_.Run();

    EXPECT_EQ(1, view_.showReservedOrdersCallCount);
    ASSERT_EQ(1u, view_.lastReservedOrders.size());
    EXPECT_EQ("ORD-00001", view_.lastReservedOrders[0].orderNumber);
}

TEST_F(OrderApprovalControllerTest, RunApprovesThenExits) {
    view_.menuChoiceSequence = {2, 0};
    view_.nextOrderNumber = "ORD-00001";

    controller_.Run();

    EXPECT_EQ(1, view_.approveResultCallCount);
    EXPECT_EQ(ApproveOrderOutcome::ConfirmedSufficientStock, view_.lastApproveResult.outcome);
}

TEST_F(OrderApprovalControllerTest, RunRejectsThenExits) {
    view_.menuChoiceSequence = {3, 0};
    view_.nextOrderNumber = "ORD-00001";

    controller_.Run();

    EXPECT_EQ(1, view_.rejectResultCallCount);
    EXPECT_EQ(RejectOrderOutcome::Success, view_.lastRejectResult.outcome);
}

TEST_F(OrderApprovalControllerTest, RunIgnoresInvalidChoiceAndContinues) {
    view_.menuChoiceSequence = {-1, 99, 0};

    controller_.Run();

    EXPECT_EQ(3, view_.showMenuCallCount);
    EXPECT_EQ(0, view_.approveResultCallCount);
    EXPECT_EQ(0, view_.rejectResultCallCount);
}

}  // namespace
}  // namespace SampleOrderSystem
