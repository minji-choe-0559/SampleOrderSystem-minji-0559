#include "JsonValue.h"

#include <stdexcept>

namespace SampleOrderSystem {

JsonValue::JsonValue() : m_value(std::monostate{}) {}

JsonValue::JsonValue(bool value) : m_value(value) {}

JsonValue::JsonValue(int value) : m_value(static_cast<double>(value)) {}

JsonValue::JsonValue(double value) : m_value(value) {}

JsonValue::JsonValue(const char* value) : m_value(std::string(value)) {}

JsonValue::JsonValue(std::string value) : m_value(std::move(value)) {}

JsonValue::JsonValue(std::vector<JsonValue> value) : m_value(std::move(value)) {}

JsonValue::JsonValue(std::map<std::string, JsonValue> value) : m_value(std::move(value)) {}

bool JsonValue::isNull() const { return std::holds_alternative<std::monostate>(m_value); }

bool JsonValue::isBool() const { return std::holds_alternative<bool>(m_value); }

bool JsonValue::isNumber() const { return std::holds_alternative<double>(m_value); }

bool JsonValue::isString() const { return std::holds_alternative<std::string>(m_value); }

bool JsonValue::isArray() const { return std::holds_alternative<std::vector<JsonValue>>(m_value); }

bool JsonValue::isObject() const {
    return std::holds_alternative<std::map<std::string, JsonValue>>(m_value);
}

bool JsonValue::asBool() const {
    if (!isBool()) {
        throw std::runtime_error("JsonValue is not a bool");
    }
    return std::get<bool>(m_value);
}

double JsonValue::asNumber() const {
    if (!isNumber()) {
        throw std::runtime_error("JsonValue is not a number");
    }
    return std::get<double>(m_value);
}

const std::string& JsonValue::asString() const {
    if (!isString()) {
        throw std::runtime_error("JsonValue is not a string");
    }
    return std::get<std::string>(m_value);
}

const std::vector<JsonValue>& JsonValue::asArray() const {
    if (!isArray()) {
        throw std::runtime_error("JsonValue is not an array");
    }
    return std::get<std::vector<JsonValue>>(m_value);
}

const std::map<std::string, JsonValue>& JsonValue::asObject() const {
    if (!isObject()) {
        throw std::runtime_error("JsonValue is not an object");
    }
    return std::get<std::map<std::string, JsonValue>>(m_value);
}

const JsonValue& JsonValue::operator[](const std::string& key) const {
    const auto& fields = asObject();
    auto it = fields.find(key);
    if (it == fields.end()) {
        throw std::runtime_error("JsonValue object has no key: " + key);
    }
    return it->second;
}

const JsonValue& JsonValue::operator[](size_t index) const {
    const auto& items = asArray();
    if (index >= items.size()) {
        throw std::runtime_error("JsonValue array index out of range");
    }
    return items[index];
}

}  // namespace SampleOrderSystem
