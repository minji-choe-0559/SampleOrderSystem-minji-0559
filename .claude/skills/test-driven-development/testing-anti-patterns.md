# Testing Anti-Patterns

이 문서는 `SKILL.md`(TDD)에서 Mock/테스트 유틸리티를 다룰 때 참고하는 부록이다. 이 저장소는
GoogleTest/GoogleMock을 사용하므로 예시는 C++ 기준이다.

## 1. Mock의 동작을 검증하는 테스트 (진짜 동작이 아니라)

Mock이 호출됐는지만 확인하고, 실제 결과값이나 상태 변화는 검증하지 않는 테스트는 아무것도
증명하지 못한다.

<Bad>
```cpp
TEST(ApproveOrderTest, CallsRepositorySave) {
    MockOrderRepository repo;
    EXPECT_CALL(repo, Save(_)).Times(1);

    ApproveOrderUseCase useCase(repo);
    useCase.Approve(orderId);
    // Save가 호출됐다는 것만 확인 — 저장된 Order가 실제로 CONFIRMED로
    // 바뀌었는지, 재고가 반영됐는지는 전혀 검증하지 않는다.
}
```
</Bad>

<Good>
```cpp
TEST(ApproveOrderTest, TransitionsToConfirmedWhenStockSufficient) {
    InMemoryOrderRepository repo;
    repo.Save(Order::Reserve(orderId, sampleId, /*quantity=*/10));
    FakeSampleRepository sampleRepo({{sampleId, /*stock=*/50}});

    ApproveOrderUseCase useCase(repo, sampleRepo);
    useCase.Approve(orderId);

    EXPECT_EQ(repo.Find(orderId)->Status(), OrderStatus::Confirmed);
}
```
Repository는 Fake(인메모리 실제 구현)를 쓰고, 실제 상태 전이 결과를 검증한다.
</Good>

**이 프로젝트에서 특히 주의할 지점**: `code-reviewer`/`test-auditor`가 이미 지적하듯,
`ProductionSchedule.Reconcile`이나 `SettleProductionUseCase` 테스트에서 재고/Order 상태를
Mock 호출 여부로만 확인하면 PRD.md 5.5.3의 catch-up 규칙(밀린 Job 전부 반영, 멱등성)이 실제로
지켜지는지 증명할 수 없다. 반드시 `FakeClock` + 실제 재고/상태 값을 확인한다.

## 2. 테스트만을 위한 public API 추가

테스트하기 쉽게 만든다고 프로덕션 클래스에 테스트 전용 getter/setter나 내부 상태 노출 메서드를
추가하지 않는다. CLAUDE.md의 Clean Code 체크리스트("테스트만을 위한 불필요한 public API 금지")와
동일한 원칙이다.

<Bad>
```cpp
class ProductionQueue {
public:
    // 테스트에서만 큐 내부를 들여다보려고 추가한 메서드
    std::vector<ProductionJob>& GetInternalJobsForTesting() { return jobs_; }
};
```
</Bad>

<Good>
테스트는 생성자 주입 가능한 Fake(`FakeProductionQueueRepository` 등)를 통해 상태를 준비하고,
공개 동작(`Reconcile`, `Enqueue` 등)의 결과만으로 검증한다. 내부 컨테이너를 직접 노출하지 않는다.

## 3. 의존성을 이해하지 못한 채 Mock으로 덮기

무엇을 왜 Mock하는지 모르는 채로 실패하는 컴파일/링크 에러를 피하려고 아무 인터페이스나
Mock으로 감싸면, 정작 검증하려는 로직(Domain)까지 Mock 뒤에 숨어 테스트가 아무것도 증명하지
못하게 된다.

- Mock은 **경계(Repository, `IClock`, 콘솔 입출력)에만** 사용한다.
- Domain(`Sample`/`Order`/`ProductionJob`/`ProductionSchedule`)과 Application의 UseCase 로직은
  가능한 한 실제 객체 + Fake 저장소로 검증한다(`test-auditor.md` 3장 참고).
- 어떤 인터페이스를 Mock할지 결정하기 전에, 그 인터페이스가 정말 "외부 세계와의 경계"인지 먼저
  확인한다. Domain 내부 협력 객체를 Mock하고 있다면 설계를 다시 본다(과도한 결합의 신호일 수
  있음).
