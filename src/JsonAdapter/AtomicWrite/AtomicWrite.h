#pragma once

#include <string>

#include "IAtomicFileOperations.h"

namespace SampleOrderSystem {

// targetPath와 같은 디렉터리에 고유한 임시 파일을 만들어 content를 기록한 뒤, 성공한 경우에만
// 원자적으로 targetPath에 반영한다. 실패하면 targetPath는 호출 이전 상태 그대로 유지된다.
// operations는 이 한 번의 호출에서만 사용되고 이후 재사용해서는 안 된다(IAtomicFileOperations
// 수명 규칙 참고). 실패 시 std::runtime_error를 던지며, 어떤 단계에서 실패했는지를 메시지에
// 포함한다(최초 실패가 primary error — cleanup 실패는 메시지에 부가 정보로만 덧붙인다).
void writeJsonAtomically(const std::string& targetPath, const std::string& content,
                         IAtomicFileOperations& operations);

}  // namespace SampleOrderSystem
