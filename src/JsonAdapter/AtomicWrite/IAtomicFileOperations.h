#pragma once

#include <string>

namespace SampleOrderSystem {

// 하나의 Atomic Write 작업(open -> write -> flush -> close -> commit)에만 사용하는 최소 인터페이스.
// docs/ATOMIC_WRITE_DESIGN.md 3장의 수명 규칙을 따른다:
//  - 인스턴스 하나는 한 번의 시퀀스에만 쓰고 재사용하지 않는다.
//  - 복사/이동하지 않는다(여러 write 작업이 상태를 공유하면 안 되므로 기반 클래스에서 금지).
//  - open()이 성공하기 전에는 write()/flush()/close()를 호출하지 않는다.
//  - close() 이후에는 write()/flush()를 다시 호출하지 않는다.
class IAtomicFileOperations {
  public:
    virtual ~IAtomicFileOperations() = default;

    IAtomicFileOperations(const IAtomicFileOperations&) = delete;
    IAtomicFileOperations& operator=(const IAtomicFileOperations&) = delete;
    IAtomicFileOperations(IAtomicFileOperations&&) = delete;
    IAtomicFileOperations& operator=(IAtomicFileOperations&&) = delete;

    // 1) tempPath 위치에 임시 파일을 새로 만든다(기존 파일을 조용히 덮어쓰지 않음). 실패 시 false.
    virtual bool open(const std::string& tempPath) = 0;

    // 2) 임시 파일에 전체 content를 기록한다. 일부만 기록된 경우도 실패로 처리해야 한다.
    virtual bool write(const std::string& content) = 0;

    // 3) 기록한 내용을 디스크에 flush한다. 실패 시 false.
    virtual bool flush() = 0;

    // 4) 핸들/스트림을 닫는다. 실패 시 false.
    virtual bool close() = 0;

    // 5) tempPath의 내용을 targetPath에 원자적으로 반영한다. 대상 존재 여부에 따른 내부 분기는
    //    구현체 책임이며 호출자는 알 필요가 없다.
    virtual bool commit(const std::string& tempPath, const std::string& targetPath) = 0;

    // 6) 실패 경로에서 남은 임시 파일을 정리한다. 실패해도 최초 오류를 덮어쓰지 않아야 한다
    //    (오케스트레이션 책임 — 이 메서드 자체는 성공/실패만 보고한다).
    virtual bool cleanup(const std::string& tempPath) = 0;

  protected:
    IAtomicFileOperations() = default;
};

}  // namespace SampleOrderSystem
