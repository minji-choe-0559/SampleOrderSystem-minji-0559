#include "Win32AtomicFileOperations.h"

#include <filesystem>

namespace SampleOrderSystem {

namespace {

std::wstring toWidePath(const std::string& path) { return std::filesystem::path(path).wstring(); }

bool fileExists(const std::wstring& widePath) {
    return GetFileAttributesW(widePath.c_str()) != INVALID_FILE_ATTRIBUTES;
}

}  // namespace

Win32AtomicFileOperations::~Win32AtomicFileOperations() {
    if (handle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
    }
}

bool Win32AtomicFileOperations::open(const std::string& tempPath) {
    std::wstring wide = toWidePath(tempPath);
    // CREATE_NEW: 같은 이름의 파일이 이미 있으면 실패한다(조용히 덮어쓰지 않음).
    handle_ = CreateFileW(wide.c_str(), GENERIC_WRITE, 0 /* 공유 없음 */, nullptr, CREATE_NEW,
                          FILE_ATTRIBUTE_NORMAL, nullptr);
    return handle_ != INVALID_HANDLE_VALUE;
}

bool Win32AtomicFileOperations::write(const std::string& content) {
    if (handle_ == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD bytesWritten = 0;
    DWORD toWrite = static_cast<DWORD>(content.size());
    BOOL ok = WriteFile(handle_, content.data(), toWrite, &bytesWritten, nullptr);
    return ok && bytesWritten == toWrite;
}

bool Win32AtomicFileOperations::flush() {
    if (handle_ == INVALID_HANDLE_VALUE) {
        return false;
    }
    return FlushFileBuffers(handle_) != 0;
}

bool Win32AtomicFileOperations::close() {
    if (handle_ == INVALID_HANDLE_VALUE) {
        return false;
    }
    BOOL ok = CloseHandle(handle_);
    handle_ = INVALID_HANDLE_VALUE;
    return ok != 0;
}

bool Win32AtomicFileOperations::commit(const std::string& tempPath, const std::string& targetPath) {
    std::wstring wideTemp = toWidePath(tempPath);
    std::wstring wideTarget = toWidePath(targetPath);

    if (fileExists(wideTarget)) {
        // 기존 파일을 대체한다 — Atomic Write에 필요한 최소 옵션만 사용한다.
        return ReplaceFileW(wideTarget.c_str(), wideTemp.c_str(), nullptr, 0, nullptr, nullptr) !=
               0;
    }

    // 최초 저장(대상이 아직 없음): MOVEFILE_COPY_ALLOWED는 사용하지 않는다 — 볼륨 간 copy+delete로
    // 대체되면 원자성이 깨지므로, 실패를 감추는 fallback 대신 그대로 실패시킨다.
    // MOVEFILE_WRITE_THROUGH: rename이 반환되기 전에 메타데이터 변경이 디스크에 반영되도록
    // 보장한다.
    return MoveFileExW(wideTemp.c_str(), wideTarget.c_str(), MOVEFILE_WRITE_THROUGH) != 0;
}

bool Win32AtomicFileOperations::cleanup(const std::string& tempPath) {
    std::wstring wide = toWidePath(tempPath);
    if (!fileExists(wide)) {
        // commit()이 이미 temp를 소비했거나 애초에 만들어지지 않은 경우 — 정리할 것이 없으므로
        // 성공으로 간주한다(멱등).
        return true;
    }
    return DeleteFileW(wide.c_str()) != 0;
}

}  // namespace SampleOrderSystem
