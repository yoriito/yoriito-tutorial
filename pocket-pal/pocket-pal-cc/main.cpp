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

#include <thread>
#include <memory>
#include <csignal>
#include "vss_broker.h"
#include "can_broker.h"

constexpr const char* VEHICLE_SPEED = "Vehicle.Speed";
constexpr const char* CAN_VDEVICE_NAME = "vcan0";

void on_vss(std::shared_ptr<CANBroker>);
void on_can(std::shared_ptr<CANBroker>);

volatile std::sig_atomic_t running = true;
void handle_sigint(int) {
    running = false;
}

int main() {
    std::signal(SIGINT, handle_sigint);
    auto can_broker = std::make_shared<CANBroker>(CAN_VDEVICE_NAME);
    auto can_thread = std::thread(on_can, can_broker);
    auto vss_thread = std::thread(on_vss, can_broker);
    if (can_thread.joinable()){
        can_thread.join();
    }
    // TODO:
    // We want to join this thread, but cannot do so because
    // yoriito::databroker::ProducerSubscribeContext::Messages() is a blocking call and does not support timeout or cancellation.
    //if (vss_thread.joinable()){
    //    vss_thread.join();
    //}
    vss_thread.detach();
    return 0;
}

void on_can(std::shared_ptr<CANBroker> can_broker){
    while(running) {
        auto frame = can_broker.get()->read();
        if (!frame.has_value()){
            continue;
        }
        auto valid_frame = frame.value();
        std::cout << "[CAN][RECV]" << "id: " << valid_frame.can_id << ", data: " << valid_frame.data << std::endl;

        VSSBroker::write<double>(VEHICLE_SPEED, 100.f, true, true);
        std::cout << "[VSS][SEND]" << "path: " << VEHICLE_SPEED << ", payload: " << 100.0f << std::endl;
    }
}

void on_vss(std::shared_ptr<CANBroker> can_broker){
    auto context = VSSBroker::subscribe(VEHICLE_SPEED);
    if (context.IsErr()){
        return;
    }
    auto valid_context = context.TakeOk();
    if (!valid_context.has_value()){
        return;
    }
    auto& stream = valid_context.value();
    while(running) {
        auto message = stream.get()->Messages();
        if (message.IsErr()){
            continue;
        }
        auto valid_message = message.TakeOk();
        if (!valid_message.has_value()){
            continue;
        }
        auto content = valid_message.value();
        auto data_points = content.GetDataPoints();
        for (auto data_point : data_points){
            auto path = data_point.GetPath();
            auto value = data_point.GetData();

            // TODO: compile value to string 
            //std::cout << "path: " << path << ", " << "value: ";
            //print_value(value);
            //std::cout << std::endl;
            std::cout << "[VSS][RECV]" << "path: " << path << std::endl;

            // TODO: users need to build any can frame from data_point
            can_broker.get()->write(256, {0x27, 0x10, 0x1F, 0x40, 0x00, 0x00, 0x00, 0x00});
            std::cout << "[CAN][SEND]" << "id: " << 256 << ", payload" << "0x27, 0x10, 0x1F, 0x40, 0x00, 0x00, 0x00, 0x00" << std::endl;
        }
    }
}
