# PLAN — SampleOrderSystem Phase별 구현 계획

각 Phase는 [CLAUDE.md](CLAUDE.md)의 SDD 흐름(요구사항 확인 → 사용자 승인 → TDD RED → 최소 구현
GREEN → REFACTOR → 독립 Review[code-reviewer/test-auditor 병렬] → Harness 검증 → 승인된
리팩터링만 refactoring-specialist 순차 실행 → 사용자 승인 → Commit/PR)을 그대로 따른다. 이 문서는
Phase별 **예상** 범위와 완료 조건만 다루며, 실제 세부 설계(테스트 이름, 파일 구조 등)는 각 Phase
착수 시점에 사용자 승인을 받아 확정한다.

> 문서 역할 분담: PRD.md는 **무엇을 만들지**, 본 PLAN.md는 **어떤 순서로 만들지**, CLAUDE.md는
> **어떻게 작업할지**를 다룬다.

---

## Phase 0. 문서/스캘레톤과 Harness 확정

### 목표
PRD/CLAUDE/PLAN 문서를 기준으로 프로젝트 구조와 단일 verify 명령을 확정한다.

### 구현 범위
- C++ 표준/테스트 프레임워크 확정(ConsoleMVC와 동일하게 C++20, GoogleTest/GoogleMock 유지 여부 확인)
- 프로젝트 구조 확정: Console Adapter / Application / Domain / JSON Adapter 계층을 어떤 단위
  (프로젝트 분리 vs 폴더 분리)로 나눌지 결정
- `DataPersistence-minji-0559`의 JSON 엔진/`atomic_write/`를 이 저장소로 가져오는 방식 확정.
  **확인된 사실**: `DataPersistence-minji-0559`는 완성된 지금도 `ConfigurationType=Application`이며
  정적 라이브러리로 분리되어 있지 않다 — 따라서 프로젝트 참조(라이브러리 링크) 방식은 선택지에서
  사실상 제외되고, **소스 파일을 복사해 이 저장소 안에서 독립적으로 유지**하는 방식으로 진행한다
  (그래도 최종 승인은 사용자에게 받는다).
- Composition Root(`main()`)에 콘솔 코드페이지 설정을 처음부터 포함한다: `SetConsoleOutputCP`/
  `SetConsoleCP(CP_UTF8)`(+ `<Windows.h>` 포함 시 `NOMINMAX` 선행 정의). `ConsoleMVC-minji-0559`와
  `DataPersistence-minji-0559`에서 독립적으로 두 번 겪은 한글 출력 깨짐 버그이므로 뒤로 미루지 않는다.
- Compiler Warning(`/W4`, `/WX`, 외부 헤더 제외) / Format Check(clang-format) 정책 적용
- 단일 verify 명령(Build/Unit Test/Warning/Format) 확정

### 제외 범위
- 실제 도메인 코드 작성

### 완료 조건
- README.md의 실행 명령 관련 `TBD` 제거
- 단일 verify 명령이 테스트 0건 상태에서도 정상 통과

### 예상 Commit
- `build(harness): 프로젝트 구조와 Harness 검증 절차 확정`

---

## Phase 1. 시료 관리 (PRD.md 5.2)

### 대상 요구사항
시료 등록 / 시료 조회 / 시료 검색

### 구현 범위
- Domain: `Sample`(ID, 이름, 평균 생산시간, 수율, 재고)
- Application: `RegisterSampleUseCase`, `ListSamplesUseCase`, `SearchSampleUseCase`
- JSON Adapter: `SampleRepository`(DataPersistence 산출물 재사용/확장)
- Console Adapter: 시료 관리 메뉴(View/Controller)

### 완료 조건
- PRD.md 5.2의 3개 기능이 GoogleTest/GoogleMock으로 검증되고 콘솔에서 실제 동작
- `SampleRepository` 실제 JSON 파일 기반 통합 테스트(Fake가 아닌 실제 파일 CRUD) 포함

