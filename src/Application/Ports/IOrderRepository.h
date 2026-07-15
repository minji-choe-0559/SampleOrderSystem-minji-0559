#pragma once

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
};

}  // namespace SampleOrderSystem
