#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace SampleOrderSystem {

class JsonValue {
  public:
    JsonValue();
    explicit JsonValue(bool value);
    explicit JsonValue(int value);
    explicit JsonValue(double value);
    explicit JsonValue(const char* value);
    explicit JsonValue(std::string value);
    explicit JsonValue(std::vector<JsonValue> value);
    explicit JsonValue(std::map<std::string, JsonValue> value);

    bool isNull() const;
    bool isBool() const;
    bool isNumber() const;
    bool isString() const;
    bool isArray() const;
    bool isObject() const;

    bool asBool() const;
    double asNumber() const;
    const std::string& asString() const;
    const std::vector<JsonValue>& asArray() const;
    const std::map<std::string, JsonValue>& asObject() const;

    const JsonValue& operator[](const std::string& key) const;
    const JsonValue& operator[](size_t index) const;

  private:
    std::variant<std::monostate, bool, double, std::string, std::vector<JsonValue>,
                 std::map<std::string, JsonValue>>
        m_value;
};

}  // namespace SampleOrderSystem
