#include "JsonStore.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "AtomicWrite.h"
#include "JsonParser.h"
#include "JsonWriter.h"
#include "Win32AtomicFileOperations.h"

namespace SampleOrderSystem {

namespace {

bool isBlank(const std::string& text) {
    for (char c : text) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

}  // namespace

JsonValue JsonStore::read(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return JsonValue(std::vector<JsonValue>{});
    }

    std::ostringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();

    if (isBlank(content)) {
        return JsonValue(std::vector<JsonValue>{});
    }

    return JsonParser::parse(content);
}

void JsonStore::write(const std::string& path, const JsonValue& value) {
    // Step 5부터는 임시 파일 + 원자적 교체(atomic_write/AtomicWrite.h)를 통해 저장한다 — 공개
    // 시그니처는 그대로 유지하되, 저장 실패 시 기존 파일이 보존되도록 내부 구현만 바꾼다.
    std::string content = JsonWriter::write(value);
    Win32AtomicFileOperations ops;
    writeJsonAtomically(path, content, ops);
}

}  // namespace SampleOrderSystem
