#pragma once

#include <string>

#include "Guid.h"

namespace SampleOrderSystem {

// 승인 처리 시 "재고 반영 + 주문 상태 전환"을 하나의 원자적 쓰기로 묶기 위한 Port(PRD.md 5.5.5의
// "상태 전이 + 재고 반영 + Order 전환은 하나의 인메모리 연산으로 묶고 단일 원자적 파일 쓰기로
// 영속화한다" 원칙을 Phase 3 승인 시점에도 동일하게 적용). ISampleRepository/IOrderRepository/
// IProductionQueueRepository는 각 애그리게잇 CRUD만 책임지고, 이 인터페이스는 승인 시점에만
// 필요한 교차 애그리게잇 원자적 갱신을 전담한다(code-reviewer 지적: 두 번의 별도 쓰기로 나뉘어
// 있으면 그 사이 크래시 시 재고만 차감되고 주문은 RESERVED로 남는 문제가 있었음).
class IOrderApprovalRepository {
  public:
    virtual ~IOrderApprovalRepository() = default;

    // 재고 충분 경로: sampleCode 재고에서 quantity를 차감하고 orderNumber를 CONFIRMED로
    // 전환한다. 시료/주문 중 하나라도 찾지 못하면 false를 반환한다(예외 아님).
    virtual bool ConfirmWithStockDeduction(const std::string& orderNumber,
                                           const std::string& sampleCode, int quantity) = 0;

    // 재고 부족 경로: ProductionJob을 생성하고 orderNumber를 PRODUCING으로 전환한다. 주문을
    // 찾지 못하면 false를 반환한다(예외 아님). productionQuantity <= 0이면 std::invalid_argument.
    virtual bool TransitionToProducing(const std::string& orderNumber, const Guid& orderId,
                                       const std::string& sampleCode, int productionQuantity) = 0;
};

}  // namespace SampleOrderSystem
