#include <gtest/gtest.h>

#include "FakeOrderRepository.h"
#include "ListReservedOrdersUseCase.h"

namespace SampleOrderSystem {
namespace {

TEST(ListReservedOrdersUseCaseTest, ReturnsEmptyWhenNoOrdersReserved) {
    FakeOrderRepository orderRepository;
    ListReservedOrdersUseCase useCase(orderRepository);

    EXPECT_TRUE(useCase.ListReserved().empty());
}

TEST(ListReservedOrdersUseCaseTest, ExcludesNonReservedOrders) {
    FakeOrderRepository orderRepository;
    orderRepository.Seed(
        Order{Guid::NewGuid(), "ORD-00001", "S-001", "고객A", 10, OrderStatus::Reserved});
    orderRepository.Seed(
        Order{Guid::NewGuid(), "ORD-00002", "S-002", "고객B", 20, OrderStatus::Confirmed});
    orderRepository.Seed(
        Order{Guid::NewGuid(), "ORD-00003", "S-003", "고객C", 30, OrderStatus::Rejected});
    ListReservedOrdersUseCase useCase(orderRepository);

    std::vector<OrderViewModel> result = useCase.ListReserved();

    ASSERT_EQ(1u, result.size());
    EXPECT_EQ("ORD-00001", result[0].orderNumber);
}

}  // namespace
}  // namespace SampleOrderSystem
