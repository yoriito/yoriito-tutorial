/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2026 JASPAR
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <sys/ioctl.h>
#include <net/if.h>  
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

class CANBroker {
public:
    explicit CANBroker(const std::string& if_name)
        : if_name_(if_name) {
        // Open CAN socket
        sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (sock_ < 0) throw std::runtime_error("Failed to open CAN socket");

        struct ifreq ifr;
        std::strncpy(ifr.ifr_name, if_name.c_str(), IFNAMSIZ);
        if (ioctl(sock_, SIOCGIFINDEX, &ifr) < 0) {
            std::cerr << "ioctl SIOCGIFINDEX failed, errno=" << errno
              << " (" << std::strerror(errno) << ")\n";
            close(sock_);
            throw std::runtime_error("Failed to get interface index");
        }

        struct sockaddr_can addr = {};
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(sock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock_);
            throw std::runtime_error("Failed to bind CAN socket");
        }

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        if (setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
            perror("setsockopt failed");
        }

    }

    ~CANBroker() {
        if (sock_ >= 0) close(sock_);
    }

    std::optional<struct can_frame> read() {
        struct can_frame frame;
        // NOTE:
        // blocking here
        ssize_t nbytes = recv(sock_, &frame, sizeof(frame), 0);
        if (nbytes == sizeof(frame)) {
            return frame;
        }
        return std::nullopt;
    }

    void write(uint16_t can_id, const std::vector<uint8_t>& data) {
        struct can_frame frame = {};
        frame.can_id = can_id;
        frame.can_dlc = std::min<size_t>(data.size(), 8);
        std::memcpy(frame.data, data.data(), frame.can_dlc);
        if (send(sock_, &frame, sizeof(frame), 0) != sizeof(frame)) {
            throw std::runtime_error("Failed to send CAN frame");
        }
    }

private:

    std::string if_name_;
    int sock_ = -1;
};
