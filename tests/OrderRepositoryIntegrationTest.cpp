#include <gtest/gtest.h>

#include <atomic>
#include <filesystem>

#include "OrderRepository.h"
#include "SampleRepository.h"

namespace SampleOrderSystem {
namespace {

std::atomic<int> g_counter{0};

class OrderRepositoryIntegrationTest : public ::testing::Test {
  protected:
    void SetUp() override {
        auto dir = std::filesystem::temp_directory_path() / "sos_order_repo_tests";
        std::filesystem::create_directories(dir);
        path_ = (dir / ("repo_" + std::to_string(++g_counter) + ".json")).string();
        std::filesystem::remove(path_);
    }

    void TearDown() override { std::filesystem::remove(path_); }

    std::string path_;
};

TEST_F(OrderRepositoryIntegrationTest, CreateThenReadAllReturnsOrder) {
    OrderRepository repo(path_);
    Order created = repo.create("S-001", "삼성전자 파운드리", 200);

    std::vector<Order> all = repo.readAll();
    ASSERT_EQ(1u, all.size());
    EXPECT_EQ(created.orderId, all[0].orderId);
    EXPECT_EQ("ORD-00001", all[0].orderNumber);
    EXPECT_EQ(OrderStatus::Reserved, all[0].status);
}

TEST_F(OrderRepositoryIntegrationTest, AssignsSequentialOrderNumbersAcrossCreates) {
    OrderRepository repo(path_);
    repo.create("S-001", "Customer A", 10);
    repo.create("S-002", "Customer B", 20);
    Order third = repo.create("S-003", "Customer C", 30);

    EXPECT_EQ("ORD-00003", third.orderNumber);
}

TEST_F(OrderRepositoryIntegrationTest, RejectsBlankSampleCode) {
    OrderRepository repo(path_);
    EXPECT_THROW(repo.create("   ", "Customer", 10), std::invalid_argument);
}

TEST_F(OrderRepositoryIntegrationTest, RejectsBlankCustomerName) {
    OrderRepository repo(path_);
    EXPECT_THROW(repo.create("S-001", "   ", 10), std::invalid_argument);
}

TEST_F(OrderRepositoryIntegrationTest, RejectsZeroOrNegativeQuantity) {
    OrderRepository repo(path_);
    EXPECT_THROW(repo.create("S-001", "Customer", 0), std::invalid_argument);
    EXPECT_THROW(repo.create("S-001", "Customer", -1), std::invalid_argument);
}

TEST_F(OrderRepositoryIntegrationTest, DataSurvivesRepositoryRestart) {
    {
        OrderRepository firstInstance(path_);
        firstInstance.create("S-001", "삼성전자 파운드리", 200);
    }

    OrderRepository secondInstance(path_);
    std::vector<Order> all = secondInstance.readAll();
    ASSERT_EQ(1u, all.size());
    EXPECT_EQ("ORD-00001", all[0].orderNumber);
}

TEST_F(OrderRepositoryIntegrationTest, SharesDocumentWithSampleRepositoryWithoutDataLoss) {
    // PRD.md 5.5.5: Sample/Order가 하나의 공유 JSON 문서를 쓴다. 한 Repository의 쓰기가 다른
    // Repository가 이미 저장한 데이터를 잃어버리면 안 된다(test-auditor 지적 반영).
    SampleRepository sampleRepo(path_);
    OrderRepository orderRepo(path_);

    sampleRepo.create("S-001", "Silicon Wafer", 1.0, 90.0, 100);
    orderRepo.create("S-001", "삼성전자 파운드리", 200);
    sampleRepo.create("S-002", "GaN Epitaxial", 2.0, 80.0, 50);
    orderRepo.create("S-002", "SK하이닉스", 150);

    std::vector<SampleRecord> samples = sampleRepo.readAll();
    std::vector<Order> orders = orderRepo.readAll();

    ASSERT_EQ(2u, samples.size());
    ASSERT_EQ(2u, orders.size());
    EXPECT_EQ("S-001", samples[0].sampleCode);
    EXPECT_EQ("S-002", samples[1].sampleCode);
    EXPECT_EQ("ORD-00001", orders[0].orderNumber);
    EXPECT_EQ("ORD-00002", orders[1].orderNumber);
}

}  // namespace
}  // namespace SampleOrderSystem
