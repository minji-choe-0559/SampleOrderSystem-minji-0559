#include "ReserveOrderUseCase.h"

#include <stdexcept>

namespace SampleOrderSystem {

namespace {

// OrderRepository::create는 sampleCode를 trim한 뒤 저장하므로, 시료 존재 확인도 동일하게 trim된
// 값으로 해야 한다(code-reviewer 지적: 앞뒤 공백이 섞인 유효한 시료 ID가 SampleNotFound로
// 잘못 처리되는 문제).
std::string Trim(const std::string& text) {
    size_t begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

}  // namespace

ReserveOrderUseCase::ReserveOrderUseCase(ISampleRepository& sampleRepository,
                                         IOrderRepository& orderRepository)
    : sampleRepository_(sampleRepository), orderRepository_(orderRepository) {}

OrderReservationResult ReserveOrderUseCase::Reserve(const OrderReservationInput& input) {
    std::string sampleCode = Trim(input.sampleCode);
    if (!sampleRepository_.findBySampleCode(sampleCode).has_value()) {
        return OrderReservationResult{OrderReservationOutcome::SampleNotFound,
                                      "등록되지 않은 시료 ID입니다: " + input.sampleCode, ""};
    }

    try {
        Order order = orderRepository_.create(sampleCode, input.customerName, input.quantity);
        return OrderReservationResult{OrderReservationOutcome::Success, "예약 접수 완료.",
                                      order.orderNumber};
    } catch (const std::invalid_argument& e) {
        return OrderReservationResult{OrderReservationOutcome::InvalidInput, e.what(), ""};
    } catch (const std::exception& e) {
        return OrderReservationResult{OrderReservationOutcome::StorageFailure, e.what(), ""};
    }
}

}  // namespace SampleOrderSystem
