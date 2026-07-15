#include "OrderApprovalRepository.h"

#include <algorithm>
#include <stdexcept>

#include "OrderDocument.h"
#include "ProductionJobDocument.h"
#include "SampleDocument.h"
#include "SharedDocument.h"

namespace SampleOrderSystem {

OrderApprovalRepository::OrderApprovalRepository(std::string path) : path_(std::move(path)) {}

bool OrderApprovalRepository::ConfirmWithStockDeduction(const std::string& orderNumber,
                                                        const std::string& sampleCode,
                                                        int quantity) {
    JsonValue document = ReadSharedDocument(path_);

    std::vector<SampleRecord> samples =
        SampleDocument::fromJson(GetArrayField(document, "records"));
    auto sampleIt = std::find_if(samples.begin(), samples.end(), [&](const SampleRecord& record) {
        return record.sampleCode == sampleCode;
    });
    if (sampleIt == samples.end()) {
        return false;
    }
    if (sampleIt->stock < quantity) {
        throw std::invalid_argument("OrderApprovalRepository: insufficient stock at write time");
    }

    std::vector<Order> orders = OrderDocument::fromJson(GetArrayField(document, "orders"));
    auto orderIt = std::find_if(orders.begin(), orders.end(), [&](const Order& order) {
        return order.orderNumber == orderNumber;
    });
    if (orderIt == orders.end()) {
        return false;
    }

    sampleIt->stock -= quantity;
    orderIt->status = OrderStatus::Confirmed;

    JsonValue updated = WithArrayField(document, "records", SampleDocument::toJson(samples));
    updated = WithArrayField(updated, "orders", OrderDocument::toJson(orders));
    WriteSharedDocument(path_, updated);
    return true;
}

bool OrderApprovalRepository::TransitionToProducing(const std::string& orderNumber,
                                                    const Guid& orderId,
                                                    const std::string& sampleCode,
                                                    int productionQuantity) {
    if (productionQuantity <= 0) {
        throw std::invalid_argument("OrderApprovalRepository: quantity must be > 0");
    }

    JsonValue document = ReadSharedDocument(path_);

    std::vector<Order> orders = OrderDocument::fromJson(GetArrayField(document, "orders"));
    auto orderIt = std::find_if(orders.begin(), orders.end(), [&](const Order& order) {
        return order.orderNumber == orderNumber;
    });
    if (orderIt == orders.end()) {
        return false;
    }

    std::vector<ProductionJob> jobs =
        ProductionJobDocument::fromJson(GetArrayField(document, "productionJobs"));
    ProductionJob job{Guid::NewGuid(),
                      orderId,
                      sampleCode,
                      productionQuantity,
                      std::chrono::system_clock::now(),
                      ProductionJobStatus::Queued};
    jobs.push_back(job);
    orderIt->status = OrderStatus::Producing;

    JsonValue updated = WithArrayField(document, "orders", OrderDocument::toJson(orders));
    updated = WithArrayField(updated, "productionJobs", ProductionJobDocument::toJson(jobs));
    WriteSharedDocument(path_, updated);
    return true;
}

}  // namespace SampleOrderSystem