---

## Phase 2. 시료 주문 접수 (PRD.md 5.3)

### 구현 범위
- Domain: `Order`(RESERVED 상태로 생성)
- Application: `ReserveOrderUseCase`
- JSON Adapter: `OrderRepository` 신규 (PRD.md 5.5.5에서 확정한 공유 JSON 문서 구조 적용,
  `SampleRepository`와 같은 물리 파일을 공유)

### 완료 조건
- 시료 ID/고객명/주문 수량 입력 → RESERVED 주문 생성이 검증됨
- `OrderRepository` 실제 JSON 파일 기반 통합 테스트(공유 문서에 Sample과 함께 저장되어도 서로
  간섭하지 않는지 포함) 포함

---

## Phase 3. 주문 승인/거절 (PRD.md 5.4)

### 구현 범위
- Application: `ApproveOrderUseCase`(재고 판정 분기, 재고 부족 시 `ProductionJob` 생성),
  `RejectOrderUseCase`
- Domain: 재고 판정 로직(충분/부족), Order 상태 전이 규칙, `ProductionJob` **최소 데이터 구조**
  (jobId/orderId/sampleId/quantity/requestedAt/status — PRD.md 5.5.1의 `quantity = ceil(부족분 /
  수율)` 산식만 여기서 확정한다. `scheduledStartAt`/`scheduledCompletionAt`를 실제로 계산하는 FIFO
  스케줄링 로직과 `ProductionSchedule.Reconcile`은 Phase 4로 유보 — 이 Phase에서 값을 어떻게 채워
  둘지는 Phase 3 착수 시 별도 확정한다)
- JSON Adapter: `ProductionQueueRepository` 최소 CRUD(Sample/Order와 같은 공유 JSON 문서 사용,
  PRD.md 5.5.5) — FIFO 체이닝/정산 로직은 포함하지 않는다

### 완료 조건
- 재고 충분 시 CONFIRMED 전환, 재고 부족 시 PRODUCING 전환 + `ProductionJob` 생성(quantity 산식
  포함)이 검증됨
- 거절 시 즉시 REJECTED 전환이 검증됨
- 생성된 `ProductionJob`이 실제 JSON 파일에 저장되고 다시 읽힘이 통합 테스트로 검증됨(FIFO 스케줄
  값 자체의 정확성 검증은 Phase 4 몫)

---

## Phase 4. 생산 라인과 정산 (PRD.md 5.5) — 가장 리스크가 큰 Phase

### 구현 범위
- Domain: `ProductionSchedule.Reconcile(jobs, now)`(순수 함수, FIFO 스케줄 계산+catch-up 규칙).
  `ProductionJob`의 데이터 구조 자체는 Phase 3에서 이미 정의됨 — 이 Phase는
  `scheduledStartAt`/`scheduledCompletionAt`를 실제로 채우는 FIFO 스케줄링 계산과 catch-up
  정산 로직만 추가한다
- Application: `SettleProductionUseCase`, `SettlingUseCaseDecorator`, `IClock`/`SystemClock`/
  `FakeClock`(PRD.md 5.5.6)
- JSON Adapter: `ProductionQueueRepository`에 FIFO 스케줄 계산/정산 반영 확장(Phase 3의 최소 CRUD
  위에 추가, PRD.md 5.5.5)
- Console Adapter: 생산 라인 조회 메뉴

### 테스트 전략
`FakeClock` 기반 단위 테스트를 우선 작성한다. PRD.md 5.5.3의 A/B/C 예시(재실행 시 밀린 완료 건
일괄 반영)를 인수 조건으로 삼는다. 최소 아래 시나리오를 모두 커버한다(`.claude/agents/
test-auditor.md` 체크리스트와 동일).

