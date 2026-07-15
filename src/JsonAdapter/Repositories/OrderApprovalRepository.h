#pragma once

#include <string>

#include "IOrderApprovalRepository.h"

namespace SampleOrderSystem {

// IOrderApprovalRepository의 JSON 구현체. Sample/Order/ProductionJob이 공유하는 문서를 한 번만
// 읽고 관련 배열을 전부 갱신한 뒤 한 번만 쓴다(원자성 보장).
class OrderApprovalRepository : public IOrderApprovalRepository {
  public:
    explicit OrderApprovalRepository(std::string path);

    bool ConfirmWithStockDeduction(const std::string& orderNumber, const std::string& sampleCode,
                                   int quantity) override;

    bool TransitionToProducing(const std::string& orderNumber, const Guid& orderId,
                               const std::string& sampleCode, int productionQuantity) override;

  private:
    std::string path_;
};

}  // namespace SampleOrderSystem
