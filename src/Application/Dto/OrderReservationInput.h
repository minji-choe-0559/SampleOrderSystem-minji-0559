#pragma once

#include <string>

namespace SampleOrderSystem {

// Presentation-layer input DTO: 시료 ID/고객명/주문 수량(PRD.md 5.3).
struct OrderReservationInput {
    std::string sampleCode;
    std::string customerName;
    int quantity = 0;
};

}  // namespace SampleOrderSystem
