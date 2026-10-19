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
  YORIITO_LOG_INITIALIZE("availability");
  auto result = yoriito::vehicle::body::lights::fog::front::ison::Get();

  if (result.IsErr()) {
    auto err = result.TakeErr();
    YORIITO_LOG_ERROR("availability",
                      std::string("Get failed: ") + err->Message() +
                          " (code=" + std::to_string(err->ErrorCode()) + ")");
    YORIITO_LOG_FINALIZE();
    return kExitNg;
  }

  auto data_point = result.TakeOk().value();
  const bool is_available_sensor = data_point.IsAvailableSensor();
  const bool is_available_actuator = data_point.IsAvailableActuator();

  YORIITO_LOG_INFO("availability",
                   std::string("Available Sensor: ") +
                       (is_available_sensor ? "true" : "false"));
  YORIITO_LOG_INFO("availability",
                   std::string("Available Actuator: ") +
                       (is_available_actuator ? "true" : "false"));

  YORIITO_LOG_FINALIZE();
  return kExitOk;
}
