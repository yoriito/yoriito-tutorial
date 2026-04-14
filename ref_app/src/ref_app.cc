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

// ref_app.cc
// gRPC service for toggle state control.
// Talks to DataBroker(Yoriito kura) via Yoriito VSS Sensor/Actuator wrappers.
// Backend failures are mapped to StatusCode::UNAVAILABLE.

#include "ref_app.h"

#include "yoriito/yoriito_log.h"
#include <grpcpp/grpcpp.h>

#include <cstdint>
#include <thread>

// Short aliases for readability in this TU.
using grpc::ServerContext;
using grpc::Status;

using namespace yoriito;

using Fn = yoriito::Result<bool>(*)();

// ──────────────────────────────
// EmergencyService implementation
// ──────────────────────────────

// Write requested state via Yoriito VSS Signals. Map backend errors to UNAVAILABLE.
Status EmergencyServiceImpl::SetToggleState(ServerContext*, const emrgency::ToggleRequest* req, emrgency::ToggleResponse*) {
    // Set the toggle state.
    bool state = req->is_on();
    YORIITO_LOG_INFO("ref_app", "Toggle state: " + std::string(state ? "ON" : "OFF"));

    Status status = Status::OK;

    // Set light signals.
    auto result = SetLightSignals(state);
    if (!result.ok()) {
        return result;
    }

    // Set horn signals.
    result = SetHornSignals(state);
    if (!result.ok()) {
        return result;
    }

    if (state) {
        // If the toggle state is on, the door will lock after it is closed.

        // Set door signals.
        result = SetDoorSignals(state);
        if (!result.ok()) {
            return result;
        }

        // Set door lock signals.
        result = SetDoorLockSignals(state);
        if (!result.ok()) {
            return result;
        }
    } else {
        // If the toggle state is off, the door will open after it is unlocked.

        // Set door lock signals.
        result = SetDoorLockSignals(state);
        if (!result.ok()) {
            return result;
        }

        // Set door signals.
        result = SetDoorSignals(state);
        if (!result.ok()) {
            return result;
        }
    }

    YORIITO_LOG_INFO("ref_app", "Return status: OK");

    return Status::OK;
}

// Determines whether processing should continue rather than abort. Map backend errors to UNAVAILABLE.
bool EmergencyServiceImpl::ShouldContinue(Result<void>& result, Status& status) {
    
    status = Status::OK;

    if (result.IsErr()) {
        auto err = result.TakeErr();
        // Does not treat the condition as an error when the capability is missing (ERROR_NOT_FOUND).
        // Note: In this tutorial, a missing capability is treated as a non-actionable signal.
        if (static_cast<uint32_t>(ErrorCode::ERROR_NOT_FOUND) != err->ErrorCode()) {
            status = Status(grpc::StatusCode::UNAVAILABLE, err->Message());
            return false;
        }
    }

    return true;
}

