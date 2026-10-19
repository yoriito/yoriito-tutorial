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
#include <string>
#include <thread>
#include <variant>

// Short aliases for readability in this TU.
using grpc::ServerContext;
using grpc::Status;

// Defines function pointer types to abstract Get/Subscribe/Unsubscribe API
// calls.
using ResultBool = yoriito::Result<yoriito::signal::SignalGetDataPoint<bool>>;
using ResultString =
    yoriito::Result<yoriito::signal::SignalGetDataPoint<std::string>>;
using SignalValue = std::variant<ResultBool, ResultString>;

// Subscription context types returned by Subscribe operations.
using SubscribeContext =
    std::variant<yoriito::signal::SignalSubscribeContext<bool>,
                 yoriito::signal::SignalSubscribeContext<std::string>>;
using SubscribeResult = yoriito::Result<SubscribeContext>;
using FnGet = SignalValue (*)();
using FnSubscribe = SubscribeResult (*)();
using FnUnSubscribe = yoriito::Result<void> (*)(SubscribeContext);

template <typename T>
using TypedSubscribeResult =
    yoriito::Result<yoriito::signal::SignalSubscribeContext<T>>;

template <typename T> using TypedFnSubscribe = TypedSubscribeResult<T> (*)();

template <typename T>
using TypedFnUnSubscribe =
    yoriito::Result<void> (*)(yoriito::signal::SignalSubscribeContext<T>);

template <typename T> SubscribeResult Subscribe(TypedFnSubscribe<T> subscribe) {
  auto result = subscribe();
  if (result.IsErr()) {
    return SubscribeResult::Err(result.TakeErr());
  }

  auto context = result.TakeOk().value();
  return SubscribeResult::Ok(SubscribeContext{
      std::in_place_type<yoriito::signal::SignalSubscribeContext<T>>,
      std::move(context)});
}

template <typename T>
yoriito::Result<void> UnSubscribe(SubscribeContext context,
                                  TypedFnUnSubscribe<T> unsubscribe) {
  return unsubscribe(
      std::get<yoriito::signal::SignalSubscribeContext<T>>(std::move(context)));
}

std::string ToString(bool value) { return value ? "ON" : "OFF"; }

const std::string &ToString(const std::string &value) { return value; }

using namespace yoriito;

// Structure defining the runtime state of a subscription (context, unsubscribe,
// initialState).
struct SubscriptionState {
  SubscribeContext context;
  FnUnSubscribe unsubscribe;
  bool initialState;
};

// Structure defining a set of APIs for subscription handling (Get, Subscribe,
// Unsubscribe).
struct SubscriptionApiSet {
  FnGet get;
  FnSubscribe subscribe;
  FnUnSubscribe unsubscribe;
};

// Macro to initialize a SubscriptionApiSet for a given namespace.
#define SUBSCRIPTION_API_SET(ns, type)                                         \
  SubscriptionApiSet {                                                         \
    []() -> SignalValue { return ns::Get(); },                                 \
        []() -> SubscribeResult { return Subscribe<type>(&ns::Subscribe); },   \
        [](SubscribeContext context) -> yoriito::Result<void> {                \
          return UnSubscribe<type>(std::move(context), &ns::UnSubscribe);      \
        }                                                                      \
  }

// ──────────────────────────────
// EmergencyService implementation
// ──────────────────────────────

// Write requested state via Yoriito VSS Signals. Map backend errors to
// UNAVAILABLE.
Status EmergencyServiceImpl::SetToggleState(ServerContext *,
                                            const emrgency::ToggleRequest *req,
                                            emrgency::ToggleResponse *) {
  // Set the toggle state.
  bool state = req->is_on();
  YORIITO_LOG_INFO("SetToggleState",
                   "Toggle state: " + std::string(state ? "ON" : "OFF"));

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

  YORIITO_LOG_INFO("SetToggleState", "Return status: OK");

  return Status::OK;
}

// Determines whether processing should continue rather than abort. Map backend
// errors to UNAVAILABLE.
bool EmergencyServiceImpl::ShouldContinue(Result<void> &result,
                                          Status &status) {

  status = Status::OK;

  if (result.IsErr()) {
    auto err = result.TakeErr();
    // Does not treat the condition as an error when the capability is missing
    // (ERROR_NOT_FOUND). Note: In this tutorial, a missing capability is
    // treated as a non-actionable signal.
    if (static_cast<uint32_t>(ErrorCode::ERROR_NOT_FOUND) != err->ErrorCode()) {
      status = Status(grpc::StatusCode::UNAVAILABLE, err->Message());
      return false;
    }
  }

  return true;
}

