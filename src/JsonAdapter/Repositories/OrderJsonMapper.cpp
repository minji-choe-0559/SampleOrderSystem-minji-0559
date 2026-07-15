#include "OrderJsonMapper.h"

#include <stdexcept>

namespace SampleOrderSystem {

namespace {

std::string ToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::Reserved:
            return "RESERVED";
        case OrderStatus::Rejected:
            return "REJECTED";
        case OrderStatus::Producing:
            return "PRODUCING";
        case OrderStatus::Confirmed:
            return "CONFIRMED";
        case OrderStatus::Release:
            return "RELEASE";
    }
    throw std::invalid_argument("OrderJsonMapper: unknown OrderStatus");
}

OrderStatus ParseStatus(const std::string& text) {
    if (text == "RESERVED") return OrderStatus::Reserved;
    if (text == "REJECTED") return OrderStatus::Rejected;
    if (text == "PRODUCING") return OrderStatus::Producing;
    if (text == "CONFIRMED") return OrderStatus::Confirmed;
    if (text == "RELEASE") return OrderStatus::Release;
    throw std::runtime_error("OrderJsonMapper: unknown order status '" + text + "'");
}

}  // namespace

JsonValue toJson(const Order& order) {
    std::map<std::string, JsonValue> fields;
    fields["orderId"] = JsonValue(order.orderId.toString());
    fields["orderNumber"] = JsonValue(order.orderNumber);
    fields["sampleCode"] = JsonValue(order.sampleCode);
    fields["customerName"] = JsonValue(order.customerName);
    fields["quantity"] = JsonValue(order.quantity);
    fields["status"] = JsonValue(ToString(order.status));
    return JsonValue(fields);
}

Order fromJson(const JsonValue& value) {
    return Order{
        Guid::Parse(value["orderId"].asString()),
        value["orderNumber"].asString(),
        value["sampleCode"].asString(),
        value["customerName"].asString(),
        static_cast<int>(value["quantity"].asNumber()),
        ParseStatus(value["status"].asString()),
    };
}

}  // namespace SampleOrderSystem
