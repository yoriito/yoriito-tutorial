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

#include "can_broker.h"
#include "vss_broker.h"
#include "yoriito/yoriito_log.h"
#include <csignal>
#include <cstdint>
#include <iomanip>
#include <memory>
#include <sstream>
#include <thread>
#include <variant>

constexpr const char *WINDOW_POSITION =
    "Vehicle.Cabin.Door.Row1.DriverSide.Window.Position";
constexpr const char *CAN_VDEVICE_NAME = "vcan0";

void on_vss(std::shared_ptr<CANBroker>);
void on_can(std::shared_ptr<CANBroker>);

std::string format_can_data(const can_frame &frame) {
  std::ostringstream stream;
  stream << std::hex << std::setfill('0');
  for (uint8_t index = 0; index < frame.can_dlc; ++index) {
    if (index != 0) {
      stream << ' ';
    }
    stream << std::setw(2) << static_cast<unsigned int>(frame.data[index]);
  }
  return stream.str();
}

volatile std::sig_atomic_t running = true;
void handle_sigint(int) { running = false; }

int main() {
  YORIITO_LOG_INITIALIZE("PAL");
  std::signal(SIGINT, handle_sigint);
  auto can_broker = std::make_shared<CANBroker>(CAN_VDEVICE_NAME);
  auto can_thread = std::thread(on_can, can_broker);
  auto vss_thread = std::thread(on_vss, can_broker);
  if (can_thread.joinable()) {
    can_thread.join();
  }
  // TODO:
  // We want to join this thread, but cannot do so because
  // yoriito::databroker::ProducerSubscribeContext::Messages() is a blocking
  // call and does not support timeout or cancellation.
  // if (vss_thread.joinable()){
  //    vss_thread.join();
  //}
  vss_thread.detach();
  YORIITO_LOG_FINALIZE();
  return 0;
}

void on_can(std::shared_ptr<CANBroker> can_broker) {
  while (running) {
    auto frame = can_broker.get()->read();
    if (!frame.has_value()) {
      continue;
    }
    auto valid_frame = frame.value();
    YORIITO_LOG_INFO("CAN", "[RECV] id: " + std::to_string(valid_frame.can_id) +
                                ", data: " + format_can_data(valid_frame));

    VSSBroker::write<std::uint8_t>(WINDOW_POSITION, 100, true, true);
    YORIITO_LOG_INFO("VSS", "[SEND] path: " + std::string(WINDOW_POSITION) +
                                ", payload: 100");
  }
}

void on_vss(std::shared_ptr<CANBroker> can_broker) {
  auto context = VSSBroker::subscribe(WINDOW_POSITION);
  if (context.IsErr()) {
    return;
  }
  auto valid_context = context.TakeOk();
  if (!valid_context.has_value()) {
    return;
  }
  auto &stream = valid_context.value();
  while (running) {
    auto message = stream.get()->Messages();
    if (message.IsErr()) {
      continue;
    }
    auto valid_message = message.TakeOk();
    if (!valid_message.has_value()) {
      continue;
    }
    auto content = valid_message.value();
    auto data_points = content.GetDataPoints();
    for (auto data_point : data_points) {
      auto path = data_point.GetPath();
      const auto &value = data_point.GetData();
      YORIITO_LOG_INFO(
          "VSS", "[RECV] path: " + path + ", value: " +
                     std::to_string(static_cast<unsigned int>(
                         std::get<std::uint8_t>(value))));

      // TODO: users need to build any can frame from data_point
      can_broker.get()->write(256,
                              {0x27, 0x10, 0x1F, 0x40, 0x00, 0x00, 0x00, 0x00});
      YORIITO_LOG_INFO("CAN", "[SEND] id: 256, payload: "
                              "27 10 1f 40 00 00 00 00");
    }
  }
}
