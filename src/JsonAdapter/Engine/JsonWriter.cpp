#include "JsonWriter.h"

#include <cmath>
#include <sstream>

namespace SampleOrderSystem {

namespace {

std::string escapeString(const std::string& text) {
    std::string result;
    for (char c : text) {
        switch (c) {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\r':
                result += "\\r";
                break;
            default:
                result += c;
                break;
        }
    }
    return result;
}

std::string writeNumber(double value) {
    if (value == std::floor(value)) {
        std::ostringstream oss;
        oss << static_cast<long long>(value);
        return oss.str();
    }
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void writeValue(const JsonValue& value, std::ostringstream& out);

void writeArray(const JsonValue& value, std::ostringstream& out) {
    out << '[';
    const auto& items = value.asArray();
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) {
            out << ',';
        }
        writeValue(items[i], out);
    }
    out << ']';
}

void writeObject(const JsonValue& value, std::ostringstream& out) {
    out << '{';
    const auto& fields = value.asObject();
    bool first = true;
    for (const auto& [key, fieldValue] : fields) {
        if (!first) {
            out << ',';
        }
        first = false;
        out << '"' << escapeString(key) << "\":";
        writeValue(fieldValue, out);
    }
    out << '}';
}

void writeValue(const JsonValue& value, std::ostringstream& out) {
    if (value.isNull()) {
        out << "null";
    } else if (value.isBool()) {
        out << (value.asBool() ? "true" : "false");
    } else if (value.isNumber()) {
        out << writeNumber(value.asNumber());
    } else if (value.isString()) {
        out << '"' << escapeString(value.asString()) << '"';
    } else if (value.isArray()) {
        writeArray(value, out);
    } else if (value.isObject()) {
        writeObject(value, out);
    }
}

}  // namespace

std::string JsonWriter::write(const JsonValue& value) {
    std::ostringstream out;
    writeValue(value, out);
    return out.str();
}

}  // namespace SampleOrderSystem
