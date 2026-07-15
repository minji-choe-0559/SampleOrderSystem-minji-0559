#include <gtest/gtest.h>

#include "FakeOrderRepository.h"
#include "RejectOrderUseCase.h"

namespace SampleOrderSystem {
namespace {

TEST(RejectOrderUseCaseTest, TransitionsReservedOrderToRejected) {
    FakeOrderRepository orderRepository;
    orderRepository.Seed(
        Order{Guid::NewGuid(), "ORD-00001", "S-001", "고객", 10, OrderStatus::Reserved});
    RejectOrderUseCase useCase(orderRepository);

    RejectOrderResult result = useCase.Reject("ORD-00001");

    EXPECT_EQ(RejectOrderOutcome::Success, result.outcome);
    EXPECT_EQ(OrderStatus::Rejected, orderRepository.findByOrderNumber("ORD-00001")->status);
}

TEST(RejectOrderUseCaseTest, ReturnsOrderNotFoundWhenOrderNumberUnknown) {
    FakeOrderRepository orderRepository;
    RejectOrderUseCase useCase(orderRepository);

    EXPECT_EQ(RejectOrderOutcome::OrderNotFound, useCase.Reject("ORD-99999").outcome);
}

TEST(RejectOrderUseCaseTest, ReturnsOrderNotReservedWhenAlreadyProcessed) {
    FakeOrderRepository orderRepository;
    orderRepository.Seed(
        Order{Guid::NewGuid(), "ORD-00001", "S-001", "고객", 10, OrderStatus::Confirmed});
    RejectOrderUseCase useCase(orderRepository);

    EXPECT_EQ(RejectOrderOutcome::OrderNotReserved, useCase.Reject("ORD-00001").outcome);
}

}  // namespace
}  // namespace SampleOrderSystem
