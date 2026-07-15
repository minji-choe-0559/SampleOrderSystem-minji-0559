#pragma once

#include <windows.h>

#include "IAtomicFileOperations.h"

namespace SampleOrderSystem {

// IAtomicFileOperations의 실제 Win32 구현. docs/ATOMIC_WRITE_DESIGN.md 4~5장 참고.
// 핸들은 멤버로 보관하며 RAII로 관리한다 — 소멸자는 열린 핸들이 남아 있으면 best-effort로
// CloseHandle만 시도한다(임시 파일 삭제는 오케스트레이션 쪽의 명시적 cleanup() 책임).
class Win32AtomicFileOperations final : public IAtomicFileOperations {
  public:
    Win32AtomicFileOperations() = default;
    ~Win32AtomicFileOperations() override;

    bool open(const std::string& tempPath) override;
    bool write(const std::string& content) override;
    bool flush() override;
    bool close() override;
    bool commit(const std::string& tempPath, const std::string& targetPath) override;
    bool cleanup(const std::string& tempPath) override;

  private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};

}  // namespace SampleOrderSystem
