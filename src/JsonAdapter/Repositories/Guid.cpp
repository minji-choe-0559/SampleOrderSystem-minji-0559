#include "Guid.h"

#include <cctype>
#include <cstdint>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>

namespace SampleOrderSystem {

namespace {

bool isHexDigit(char c) { return std::isxdigit(static_cast<unsigned char>(c)); }

}  // namespace

Guid::Guid(std::string value) : value_(std::move(value)) {}

Guid Guid::NewGuid() {
    static thread_local std::mt19937_64 engine(std::random_device{}());
    std::uniform_int_distribution<std::uint64_t> dist;

    std::uint64_t hi = dist(engine);
    std::uint64_t lo = dist(engine);
    // RFC 4122 version 4 / variant 1 비트를 설정한다.
    hi = (hi & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    lo = (lo & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(8) << static_cast<std::uint32_t>(hi >> 32)
        << '-' << std::setw(4) << static_cast<std::uint16_t>(hi >> 16) << '-' << std::setw(4)
        << static_cast<std::uint16_t>(hi) << '-' << std::setw(4)
        << static_cast<std::uint16_t>(lo >> 48) << '-' << std::setw(12) << (lo & 0xFFFFFFFFFFFFULL);
    return Guid(oss.str());
}

Guid Guid::Parse(const std::string& text) {
    if (text.size() != 36) {
        throw std::invalid_argument("Guid: invalid length");
    }
    for (size_t pos : {8u, 13u, 18u, 23u}) {
        if (text[pos] != '-') {
            throw std::invalid_argument("Guid: invalid format");
        }
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            continue;
        }
        if (!isHexDigit(text[i])) {
            throw std::invalid_argument("Guid: invalid hex character");
        }
    }
    return Guid(text);
}

const std::string& Guid::toString() const { return value_; }

bool Guid::operator==(const Guid& other) const { return value_ == other.value_; }

bool Guid::operator!=(const Guid& other) const { return !(*this == other); }

}  // namespace SampleOrderSystem
