#include "AtomicWrite.h"

#include <filesystem>
#include <random>
#include <sstream>
#include <stdexcept>

namespace SampleOrderSystem {

namespace {

constexpr int kMaxTempNameAttempts = 8;

// 대상 파일과 같은 디렉터리(=같은 볼륨)에 고유한 sibling 임시 파일명을 만든다. 고정된
// "target.tmp" 이름을 쓰지 않아 동시 실행이나 이전 실패의 잔여 임시 파일과 충돌하지 않게 한다.
// 이름이 이미 존재하면 제한된 횟수만 재시도한다(무한 재시도 없음).
std::string makeTempPath(const std::string& targetPath) {
    std::filesystem::path target(targetPath);
    std::filesystem::path dir =
        target.has_parent_path() ? target.parent_path() : std::filesystem::path(".");
    std::string stem = target.filename().string();

    static thread_local std::mt19937_64 rng(std::random_device{}());

    for (int attempt = 0; attempt < kMaxTempNameAttempts; ++attempt) {
        std::ostringstream suffix;
        suffix << std::hex << rng();
        std::filesystem::path candidate = dir / (stem + "." + suffix.str() + ".tmp");
        if (!std::filesystem::exists(candidate)) {
            return candidate.string();
        }
    }
    throw std::runtime_error(
        "writeJsonAtomically: failed to allocate a unique temp file name after " +
        std::to_string(kMaxTempNameAttempts) + " attempts");
}

}  // namespace

void writeJsonAtomically(const std::string& targetPath, const std::string& content,
                         IAtomicFileOperations& operations) {
    std::string tempPath = makeTempPath(targetPath);

    if (!operations.open(tempPath)) {
        throw std::runtime_error("writeJsonAtomically: failed to create temp file: " + tempPath);
    }

    bool writeOk = operations.write(content);
    bool flushOk = writeOk && operations.flush();
    // open이 성공한 뒤에는 write/flush 성공 여부와 무관하게 핸들을 열어둔 채로 남기지 않기 위해
    // close를 시도한다 — 다만 close 실패는 write/flush 실패보다 먼저 발생한 것으로 간주하지 않는다.
    bool closeOk = operations.close();

    std::string primaryError;
    if (!writeOk) {
        primaryError = "writeJsonAtomically: write failed for temp file: " + tempPath;
    } else if (!flushOk) {
        primaryError = "writeJsonAtomically: flush failed for temp file: " + tempPath;
    } else if (!closeOk) {
        primaryError = "writeJsonAtomically: close failed for temp file: " + tempPath;
    }

    if (!primaryError.empty()) {
        if (!operations.cleanup(tempPath)) {
            primaryError += " (temp file cleanup also failed: " + tempPath + ")";
        }
        throw std::runtime_error(primaryError);
    }

    if (!operations.commit(tempPath, targetPath)) {
        std::string error = "writeJsonAtomically: commit failed (target unchanged): " + targetPath;
        if (!operations.cleanup(tempPath)) {
            error += " (temp file cleanup also failed: " + tempPath + ")";
        }
        throw std::runtime_error(error);
    }

    // 성공: 임시 파일은 commit()이 대상 파일로 이미 반영/소비했으므로 cleanup을 호출하지 않는다.
}

}  // namespace SampleOrderSystem
