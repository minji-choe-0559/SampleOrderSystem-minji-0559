#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Order.h"

namespace SampleOrderSystem {

// Application이 정의하는 Port. JsonAdapter의 OrderRepository가 이를 구현한다.
class IOrderRepository {
  public:
    virtual ~IOrderRepository() = default;

    virtual Order create(const std::string& sampleCode, const std::string& customerName,
                         int quantity) = 0;

    virtual std::vector<Order> readAll() const = 0;

    virtual std::optional<Order> findByOrderNumber(const std::string& orderNumber) const = 0;

    // orderNumber를 찾지 못하면 std::nullopt를 반환한다(예외 아님).
    virtual std::optional<Order> updateStatus(const std::string& orderNumber,
                                              OrderStatus newStatus) = 0;
};

}  // namespace SampleOrderSystem
