#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace SampleOrderSystem {

// RegisterSampleUseCase(중복 sampleCode 메시지 분류)와 SearchSampleUseCase(부분일치 검색)가
// 공유하는 대소문자 무시 부분일치 판정(code-reviewer 지적: 중복 정의 제거).
inline bool ContainsIgnoreCase(const std::string& haystack, const std::string& needle) {
    auto it = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                          [](char a, char b) {
                              return std::tolower(static_cast<unsigned char>(a)) ==
                                     std::tolower(static_cast<unsigned char>(b));
                          });
    return it != haystack.end();
}

}  // namespace SampleOrderSystem
