# PRD — SampleOrderSystem 요구사항 정의

이 문서는 반도체 시료 생산주문관리 시스템(`SampleOrderSystem-minji-0559`)의 요구사항을 정의한다.
원본 명세는 `docs/[CRA_AI] Day3_개인과제_반도체시료관리.pdf`이며, 이 PRD는 원본 명세를 분석하고
그 안의 모호한 지점에 대해 사용자와 논의해 확정한 결과를 반영한다. 구현 순서/Phase 분할은
[PLAN.md](PLAN.md), AI 작업 규칙은 [CLAUDE.md](CLAUDE.md) 참고.

문서 우선순위 충돌 시: 원본 PDF > 본 PRD.md > PLAN.md > CLAUDE.md > 코드/테스트. 충돌을 발견하면
임의로 판단하지 않고 사용자에게 보고한다.

## 1. 배경

가상 반도체 회사 "S-Semi"는 다양한 반도체 시료(Sample)를 생산해 연구소·팹리스·대학 연구실에
납품한다. 기존에는 엑셀/메모장으로 주문·재고·공정 현황을 관리해 처리 여부 불명, 완성 시점 불명,
불필요한 추가 공정 같은 문제가 반복되었다. 이를 해결하기 위해 콘솔 기반 "반도체 시료
생산주문관리 시스템"을 구축한다.

## 2. 역할과 업무 흐름

| 역할 | 책임 |
|---|---|
| 고객 | 필요한 시료를 이메일로 요청 (시스템 밖 행위, 시스템 입력 대상 아님) |
| 주문 담당자 | 요청에 맞춰 주문서 작성(시료 예약), 생산 담당자에게 전달 |
| 생산 담당자 | 개발 시료 등록, 주문 수신 후 승인/거절, 생산·출고 처리 |

## 3. 용어 정의

| 용어 | 정의 |
|---|---|
| Sample(시료) | 시스템에 등록된 생산 대상 물품. 고유 ID, 이름, 평균 생산시간, 수율, 현재 재고를 가진다. |
| Order(주문) | 특정 Sample에 대한 고객의 주문 1건. 상태 머신을 가진다(4장). |
| 수율(YieldRate) | 정상 생산 시료 수 / 총 생산 시료 수. 예: 100개 생산 중 정상 90개 = 0.9 |
| ProductionJob(생산 요청) | 재고 부족으로 자동 등록된 생산 작업 1건. Order와 별도의 상태 머신을 가진다(4장). PPT 원본에는 없던 개념으로, 실제 시간 기반 생산 정산을 설계하며 이번 프로젝트에서 도입이 확정되었다. |
| 생산 라인 | 시료 하나를 생산하는 설비 흐름. 하나의 생산 라인은 시료를 하나씩, 주문이 들어온 시료만 생산한다. |
| FIFO 생산 큐 | ProductionJob이 대기하는 큐. 선입선출로 처리된다. |

## 4. 도메인 상태 머신 (확정)

### 4.1 Order 상태

```
RESERVED --(승인, 재고 충분)--> CONFIRMED --(출고 처리)--> RELEASE
RESERVED --(승인, 재고 부족)--> PRODUCING --(연결 ProductionJob 완료)--> CONFIRMED --(출고 처리)--> RELEASE
RESERVED --(거절)--> REJECTED
```

| 상태 | 의미 |
|---|---|
| `RESERVED` | 주문 접수 |
| `REJECTED` | 주문 거절. 정상 흐름 밖의 상태이며 모니터링에서 제외한다. |
| `PRODUCING` | 주문 승인 완료, 재고 부족으로 생산 중 |
| `CONFIRMED` | 주문 승인 완료, 출고 대기 중 |
| `RELEASE` | 출고 완료 |

> **결정 사항**: 원본 PPT는 상태 흐름 표(8p)에서 `RELEASE`, 시스템 개요 다이어그램(7p)에서
> `RELEASED`로 서로 다르게 표기했다. 이 프로젝트는 **`RELEASE`로 표기를 통일**한다.

### 4.2 ProductionJob 상태

```
QUEUED --(scheduledStartAt 도달)--> IN_PROGRESS --(scheduledCompletionAt 도달)--> COMPLETED
```

| 상태 | 의미 |
|---|---|
| `QUEUED` | 생성됨, 아직 예정 시작 시각에 도달하지 않음 |
| `IN_PROGRESS` | 현재 시각이 [예정 시작 시각, 예정 완료 시각) 구간, 재고 미반영 |
| `COMPLETED` | 예정 완료 시각 도달, 재고 반영 완료, 연결 Order를 CONFIRMED로 전환 완료 |

> **결정 사항**: Order의 `CONFIRMED`와 이름이 겹치는 것을 피하기 위해, ProductionJob의 완료 상태는
> `CONFIRMED`가 아니라 **`COMPLETED`**로 명명한다(사용자 승인 완료).

