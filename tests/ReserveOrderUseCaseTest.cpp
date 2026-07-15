#include <gtest/gtest.h>

#include "FakeOrderRepository.h"
#include "FakeSampleRepository.h"
#include "ReserveOrderUseCase.h"

namespace SampleOrderSystem {
namespace {

class ReserveOrderUseCaseTest : public ::testing::Test {
  protected:
    void SetUp() override {
        sampleRepository_.Seed(
            SampleRecord{Guid::NewGuid(), "S-001", "Silicon Wafer", 1.0, 90.0, 100});
    }

    OrderReservationInput MakeInput(std::string sampleCode = "S-001",
                                    std::string customerName = "삼성전자 파운드리",
                                    int quantity = 10) {
        OrderReservationInput input;
        input.sampleCode = std::move(sampleCode);
        input.customerName = std::move(customerName);
        input.quantity = quantity;
        return input;
    }

    FakeSampleRepository sampleRepository_;
    FakeOrderRepository orderRepository_;
};

TEST_F(ReserveOrderUseCaseTest, ReturnsSuccessWithOrderNumberWhenSampleExists) {
    ReserveOrderUseCase useCase(sampleRepository_, orderRepository_);

    OrderReservationResult result = useCase.Reserve(MakeInput());

    EXPECT_EQ(OrderReservationOutcome::Success, result.outcome);
    EXPECT_EQ("ORD-00001", result.orderNumber);
    ASSERT_EQ(1u, orderRepository_.readAll().size());
    EXPECT_EQ(OrderStatus::Reserved, orderRepository_.readAll()[0].status);
}

TEST_F(ReserveOrderUseCaseTest, AssignsSequentialOrderNumbers) {
    ReserveOrderUseCase useCase(sampleRepository_, orderRepository_);

    useCase.Reserve(MakeInput());
    OrderReservationResult second = useCase.Reserve(MakeInput());

    EXPECT_EQ("ORD-00002", second.orderNumber);
}

TEST_F(ReserveOrderUseCaseTest, ReturnsSampleNotFoundWhenSampleCodeUnknown) {
    ReserveOrderUseCase useCase(sampleRepository_, orderRepository_);

    OrderReservationResult result = useCase.Reserve(MakeInput("S-999"));

    EXPECT_EQ(OrderReservationOutcome::SampleNotFound, result.outcome);
    EXPECT_TRUE(orderRepository_.readAll().empty());
}

TEST_F(ReserveOrderUseCaseTest, ReturnsInvalidInputWhenCustomerNameIsBlank) {
    ReserveOrderUseCase useCase(sampleRepository_, orderRepository_);

    OrderReservationResult result = useCase.Reserve(MakeInput("S-001", ""));

    EXPECT_EQ(OrderReservationOutcome::InvalidInput, result.outcome);
}

TEST_F(ReserveOrderUseCaseTest, ReturnsInvalidInputWhenCustomerNameIsWhitespaceOnly) {
    // FakeOrderRepository도 실제 OrderRepository처럼 trim 후 판정해야 한다(test-auditor 지적).
    ReserveOrderUseCase useCase(sampleRepository_, orderRepository_);

    OrderReservationResult result = useCase.Reserve(MakeInput("S-001", "   "));

    EXPECT_EQ(OrderReservationOutcome::InvalidInput, result.outcome);
}

TEST_F(ReserveOrderUseCaseTest, ReturnsInvalidInputWhenQuantityIsZeroOrNegative) {
    ReserveOrderUseCase useCase(sampleRepository_, orderRepository_);

    EXPECT_EQ(OrderReservationOutcome::InvalidInput,
              useCase.Reserve(MakeInput("S-001", "Customer", 0)).outcome);
    EXPECT_EQ(OrderReservationOutcome::InvalidInput,
              useCase.Reserve(MakeInput("S-001", "Customer", -1)).outcome);
}

}  // namespace
}  // namespace SampleOrderSystem
