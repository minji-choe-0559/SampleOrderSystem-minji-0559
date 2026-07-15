#include "ListReservedOrdersUseCase.h"

namespace SampleOrderSystem {

namespace {

OrderViewModel ToViewModel(const Order& order) {
    OrderViewModel viewModel;
    viewModel.orderNumber = order.orderNumber;
    viewModel.sampleCode = order.sampleCode;
    viewModel.customerName = order.customerName;
    viewModel.quantity = order.quantity;
    viewModel.status = "RESERVED";  // 이 UseCase는 RESERVED만 다루므로 고정 문자열로 충분하다.
    return viewModel;
}

}  // namespace

ListReservedOrdersUseCase::ListReservedOrdersUseCase(const IOrderRepository& orderRepository)
    : orderRepository_(orderRepository) {}

std::vector<OrderViewModel> ListReservedOrdersUseCase::ListReserved() const {
    std::vector<OrderViewModel> result;
    for (const Order& order : orderRepository_.readAll()) {
        if (order.status == OrderStatus::Reserved) {
            result.push_back(ToViewModel(order));
        }
    }
    return result;
}

}  // namespace SampleOrderSystem
