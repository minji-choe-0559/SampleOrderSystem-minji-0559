# CLAUDE.md — SampleOrderSystem 프로젝트 규칙

이 저장소는 반도체 시료 생산주문관리 시스템(SampleOrderSystem)의 최종 구현체다. 도메인 요구사항은
[PRD.md](PRD.md), Phase별 구현 계획은 [PLAN.md](PLAN.md), 커밋/PR 규칙은
[COMMIT_CONVENTION.md](COMMIT_CONVENTION.md)/[.github/PULL_REQUEST_TEMPLATE.md](.github/PULL_REQUEST_TEMPLATE.md)
참고. 이 문서는 그중 Claude가 이 저장소에서 작업할 때 지켜야 할 규칙만 정의한다.

## 문서 우선순위

충돌이 발견되면 아래 우선순위에 따라 판단하되, **Claude는 임의로 판단하지 않고 충돌 내용을
사용자에게 보고한 뒤 지시를 받는다.**

1. 과제 원본 명세(`docs/[CRA_AI] Day3_개인과제_반도체시료관리.pdf`)
2. [PRD.md](PRD.md)
3. [PLAN.md](PLAN.md)
4. CLAUDE.md (본 문서)
5. 코드와 테스트

## 재사용 원칙 (다른 PoC 산출물)

- **`DataPersistence-minji-0559`**(완성): JSON 엔진(`JsonValue`/`JsonParser`/`JsonWriter`/
  `JsonStore`)과 `atomic_write/`(원자적 파일 교체), `SampleRepository`의 "전체 로드 → 메모리 수정 →
  전체 저장" 패턴을 가져와 재사용한다. 코드를 가져오기 전 반드시 먼저 분석하고, Characterization
  Test 없이 대규모로 고치지 않는다. 기존 코드의 Public API를 변경해야 하면 사용자 승인을 먼저 받는다.
  - **확인된 사실(Phase 0 결정에 반영)**: 이 저장소는 완성된 지금도 `ConfigurationType=Application`
    이며 정적 라이브러리로 분리되지 않았다. 따라서 프로젝트 참조로 링크하는 방식은 현실적이지 않고,
    JSON 엔진 소스 파일을 이 저장소로 **복사해 독립적으로 유지**하는 방식만 남아 있다(Phase 0에서
    최종 승인 필요, PLAN.md 참고).
- **`ConsoleMVC-minji-0559`**(완성): Passive-View MVC 경계(View는 입출력만, Controller가 흐름 소유,
  Repository가 중복 판정 최종 책임)를 그대로 적용한다. 다만 아래는 두 PoC 모두의 결과 기록에서
  실제로 발견된 문제이므로 처음부터 반영한다.
  - 콘솔 입력은 `std::getline` 기반으로 구현한다(`std::cin >>` 토큰 읽기는 공백 값 검증 경로를
    재현하지 못했음).
  - 모든 콘솔 입력 지점은 EOF/실패 상태에서 무한 루프에 빠지지 않아야 한다.
  - **콘솔 한글 출력 깨짐 방지**: 소스를 `/utf-8`로 컴파일해도 Windows 콘솔은 기본 코드페이지
    (한국어 Windows에서 CP949)로 출력을 해석해 한글이 깨진다. `ConsoleMVC-minji-0559`와
    `DataPersistence-minji-0559`에서 **독립적으로 두 번** 겪고 고친 문제이므로, Composition Root
    (`main()`) 시작 시 반드시 `SetConsoleOutputCP(CP_UTF8)`/`SetConsoleCP(CP_UTF8)`를 설정한다.
    `<Windows.h>`를 포함할 때는 `std::numeric_limits<...>::max()`와의 매크로 충돌을 막기 위해
    `NOMINMAX`를 먼저 정의한다.
- **`DataMonitor-minji-0559`**(완성): 별도 실행 파일로 남기며 SampleOrderSystem이 코드를 직접
  가져오지는 않는다(모니터링 UseCase는 Domain Repository에 직접 질의하는 인프로세스 방식으로
  구현). 다만 그 PoC의 Summary 항목(전체 수/총재고/평균수율/평균생산시간/재고0개수)은 PRD.md 8장의
  "재고 여유/부족/고갈 임계치"를 다루지 않으므로, 이 결정은 여전히 보류 상태다.
