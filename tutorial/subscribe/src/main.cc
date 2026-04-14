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
  YORIITO_LOG_INITIALIZE("subscribe");
  auto sub_result =
      yoriito::vehicle::body::lights::beam::high::ison::Subscribe();

  if (sub_result.IsErr()) {
    auto err = sub_result.TakeErr();
    YORIITO_LOG_INFO("subscribe",
                    std::string("Subscribe failed: ") + err->Message() +
                        " (code=" + std::to_string(err->ErrorCode()) + ")");
    return kExitNg;
  }

  auto subscription_context = sub_result.TakeOk().value();
  YORIITO_LOG_INFO("subscribe", "Subscribe succeeded. Waiting for updates...");

  while (true) {
    auto msg_result = subscription_context->Messages();
    if (msg_result.IsErr()) {
      auto err = msg_result.TakeErr();
      YORIITO_LOG_INFO("subscribe",
                      std::string("Messages() failed: ") + err->Message() +
                          " (code=" + std::to_string(err->ErrorCode()) + ")");
      break;
    }

    auto response = msg_result.TakeOk().value();
    const auto &data_points = response.GetDataPoints();
    const auto &data_point = data_points.front();
    auto data = data_point.GetData();
    const bool is_on = std::get<bool>(data);

    YORIITO_LOG_INFO("subscribe",
                    std::string("IsOn updated: ") + (is_on ? "true" : "false"));
  }

  return kExitOk;
}