## 5. 기능 요구사항

### 5.1 메인 메뉴

기능별 선택 화면을 표시하고 전체 시료 요약 정보를 확인할 수 있다.

| 메뉴 | 의미 |
|---|---|
| 시료 관리 | 신규 시료 등록, 목록 조회, 이름 검색 |
| 주문(접수/승인/거절) | 고객 주문 접수 및 생산 라인 담당자의 승인·거절 처리 |
| 모니터링 | 상태별 주문 수 및 시료별 재고 현황 확인 |
| 출고 처리 | CONFIRMED 상태 주문에 대해 출고 실행 |
| 생산 라인 | 현재 생산 중인 시료 및 대기 중인 생산 큐 확인 |

화면 세부 레이아웃은 자유롭게 결정한다(원본 PPT의 예시 화면은 참고용).

### 5.2 시료 관리

- **시료 등록**: 시료 ID, 이름, 평균 생산시간, 수율을 입력받아 신규 등록한다.
- **시료 조회**: 등록된 모든 시료 목록과 현재 재고 수량을 표시한다.
- **시료 검색**: 이름(`name`) 또는 시료 ID(`sampleCode`)로 검색한다(확정). 두 필드 모두 부분
  일치(대소문자 무시)로 검색하며, 검색어가 이름/시료ID 어느 한쪽에라도 부분 일치하면 결과에
  포함한다.

### 5.3 시료 주문(예약)

고객이 시료를 요청하면 주문 담당자가 주문을 생성한다.

- 입력값: 시료 ID, 고객명(자유 텍스트, 확정 — 8장 참고), 주문 수량
- 생성 시점 주문 상태는 `RESERVED`
- **식별자(확정)**: Sample과 동일한 2단 구조를 따른다 — 내부 시스템 식별자(`orderId`)는 `Guid`로
  발급하고, 사용자 대면 식별자("주문번호")는 `SampleRecord.sampleCode`처럼 사용자가 입력하는 값이
  아니라 **시스템이 순번 기반으로 자동 채번**한다(정확한 표기 형식은 Phase 2 착수 시 확정).

### 5.4 주문 승인/거절

- **접수된 주문 목록**: `RESERVED` 상태 주문 목록을 표시한다.
- **주문 승인**: 재고 상황에 따라 자동 분기한다.
  - 재고 충분 → 시료 재고에서 주문 수량만큼 **즉시 차감**하고 주문을 `CONFIRMED`로 전환한다
    (확정 — 원본 PRD에 명시되어 있지 않았으나, 차감하지 않으면 같은 재고로 여러 주문을 중복
    승인할 수 있는 문제가 있어 확정함). 5.5.3의 ProductionJob 완료 시 재고 증가(`+=`)와 대칭되는
    규칙이다.
  - 재고 부족 → ProductionJob을 생성해 FIFO 생산 큐에 등록하고, 주문을 `PRODUCING`으로 전환
- **주문 거절**: 즉시 `REJECTED`로 전환한다.

### 5.5 생산 라인과 생산 정산 (실제 시간 기반, 확정 설계)

시간 기준은 실제 시스템 현재 시각(`IClock`)을 사용한다. 결과를 반영하는 정산(Settlement)은
백그라운드 스레드가 아니라, 아래 5.5.4의 지정된 시점마다 실행되는 **일괄 재계산(Catch-up)** 방식으로
처리한다.

#### 5.5.1 ProductionJob 데이터 구조

| 필드 | 설명 |
|---|---|
| `jobId` | 생산 요청 식별자 |
| `orderId` | 이 Job을 유발한 Order 참조 |
| `sampleId` | 대상 Sample |
| `quantity` | 실 생산량. `ceil(부족분 / 수율)`로 Job 생성 시점에 한 번 확정, 이후 변경하지 않는다. **완료 시 이 값을 그대로 재고에 반영하며, 수율을 완료 시점에 다시 적용하지 않는다**(정책 확정 — 수율은 이미 이 값을 정하는 데 반영되었으므로 이중 적용하지 않음). |
| `requestedAt` | 요청 시각(`IClock` 기준) |
| `scheduledStartAt` | 예정 시작 시각 |
| `scheduledCompletionAt` | 예정 완료 시각 |
| `status` | `QUEUED` / `IN_PROGRESS` / `COMPLETED` |

#### 5.5.2 FIFO 스케줄 계산 규칙 (Job 생성 시점에 1회 확정)

```
if 큐에 완료(COMPLETED)되지 않은 Job이 없으면:
    scheduledStartAt = IClock.Now()
else:
    scheduledStartAt = 큐의 마지막 Job.scheduledCompletionAt

scheduledCompletionAt = scheduledStartAt + (평균생산시간(sample) * quantity)
```