- Order/ProductionJob의 저장소 구조는 **확정됨**(PRD.md 5.5.5): Sample과 같은 공유 JSON 문서에
  저장하고, `OrderRepository`/`ProductionQueueRepository` 인터페이스는 애그리게잇별로 분리 유지.
  실제 스키마는 [docs/DATA_CONTRACT.md](docs/DATA_CONTRACT.md) 참고.

## 범위 제한

- PRD.md에 정의된 기능만 구현한다. 원본 PPT의 예시 UI는 참고용이며 화면 구성은 자유롭게 결정한다.
- PRD.md 8장 "결정 보류 사항"에 해당하는 내용을 임의로 확정하지 않는다.
- 아직 필요하지 않은 미래 기능을 위한 과도한 추상화(인터페이스 남발, 팩토리 패턴 등)를 넣지 않는다.
- 인증, 네트워크, GUI, 다중 프로세스 동시 접근 제어를 구현하지 않는다(PRD.md 9장).

## 아키텍처 규칙

```
Console Adapter (Passive-View MVC: View/Controller)
        ↓ Presentation DTO / ViewModel
Application Layer (UseCase, SettlingUseCaseDecorator, Port 인터페이스)
        ↓ Domain 행위 호출
Domain Layer (Sample/Order/ProductionJob, ProductionSchedule.Reconcile)
        ↑ implements
JSON Adapter (JsonStore 계승, 애그리게잇별 Repository)
```

- Domain은 콘솔, JSON 라이브러리, 파일 시스템, `IClock`의 구체 구현에 의존하지 않는다.
- Repository는 애그리게잇별로 분리한다(`SampleRepository`/`OrderRepository`/
  `ProductionQueueRepository`). 여러 애그리게잇을 하나의 포트로 묶지 않는다(ISP 위반 금지).
- `ProductionSchedule.Reconcile(jobs, now)`(PRD.md 5.5.3)는 순수 함수로 두고, 저장소 접근은
  Application의 `SettleProductionUseCase`에서만 수행한다.
- 시간 계산/정산 로직을 각 Controller나 개별 UseCase에 중복 삽입하지 않는다 —
  `SettlingUseCaseDecorator`로만 감싼다(PRD.md 5.5.4의 시점 목록 대상).
- Controller는 Service/UseCase만 호출하며 Repository를 직접 호출하지 않는다.
- 전역 가변 상태를 사용하지 않는다.

## SDD Phase 진행 규칙

각 Phase는 다음 순서를 반드시 지키며, **각 화살표 지점에서 사용자 승인 없이 다음 단계로 넘어가지
않는다.**

```
요구사항 확인 → 사용자 승인 → TDD RED → 최소 구현 GREEN → REFACTOR
→ 독립 Review(code-reviewer / test-auditor 병렬) → Harness 검증
→ 승인된 리팩터링만 refactoring-specialist 순차 실행 → 사용자 승인 → Commit/PR
```

- TDD RED/GREEN/REFACTOR 단계는 `test-driven-development` Skill(`.claude/skills/`)을 반드시
  호출한 뒤 그 절차(Red-Green-Refactor, Iron Law: 실패하는 테스트 없이 프로덕션 코드 작성 금지)를
  따른다.
- `code-reviewer`(요구사항/SOLID/Clean Code 검토)와 `test-auditor`(테스트 누락/품질 검토)는
  `.claude/agents/`에 정의하며, **병렬**로 실행한다(같은 응답 안에서 두 Agent 호출을 동시에 보내
  서로 다른 context window에서 독립적으로 검토하게 한다).
- `refactoring-specialist`는 두 리뷰 결과가 사용자 승인을 받은 뒤에만 **순차**로 실행하며, 승인된
  항목만 동작 변경 없이 개선한다.
- Phase 하나가 끝나기 전에 다음 Phase의 구현에 착수하지 않는다.

### Phase별 Rigor 차등 적용

Phase마다 위험도가 다르므로, 아래처럼 두 그룹으로 나눠 절차의 엄격함을 차등 적용한다(사용자
승인 완료, Phase 1 완료 후 결정).

