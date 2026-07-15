#pragma once

#include <string>

namespace SampleOrderSystem {

// Presentation-layer read model: View는 Domain 타입 Order를 직접 다루지 않는다.
struct OrderViewModel {
    std::string orderNumber;
    std::string sampleCode;
    std::string customerName;
    int quantity = 0;
    std::string status;  // "RESERVED" 등 표시용 문자열
};

}  // namespace SampleOrderSystem
