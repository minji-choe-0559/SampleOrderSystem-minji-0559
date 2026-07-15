#include "ConsoleInputParsing.h"

#include <cctype>
#include <stdexcept>

namespace SampleOrderSystem {

namespace {

bool IsBlank(const std::string& text) {
    for (char c : text) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

bool HasOnlyTrailingWhitespace(const std::string& line, size_t consumed) {
    for (size_t i = consumed; i < line.size(); ++i) {
        if (!std::isspace(static_cast<unsigned char>(line[i]))) {
            return false;
        }
    }
    return true;
}

}  // namespace

std::optional<int> ParseInt(const std::string& line) {
    if (IsBlank(line)) {
        return std::nullopt;
    }
    try {
        size_t consumed = 0;
        int value = std::stoi(line, &consumed);
        if (!HasOnlyTrailingWhitespace(line, consumed)) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<int> ParseMenuChoice(const std::string& line) { return ParseInt(line); }

std::optional<double> ParseDouble(const std::string& line) {
    if (IsBlank(line)) {
        return std::nullopt;
    }
    try {
        size_t consumed = 0;
        double value = std::stod(line, &consumed);
        if (!HasOnlyTrailingWhitespace(line, consumed)) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

}  // namespace SampleOrderSystem