- **저위험 Phase (1, 2, 3, 5, 6)**: 이미 검증된 PoC 패턴을 그대로 가져다 쓰거나(1/2/5: ConsoleMVC/
  DataPersistence 패턴 재사용) 상대적으로 단순한 로직(3/5: 재고 판정 분기, 집계 조회, 출고 상태
  전환)이라 리스크가 낮다. Phase 5는 PLAN.md에서 기존 "모니터링"과 "출고 처리"를 합친 Phase다.
  Phase 6(통합 리팩터링과 최종 Review, 기존 Phase 7)도 이 그룹에 포함한다 — **단, PLAN.md에 이미
  명시된 Phase 6의 완료 조건(단일 verify 명령 전체 통과, code-reviewer/test-auditor 최종 통합
  리뷰에서 Critical/High findings 0건, PRD.md 전체 흐름을 가로지르는 최종 E2E 통합 테스트 통과)은
  Rigor 등급과 무관하게 그대로 유지된다.** 아래 완화는 승인 왕복 횟수·PR 장황함·빌드 배치 실행
  같은 절차상 오버헤드만 줄이는 것이며, Phase 6은 전체 코드베이스를 리뷰하는 특성상 findings가
  실제로 나올 가능성이 높아 "findings 없으면 refactoring-specialist 스킵" 규칙이 잘 적용되지
  않을 수 있음을 참고한다. 이 그룹에서는:
  - Phase 착수 시 파일 구조/설계 계획을 한 번에 승인받은 뒤, RED → GREEN → REFACTOR 중간에는
    사용자 승인 없이 이어서 진행하고 Phase 완료 시점에 한 번에 보고한다("승인 게이트에서 출력해야
    할 정보"는 Phase 완료 보고 하나에 모아 담는다).
  - 재빌드/테스트 실행은 파일 하나 고칠 때마다가 아니라, 논리적 변경 묶음 단위로 배치 실행한다.
  - PR 템플릿에서 해당 없는 섹션은 장황하게 채우지 않고 "N/A"로 짧게 표기한다.
  - 커밋 메시지 60자 줄 길이 규칙(COMMIT_CONVENTION.md)은 이 완화 대상에서 **제외**한다 — Phase
    위험도와 무관하게 항상 엄격히 지킨다.
  - 단, **독립 Review(`code-reviewer`/`test-auditor` 병렬)는 절대 생략하지 않는다** — 위 완화는
    승인 절차/커밋 스타일에만 적용되고 품질 게이트 자체를 줄이는 게 아니다.
  - **Review는 Phase 안의 UseCase마다 따로 돌리지 않고, Phase 전체 diff를 한 번에 묶어서
    `code-reviewer`/`test-auditor`를 각각 딱 1번씩만 호출한다.**
  - **`code-reviewer`/`test-auditor` 둘 다 findings가 없으면 `refactoring-specialist`는 호출하지
    않고 바로 Harness→커밋 승인으로 넘어간다.**
- **고위험 Phase (4)**: 위 완화를 적용하지 않고 이 문서 본문의 SDD Phase 진행 규칙을 문자 그대로
  지킨다(각 화살표 지점마다 사용자 승인). PRD.md 5.5의 FIFO+catch-up+멱등성 규칙이 이 프로젝트에서
  가장 복잡하고 실수 가능성이 크기 때문.

### 저위험 Phase 커밋 압축 (선택 적용, 트레이드오프 있음)

저위험 Phase(1, 2, 3, 5)는 RED(`test`) 커밋과 GREEN(`feat`)/REFACTOR(`refactor`) 커밋을 따로
나누지 않고 **Phase당 커밋 1개**로 합칠 수 있다. 이 경우 커밋 메시지 본문에 RED→GREEN→REFACTOR
전체 사이클을 요약해서 담아 `Practices Applied`의 TDD 항목 밀도를 유지한다(예: "RED: 실패 테스트
N건 선작성 → GREEN: 최소 구현으로 통과 → REFACTOR: 중복 X 제거, 테스트 결과 동일"). 커밋 개수는
줄지만 각 커밋 메시지의 설명 밀도로 SDD/TDD 실천 근거를 보존한다. 고위험 Phase(4)는 이 압축을
적용하지 않고 RED/GREEN/REFACTOR를 계속 별도 커밋으로 남긴다. Phase 6은 새 기능의 RED/GREEN
사이클이 아니라 전체 코드베이스 리팩터링/최종 점검이 본질이라 이 압축 대상에 자연스럽게 해당하지
않는다 — findings별로 별도 `refactor` 커밋을 남기는 편이 검토하기 더 낫다.

### 테스트 작성 간소화 원칙 (검증 범위는 유지, 작성량만 절감)

- **Parameterized/Table-driven 테스트 활용**: 입력값만 다르고 로직이 같은 시나리오(예: `FakeClock`
  시각만 다른 여러 케이스)는 GoogleTest `TEST_P`로 하나의 테스트 함수 + 파라미터 목록으로 묶는다.
  검증하는 케이스 수는 줄이지 않고 작성/리뷰 단위만 줄인다.
- **저위험 Phase(1/2/6)는 ConsoleMVC-minji-0559/DataPersistence-minji-0559의 기존 인수조건
  (AC-1~AC-7 등) 구조를 그대로 재사용**한다 — 무엇을 테스트할지부터 새로 설계하지 않는다.
- 순수 데이터 타입(로직 없는 DTO/ViewModel)에는 별도 단위 테스트를 만들지 않는다.
- 같은 비즈니스 규칙을 단위 테스트와 통합 테스트에서 이중으로 검증하지 않는다 — Repository 유효성/
  중복 로직은 Fake 기반 단위 테스트로, 통합 테스트는 "실제 JSON 파일 배선이 맞는지"(저장→재조회
  왕복) 확인 정도로 최소화한다.
- **Phase 4(생산 정산)는 예외 — 시나리오 개수(15개)를 줄이지 않는다.** Parameterized로 묶는 것은
  허용하되, 멱등성·catch-up·FIFO 오류 처리 등 커버하는 케이스 자체를 빼지 않는다. 실제 JSON 파일
  기반 통합 테스트(공유 문서 원자적 쓰기 확인)도 그대로 유지한다.

## 승인 게이트에서 출력해야 할 정보 (필수)

사용자가 매번 직접 diff를 확인하기 어렵다는 전제 하에, 각 승인 요청 시점마다 아래를 반드시 정리해
보고한다(생략 금지). **아래 5개 블록은 고위험 Phase(4)에서는 각 화살표 지점마다 순서대로 별도
승인받는 그대로 적용한다. 저위험 Phase(1/2/3/5/6)에서는 5개를 각각 승인받지 않고, Phase 완료
시점의 단일 보고 하나에 5개 블록 내용을 전부 모아 담아 한 번에 승인받는다**(Phase별 Rigor 차등
적용 참고).

**TDD RED 승인 요청 시**
- 이번에 작성한 실패 테스트 목록과 각 테스트가 검증하는 요구사항(PRD.md 절 번호)
- 왜 지금 실패하는지(컴파일 실패 vs 어서션 실패)

**GREEN 완료 후 REFACTOR 착수 승인 요청 시**
- 변경된 파일 목록과 각 파일의 변경 요지(1줄 요약)
- 새로 통과한 테스트 수, 전체 테스트 통과 여부

**독립 Review 결과 승인 요청 시**
- `code-reviewer`/`test-auditor` 각각의 findings를 심각도 순으로 요약(파일:라인 포함)
- 두 Subagent의 결과가 겹치는지, 서로 다른 지점을 지적했는지

**refactoring-specialist 실행 후**
- 승인된 항목 중 실제로 적용된 것 / 스킵된 것과 사유
- 동작 변경이 없음을 증명하는 근거(리팩터링 전후 테스트 결과 동일)

**Commit/PR 승인 요청 시(고위험 Phase 4) / 완료 보고 시(저위험 Phase, 커밋·푸시는 이미 수행됨)**
- Harness(Build/Test/Warning/Format) 최종 실행 결과
- 이번 커밋에 포함된 변경 파일 목록과 Breaking Change 여부
- 실제 커밋 메시지 전문(COMMIT_CONVENTION.md 형식)

## Clean Code 체크리스트 (매 Review 단계에서 확인)

- SRP: 클래스/함수가 한 가지 이유로만 변경되는가
- OCP/DIP: Domain이 구체 Adapter가 아니라 Port(인터페이스)에 의존하는가
- ISP: Repository 인터페이스가 애그리게잇별로 분리되어 있는가
- 의미 없는 이름(`Manager`/`Helper`/`Util`) 금지, 중복 로직 제거
- 테스트만을 위한 불필요한 public API 금지
- 소유권/RAII: owning raw pointer 금지, 인터페이스에 virtual destructor 선언, `main`이 구체 객체
  생명주기 소유, Controller/Service는 주입받은 의존성을 non-owning reference로 보관, 불필요한
  `shared_ptr`/수동 `delete` 금지

## Harness

단일 명령(`verify.ps1` 또는 동등 MSBuild `Verify` 타겟)으로 아래를 검증한다. 정확한 명령은
Phase 0에서 확정하고 README.md에 기록한다(그 전까지 `TBD`로 표기).

- Configure
- Build (`/W4`, `/WX`, 외부 벤더 헤더 제외)
- Unit Test
- 실제 JSON 파일 CRUD/정산 통합 테스트 — Sample/Order/ProductionJob이 **공유하는 단일 JSON 문서**
  기준(PRD.md 5.5.5)으로, 세 Repository가 동시에 관여해도 원자적 쓰기가 깨지지 않는지 포함
- Restart Recovery Test (ProductionJob Catch-up 정산, PRD.md 5.5.3)
- Format Check (clang-format, 검사 전용, 자동 수정 금지) — 기본은 `verify.ps1`이 git으로 변경
  감지된 파일만 검사한다(Phase가 늘어날수록 이미 검증된 파일을 매번 재검사하는 낭비 방지).
  Phase 6(통합 리팩터링과 최종 Review) 완료 조건과 신규 클론 직후 최초 확인은 반드시
  `verify.ps1 -FullFormatCheck`로 src/ 전체를 검사해서 통과를 확인한다.

## Git 작업 규칙

### 커밋 수행 가능 범위

- **고위험 Phase(4)**: 사용자의 명시적 승인 없이 Commit, Push, Merge, PR 생성을 하지 않는다.
  각 화살표 지점마다 실시간 승인을 받는다(DataPersistence류의 "Step 구간 사전 승인" 방식을
  적용하지 않는다).
- **저위험 Phase(1, 2, 3, 5, 6)**: **Phase 착수 시 요구사항/설계 계획을 승인받은 시점에 그
  Phase의 Commit·Push·Merge·PR 생성까지 함께 사전 승인된 것으로 처리한다**(왕복 대기 시간이
  이 프로젝트의 실질적 병목이라는 사용자 판단에 따른 결정). 즉 RED→GREEN→REFACTOR→Review→
  Harness까지 마친 뒤 중간에 승인을 기다리지 않고 바로 커밋·푸시(및 PR 워크플로를 쓰는 경우
  merge까지)를 진행하고, 완료 후 결과를 보고한다. 단, 아래 중 하나라도 해당하면 **사전 승인
  범위를 벗어난 것으로 보고 진행을 멈추고 실시간 승인을 받는다**:
  - Harness(Build/Test/Warning/Format)가 하나라도 실패한 상태
  - PRD.md 8장 "결정 보류 사항"에 해당하는 내용을 새로 확정해야 하는 경우(Breaking Change 처리
    흐름 참고)
  - code-reviewer/test-auditor가 Critical/High 수준 findings를 낸 경우
- 커밋 전 [COMMIT_CONVENTION.md](COMMIT_CONVENTION.md)의 형식/`Practices Applied` 섹션 작성 여부를
  확인한다.
- 커밋 전 Build, Test, Warning(`/W4`/`/WX`), Format 검증 결과를 확인한다.
- staging 전에 서로 다른 목적의 변경이 섞여 있는지 확인하고, 섞여 있으면 목적별로 분리해서
  staging한다.
- 실제 데이터 파일은 테스트 fixture가 아닌 한 커밋하지 않는다. 테스트가 생성한 임시 파일을
  커밋하지 않는다.

### 금지된 Git 작업

다음 작업은 사용자가 그 자리에서 명시적으로 별도 요청하지 않는 한 수행하지 않는다.

- `git push`
- 브랜치 생성
- `git commit --amend`
- `rebase`
- `reset`
- force push
- tag 생성
- 원격 저장소 설정 변경(remote add/remove/set-url 등)
- 기존 커밋 히스토리를 다시 쓰는 모든 작업
- 다른 저장소(`DataPersistence-`, `ConsoleMVC-`, `DataMonitor-`, `DummyDataGenerator-`)의 변경을
  이 저장소 커밋에 포함하는 작업

## Breaking Change 처리 흐름

PRD.md 8장의 "결정 보류 사항"에 해당하는 내용을 확정해야 하는 시점이 오면, **작업을 진행하기 전에**
아래를 포함해 보고하고 사용자 승인을 기다린다.

- 확정하려는 결정 사항
- 대안과 선택 이유
- 기존에 이미 구현된 코드/문서에 미치는 영향
- 예상 커밋 메시지

## 완료 보고 형식

각 Phase가 완료되고 커밋까지 생성되면 다음 형식으로 보고한다.

```text
완료한 Phase:
주요 변경 사항:
TDD 사이클 요약(RED 테스트 수 → GREEN 통과 → REFACTOR 내용):
Review 결과(code-reviewer / test-auditor 요약):
Refactoring 적용 내역:
Harness 결과:
생성한 커밋:
Breaking Change 여부:
남아 있는 작업:
```
