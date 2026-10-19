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

namespace high_beam = yoriito::vehicle::body::lights::beam::high::ison;
} // namespace

int main() {
  YORIITO_LOG_INITIALIZE("subscribe");

  auto subscribe_result = high_beam::Subscribe();
  if (subscribe_result.IsErr()) {
    auto error = subscribe_result.TakeErr();
    YORIITO_LOG_ERROR("subscribe",
                      std::string("Subscribe failed: ") + error->Message() +
                          " (code=" + std::to_string(error->ErrorCode()) + ")");
    YORIITO_LOG_FINALIZE();
    return kExitNg;
  }

  auto subscription_context = subscribe_result.TakeOk().value();
  YORIITO_LOG_INFO("subscribe", "Waiting for one update...");

  int exit_code = kExitOk;

  auto message_result = subscription_context.Messages();
  if (message_result.IsErr()) {
    auto error = message_result.TakeErr();
    YORIITO_LOG_ERROR("subscribe",
                      std::string("Messages() failed: ") + error->Message() +
                          " (code=" + std::to_string(error->ErrorCode()) + ")");
    exit_code = kExitNg;
  } else {
    auto data_point = message_result.TakeOk().value();
    const bool is_on = data_point.GetData();

    YORIITO_LOG_INFO("subscribe", std::string("IsOn updated: ") +
                                      (is_on ? "true" : "false"));
  }

  auto unsubscribe_result =
      high_beam::UnSubscribe(std::move(subscription_context));
  if (unsubscribe_result.IsErr()) {
    auto error = unsubscribe_result.TakeErr();
    YORIITO_LOG_ERROR("subscribe",
                      std::string("UnSubscribe failed: ") + error->Message() +
                          " (code=" + std::to_string(error->ErrorCode()) + ")");
    exit_code = kExitNg;
  } else {
    YORIITO_LOG_INFO("subscribe", "UnSubscribe succeeded.");
  }

  YORIITO_LOG_FINALIZE();
  return exit_code;
}
