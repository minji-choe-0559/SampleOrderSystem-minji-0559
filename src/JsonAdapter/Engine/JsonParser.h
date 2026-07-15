#pragma once

#include <string>

#include "JsonValue.h"

namespace SampleOrderSystem {

class JsonParser {
  public:
    static JsonValue parse(const std::string& text);
};

}  // namespace SampleOrderSystem
