#pragma once

#include <string>

namespace SampleOrderSystem {

enum class OrderReservationOutcome {
    Success,
    SampleNotFound,
    InvalidInput,
    StorageFailure,
};

// Presentation-layer result: ReserveOrderUseCase가 시료 존재 여부/Repository 예외를 이 값으로
// 변환한 뒤 Controller가 View에 전달한다.
struct OrderReservationResult {
    OrderReservationOutcome outcome = OrderReservationOutcome::Success;
    std::string message;
    std::string orderNumber;  // Success일 때만 의미 있음
};

}  // namespace SampleOrderSystem
