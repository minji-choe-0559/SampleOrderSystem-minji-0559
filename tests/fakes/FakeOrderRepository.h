#pragma once

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "IOrderRepository.h"

namespace SampleOrderSystem {

// 실제 파일 I/O 없이 ReserveOrderUseCase를 검증하기 위한 인메모리 Fake. 실제 OrderRepository와
// 동일하게 sampleCode/customerName을 trim한 뒤 공백 여부를 판정하고, quantity <= 0을 거부하며
// 순번 기반 주문번호를 채번한다(test-auditor 지적: trim 없이는 공백-only 값이 real Repository와
// 다르게 통과해버려 Fake/실물 검증 로직이 갈라진다).
class FakeOrderRepository : public IOrderRepository {
  public:
    Order create(const std::string& sampleCode, const std::string& customerName,
                 int quantity) override {
        std::string code = Trim(sampleCode);
        std::string name = Trim(customerName);
        if (code.empty()) {
            throw std::invalid_argument("FakeOrderRepository: sampleCode must not be blank");
        }
        if (name.empty()) {
            throw std::invalid_argument("FakeOrderRepository: customerName must not be blank");
        }
        if (quantity <= 0) {
            throw std::invalid_argument("FakeOrderRepository: quantity must be > 0");
        }

        std::ostringstream oss;
        oss << "ORD-" << std::setfill('0') << std::setw(5) << (orders_.size() + 1);
        Order order{Guid::NewGuid(), oss.str(), code, name, quantity, OrderStatus::Reserved};
        orders_.push_back(order);
        return order;
    }

    std::vector<Order> readAll() const override { return orders_; }

  private:
    static std::string Trim(const std::string& text) {
        size_t begin = text.find_first_not_of(" \t\r\n");
        if (begin == std::string::npos) {
            return "";
        }
        size_t end = text.find_last_not_of(" \t\r\n");
        return text.substr(begin, end - begin + 1);
    }

    std::vector<Order> orders_;
};

}  // namespace SampleOrderSystem