#### 5.5.3 정산(Catch-up) 규칙

큐를 앞에서부터 순회하며 반복한다.

```
while 큐 맨 앞 Job이 존재하고 Job.status != COMPLETED and Job.scheduledCompletionAt <= now:
    Job.status = COMPLETED
    Sample(Job.sampleId).재고 += Job.quantity   // 전체 수량 한 번에 반영, 부분 반영 금지
    Order(Job.orderId).status = CONFIRMED        // PRODUCING -> CONFIRMED
    큐에서 다음 Job으로 이동

if 맨 앞 Job 존재 and Job.status == QUEUED and Job.scheduledStartAt <= now < Job.scheduledCompletionAt:
    Job.status = IN_PROGRESS
```

- 재고 반영은 항상 `Job.quantity` 전체 단위로만 일어난다. 부분/중간 반영은 없다.
- 프로그램이 꺼져 있던 동안 여러 Job의 완료 시각이 지났다면, FIFO 순서대로 **완료 대상 전부**를
  반영한다(첫 번째만 처리하지 않는다).

#### 5.5.4 정산 실행 시점

정산 로직은 별도의 공통 서비스(`SettleProductionUseCase`)로 분리하고, 아래 시점마다 자동 실행한다.
각 메뉴/Controller에 시간 계산 로직을 중복 삽입하지 않는다(구현 방식은 CLAUDE.md의
`SettlingUseCaseDecorator` 참고).

- 프로그램 시작 직후 (필수)
- 생산 라인/생산 큐 조회 전
- 모니터링 정보 조회 전
- 생산 요청(주문 승인) 등록 전
- 생산 상태를 변경하는 작업 전
- 재고 조회 전

#### 5.5.5 멱등성과 저장 안전성

- 이미 `COMPLETED`인 Job은 재정산 시 건드리지 않는다.
- "상태 전이 + 재고 반영 + Order 전환"은 하나의 인메모리 연산으로 묶고, 이를 단일 원자적 파일 쓰기
  (DataPersistence의 전체 스냅샷 교체 패턴 재사용)로 영속화한다. 쓰기 실패 시 디스크의 상태는
  정산 이전 그대로 남으므로, 다음 실행 시 동일한 정산을 재수행해도 중복 반영이 발생하지 않는다.
- **저장 구조 결정(확정)**: Sample/Order/ProductionJob은 **하나의 공유 JSON 문서**에 함께 저장한다.
  `ISampleRepository`/`IOrderRepository`/`IProductionQueueRepository` 인터페이스는 애그리게잇별로
  분리하되(6장 SOLID/ISP), JsonAdapter 내부 구현은 세 Repository가 같은 물리적 문서를 공유해서
  읽고/쓴다. 이렇게 하면 "재고는 반영됐는데 상태는 아직" 같은 불일치가 파일 하나가 통째로 원자적
  교체되는 구조상 애초에 발생할 수 없고, 별도의 `stockApplied` 같은 플래그도 필요 없다(상태값
  `COMPLETED` 자체가 멱등성 판정 기준). 파일을 분리하는 대안은 두 파일 사이의 크래시로 상태-재고
  불일치가 생길 수 있어 기각했다.

#### 5.5.6 `IClock` / `FakeClock` 인터페이스

```cpp
class IClock {
 public:
  virtual ~IClock() = default;
  virtual std::chrono::system_clock::time_point Now() const = 0;
};

class SystemClock : public IClock {           // 실제 실행
  std::chrono::system_clock::time_point Now() const override { return std::chrono::system_clock::now(); }
};

class FakeClock : public IClock {             // 테스트 전용
 public:
  explicit FakeClock(std::chrono::system_clock::time_point t) : now_(t) {}
  void Advance(std::chrono::minutes d) { now_ += d; }
  void Set(std::chrono::system_clock::time_point t) { now_ = t; }
  std::chrono::system_clock::time_point Now() const override { return now_; }
 private:
  std::chrono::system_clock::time_point now_;
};
```

Domain/Application은 `SystemClock`/`FakeClock`의 구체 타입을 알지 못하고 `IClock&`로만 주입받는다.

#### 5.5.7 검증 규칙과 오류 처리 (확정)

- **생산 수량이 0 이하**인 ProductionJob 생성 요청은 거부한다(생성 자체를 막음, 예외/오류 반환).
- **생산 소요 시간이 유효하지 않은 경우**(음수 등, 평균 생산시간 또는 quantity 이상으로 계산된 총
  소요 시간이 0 이하)도 생성을 거부한다.
- **저장된 FIFO 순서가 잘못된 경우**(예: 뒤 Job의 `scheduledStartAt`이 앞 Job의
  `scheduledCompletionAt`보다 이전인 손상된 상태를 발견) — **자동으로 재정렬하거나 보정하지 않고,
  명확한 오류로 정산을 중단한다.** DataPersistence가 손상된 JSON을 자동으로 빈 데이터로 덮어쓰지
  않는 것과 같은 철학이다(사용자 확정).

