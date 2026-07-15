#include <gtest/gtest.h>

#include <atomic>
#include <filesystem>

#include "ApproveOrderUseCase.h"
#include "OrderRepository.h"
#include "ProductionQueueRepository.h"
#include "SampleRepository.h"

namespace SampleOrderSystem {
namespace {

std::atomic<int> g_counter{0};

class ProductionQueueRepositoryIntegrationTest : public ::testing::Test {
  protected:
    void SetUp() override {
        auto dir = std::filesystem::temp_directory_path() / "sos_production_queue_tests";
        std::filesystem::create_directories(dir);
        path_ = (dir / ("repo_" + std::to_string(++g_counter) + ".json")).string();
        std::filesystem::remove(path_);
    }

    void TearDown() override { std::filesystem::remove(path_); }

    std::string path_;
};

TEST_F(ProductionQueueRepositoryIntegrationTest, CreateThenReadAllReturnsJob) {
    ProductionQueueRepository repo(path_);
    ProductionJob created = repo.create(Guid::NewGuid(), "S-001", 189);

    std::vector<ProductionJob> all = repo.readAll();
    ASSERT_EQ(1u, all.size());
    EXPECT_EQ(created.jobId, all[0].jobId);
    EXPECT_EQ("S-001", all[0].sampleCode);
    EXPECT_EQ(189, all[0].quantity);
    EXPECT_EQ(ProductionJobStatus::Queued, all[0].status);
}

TEST_F(ProductionQueueRepositoryIntegrationTest, RejectsZeroOrNegativeQuantity) {
    ProductionQueueRepository repo(path_);
    EXPECT_THROW(repo.create(Guid::NewGuid(), "S-001", 0), std::invalid_argument);
    EXPECT_THROW(repo.create(Guid::NewGuid(), "S-001", -1), std::invalid_argument);
}

TEST_F(ProductionQueueRepositoryIntegrationTest, DataSurvivesRepositoryRestart) {
    Guid orderId = Guid::NewGuid();
    {
        ProductionQueueRepository firstInstance(path_);
        firstInstance.create(orderId, "S-001", 189);
    }

    ProductionQueueRepository secondInstance(path_);
    std::vector<ProductionJob> all = secondInstance.readAll();
    ASSERT_EQ(1u, all.size());
    EXPECT_EQ(orderId, all[0].orderId);
}

TEST_F(ProductionQueueRepositoryIntegrationTest, SharesDocumentWithSampleAndOrderRepositories) {
    // PRD.md 5.5.5: Sample/Order/ProductionJob 세 애그리게잇이 하나의 공유 JSON 문서를 쓴다. 한
    // Repository의 쓰기가 다른 두 Repository가 이미 저장한 데이터를 잃어버리면 안 된다.
    SampleRepository sampleRepo(path_);
    OrderRepository orderRepo(path_);
    ProductionQueueRepository productionRepo(path_);

    sampleRepo.create("S-001", "Silicon Wafer", 1.0, 90.0, 30);
    Order order = orderRepo.create("S-001", "삼성전자 파운드리", 200);
    productionRepo.create(order.orderId, "S-001", 189);

    std::vector<SampleRecord> samples = sampleRepo.readAll();
    std::vector<Order> orders = orderRepo.readAll();
    std::vector<ProductionJob> jobs = productionRepo.readAll();

    ASSERT_EQ(1u, samples.size());
    ASSERT_EQ(1u, orders.size());
    ASSERT_EQ(1u, jobs.size());
    EXPECT_EQ("S-001", samples[0].sampleCode);
    EXPECT_EQ("ORD-00001", orders[0].orderNumber);
    EXPECT_EQ(189, jobs[0].quantity);
}

TEST_F(ProductionQueueRepositoryIntegrationTest, ApproveOrderUseCasePersistsProductionJobToFile) {
    // PLAN.md Phase 3 완료 조건: 생성된 ProductionJob이 실제 JSON 파일에 저장되고 다시 읽힘.
    SampleRepository sampleRepo(path_);
    OrderRepository orderRepo(path_);
    ProductionQueueRepository productionRepo(path_);

    sampleRepo.create("S-001", "Silicon Wafer", 1.0, 90.0, 30);
    orderRepo.create("S-001", "삼성전자 파운드리", 200);

    ApproveOrderUseCase useCase(orderRepo, sampleRepo, productionRepo);
    ApproveOrderResult result = useCase.Approve("ORD-00001");

    EXPECT_EQ(ApproveOrderOutcome::ProducingInsufficientStock, result.outcome);

    ProductionQueueRepository reloaded(path_);
    std::vector<ProductionJob> jobs = reloaded.readAll();
    ASSERT_EQ(1u, jobs.size());
    EXPECT_EQ(189, jobs[0].quantity);
    EXPECT_EQ(OrderStatus::Producing, orderRepo.findByOrderNumber("ORD-00001")->status);
}

}  // namespace
}  // namespace SampleOrderSystem
