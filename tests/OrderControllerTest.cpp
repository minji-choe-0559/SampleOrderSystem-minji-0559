#include <gtest/gtest.h>

#include "FakeOrderRepository.h"
#include "FakeOrderView.h"
#include "FakeSampleRepository.h"
#include "OrderController.h"

namespace SampleOrderSystem {
namespace {

class OrderControllerTest : public ::testing::Test {
  protected:
    void SetUp() override {
        sampleRepository_.Seed(
            SampleRecord{Guid::NewGuid(), "S-001", "Silicon Wafer", 1.0, 90.0, 100});
    }

    FakeSampleRepository sampleRepository_;
    FakeOrderRepository orderRepository_;
    FakeOrderView view_;
    ReserveOrderUseCase reserveUseCase_{sampleRepository_, orderRepository_};
    OrderController controller_{view_, reserveUseCase_};
};

TEST_F(OrderControllerTest, RunExitsImmediatelyOnMenuChoiceZero) {
    view_.menuChoiceSequence = {0};

    controller_.Run();

    EXPECT_EQ(1, view_.showMenuCallCount);
}

TEST_F(OrderControllerTest, RunReservesThenExits) {
    view_.menuChoiceSequence = {1, 0};
    view_.nextReservationInput.sampleCode = "S-001";
    view_.nextReservationInput.customerName = "삼성전자 파운드리";
    view_.nextReservationInput.quantity = 10;

    controller_.Run();

    EXPECT_EQ(1, view_.reservationResultCallCount);
    EXPECT_EQ(OrderReservationOutcome::Success, view_.lastResult.outcome);
    EXPECT_EQ("ORD-00001", view_.lastResult.orderNumber);
}

TEST_F(OrderControllerTest, RunPropagatesSampleNotFoundToView) {
    view_.menuChoiceSequence = {1, 0};
    view_.nextReservationInput.sampleCode = "S-999";
    view_.nextReservationInput.customerName = "고객";
    view_.nextReservationInput.quantity = 10;

    controller_.Run();

    EXPECT_EQ(OrderReservationOutcome::SampleNotFound, view_.lastResult.outcome);
}

TEST_F(OrderControllerTest, RunPropagatesInvalidInputToView) {
    view_.menuChoiceSequence = {1, 0};
    view_.nextReservationInput.sampleCode = "S-001";
    view_.nextReservationInput.customerName = "고객";
    view_.nextReservationInput.quantity = 0;  // 유효성 위반: quantity는 0보다 커야 한다.

    controller_.Run();

    EXPECT_EQ(OrderReservationOutcome::InvalidInput, view_.lastResult.outcome);
}

TEST_F(OrderControllerTest, RunIgnoresInvalidChoiceAndContinues) {
    view_.menuChoiceSequence = {-1, 99, 0};

    controller_.Run();

    EXPECT_EQ(3, view_.showMenuCallCount);
    EXPECT_EQ(0, view_.reservationResultCallCount);
}

}  // namespace
}  // namespace SampleOrderSystem
