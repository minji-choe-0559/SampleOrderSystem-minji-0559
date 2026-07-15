#pragma once

#include <string>

#include "JsonValue.h"

namespace SampleOrderSystem {

class JsonWriter {
  public:
    static std::string write(const JsonValue& value);
};

}  // namespace SampleOrderSystem
