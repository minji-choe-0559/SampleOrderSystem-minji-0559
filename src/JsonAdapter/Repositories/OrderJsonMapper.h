#pragma once

#include "JsonValue.h"
#include "Order.h"

namespace SampleOrderSystem {

JsonValue toJson(const Order& order);
Order fromJson(const JsonValue& value);

}  // namespace SampleOrderSystem
