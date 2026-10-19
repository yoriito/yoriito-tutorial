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

#ifndef REF_APP_H
#define REF_APP_H

#include <mutex>
#include <atomic>
#include <vector>
#include <grpcpp/grpcpp.h>

// gRPC generated
#include "emergency_toggle.grpc.pb.h"
#include "emergency_availability.grpc.pb.h"

// Yoriito Vehicle API (Signal API)
#include "result.h"
#include "yoriito/signal/body/lights/Lights.h"
#include "yoriito/signal/body/horn/Horn.h"
#include "yoriito/signal/cabin/door/Door.h"

/**
 * @file ref_app.h
 * @brief gRPC service for toggle state control.
 * @deprecated -
 * @version -
 */

// ──────────────────────────────────────────────
// EmergencyService: Set for state
// ──────────────────────────────────────────────

/**
 * @ingroup State
 * @class EmergencyServiceImpl
 * @brief Implements `emrgency::EmergencyService::Service`.
 * @deprecated -
 * @version -
 */
class EmergencyServiceImpl final : public emrgency::EmergencyService::Service {
public:
    /**
    * @ingroup Set
    * @fn grpc::Status EmergencyServiceImpl::SetToggleState(grpc::ServerContext*, const
    * emrgency::ToggleRequest*, emrgency::ToggleResponse*)
    * @brief Sets the Toggle state.
    * @deprecated -
    * @pre DataBroker is reachable.
    * @param context Server context.
    * @param req Request with the target state.
    * @param _ Response (empty on success).
    * @retval Status::OK Success.
    * @retval Status(StatusCode::UNAVAILABLE) Backend write failed.
    */
    grpc::Status SetToggleState(grpc::ServerContext* context,
                            const emrgency::ToggleRequest* req,
                            emrgency::ToggleResponse* _) override;

private:
    /**
    * @ingroup Get
    * @fn bool ShouldContinue(yoriito::Result<void>& result, grpc::Status& status)
    * @brief Determines whether processing should continue.
    * @deprecated -
    * @pre -
    * @param result Set API result.
    * @param status Error status result.
    * @retval Status::OK Success.
    * @retval Status(StatusCode::UNAVAILABLE) Backend write failed.
    */
    bool ShouldContinue(yoriito::Result<void>& result, grpc::Status& status);

    /**
    * @ingroup Set
    * @fn grpc::Status EmergencyServiceImpl::SetLightSignals(bool)
    * @brief Sets the light signals.
    * @deprecated -
    * @pre DataBroker is reachable.
    * @param state Request with the toggle state.
    * @retval Status::OK Success.
    * @retval Status(StatusCode::UNAVAILABLE) Backend write failed.
    */
    grpc::Status SetLightSignals(bool state);

    /**
    * @ingroup Set
    * @fn grpc::Status EmergencyServiceImpl::SetHornSignals(bool)
    * @brief Sets the horn signals.
    * @deprecated -
    * @pre DataBroker is reachable.
    * @param state Request with the toggle state.
    * @retval Status::OK Success.
    * @retval Status(StatusCode::UNAVAILABLE) Backend write failed.
    */
    grpc::Status SetHornSignals(bool state);

    /**
    * @ingroup Set
    * @fn grpc::Status EmergencyServiceImpl::SetDoorSignals(bool)
    * @brief Sets the door signals.
    * @deprecated -
    * @pre DataBroker is reachable.
    * @param state Request with the toggle state.
    * @retval Status::OK Success.
    * @retval Status(StatusCode::UNAVAILABLE) Backend write failed.
    */
    grpc::Status SetDoorSignals(bool state);

    /**
    * @ingroup Set
    * @fn grpc::Status EmergencyServiceImpl::SetDoorLockSignals(bool)
    * @brief Sets the door lock signals.
    * @deprecated -
    * @pre DataBroker is reachable.
    * @param state Request with the toggle state.
    * @retval Status::OK Success.
    * @retval Status(StatusCode::UNAVAILABLE) Backend write failed.
    */
    grpc::Status SetDoorLockSignals(bool state);
};

// ────────────────────────────────────────────────
// EmergencyTelemetry: Poll for Toggle Availability
// ────────────────────────────────────────────────

/**
 * @ingroup Telemetry
 * @class EmergencyTelemetryImpl
 * @brief Implements `emrgency::EmergencyTelemetry::Service`.
 * @deprecated -
 * @version -
 */
class EmergencyTelemetryImpl final : public emrgency::EmergencyTelemetry::Service {
public:
    /**
    * @ingroup Subscribe
    * @fn grpc::Status
    * WiperTelemetryImpl::PollToggleAvailability(grpc::ServerContext*, const
    * emrgency::PollOptions*, ::grpc::ServerWriter<emrgency::ToggleAvailability>*)
    * @brief Streams toggle availability at a fixed interval.
    * @deprecated -
    * @pre DataBroker is reachable.
    * @param context Server context; cancel to stop.
    * @param opt Options. If `interval_ms <= 0`, a default is used. Very large values
    * are clamped.
    * @param writer Server writer for `emrgency::ToggleAvailability`.
    * @retval Status::OK Stream closed normally.
    * @retval Status(StatusCode::UNAVAILABLE) Initial read/write failed.
    */
    grpc::Status PollToggleAvailability(
        grpc::ServerContext* context, const emrgency::PollOptions* opt,
        ::grpc::ServerWriter<emrgency::ToggleAvailability>* writer) override;
};

// ──────────────────────────────────────────────
// SubscriptionManager: Subscription management
// ──────────────────────────────────────────────

/**
 * @ingroup Telemetry
 * @class SubscriptionManager
 * @brief Manage availability state.
 * @deprecated -
 * @version -
 */
class SubscriptionManager {
public:
    SubscriptionManager(size_t elementCount)
        : availability_(elementCount, false),
          allEnabled_(false)
    {}

    // Update process called from worker thread.
    void updateAvailability(size_t index, bool available) {
        std::lock_guard<std::mutex> lock(mutex_);
        availability_[index] = available;

        bool newState = checkAllEnabled();

        if (newState != allEnabled_) {
            allEnabled_ = newState;
        }
    }

    // Check process called from the main thread
    bool getAvailability() const {
        return allEnabled_.load();
    }

private:
    bool checkAllEnabled() {
        for (auto v : availability_) {
            if (!v) return false;
        }
        return true;
    }

    mutable std::mutex mutex_;
    std::vector<bool> availability_;
    std::atomic<bool> allEnabled_;
};

#endif  // REF_APP_H
