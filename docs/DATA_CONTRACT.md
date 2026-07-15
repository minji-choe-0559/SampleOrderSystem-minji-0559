# DATA_CONTRACT — SampleOrderSystem 공유 JSON 문서 계약

이 문서는 Sample/Order/ProductionJob이 공유하는 단일 JSON 문서의 저장 포맷을 정의한다(PRD.md
5.5.5에서 확정한 "공유 문서" 결정의 실제 스키마). Repository **인터페이스**는
`ISampleRepository`/`IOrderRepository`/`IProductionQueueRepository`로 애그리게잇별로 분리되지만,
물리적으로는 이 문서 하나(`data/sample.json`)를 세 Repository가 함께 읽고 쓴다.

## 1. 루트 구조

```json
{
  "schemaVersion": 1,
  "records": [ /* Sample */ ],
  "orders": [ /* Order */ ],
  "productionJobs": [ /* ProductionJob */ ]
}
```

- `schemaVersion`: 정수, 현재 지원 값은 `1`. 코드상 `kSharedDocumentSchemaVersion`
  (`src/JsonAdapter/Repositories/SharedDocument.h`)로 관리된다.
- 세 배열은 서로 독립적으로 갱신된다 — 한 Repository가 저장할 때는 자신이 담당하는 배열만
  교체하고 다른 배열은 그대로 보존한다(`WithArrayField`, `SharedDocument.h` 참고). 배열이 아직
  없으면(예: ProductionJob 미도입 상태) 빈 배열로 취급한다(`GetArrayField`).
- 파일이 없으면 새 문서(`schemaVersion`만 있고 세 배열은 없음/빈 배열)로 간주한다. 루트가
  object가 아니거나 `schemaVersion`이 지원 범위를 벗어나면 읽지 않고 오류를 반환한다
  (DataPersistence `docs/DATA_CONTRACT.md`와 동일한 정책).
- 저장은 항상 **문서 전체를 다시 직렬화해 원자적으로 교체**한다(부분 갱신 없음, DataPersistence의
  atomic write 패턴 그대로 재사용, `JsonAdapter/AtomicWrite/`).

## 2. `records[]` — Sample (구현 완료, DataPersistence 계약 그대로)

| JSON 키 | 타입 | 규칙 |
|---|---|---|
| `id` | string(UUID) | 시스템 발급 Guid, 불변 |
| `sampleCode` | string | 사용자 입력, 필수, trim, 대소문자 무시 중복 검사 |
| `name` | string | 필수, trim |
| `avgProcessTime` | number | 분 단위, 0 이상 |
| `yield` | number | 0~100, 모델 멤버명은 `YieldRate` |
| `stock` | number(정수) | 0 이상 |

세부 규칙은 DataPersistence의 `docs/DATA_CONTRACT.md`를 그대로 따른다(변경 없음).

## 3. `orders[]` — Order (구현 완료, Phase 2)

| JSON 키 | 타입 | 규칙 |
|---|---|---|
| `orderId` | string(UUID) | 시스템 발급 Guid, 불변(내부 식별자) |
| `orderNumber` | string | 사용자 대면 식별자. 시스템이 순번 기반으로 자동 채번(예: `ORD-0001`), 사용자 입력 아님(PRD.md 8장 확정) |
| `sampleCode` | string | 대상 Sample 참조. `records[].sampleCode`와 값으로 연결(외래키 성격, DB 제약은 아님) |
| `customerName` | string | 필수, trim, 중복 검사 없음(동일 고객의 복수 주문 허용) |
| `quantity` | number(정수) | 0 초과 |
| `status` | string | `RESERVED` / `REJECTED` / `PRODUCING` / `CONFIRMED` / `RELEASE` (PRD.md 4.1) |

실제 직렬화는 `src/JsonAdapter/Repositories/OrderJsonMapper.cpp`가 수행한다.

## 4. `productionJobs[]` — ProductionJob (Phase 3에서 최소 구조 구현 완료, Phase 4에서 확장 예정)

PRD.md 5.5.1 필드를 따르되, Order와 동일하게 Sample 참조는 `sampleCode`로 통일한다(원래 PRD
초안의 `sampleId` 표기에서 변경 — PRD.md 5.5.1도 함께 갱신했다). 아래는 실제 구현
(`src/JsonAdapter/Repositories/ProductionJobJsonMapper.cpp`)을 그대로 반영한 것이다.

| JSON 키 | 타입 | 규칙 | 구현 상태 |
|---|---|---|---|
| `jobId` | string(UUID) | 시스템 발급 Guid, 불변 | Phase 3 |
| `orderId` | string(UUID) | 이 Job을 유발한 `orders[].orderId` 참조 | Phase 3 |
| `sampleCode` | string | 대상 Sample 참조(`records[].sampleCode`) | Phase 3 |
| `quantity` | number(정수) | `ceil(부족분 / 수율)`, Job 생성 시점에 1회 확정, 이후 불변 | Phase 3 |
| `requestedAt` | number | 요청 시각(`IClock` 기준), UTC epoch **밀리초** 정수로 직렬화(`std::chrono::system_clock::time_point` ↔ `duration_cast<milliseconds>`) | Phase 3 |
| `status` | string | `QUEUED` / `IN_PROGRESS` / `COMPLETED` (PRD.md 4.2) | Phase 3 |
| `scheduledStartAt` | number(예정) | 예정 시작 시각, `requestedAt`과 동일하게 epoch 밀리초로 직렬화 예정 | **Phase 4에서 추가** |
| `scheduledCompletionAt` | number(예정) | 예정 완료 시각, 동일 포맷 예정 | **Phase 4에서 추가** |

Phase 4에서 두 필드를 추가할 때는 이미 저장된 Phase 3 시점 `productionJobs[]` 레코드(두 필드가
없는 상태)를 읽을 수 있어야 하는지도 함께 결정한다(마이그레이션 필요 여부 — 아직 미정, Phase 4
착수 시 확인).

## 5. 동시 접근

DataPersistence와 동일하게 파일 쓰기 자체의 원자성은 보장되지만, 여러 Repository가 "읽기 → 자신의
배열만 수정 → 쓰기"를 수행하는 시퀀스 전체를 감싸는 프로세스 간 잠금은 없다(PRD.md 8장 TOCTOU
참고 사항과 동일한 제한). 이 프로젝트는 단일 프로세스(콘솔 앱 1개) 사용을 전제로 하므로 별도
설계 없이 진행한다.

## 6. 변경 이력

| 버전 | 날짜 | 내용 |
|---|---|---|
| v1 | 2026-07-15 | 초안 — `records[]`(Sample, 기존 DataPersistence 계약) + `orders[]`(Order, Phase 2 구현 완료) + `productionJobs[]`(ProductionJob, Phase 3에서 최소 구조 구현 완료, `scheduledStartAt`/`scheduledCompletionAt`는 Phase 4에서 추가 예정) 스키마 확정. Sample 참조 필드명을 PRD 초안의 `sampleId`에서 실제 구현과 일치하는 `sampleCode`로 정정 |
