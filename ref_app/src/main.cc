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


#include <memory>

#include "ref_app.h"
#include "yoriito/yoriito_log.h"

int main(int, char**) {
  // Initialize Logger
  YORIITO_LOG_INITIALIZE("ref_app");

  // Address to listen on (host:port). Change as needed for your environment.
  const std::string addr = "127.0.0.1:50055";

  // Instantiate service objects. They are stateless aside from backend handles.
  EmergencyServiceImpl service_toggle;
  EmergencyTelemetryImpl service_telemetry;

  // Build and configure the gRPC server.
  grpc::ServerBuilder builder;

  // Listen on localhost with insecure credentials.
  builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

  // Keep the HTTP/2 connection alive by sending frequent pings and allowing pinging while idle.
  builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 5000);
  builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 5000);
  builder.AddChannelArgument(GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA, 0);

  // Register RPC services. The builder takes ownership for the server lifetime.
  builder.RegisterService(&service_toggle);
  builder.RegisterService(&service_telemetry);

  // Create and start the server.
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  YORIITO_LOG_INFO("ref_app", "gRPC listening on " + addr);

  // Block until the server is asked to shut down.
  server->Wait();
  YORIITO_LOG_FINALIZE();
  return 0;
}