### 5.6 모니터링

- **주문량 확인**: 상태별(`RESERVED`/`CONFIRMED`/`PRODUCING`/`RELEASE`) 주문 수를 확인한다.
  `REJECTED`는 유효한 주문이 아니므로 집계에서 제외한다.
- **재고량 확인**: 시료별 현재 재고 수량을 확인한다. 주문 대비 재고 수량에 따라 상태를 표기한다.
  - 여유: 주문 대비 재고 충분
  - 부족: 주문 대비 재고 수량 부족
  - 고갈: 수량이 0
  - 정확한 임계치는 8장 "결정 보류 사항" 참고(미정)

### 5.7 출고 처리

- `CONFIRMED` 상태 주문만 대상으로 한다.
- 특정 주문에 대해 출고를 실행하면 상태가 `RELEASE`로 전환된다.

## 6. 비기능 요구사항

- **아키텍처**: 외부는 MVC(Passive View), 내부는 경량 Clean Architecture.
  `Console Adapter → Application → Domain ← JSON Adapter`. Domain은 콘솔, JSON 라이브러리, 파일
  시스템, `IClock`의 구체 구현에 의존하지 않는다.
- **SOLID**: 애그리게잇별 Repository **인터페이스** 분리(`SampleRepository`/`OrderRepository`/
  `ProductionQueueRepository`), Domain은 Port(인터페이스)에만 의존. 단, 물리적 저장은 5.5.5에서
  확정한 대로 하나의 공유 JSON 문서를 세 Repository 구현체가 함께 사용한다(인터페이스 분리와
  물리적 저장 공유는 별개 층위의 결정).
- **기존 PoC 재사용**:
  - `DataPersistence-minji-0559`의 JSON 엔진(`JsonValue`/`JsonParser`/`JsonWriter`/`JsonStore`)과
    `atomic_write/`(원자적 파일 교체), `SampleRepository`의 전체 로드→전체 저장 패턴.
  - `ConsoleMVC-minji-0559`의 Passive-View MVC 경계(View는 입출력만, Controller가 흐름 소유,
    Repository가 중복 판정 최종 책임)와, 그 PoC에서 실제로 발견된 개선 사항(`std::getline` 기반
    입력, EOF 처리)을 반영한다.
- **테스트 가능성**: Domain/Application은 콘솔·파일 시스템 없이 단위 테스트 가능해야 한다.
  `IClock`은 `FakeClock`으로 대체 가능해야 한다.
- **데이터 영속성**: 데이터는 JSON 파일로 저장하며, 프로그램을 재시작해도 유지된다.

## 7. 데이터 계약 재사용 방침

`data/sample.json`의 `SampleRecord` 스키마(`schemaVersion`, `records[]`)는 DataPersistence의
`docs/DATA_CONTRACT.md` v1을 그대로 따른다. **Order/ProductionJob은 5.5.5에서 확정한 대로 같은
공유 JSON 문서에 추가 배열(`orders`, `productionJobs` 등)로 확장**한다. 확장된 계약은
`docs/DATA_CONTRACT.md`(신규 작성, Phase 4 착수 시)에 반영한다.

## 8. 결정 보류 사항 (임의로 정하지 않음)

- 재고 "여유/부족/고갈" 판정 임계치(정량 기준)
- 생산 라인 개수/구조(단일 라인 고정 여부, 시료별 전용 여부) — PPT는 "하나의 생산 라인은 시료를
  하나씩 생산"만 명시, 개수는 불명확
- 다중 프로세스 동시 접근 시 Order 승인 등에서 발생 가능한 TOCTOU 문제(참고 사항으로만 기록,
  필요해지면 별도 설계)

## 9. 범위 밖

- 인증, 네트워크 통신, GUI
- 다중 사용자 동시 접속
- 성능 최적화, 대용량 데이터 처리
- Database/ORM 도입

## 10. 참고 문서

- 원본 명세: `docs/[CRA_AI] Day3_개인과제_반도체시료관리.pdf` (본 저장소 내 포함)
- 별도 GitHub 저장소 `DataPersistence-minji-0559`의 `docs/DATA_CONTRACT.md`, `ATOMIC_WRITE_DESIGN.md`
  (이 저장소 밖의 독립 Repository이며, 위 경로는 그 저장소 안에서의 위치를 가리키는 설명일 뿐
  이 저장소 안의 링크가 아니다)
- 별도 GitHub 저장소 `ConsoleMVC-minji-0559`의 `POC_SPEC.md` 17절(최종 결과 기록 — 최종 프로젝트
  적용 사항, 위와 동일하게 독립 Repository 참조)