1. 빈 큐 첫 Job 즉시 시작
2. 진행 중 요청이 있을 때 새 요청 FIFO 뒤에 추가(체이닝)
3. 완료 시각 이전 재고 불변
4. 완료 시각 도달 시 전체 수량 일괄 반영
5. 한 번의 정산에서 여러 요청 연속 완료
6. 미완료 첫 요청 IN_PROGRESS, 뒤 요청 QUEUED
7. 재시작 후 완료 요청 catch-up
8. 재시작 후 큐에 밀린 여러 요청 완료 처리
9. 정산 반복 호출해도 재고 중복 증가 없음(멱등성)
10. 저장 실패 시 상태-재고 불일치 없음(공유 문서 원자적 쓰기로 구조적 보장, PRD.md 5.5.5)
11. **저장된 FIFO 순서가 잘못된 경우 오류로 정산 중단**(PRD.md 5.5.7, 자동 보정 금지)
12. 현재 시각이 예정 시작 시각 이전인 요청은 QUEUED 유지
13. **생산 수량 0 이하인 요청 생성 거부**(PRD.md 5.5.7)
14. **생산 소요 시간이 유효하지 않은 요청 생성 거부**(PRD.md 5.5.7)
15. 재시작 시 기존 재고와 생산 큐가 모두 복원됨

### 완료 조건
- FakeClock으로 QUEUED→IN_PROGRESS→COMPLETED 전이, FIFO 체이닝, catch-up, 멱등성이 모두 검증됨
- 실제 재시작 시나리오(정산 이전 상태 보존 → 재기동 시 재계산)가 통합 테스트로 검증됨
- `ProductionQueueRepository` 실제 JSON 파일 기반 통합 테스트(공유 문서 원자적 쓰기 확인 포함)
  포함
- 위 15개 시나리오가 전부 `FakeClock` 기반 테스트로 커버됨

---

## Phase 5. 모니터링과 출고 처리 (PRD.md 5.6, 5.7)

> 속도 개선을 위해 기존 "Phase 5(모니터링)"과 "Phase 6(출고 처리)"를 하나의 Phase로 합쳤다. 둘 다
> 저위험군이고 Order 조회/단순 상태 전환 수준이라 별도 사이클로 나눌 실익이 적다고 판단했다
> (Review/Harness/커밋을 한 번만 거친다).

### 구현 범위
- Application: `MonitoringQuery`(상태별 주문 수, 시료별 재고 현황), `ReleaseOrderUseCase`
  (CONFIRMED 대상만 처리, RELEASE로 전환)
- `SettlingUseCaseDecorator`로 `MonitoringQuery`를 감싸 조회 전 정산이 선행되도록 연결

### 완료 조건
- REJECTED 제외 상태별 집계, 재고 상태(여유/부족/고갈) 판정이 검증됨(임계치는 Phase 착수 시
  사용자 승인 필요 — PRD.md 8장)
- CONFIRMED가 아닌 주문에 대한 출고 시도가 거부됨이 검증됨
- **재고 충분 경로 E2E 통합 테스트**: `RESERVED → CONFIRMED → RELEASE` 전체 흐름이 실제 JSON 파일
  기준으로 한 번에 검증됨(이 경로는 생산 정산을 거치지 않으므로 Phase 4 완료를 기다리지 않고 이
  Phase에서 바로 가능)

---

## Phase 6. 통합 리팩터링과 최종 Review

### 구현 범위
- 전체 계층 의존 방향 재검토(Domain의 인프라 비의존 여부)
- 전체 Harness 재실행
- README.md/PRD.md/PLAN.md 최종 상태 갱신

### 완료 조건
- 단일 verify 명령 전체 통과
- code-reviewer/test-auditor의 최종 통합 리뷰에서 Critical/High findings 없음
- **PRD.md 전체 흐름을 가로지르는 최종 E2E 통합 테스트 통과**: 재고 부족 경로 포함
  (`RESERVED → PRODUCING → [정산] → CONFIRMED → RELEASE`)와 모니터링이 정산 후 상태를 정확히
  반영하는지까지 실제 JSON 파일 기준으로 검증
