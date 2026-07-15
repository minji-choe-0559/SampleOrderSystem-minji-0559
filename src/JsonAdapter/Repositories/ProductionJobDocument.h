#pragma once

#include <vector>

#include "JsonValue.h"
#include "ProductionJob.h"

namespace SampleOrderSystem {

// ProductionJob 배열 <-> JsonValue 배열 변환만 전담한다(SampleDocument/OrderDocument와 동일한
// 역할 분리).
namespace ProductionJobDocument {

JsonValue toJson(const std::vector<ProductionJob>& jobs);

std::vector<ProductionJob> fromJson(const std::vector<JsonValue>& items);

}  // namespace ProductionJobDocument

}  // namespace SampleOrderSystem
