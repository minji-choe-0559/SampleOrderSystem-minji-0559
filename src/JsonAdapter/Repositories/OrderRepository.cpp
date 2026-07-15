#include "OrderRepository.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "OrderDocument.h"
#include "SharedDocument.h"

namespace SampleOrderSystem {

namespace {

std::string trim(const std::string& text) {
    size_t begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

void validateFields(const std::string& sampleCode, const std::string& customerName, int quantity) {
    if (sampleCode.empty()) {
        throw std::invalid_argument("OrderRepository: sampleCode must not be blank");
    }
    if (customerName.empty()) {
        throw std::invalid_argument("OrderRepository: customerName must not be blank");
    }
    if (quantity <= 0) {
        throw std::invalid_argument("OrderRepository: quantity must be > 0");
    }
}

}  // namespace

OrderRepository::OrderRepository(std::string path) : path_(std::move(path)) {}

std::vector<Order> OrderRepository::readAll() const {
    JsonValue document = ReadSharedDocument(path_);
    return OrderDocument::fromJson(GetArrayField(document, "orders"));
}

void OrderRepository::writeAll(const std::vector<Order>& orders) const {
    JsonValue document = ReadSharedDocument(path_);
    JsonValue updated = WithArrayField(document, "orders", OrderDocument::toJson(orders));
    WriteSharedDocument(path_, updated);
}

std::string OrderRepository::nextOrderNumber(const std::vector<Order>& existing) const {
    std::ostringstream oss;
    oss << "ORD-" << std::setfill('0') << std::setw(5) << (existing.size() + 1);
    return oss.str();
}

Order OrderRepository::create(const std::string& sampleCode, const std::string& customerName,
                              int quantity) {
    std::string code = trim(sampleCode);
    std::string name = trim(customerName);
    validateFields(code, name, quantity);

    std::vector<Order> orders = readAll();
    Order order{Guid::NewGuid(), nextOrderNumber(orders), code, name,
                quantity,        OrderStatus::Reserved};
    orders.push_back(order);
    writeAll(orders);
    return order;
}

std::optional<Order> OrderRepository::findByOrderNumber(const std::string& orderNumber) const {
    std::vector<Order> orders = readAll();
    auto it = std::find_if(orders.begin(), orders.end(),
                           [&](const Order& order) { return order.orderNumber == orderNumber; });
    if (it == orders.end()) {
        return std::nullopt;
    }
    return *it;
}

std::optional<Order> OrderRepository::updateStatus(const std::string& orderNumber,
                                                   OrderStatus newStatus) {
    std::vector<Order> orders = readAll();
    auto it = std::find_if(orders.begin(), orders.end(),
                           [&](const Order& order) { return order.orderNumber == orderNumber; });
    if (it == orders.end()) {
        return std::nullopt;
    }
    it->status = newStatus;
    writeAll(orders);
    return *it;
}

}  // namespace SampleOrderSystem
