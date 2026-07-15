# SampleOrderSystem-minji-0559

반도체 시료 생산주문관리 시스템(S-Semi) — [DataPersistence-minji-0559](https://github.com)/
[ConsoleMVC-minji-0559]/[DataMonitor-minji-0559]/[DummyDataGenerator-minji-0559] 4개 PoC에서
검증한 패턴을 조합해 만드는 최종 구현체.

> 이 저장소는 SDD(Spec-Driven Development) 방식으로 Phase를 나누어 진행 중이다. 진행 순서와 각
> Phase의 완료 조건은 [PLAN.md](PLAN.md), AI가 지켜야 할 작업 규칙은 [CLAUDE.md](CLAUDE.md) 참고.

## 1. 현재 상태

**Phase 1(시료 관리) 완료 — Domain/Application/JsonAdapter/ConsoleAdapter 4계층이 실제로
분리되어 동작한다.**

| 항목 | 상태 |
|---|---|
| PRD.md / CLAUDE.md / PLAN.md / COMMIT_CONVENTION.md | 작성 완료 |
| `.github/PULL_REQUEST_TEMPLATE.md` | 작성 완료 |
| `.claude/skills/test-driven-development/` | 준비 완료 |
| `.claude/agents/`(code-reviewer / test-auditor / refactoring-specialist) | 작성 완료 |
| 원본 명세 PDF | `docs/` 안에 포함 |
| `docs/UI_DESIGN_PROPOSAL.md` | 화면 구성 시안 작성(구속력 없음, PRD.md 5장 "화면 구성은 자유") |
| `src/Domain/{Common,Sample}` | `Guid`/`SampleRecord` — JSON 라이브러리 의존 제거, 순수 Domain 타입 |
| `src/JsonAdapter/*` | `SampleJsonMapper`로 직렬화 분리, `SampleRepository`가 `ISampleRepository` Port 구현 |
| `src/Application/{Ports,UseCases,Dto}` | `ISampleRepository`, `RegisterSampleUseCase`/`ListSamplesUseCase`/`SearchSampleUseCase` |
| `src/ConsoleAdapter/{Views,Controllers}` | `SampleController` 3-UseCase 구조로 재작성, `std::getline` 기반 입력(+검색 메뉴)으로 교체 |
| 시료 관리(PRD.md 5.2: 등록/조회/검색) | 완료 — 콘솔에서 실제 동작, GoogleTest 26건 통과 |
| 도메인 코드(Order/ProductionJob) | 미착수 (Phase 2부터) |
| Harness(단일 verify 명령) | `verify.ps1` — Build(/W4 /WX, /utf-8)/Unit Test(26건 통과)/Format Check(clang-format) |

## 2. 문서 구조

| 문서 | 역할 |
|---|---|
| [PRD.md](PRD.md) | 요구사항 정의. 원본 PPT 분석 + 확정된 도메인 상태 머신 + 실제 시간 기반 생산 정산 설계 + 아직 결정하지 않은 사항(8장) |
| [CLAUDE.md](CLAUDE.md) | AI 작업 규칙. 재사용 원칙, 아키텍처 규칙, SDD Phase 진행 규칙, 승인 게이트 출력 형식, Clean Code 체크리스트, Git 규칙 |
| [PLAN.md](PLAN.md) | Phase 0~7 구현 계획 |
| [COMMIT_CONVENTION.md](COMMIT_CONVENTION.md) | 커밋 메시지 형식(`Practices Applied`: SDD/TDD/Clean Code/Refactoring 명시 필수) |
| [.github/PULL_REQUEST_TEMPLATE.md](.github/PULL_REQUEST_TEMPLATE.md) | PR 본문 템플릿(승인 게이트 기록, TDD 사이클, 독립 Review 결과, Harness 결과) |
| [docs/UI_DESIGN_PROPOSAL.md](docs/UI_DESIGN_PROPOSAL.md) | 화면 구성 시안(원본 PPT 예시 화면 기반, 확정 전 논의용) |
| `docs/[CRA_AI] Day3_개인과제_반도체시료관리.pdf` | 과제 원본 명세 |

## 3. 다른 4개 PoC와의 관계

| PoC | 상태 | SampleOrderSystem에서의 역할 |
|---|---|---|
| `DataPersistence-minji-0559` | 완성 | JSON 엔진(`JsonValue`/`JsonParser`/`JsonWriter`/`JsonStore`)과 `atomic_write/` 소스를 복사해 재사용(정적 라이브러리로 분리되어 있지 않아 프로젝트 참조가 아닌 소스 복사 방식으로 확정, CLAUDE.md 참고) |
| `ConsoleMVC-minji-0559` | 완성 | Passive-View MVC 경계를 그대로 적용. 그 PoC에서 발견된 `std::getline` 필요성/EOF 처리/콘솔 코드페이지(UTF-8) 이슈를 처음부터 반영 |
| `DataMonitor-minji-0559` | 완성 | 별도 실행 파일로 유지, 코드는 가져오지 않음(모니터링 UseCase는 인프로세스로 별도 구현) |
| `DummyDataGenerator-minji-0559` | 코드 구현 진행 중 | 별도 실행 파일. 이 저장소가 직접 재사용하지는 않으나, 공유 JSON 계약(`docs/DATA_CONTRACT.md`)의 실사용 사례로 참고 가능 |

## 4. Subagent / Skill 구성

| 이름 | 위치 | 역할 |
|---|---|---|
| `test-driven-development` | `.claude/skills/test-driven-development/SKILL.md` | Phase별 TDD RED-GREEN-REFACTOR 사이클 강제 |
| `code-reviewer` | `.claude/agents/code-reviewer.md` | 요구사항/SOLID/Clean Code 검토, `test-auditor`와 병렬 실행 |
| `test-auditor` | `.claude/agents/test-auditor.md` | 테스트 누락/품질 검토, `code-reviewer`와 병렬 실행 |
| `refactoring-specialist` | `.claude/agents/refactoring-specialist.md` | 두 Review 승인 후 순차 실행, 승인된 항목만 동작 변경 없이 개선 |

## 5. 빌드/테스트 방법

```powershell
powershell -ExecutionPolicy Bypass -File .\verify.ps1
```

`verify.ps1`(Build → Unit Test → Compiler Warning → Format Check)이 단일 진입점이다.
Build는 `Directory.Build.props`가 전체 프로젝트에 적용하는 `/W4 /WX`(외부 벤더 헤더 제외)와
`/utf-8`로 검증된다. Unit Test는 별도 Test 프로젝트 없이, 같은 실행 파일을 인자와 함께 호출하면
`main()`이 GoogleTest 러너로 분기하는 방식이다(`DataPersistence-minji-0559`와 동일한 패턴).

## 6. 실행 방법

```powershell
.\x64\Debug\SampleOrderSystem-minji-0559.exe          # 콘솔 앱(시료 관리 메뉴)
.\x64\Debug\SampleOrderSystem-minji-0559.exe --gtest_color=no   # 인자가 있으면 테스트 모드
```

## 7. 진행 방식 요약

이 저장소는 [CLAUDE.md](CLAUDE.md)에 정의된 아래 순서를 Phase마다 반복한다.

```
요구사항 확인 → 사용자 승인 → TDD RED → 최소 구현 GREEN → REFACTOR
→ 독립 Review(code-reviewer / test-auditor 병렬) → Harness 검증
→ 승인된 리팩터링만 refactoring-specialist 순차 실행 → 사용자 승인 → Commit/PR
```

Phase 1 완료 후, Phase별 위험도에 따라 이 절차의 엄격함을 차등 적용하기로 했다(CLAUDE.md
"Phase별 Rigor 차등 적용" 참고). 저위험 Phase(1/2/3/5/6: PoC 재사용 또는 단순 로직)는 Phase
착수/완료 시점에만 승인받고 중간은 이어서 진행하며, 고위험 Phase(4/7: 생산 정산, 최종 통합)는
위 절차를 화살표 하나하나 그대로 지킨다. 독립 Review는 두 그룹 모두에서 생략하지 않는다.
