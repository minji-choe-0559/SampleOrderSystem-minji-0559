#pragma once

#include "JsonValue.h"
#include "SampleRecord.h"

namespace SampleOrderSystem {

JsonValue toJson(const SampleRecord& record);
SampleRecord fromJson(const JsonValue& value);

}  // namespace SampleOrderSystem
