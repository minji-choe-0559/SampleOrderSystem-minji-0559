#pragma once

#include "JsonValue.h"
#include "ProductionJob.h"

namespace SampleOrderSystem {

JsonValue toJson(const ProductionJob& job);
ProductionJob fromJson(const JsonValue& value);

}  // namespace SampleOrderSystem
