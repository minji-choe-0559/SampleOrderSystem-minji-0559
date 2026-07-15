#pragma once

#include <optional>
#include <string>
#include <vector>

#include "IOrderRepository.h"

namespace SampleOrderSystem {

// Order CRUD 전담 계층. Sample과 같은 공유 JSON 문서를 사용한다(PRD.md 5.5.5) — SharedDocument로
// 문서를 읽고 "orders" 필드만 갱신해, 같은 파일에 있는 다른 애그리게잇의 데이터를 보존한다.
class OrderRepository : public IOrderRepository {
  public:
    explicit OrderRepository(std::string path);

    // sampleCode/customerName 공백, quantity <= 0 이면 std::invalid_argument를 던진다.
    Order create(const std::string& sampleCode, const std::string& customerName,
                 int quantity) override;

    std::vector<Order> readAll() const override;

    std::optional<Order> findByOrderNumber(const std::string& orderNumber) const override;

    std::optional<Order> updateStatus(const std::string& orderNumber,
                                      OrderStatus newStatus) override;

  private:
    void writeAll(const std::vector<Order>& orders) const;
    std::string nextOrderNumber(const std::vector<Order>& existing) const;

    std::string path_;
};

}  // namespace SampleOrderSystem
