#include "SharedDocument.h"

#include <filesystem>
#include <map>
#include <stdexcept>

#include "JsonStore.h"

namespace SampleOrderSystem {

JsonValue ReadSharedDocument(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::map<std::string, JsonValue> empty;
        empty["schemaVersion"] = JsonValue(kSharedDocumentSchemaVersion);
        return JsonValue(empty);
    }

    JsonValue document = JsonStore::read(path);
    if (!document.isObject()) {
        throw std::runtime_error(
            "SharedDocument: unsupported root — expected a versioned object document "
            "{schemaVersion, ...}; legacy bare-array JSON_CRUD files are not supported");
    }

    const auto& fields = document.asObject();
    auto versionIt = fields.find("schemaVersion");
    if (versionIt == fields.end() || !versionIt->second.isNumber()) {
        throw std::runtime_error("SharedDocument: missing or invalid schemaVersion");
    }
    int version = static_cast<int>(versionIt->second.asNumber());
    if (version != kSharedDocumentSchemaVersion) {
        throw std::runtime_error(
            "SharedDocument: unsupported schemaVersion " + std::to_string(version) +
            " (supported: " + std::to_string(kSharedDocumentSchemaVersion) + ")");
    }
    return document;
}

void WriteSharedDocument(const std::string& path, const JsonValue& document) {
    std::filesystem::path filePath(path);
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path());
    }
    JsonStore::write(path, document);
}

JsonValue WithArrayField(const JsonValue& document, const std::string& field,
                         JsonValue arrayValue) {
    std::map<std::string, JsonValue> fields = document.asObject();
    fields["schemaVersion"] = JsonValue(kSharedDocumentSchemaVersion);
    fields[field] = std::move(arrayValue);
    return JsonValue(fields);
}

std::vector<JsonValue> GetArrayField(const JsonValue& document, const std::string& field) {
    const auto& fields = document.asObject();
    auto it = fields.find(field);
    if (it == fields.end()) {
        return {};
    }
    if (!it->second.isArray()) {
        throw std::runtime_error("SharedDocument: field '" + field + "' is not an array");
    }
    return it->second.asArray();
}

}  // namespace SampleOrderSystem
