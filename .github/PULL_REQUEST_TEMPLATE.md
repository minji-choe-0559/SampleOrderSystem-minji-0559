<!--
이 PR 템플릿은 SDD(요구사항 확인 → 승인 → TDD RED → GREEN → REFACTOR → 독립 Review → Harness →
승인 → Commit/PR)의 증거를 PR 본문 하나로 확인할 수 있게 하기 위한 것이다. 채점자가 커밋 로그를
일일이 따라가지 않아도 이 PR만 보고 "무엇을, 어떻게, 누구 승인 하에" 만들었는지 알 수 있어야 한다.
비어 있는 칸을 지우지 말고 실제로 없으면 "N/A"라고 적을 것.
-->

## 1. Phase / 요구사항 연결

- PLAN.md Phase: <!-- 예: Phase 3. 주문 승인/거절 -->
- PRD.md 대상 절: <!-- 예: 5.4 주문 승인/거절 -->
- 요약: <!-- 이 PR이 구현하는 것을 1~3줄로 -->

## 2. 사용자 승인 기록 (SDD 게이트)

각 게이트를 실제로 통과했는지 표시한다. 체크만 하지 말고, 그 승인이 이루어진 시점의 근거(대화
맥락 요약 1줄)를 함께 남긴다.

| 게이트 | 승인 여부 | 근거 |
|---|---|---|
| 요구사항 확인 및 착수 승인 | [ ] | |
| TDD RED 테스트 승인 | [ ] | |
| REFACTOR 착수 승인 | [ ] | |
| 독립 Review(code-reviewer/test-auditor) 결과 승인 | [ ] | |
| refactoring-specialist 적용 결과 승인 | [ ] | |
| 최종 Commit/PR 생성 승인 | [ ] | |

## 3. TDD 사이클

| 단계 | 내용 |
|---|---|
| RED | 먼저 작성한 실패 테스트 목록과 각 테스트가 검증하는 요구사항 |
| GREEN | 테스트를 통과시킨 최소 구현 요지 |
| REFACTOR | 동작 변경 없이 개선한 내용 (없으면 "N/A") |

<!-- 테스트 이름을 나열: 예)
- OrderApproveTest.ApprovesImmediatelyWhenStockSufficient (PRD 5.4)
- OrderApproveTest.CreatesProductionJobWhenStockInsufficient (PRD 5.4, 5.5.2)
-->

## 4. 관련 커밋

이 PR에 포함된 커밋과 각 커밋이 SDD/TDD 사이클에서 어떤 역할인지 매핑한다(COMMIT_CONVENTION.md
`Practices Applied` 참고).

| 커밋 | 역할(RED/GREEN/REFACTOR/기타) | 요지 |
|---|---|---|
| | | |

## 5. 독립 Review 결과

`code-reviewer`와 `test-auditor`를 병렬로 실행한 결과를 심각도순으로 요약한다. 발견된 게 없으면
"findings 없음"이라고 명시한다.

**code-reviewer**

| 심각도 | 파일:라인 | 내용 | 처리 |
|---|---|---|---|
| | | | 반영 / 반려(사유: ) |

**test-auditor**

| 심각도 | 파일:라인 | 내용 | 처리 |
|---|---|---|---|
| | | | 반영 / 반려(사유: ) |

## 6. refactoring-specialist 적용 내역

- 승인된 항목 중 실제로 적용: <!-- 목록 -->
- 승인됐으나 스킵한 항목과 사유: <!-- 목록, 없으면 "N/A" -->
- 리팩터링 전후 테스트 결과가 동일함을 확인: [ ] (예: 42건 통과 → 42건 통과)

## 7. Clean Code 체크리스트

- [ ] SOLID 위반 없음 (SRP/OCP/DIP/ISP)
- [ ] 의미 없는 이름(`Manager`/`Helper`/`Util`)이나 중복 로직 없음
- [ ] 소유권/RAII 규칙 준수 (owning raw pointer 없음, 수동 delete 없음)
- [ ] Domain이 콘솔/JSON 라이브러리/파일 시스템/`IClock` 구체 구현에 의존하지 않음
- [ ] Repository가 애그리게잇별로 분리되어 있음(단일 포트로 뭉치지 않음)

## 8. Harness 결과

<!-- 실제 실행 결과를 붙여넣는다. 예)
Build: 성공
Unit Test: 42 / 42 통과
Compiler Warning: 0 (/W4 /WX)
Format Check: 통과
실제 JSON 파일 통합 테스트: 통과
-->

## 9. Breaking Change

- [ ] 없음
- [ ] 있음 (상세: 변경 계약/기존 동작/변경 후 동작/하위 호환성 영향)

## 10. 남은 작업

<!-- 다음 Phase로 넘어가기 전 남은 것, 또는 "없음" -->
