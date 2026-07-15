# COMMIT_CONVENTION — SampleOrderSystem

이 문서는 이 저장소의 커밋 메시지 형식과 분리 기준을 정의한다. 커밋을 실행할 수 있는 조건/승인
절차는 [CLAUDE.md](CLAUDE.md) "Git 작업 규칙"을 따른다.

## 형식

```
<type>(<scope>): <subject>

<body>

Practices Applied:
- SDD: <해당 Phase 번호(PLAN.md)와 어떤 요구사항 확인/승인 단계를 거쳤는지. 없으면 "N/A">
- TDD: <RED에서 먼저 작성한 테스트와 GREEN에서 무엇으로 통과시켰는지. 없으면 "N/A">
- Clean Code: <이번 변경이 지킨 원칙(SRP/DIP/이름 등). 없으면 "N/A">
- Refactoring: <동작 변경 없이 개선한 내용과 근거(리팩터링 전후 테스트 결과 동일). 없으면 "N/A">

Breaking Change: <No 또는 Yes(상세)>
```

`Practices Applied` 섹션은 **모든 커밋에 필수**다. 해당 사항이 없으면 반드시 "N/A"라고 명시하고
줄 자체를 생략하지 않는다.

## type

| type | 의미 |
|---|---|
| `feat` | 새로운 기능 |
| `fix` | 버그 수정 |
| `test` | 테스트 추가/보강(RED 단계 커밋을 별도로 분리할 때) |
| `refactor` | 동작 변경 없는 개선(refactoring-specialist 승인 항목 적용) |
| `docs` | 문서 변경 |
| `build` | 빌드/Harness/의존성 설정 |
| `chore` | 그 외 잡무성 변경 |

## scope

Phase가 다루는 도메인 단위를 사용한다. 예: `sample`, `order`, `production`, `monitoring`,
`release`, `harness`.

## 규칙

- **줄 길이**: 제목과 본문의 모든 줄은 60자 이내로 작성한다(한글 기준). 60자를 넘으면 의미 단위로
  줄바꿈한다. Phase 위험도와 무관하게 모든 커밋에 동일하게 적용한다(CLAUDE.md "Phase별 Rigor
  차등 적용"의 완화 대상이 아니다).
- **Atomic Commit**: 하나의 커밋은 하나의 목적만 담는다. RED(`test`)와 GREEN(`feat`/`fix`)을
  하나의 커밋으로 합칠지 분리할지는 Phase 착수 시 결정하되, 서로 다른 계층(예: Domain과 Console
  Adapter)의 변경을 하나의 커밋에 섞지 않는다.
- 커밋 전 반드시 [CLAUDE.md](CLAUDE.md)의 Harness(Build/Test/Warning/Format)를 통과한 상태여야
  한다.
- `Practices Applied`의 각 항목은 실제로 한 일만 적는다 — 하지 않은 것을 했다고 적지 않는다.
- Breaking Change는 별도 줄로 명시하고, PRD.md 8장 "결정 보류 사항"을 확정하는 커밋인 경우 어떤
  결정이 확정되었는지 본문에 포함한다.
- 테스트가 생성한 임시 파일, 실제 데이터 파일(fixture 제외)은 커밋하지 않는다.

## 예시

### 기능 구현 커밋

```
feat(order): 주문 승인 시 재고 부족분 ProductionJob 자동 등록

승인 시 재고가 부족하면 FIFO 큐에 ProductionJob을 등록하고 Order를 PRODUCING으로
전환한다.

Practices Applied:
- SDD: Phase 3 "주문 승인/거절" 대상, 사용자 승인 후 착수(PLAN.md)
- TDD: "재고 부족 시 PRODUCING 전환 및 큐 등록" RED 테스트 3건을 먼저 작성해 실패를 확인한 뒤 GREEN
- Clean Code: 재고 판정 분기를 Domain의 Order::Approve()에 응집, UseCase/Controller에는 분기 없음
- Refactoring: N/A

Breaking Change: No
```

### 리팩터링 커밋

```
refactor(production): ProductionSchedule.Reconcile 내부 중복 시각 비교 로직 추출

Practices Applied:
- SDD: N/A
- TDD: N/A(동작 변경 없음, 기존 테스트로 회귀 확인)
- Clean Code: 3곳에 중복되던 "now와 scheduledXAt 비교" 로직을 헬퍼 함수로 추출(SRP)
- Refactoring: code-reviewer가 지적한 중복 제거 항목만 적용, 리팩터링 전/후 전체 테스트 결과 동일
  (42건 통과 → 42건 통과)

Breaking Change: No
```

### 문서 커밋

```
docs(prd): Order/ProductionJob 저장 구조 결정 사항 반영

Practices Applied:
- SDD: PRD.md 8장 결정 보류 사항 중 하나를 사용자 승인받아 확정
- TDD: N/A
- Clean Code: N/A
- Refactoring: N/A

Breaking Change: No
```
