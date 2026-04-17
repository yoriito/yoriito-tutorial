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
  YORIITO_LOG_INITIALIZE("set");

  const bool is_on = true;
  yoriito::Result<void> set_result =
      yoriito::vehicle::body::lights::beam::high::ison::Set(is_on);

  if (set_result.IsErr()) {
    const yoriito::Result<void>::ErrorPtr err = set_result.TakeErr();
    YORIITO_LOG_INFO("set", std::string("Set failed: ") + err->Message() +
                               " (code=" + std::to_string(err->ErrorCode()) +
                               ")");
    return kExitNg;
  }

  YORIITO_LOG_INFO("set",
                  std::string("Set succeed: ") + (is_on ? "true" : "false"));

  return kExitOk;
}
