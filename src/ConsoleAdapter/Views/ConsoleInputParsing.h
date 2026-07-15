#pragma once

#include <optional>
#include <string>

namespace SampleOrderSystem {

// std::cin >> 토큰 읽기는 공백을 포함한 값과 EOF를 올바르게 처리하지 못했다(ConsoleMVC PoC에서
// 실제로 발견된 문제, CLAUDE.md 재사용 원칙). std::getline으로 원시 줄을 읽은 뒤 이 함수들로
// 파싱해, 빈 줄/잘못된 형식의 실패 경로를 단위 테스트로 직접 재현할 수 있게 분리한다.
std::optional<int> ParseMenuChoice(const std::string& line);
std::optional<double> ParseDouble(const std::string& line);
std::optional<int> ParseInt(const std::string& line);

}  // namespace SampleOrderSystem
