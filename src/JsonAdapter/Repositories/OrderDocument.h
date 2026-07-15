#pragma once

#include <vector>

#include "JsonValue.h"
#include "Order.h"

namespace SampleOrderSystem {

// Order 배열 <-> JsonValue 배열 변환만 전담한다(SampleDocument와 동일한 역할 분리).
namespace OrderDocument {

JsonValue toJson(const std::vector<Order>& orders);

std::vector<Order> fromJson(const std::vector<JsonValue>& items);

}  // namespace OrderDocument

}  // namespace SampleOrderSystem
