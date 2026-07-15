#include "SampleRepository.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <stdexcept>

#include "JsonStore.h"
#include "SampleDocument.h"

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
    // JsonStore::read는 파일이 없거나 내용이 비어 있으면 "빈 배열"(JSON_CRUD 시절의 bare array
    // 기본값)을 반환한다. 이 저장소의 계약은 object 루트({schemaVersion, records})이므로, 파일이
    // 아직 없는 경우는 SampleDocument::fromJson으로 넘기지 않고 여기서 "레코드 없음"으로 직접
    // 처리한다 — 그래야 실제로 손상된 object 루트(잘못된 schemaVersion 등)와 혼동되지 않는다.
    if (!std::filesystem::exists(path_)) {
        return {};
    }
    return SampleDocument::fromJson(JsonStore::read(path_));
}

void SampleRepository::writeAll(const std::vector<SampleRecord>& records) const {
    std::filesystem::path filePath(path_);
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path());
    }
    JsonStore::write(path_, SampleDocument::toJson(records));
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
