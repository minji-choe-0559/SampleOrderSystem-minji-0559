#include "SampleRepository.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

#include "SampleDocument.h"
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

bool equalsIgnoreCase(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return false;
    }
    return std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
        return std::tolower(static_cast<unsigned char>(x)) ==
               std::tolower(static_cast<unsigned char>(y));
    });
}

void validateFields(const std::string& sampleCode, const std::string& name, double avgProcessTime,
                    double yieldRate, int stock) {
    if (sampleCode.empty()) {
        throw std::invalid_argument("SampleRepository: sampleCode must not be blank");
    }
    if (name.empty()) {
        throw std::invalid_argument("SampleRepository: name must not be blank");
    }
    if (avgProcessTime < 0.0) {
        throw std::invalid_argument("SampleRepository: avgProcessTime must be >= 0");
    }
    if (yieldRate < 0.0 || yieldRate > 100.0) {
        throw std::invalid_argument("SampleRepository: yieldRate must be within [0, 100]");
    }
    if (stock < 0) {
        throw std::invalid_argument("SampleRepository: stock must be >= 0");
    }
}

}  // namespace

SampleRepository::SampleRepository(std::string path) : path_(std::move(path)) {}

std::vector<SampleRecord> SampleRepository::readAll() const {
    JsonValue document = ReadSharedDocument(path_);
    return SampleDocument::fromJson(GetArrayField(document, "records"));
}

void SampleRepository::writeAll(const std::vector<SampleRecord>& records) const {
    // Order/ProductionJob과 공유하는 문서이므로, 먼저 현재 문서를 읽어 "records" 필드만 교체한다 —
    // 그래야 다른 애그리게잇이 이미 써 둔 데이터를 잃지 않는다(PRD.md 5.5.5).
    JsonValue document = ReadSharedDocument(path_);
    JsonValue updated = WithArrayField(document, "records", SampleDocument::toJson(records));
    WriteSharedDocument(path_, updated);
}

SampleRecord SampleRepository::create(const std::string& sampleCode, const std::string& name,
                                      double avgProcessTime, double yieldRate, int stock) {
    std::string code = trim(sampleCode);
    std::string trimmedName = trim(name);
    validateFields(code, trimmedName, avgProcessTime, yieldRate, stock);

    std::vector<SampleRecord> records = readAll();
    for (const auto& existing : records) {
        if (equalsIgnoreCase(existing.sampleCode, code)) {
            throw std::invalid_argument("SampleRepository: duplicate sampleCode '" + code + "'");
        }
    }

    SampleRecord record{Guid::NewGuid(), code, trimmedName, avgProcessTime, yieldRate, stock};
    records.push_back(record);
    writeAll(records);
    return record;
}

std::optional<SampleRecord> SampleRepository::findBySampleCode(
    const std::string& sampleCode) const {
    std::vector<SampleRecord> records = readAll();
    auto it = std::find_if(records.begin(), records.end(), [&](const SampleRecord& record) {
        return record.sampleCode == sampleCode;
    });
    if (it == records.end()) {
        return std::nullopt;
    }
    return *it;
}

std::optional<SampleRecord> SampleRepository::adjustStock(const std::string& sampleCode,
                                                          int delta) {
    std::vector<SampleRecord> records = readAll();
    auto it = std::find_if(records.begin(), records.end(), [&](const SampleRecord& record) {
        return record.sampleCode == sampleCode;
    });
    if (it == records.end()) {
        return std::nullopt;
    }

    int newStock = it->stock + delta;
    if (newStock < 0) {
        throw std::invalid_argument("SampleRepository: stock must be >= 0");
    }
    it->stock = newStock;
    writeAll(records);
    return *it;
}

std::optional<SampleRecord> SampleRepository::update(const std::string& currentSampleCode,
                                                     const std::string& newSampleCode,
                                                     const std::string& name, double avgProcessTime,
                                                     double yieldRate, int stock) {
    std::vector<SampleRecord> records = readAll();
    auto it = std::find_if(records.begin(), records.end(), [&](const SampleRecord& record) {
        return record.sampleCode == currentSampleCode;
    });
    if (it == records.end()) {
        return std::nullopt;
    }

    std::string code = trim(newSampleCode);
    std::string trimmedName = trim(name);
    validateFields(code, trimmedName, avgProcessTime, yieldRate, stock);

    for (auto other = records.begin(); other != records.end(); ++other) {
        if (other == it) {
            continue;
        }
        if (equalsIgnoreCase(other->sampleCode, code)) {
            throw std::invalid_argument("SampleRepository: duplicate sampleCode '" + code + "'");
        }
    }

    it->sampleCode = code;
    it->name = trimmedName;
    it->avgProcessTime = avgProcessTime;
    it->yieldRate = yieldRate;
    it->stock = stock;
    writeAll(records);
    return *it;
}

bool SampleRepository::remove(const std::string& sampleCode) {
    std::vector<SampleRecord> records = readAll();
    auto it = std::find_if(records.begin(), records.end(), [&](const SampleRecord& record) {
        return record.sampleCode == sampleCode;
    });
    if (it == records.end()) {
        return false;
    }
    records.erase(it);
    writeAll(records);
    return true;
}

}  // namespace SampleOrderSystem
