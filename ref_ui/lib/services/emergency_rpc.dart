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

import 'dart:async';
import 'package:grpc/grpc.dart';
import '../gen/emergency_toggle.pbgrpc.dart';
import '../gen/emergency_availability.pbgrpc.dart' as eavl;

/// gRPC helper for the Emergency Toggle demo backend.
///
/// Public API:
///  - setToggleState         : set toggle state (true:ON / false:OFF)
///  - pollToggleAvailability : poll toggle availability (true:available / false:unavailable)
///
/// Each call opens a short-lived channel and shuts it down on completion.
///
/// Default endpoint: 127.0.0.1:50055 (insecure).
class EmergencyRpc {
  static const String _host = '127.0.0.1';
  static const int _port = 50055;

  static ClientChannel _channel() => ClientChannel(
    _host,
    port: _port,
    options: const ChannelOptions(
      credentials: ChannelCredentials.insecure(),
      // This option is not strictly required. It is applied to avoid short idle timeouts
      // commonly seen on devices like Raspberry Pi when gRPC channels become temporarily idle.
      idleTimeout: Duration.zero,
    ),
  );

  static Future<T> _withClient<T>(
    Future<T> Function(EmergencyServiceClient stub) body,
  ) async {
    final ch = _channel();
    final stub = EmergencyServiceClient(ch);
    try {
      return await body(stub);
    } finally {
      await ch.shutdown().catchError((_) {});
    }
  }

  // --------------------------------------------------------------------------
  // Toggle control
  // --------------------------------------------------------------------------

  /// Set the toggle state. Throws on RPC failure.
  static Future<void> setToggleState(
    bool is_on, {
    Duration timeout = const Duration(seconds: 30),
  }) {
    return _withClient((stub) async {
      await stub.setToggleState(
        ToggleRequest()..isOn = is_on,
        options: CallOptions(timeout: timeout),
      );
    });
  }

  // --------------------------------------------------------------------------
  // Telemetry (toggle availability)
  // --------------------------------------------------------------------------

  /// Streams toggle availability updates. Cancelling the returned stream will
  /// also shut down the underlying channel.
  static Stream<eavl.ToggleAvailability> pollToggleAvailability({
    int interval_ms = 100,
    CallOptions? options,
  }) async* {
    final ch = _channel();
    final client = eavl.EmergencyTelemetryClient(ch);
    final req = eavl.PollOptions()..intervalMs = interval_ms;
    final resp = client.pollToggleAvailability(req, options: options);

    try {
      await for (final msg in resp) {
        yield msg;
      }
    } finally {
      // If the consumer cancels the stream, this finally block still runs.
      // Some grpc versions may expose resp.cancel(); we rely on channel shutdown.
      await ch.shutdown().catchError((_) {});
    }
  }
}
