#include "JsonParser.h"

#include <cctype>
#include <stdexcept>

namespace SampleOrderSystem {

namespace {

void skipWhitespace(const std::string& text, size_t& pos) {
    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
        ++pos;
    }
}

void expect(const std::string& text, size_t& pos, char c) {
    if (pos >= text.size() || text[pos] != c) {
        throw std::runtime_error("JsonParser: expected '" + std::string(1, c) + "'");
    }
    ++pos;
}

JsonValue parseValue(const std::string& text, size_t& pos);

JsonValue parseNull(const std::string& text, size_t& pos) {
    if (text.compare(pos, 4, "null") != 0) {
        throw std::runtime_error("JsonParser: invalid literal");
    }
    pos += 4;
    return JsonValue();
}

JsonValue parseBool(const std::string& text, size_t& pos) {
    if (text.compare(pos, 4, "true") == 0) {
        pos += 4;
        return JsonValue(true);
    }
    if (text.compare(pos, 5, "false") == 0) {
        pos += 5;
        return JsonValue(false);
    }
    throw std::runtime_error("JsonParser: invalid literal");
}

JsonValue parseNumber(const std::string& text, size_t& pos) {
    size_t start = pos;
    if (pos < text.size() && text[pos] == '-') {
        ++pos;
    }
    if (pos >= text.size() || !std::isdigit(static_cast<unsigned char>(text[pos]))) {
        throw std::runtime_error("JsonParser: invalid number");
    }
    while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos]))) {
        ++pos;
    }
    if (pos < text.size() && text[pos] == '.') {
        ++pos;
        if (pos >= text.size() || !std::isdigit(static_cast<unsigned char>(text[pos]))) {
            throw std::runtime_error("JsonParser: invalid number");
        }
        while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos]))) {
            ++pos;
        }
    }
    return JsonValue(std::stod(text.substr(start, pos - start)));
}

std::string parseRawString(const std::string& text, size_t& pos) {
    expect(text, pos, '"');
    std::string result;
    while (true) {
        if (pos >= text.size()) {
            throw std::runtime_error("JsonParser: unterminated string");
        }
        char c = text[pos];
        if (c == '"') {
            ++pos;
            break;
        }
        if (c == '\\') {
            ++pos;
            if (pos >= text.size()) {
                throw std::runtime_error("JsonParser: unterminated escape");
            }
            char escaped = text[pos];
            switch (escaped) {
                case '"':
                    result += '"';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '/':
                    result += '/';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'r':
                    result += '\r';
                    break;
                default:
                    throw std::runtime_error("JsonParser: unsupported escape sequence");
            }
            ++pos;
        } else {
            result += c;
            ++pos;
        }
    }
    return result;
}

JsonValue parseString(const std::string& text, size_t& pos) {
    return JsonValue(parseRawString(text, pos));
}

JsonValue parseArray(const std::string& text, size_t& pos) {
    expect(text, pos, '[');
    std::vector<JsonValue> items;
    skipWhitespace(text, pos);
    if (pos < text.size() && text[pos] == ']') {
        ++pos;
        return JsonValue(items);
    }
    while (true) {
        skipWhitespace(text, pos);
        items.push_back(parseValue(text, pos));
        skipWhitespace(text, pos);
        if (pos < text.size() && text[pos] == ',') {
            ++pos;
            continue;
        }
        break;
    }
    expect(text, pos, ']');
    return JsonValue(items);
}

JsonValue parseObject(const std::string& text, size_t& pos) {
    expect(text, pos, '{');
    std::map<std::string, JsonValue> fields;
    skipWhitespace(text, pos);
    if (pos < text.size() && text[pos] == '}') {
        ++pos;
        return JsonValue(fields);
    }
    while (true) {
        skipWhitespace(text, pos);
        std::string key = parseRawString(text, pos);
        skipWhitespace(text, pos);
        expect(text, pos, ':');
        skipWhitespace(text, pos);
        fields[key] = parseValue(text, pos);
        skipWhitespace(text, pos);
        if (pos < text.size() && text[pos] == ',') {
            ++pos;
            continue;
        }
        break;
    }
    expect(text, pos, '}');
    return JsonValue(fields);
}

JsonValue parseValue(const std::string& text, size_t& pos) {
    skipWhitespace(text, pos);
    if (pos >= text.size()) {
        throw std::runtime_error("JsonParser: unexpected end of input");
    }
    char c = text[pos];
    if (c == 'n') return parseNull(text, pos);
    if (c == 't' || c == 'f') return parseBool(text, pos);
    if (c == '"') return parseString(text, pos);
    if (c == '[') return parseArray(text, pos);
    if (c == '{') return parseObject(text, pos);
    if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber(text, pos);
    throw std::runtime_error("JsonParser: unexpected character");
}

}  // namespace

JsonValue JsonParser::parse(const std::string& text) {
    size_t pos = 0;
    JsonValue value = parseValue(text, pos);
    skipWhitespace(text, pos);
    if (pos != text.size()) {
        throw std::runtime_error("JsonParser: trailing characters after value");
    }
    return value;
}

}  // namespace SampleOrderSystem
