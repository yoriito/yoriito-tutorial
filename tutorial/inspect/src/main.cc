// SPDX-License-Identifier: Apache-2.0
//
// Copyright 2026 JASPAR
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "yoriito/yoriito_log.h"
#include <yoriito/signal/body/Body.h>

namespace {
constexpr int kExitOk = 0;
constexpr int kExitNg = 1;
} // namespace

int main() {
  YORIITO_LOG_INITIALIZE("inspect");

  auto result = yoriito::vehicle::body::lights::beam::high::ison::Inspect();

  if (result.IsErr()) {
    auto err = result.TakeErr();
    YORIITO_LOG_ERROR("inspect",
                      std::string("Inspect failed: ") + err->Message() +
                          " (code=" + std::to_string(err->ErrorCode()) + ")");
    YORIITO_LOG_FINALIZE();
    return kExitNg;
  }

  auto inspect_response = result.TakeOk().value();
  auto data_points = inspect_response.GetDataPoints();

  if (data_points.empty()) {
    YORIITO_LOG_ERROR("inspect", "Inspect failed: No data points returned.");
    YORIITO_LOG_FINALIZE();
    return kExitNg;
  }

  const auto &inspect_data = data_points.front();

  const auto &path = inspect_data.GetPath();
  // const auto &data_type = inspect_data.GetDataType();
  // const auto &entry_type = inspect_data.GetEntryType();
  // const auto &deprecation = inspect_data.GetDeprecation();
  // const auto &unit = inspect_data.GetUnit();
  // const auto &min = inspect_data.GetMin();
  // const auto &max = inspect_data.GetMax();
  // const auto &comment = inspect_data.GetComment();
  // const auto &allowed = inspect_data.GetAllowed();
  // const auto &default_value = inspect_data.GetDefaultValue();
  const auto &description = inspect_data.GetDescription();
  // const auto &array_size = inspect_data.GetArraySize();
  // const auto &pattern = inspect_data.GetPattern();
  // const auto &custom_attributes = inspect_data.GetCustomAttributes();

  YORIITO_LOG_INFO("inspect", "Inspect succeeded");
  YORIITO_LOG_INFO("inspect", std::string("Path: ") + path);
  YORIITO_LOG_INFO("inspect", std::string("Description: ") + description);

  YORIITO_LOG_FINALIZE();
  return kExitOk;
}