// Write requested state via Yoriito VSS Signals for light. Map backend errors
// to UNAVAILABLE.
Status EmergencyServiceImpl::SetLightSignals(bool state) {

  Status status = Status::OK;

  // Set the hazard light signal.
  auto rc = vehicle::body::lights::hazard::issignaling::Set(state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the high beam signal.
  rc = vehicle::body::lights::beam::high::ison::Set(state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the low beam signal.
  rc = vehicle::body::lights::beam::low::ison::Set(state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the brake light signal.
  rc = vehicle::body::lights::brake::isactive::Set(state ? "ACTIVE"
                                                         : "INACTIVE");
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the license plate light signal.
  rc = vehicle::body::lights::licenseplate::ison::Set(state);
  if (false == ShouldContinue(rc, status))
    return status;

  return Status::OK;
}

// Write requested state via Yoriito VSS Signals for horn. Map backend errors to
// UNAVAILABLE.
Status EmergencyServiceImpl::SetHornSignals(bool state) {

  Status status = Status::OK;

  // Set the horn signal.
  auto rc = vehicle::body::horn::isactive::Set(state);
  if (false == ShouldContinue(rc, status))
    return status;

  return Status::OK;
}

// Write requested state via Yoriito VSS Signals for door. Map backend errors to
// UNAVAILABLE.
Status EmergencyServiceImpl::SetDoorSignals(bool state) {

  Status status = Status::OK;

  // Set the driver's side door open signal.
  auto rc = vehicle::cabin::door::row1::driverside::isopen::Set(!state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the passenger side door open signal.
  rc = vehicle::cabin::door::row1::passengerside::isopen::Set(!state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the driver's side rear seat door open signal.
  rc = vehicle::cabin::door::row2::driverside::isopen::Set(!state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the passenger side rear seat door open signal.
  rc = vehicle::cabin::door::row2::passengerside::isopen::Set(!state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the driver's side window open signal.
  rc = vehicle::cabin::door::row1::driverside::window::isopen::Set(!state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the passenger side window open signal.
  rc = vehicle::cabin::door::row1::passengerside::window::isopen::Set(!state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the driver's side rear seat window open signal.
  rc = vehicle::cabin::door::row2::driverside::window::isopen::Set(!state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the passenger side rear seat window open signal.
  rc = vehicle::cabin::door::row2::passengerside::window::isopen::Set(!state);
  if (false == ShouldContinue(rc, status))
    return status;

  return Status::OK;
}

// Write requested state via yoriito-vss Signals for door lock. Map backend
// errors to UNAVAILABLE.
Status EmergencyServiceImpl::SetDoorLockSignals(bool state) {

  Status status = Status::OK;

  // Set the driver's side door lock signal.
  auto rc = vehicle::cabin::door::row1::driverside::islocked::Set(state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the passenger side door lock signal.
  rc = vehicle::cabin::door::row1::passengerside::islocked::Set(state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the driver's side rear seat door lock signal.
  rc = vehicle::cabin::door::row2::driverside::islocked::Set(state);
  if (false == ShouldContinue(rc, status))
    return status;

  // Set the passenger side rear seat door lock signal.
  rc = vehicle::cabin::door::row2::passengerside::islocked::Set(state);
  if (false == ShouldContinue(rc, status))
    return status;

  return Status::OK;
}

// ─────────────────────────────────
// EmergencyTelemetry implementation
// ─────────────────────────────────

// Stream toggle_availability at a fixed interval until the client cancels.
// Choose interval from options (default if <= 0), clamp a safe max, sleep
// between sends.
Status EmergencyTelemetryImpl::PollToggleAvailability(
    ServerContext *ctx, const emrgency::PollOptions *opt,
    ::grpc::ServerWriter<emrgency::ToggleAvailability> *writer) {

  // Check the interval range.
  uint32_t interval_ms =
      (opt && opt->interval_ms() > 0) ? opt->interval_ms() : 10;
  if (interval_ms > 1000)
    interval_ms = 1000;

  // Enable subscription processing.
  std::atomic<bool> running = true;

  // List of subscription-based signal API sets.
  static const std::vector<SubscriptionApiSet> api_set_list = {
      SUBSCRIPTION_API_SET(vehicle::body::lights::hazard::issignaling, bool),
      SUBSCRIPTION_API_SET(vehicle::body::lights::beam::high::ison, bool),
      SUBSCRIPTION_API_SET(vehicle::body::lights::beam::low::ison, bool),
      SUBSCRIPTION_API_SET(vehicle::body::lights::brake::isactive, std::string),
      SUBSCRIPTION_API_SET(vehicle::body::lights::licenseplate::ison, bool),
      SUBSCRIPTION_API_SET(vehicle::body::horn::isactive, bool),
      SUBSCRIPTION_API_SET(vehicle::cabin::door::row1::driverside::islocked,
                           bool),
      SUBSCRIPTION_API_SET(vehicle::cabin::door::row1::passengerside::islocked,
                           bool),
      SUBSCRIPTION_API_SET(vehicle::cabin::door::row2::driverside::islocked,
                           bool),
      SUBSCRIPTION_API_SET(vehicle::cabin::door::row2::passengerside::islocked,
                           bool),
      SUBSCRIPTION_API_SET(vehicle::cabin::door::row1::driverside::isopen,
                           bool),
      SUBSCRIPTION_API_SET(vehicle::cabin::door::row1::passengerside::isopen,
                           bool),
      SUBSCRIPTION_API_SET(vehicle::cabin::door::row2::driverside::isopen,
                           bool),
      SUBSCRIPTION_API_SET(vehicle::cabin::door::row2::passengerside::isopen,
                           bool),
      SUBSCRIPTION_API_SET(
          vehicle::cabin::door::row1::driverside::window::isopen, bool),
      SUBSCRIPTION_API_SET(
          vehicle::cabin::door::row1::passengerside::window::isopen, bool),
      SUBSCRIPTION_API_SET(
          vehicle::cabin::door::row2::driverside::window::isopen, bool),
      SUBSCRIPTION_API_SET(
          vehicle::cabin::door::row2::passengerside::window::isopen, bool),
  };

  // Check availability.
  auto IsAvailable = [&](FnGet fn, bool &is_capable) -> bool {
    is_capable = true;
    SignalValue result = fn();

    return std::visit(
        [&](auto &get_result) {
          if (get_result.IsErr()) {
            auto error = get_result.TakeErr()->ErrorCode();

            // ERROR_NOT_FOUND indicates that the capability is unsupported.
            // Other Get errors do not mark the capability as unsupported.
            is_capable =
                (static_cast<uint32_t>(ErrorCode::ERROR_NOT_FOUND) != error);
            return false;
          }

          auto data_point = get_result.TakeOk().value();
          return data_point.IsAvailableActuator() &&
                 data_point.IsAvailableSensor();
        },
        result);
  };

  // Worker thread that listens for subscription updates and
  // propagates availability changes to the manager.
  auto subscriptionWorker = [&](size_t index, SubscriptionState &state,
                                SubscriptionManager &manager) -> void {
    bool is_available = state.initialState;

    while (running) {
      // Update the availability state managed by SubscriptionManager.
      manager.updateAvailability(index, is_available);

      // Wait for the next availability update message.
      auto msg_result = std::visit(
          [](auto &context) -> SignalValue { return context.Messages(); },
          state.context);

      bool message_failed = false;
      std::visit(
          [&](auto &result) {
            if (result.IsErr()) {
              auto err = result.TakeErr();
              YORIITO_LOG_INFO("PollToggleAvailability", err->Message());
              manager.updateAvailability(index, false);
              message_failed = true;
              return;
            }

            // Extract the data point from the received message.
            auto data_point = result.TakeOk().value();
            std::string path = data_point.GetPath();
            is_available = data_point.IsAvailableActuator() &&
                           data_point.IsAvailableSensor();
            if (is_available) {
              // Log the signal value if it is available.
              YORIITO_LOG_INFO("PollToggleAvailability",
                               path + ": " + ToString(data_point.GetData()));
            } else {
              // Log that the signal is unavailable.
              YORIITO_LOG_INFO("PollToggleAvailability",
                               path + ": this signal is unavailable");
            }
          },
          msg_result);

      if (message_failed) {
        break;
      }
    }
  };

  std::vector<SubscriptionState> subscriptionStates;
  std::vector<std::thread> workers;

  for (const auto &api : api_set_list) {
    // Check whether the actuator capability is available.
    bool is_capable = true;
    bool is_signal_available = IsAvailable(api.get, is_capable);
    if (!is_signal_available) {
      // Skip subscription if the capability is not supported.
      if (!is_capable)
        continue;
    }

    // Start subscription.
    auto sub_result = api.subscribe();
    if (sub_result.IsErr()) {
      // Disable subscription processing.
      running = false;

      // Unsubscribe from all active subscriptions.
      for (auto &state : subscriptionStates) {
        state.unsubscribe(std::move(state.context));
      }

      auto err = sub_result.TakeErr();
      return Status(grpc::StatusCode::UNAVAILABLE, err->Message());
    }

    subscriptionStates.push_back(SubscriptionState{
        sub_result.TakeOk().value(), api.unsubscribe, is_signal_available});
  }

  // Initialize subscription manager.
  SubscriptionManager manager(subscriptionStates.size());

  for (size_t index = 0; index < subscriptionStates.size(); ++index) {
    // Launch worker thread for this subscription.
    workers.emplace_back(subscriptionWorker, index,
                         std::ref(subscriptionStates[index]),
                         std::ref(manager));
  }

  // Get the current aggregated availability state.
  bool is_available = manager.getAvailability();

  // Send the initial availability state to the client.
  // The toggle is available only if all signals are available.
  emrgency::ToggleAvailability msg;
  msg.set_is_available(is_available);
  writer->Write(msg);

  // Periodically check and stream availability updates.
  while (!ctx->IsCancelled()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));

    // Get the current aggregated availability state.
    bool new_available = manager.getAvailability();

    // Send update only if the state has changed.
    if (is_available != new_available) {
      is_available = new_available;
      msg.set_is_available(is_available);
      writer->Write(msg);
    }
  }

  // Disable subscription processing.
  running = false;

  // Unsubscribe from all active subscriptions.
  for (auto &state : subscriptionStates) {
    state.unsubscribe(std::move(state.context));
  }

  // Wait for all worker threads to finish.
  for (auto &worker : workers) {
    worker.join();
  }

  YORIITO_LOG_INFO("PollToggleAvailability", "Return status: OK");

  return Status::OK;
}
