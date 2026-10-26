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

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "databroker/databroker_producer.h"
#include "databroker/databroker_producer_factory.h"
#include "result.h"

namespace {
inline double get_time() {
  const auto now = std::chrono::system_clock::now();
  const auto epoch_duration = now.time_since_epoch();
  std::chrono::duration<double> seconds_since_epoch = epoch_duration;
  double epoch_time_as_double = seconds_since_epoch.count();
  return epoch_time_as_double;
}
} // namespace

namespace VSSBroker {

template <typename T>
void write(std::string path, T data, bool is_available_sensor,
           bool is_available_actuator) {
  yoriito::databroker::DataBrokerProducer *producer =
      yoriito::databroker::DataBrokerProducerFactory::GetInstance();
  const std::optional<double> produced_timestamp = get_time();
  auto data_point = yoriito::databroker::ProducerSetDataPoint{
      path, data, is_available_sensor, is_available_actuator,
      produced_timestamp};
  const std::vector<yoriito::databroker::ProducerSetDataPoint> data_points = {
      data_point};
  auto set_request = yoriito::databroker::ProducerSetRequest{data_points};
  producer->Set(set_request);
}

yoriito::Result<std::unique_ptr<yoriito::databroker::ProducerSubscribeContext>>
subscribe(std::string path) {
  yoriito::databroker::DataBrokerProducer *producer =
      yoriito::databroker::DataBrokerProducerFactory::GetInstance();
  auto data = yoriito::databroker::ProducerSubscribeRequest{path};
  auto ret = producer->Subscribe(data);
  return ret;
}

void unsubscribe(std::string subscription_id) {}
}; // namespace VSSBroker
