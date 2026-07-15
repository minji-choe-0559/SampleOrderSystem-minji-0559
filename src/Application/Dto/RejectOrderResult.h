#pragma once

#include <string>

namespace SampleOrderSystem {

enum class RejectOrderOutcome {
    Success,
    OrderNotFound,
    OrderNotReserved,
    StorageFailure,
};

struct RejectOrderResult {
    RejectOrderOutcome outcome = RejectOrderOutcome::Success;
    std::string message;
};

}  // namespace SampleOrderSystem
