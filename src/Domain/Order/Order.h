#pragma once

#include <string>

#include "Guid.h"

namespace SampleOrderSystem {

// PRD.md 4.1 확정 상태 머신. REJECTED는 정상 흐름 밖이며 모니터링에서 제외한다(PRD.md 5.6).
enum class OrderStatus {
    Reserved,
    Rejected,
    Producing,
    Confirmed,
    Release,
};

// PRD.md 5.3에서 확정된 필드. orderId는 시스템이 발급하는 내부 식별자(Guid)이고, orderNumber는
// 사용자 대면 식별자로 SampleRecord.sampleCode와 달리 사용자 입력이 아니라 시스템이 순번 기반으로
// 자동 채번한다(PRD.md 8장 확정).
struct Order {
    Guid orderId;
    std::string orderNumber;
    std::string sampleCode;
    std::string customerName;
    int quantity = 0;
    OrderStatus status = OrderStatus::Reserved;
};

}  // namespace SampleOrderSystem