// Write requested state via Yoriito VSS Signals for light. Map backend errors to UNAVAILABLE.
Status EmergencyServiceImpl::SetLightSignals(bool state) {

    Status status = Status::OK;

    // Set the hazard light signal.
    auto rc = vehicle::body::lights::hazard::issignaling::Set(state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the high beam signal.
    rc = vehicle::body::lights::beam::high::ison::Set(state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the low beam signal.
    rc = vehicle::body::lights::beam::low::ison::Set(state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the brake light signal.
    rc = vehicle::body::lights::brake::isactive::Set(state ? "ACTIVE" : "INACTIVE");
    if (false == ShouldContinue(rc, status)) return status;

    // Set the license plate light signal.
    rc = vehicle::body::lights::licenseplate::ison::Set(state);
    if (false == ShouldContinue(rc, status)) return status;

    return Status::OK;
}

// Write requested state via Yoriito VSS Signals for horn. Map backend errors to UNAVAILABLE.
Status EmergencyServiceImpl::SetHornSignals(bool state) {

    Status status = Status::OK;

    // Set the horn signal.
    auto rc = vehicle::body::horn::isactive::Set(state);
    if (false == ShouldContinue(rc, status)) return status;

    return Status::OK;
}

// Write requested state via Yoriito VSS Signals for door. Map backend errors to UNAVAILABLE.
Status EmergencyServiceImpl::SetDoorSignals(bool state) {

    Status status = Status::OK;

    // Set the driver's side door open signal.
    auto rc = vehicle::cabin::door::row1::driverside::isopen::Set(!state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the passenger side door open signal.
    rc = vehicle::cabin::door::row1::passengerside::isopen::Set(!state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the driver's side rear seat door open signal.
    rc = vehicle::cabin::door::row2::driverside::isopen::Set(!state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the passenger side rear seat door open signal.
    rc = vehicle::cabin::door::row2::passengerside::isopen::Set(!state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the driver's side window open signal.
    rc = vehicle::cabin::door::row1::driverside::window::isopen::Set(!state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the passenger side window open signal.
    rc = vehicle::cabin::door::row1::passengerside::window::isopen::Set(!state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the driver's side rear seat window open signal.
    rc = vehicle::cabin::door::row2::driverside::window::isopen::Set(!state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the passenger side rear seat window open signal.
    rc = vehicle::cabin::door::row2::passengerside::window::isopen::Set(!state);
    if (false == ShouldContinue(rc, status)) return status;

    return Status::OK;
}

// Write requested state via yoriito-vss Signals for door lock. Map backend errors to UNAVAILABLE.
Status EmergencyServiceImpl::SetDoorLockSignals(bool state) {

    Status status = Status::OK;

    // Set the driver's side door lock signal.
    auto rc = vehicle::cabin::door::row1::driverside::islocked::Set(state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the passenger side door lock signal.
    rc = vehicle::cabin::door::row1::passengerside::islocked::Set(state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the driver's side rear seat door lock signal.
    rc = vehicle::cabin::door::row2::driverside::islocked::Set(state);
    if (false == ShouldContinue(rc, status)) return status;

    // Set the passenger side rear seat door lock signal.
    rc = vehicle::cabin::door::row2::passengerside::islocked::Set(state);
    if (false == ShouldContinue(rc, status)) return status;

    return Status::OK;
}

// ─────────────────────────────────
// EmergencyTelemetry implementation
// ─────────────────────────────────

// Stream toggle_availability at a fixed interval until the client cancels.
// Choose interval from options (default if <= 0), clamp a safe max, sleep between
// sends.
Status EmergencyTelemetryImpl::PollToggleAvailability(
    ServerContext* ctx,
    const emrgency::PollOptions* opt,
    ::grpc::ServerWriter<emrgency::ToggleAvailability>* writer) {
    
    // Check the interval range.
    uint32_t interval_ms = (opt && opt->interval_ms() > 0) ? opt->interval_ms() : 10;
    if (interval_ms > 1000) interval_ms = 1000;
    
    // Actuator availability signal getter list.
    // Note: In this tutorial, only actuator signals are used.
    static const std::vector<Fn> getterlist = {
        {&vehicle::body::lights::hazard::issignaling_availability_actuator::Get},
        {&vehicle::body::lights::beam::high::ison_availability_actuator::Get},
        {&vehicle::body::lights::beam::low::ison_availability_actuator::Get},
        {&vehicle::body::lights::brake::isactive_availability_actuator::Get},
        {&vehicle::body::lights::licenseplate::ison_availability_actuator::Get},
        {&vehicle::body::horn::isactive_availability_actuator::Get},
        {&vehicle::cabin::door::row1::driverside::islocked_availability_actuator::Get},
        {&vehicle::cabin::door::row1::passengerside::islocked_availability_actuator::Get},
        {&vehicle::cabin::door::row2::driverside::islocked_availability_actuator::Get},
        {&vehicle::cabin::door::row2::passengerside::islocked_availability_actuator::Get},
        {&vehicle::cabin::door::row1::driverside::isopen_availability_actuator::Get},
        {&vehicle::cabin::door::row1::passengerside::isopen_availability_actuator::Get},
        {&vehicle::cabin::door::row2::driverside::isopen_availability_actuator::Get},
        {&vehicle::cabin::door::row2::passengerside::isopen_availability_actuator::Get},
        {&vehicle::cabin::door::row1::driverside::window::isopen_availability_actuator::Get},
        {&vehicle::cabin::door::row1::passengerside::window::isopen_availability_actuator::Get},
        {&vehicle::cabin::door::row2::driverside::window::isopen_availability_actuator::Get},
        {&vehicle::cabin::door::row2::passengerside::window::isopen_availability_actuator::Get},
    };

    // Check availability.
    auto IsAvailable = [&](Fn fn, bool& is_capable) -> bool {
        is_capable = true;
        auto result = fn();
        if (!result.IsOk()) {
            // Determine whether the capability exists.
            // ERROR_NOT_FOUND indicates the capability is unavailable.
            is_capable = (static_cast<uint32_t>(ErrorCode::ERROR_NOT_FOUND) != result.TakeErr()->ErrorCode());
            // Processing continues even if an error occurs.
            return false;
        }
        return result.TakeOk().value();
    };

    // Send a message once.
    auto SendOnce = [&]() -> bool {
        // Toggle button is available if all signals are available.
        bool is_available = true;
        for (const auto& fn : getterlist) {
            bool is_capable = true;
            if (!IsAvailable(fn, is_capable)) {
                // If the capability exists but is unavailable, the process will terminate.
                // If the capability does not exist, the process will continue.
                if (true == is_capable) {
                    is_available = false;
                    break;
                }
            }
        }

        // return response.
        emrgency::ToggleAvailability msg;
        msg.set_is_available(is_available);
        return writer->Write(msg);
    };
    
    // Poll all signals for availability.
    while (!ctx->IsCancelled()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        if (!SendOnce()) {
            // Simple backoff on a transient read failure.
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    return Status::OK;
}
