#include "OrderDocument.h"

#include "OrderJsonMapper.h"

namespace SampleOrderSystem {

namespace OrderDocument {

JsonValue toJson(const std::vector<Order>& orders) {
    std::vector<JsonValue> items;
    items.reserve(orders.size());
    for (const auto& order : orders) {
        items.push_back(SampleOrderSystem::toJson(order));
    }
    return JsonValue(items);
}

std::vector<Order> fromJson(const std::vector<JsonValue>& items) {
    std::vector<Order> orders;
    orders.reserve(items.size());
    for (const auto& item : items) {
        orders.push_back(SampleOrderSystem::fromJson(item));
    }
    return orders;
}

}  // namespace OrderDocument

}  // namespace SampleOrderSystem
