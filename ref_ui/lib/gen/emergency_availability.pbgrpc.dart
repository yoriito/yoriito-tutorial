//
//  Generated code. Do not modify.
//  source: emergency_availability.proto
//
// @dart = 3.3

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:async' as $async;
import 'dart:core' as $core;

import 'package:grpc/service_api.dart' as $grpc;
import 'package:protobuf/protobuf.dart' as $pb;

import 'emergency_availability.pb.dart' as $0;

export 'emergency_availability.pb.dart';

@$pb.GrpcServiceName('emrgency.EmergencyTelemetry')
class EmergencyTelemetryClient extends $grpc.Client {
  static final _$pollToggleAvailability = $grpc.ClientMethod<$0.PollOptions, $0.ToggleAvailability>(
      '/emrgency.EmergencyTelemetry/PollToggleAvailability',
      ($0.PollOptions value) => value.writeToBuffer(),
      ($core.List<$core.int> value) => $0.ToggleAvailability.fromBuffer(value));

  EmergencyTelemetryClient(super.channel, {super.options, super.interceptors});

  $grpc.ResponseStream<$0.ToggleAvailability> pollToggleAvailability($0.PollOptions request, {$grpc.CallOptions? options}) {
    return $createStreamingCall(_$pollToggleAvailability, $async.Stream.fromIterable([request]), options: options);
  }
}

@$pb.GrpcServiceName('emrgency.EmergencyTelemetry')
abstract class EmergencyTelemetryServiceBase extends $grpc.Service {
  $core.String get $name => 'emrgency.EmergencyTelemetry';

  EmergencyTelemetryServiceBase() {
    $addMethod($grpc.ServiceMethod<$0.PollOptions, $0.ToggleAvailability>(
        'PollToggleAvailability',
        pollToggleAvailability_Pre,
        false,
        true,
        ($core.List<$core.int> value) => $0.PollOptions.fromBuffer(value),
        ($0.ToggleAvailability value) => value.writeToBuffer()));
  }

  $async.Stream<$0.ToggleAvailability> pollToggleAvailability_Pre($grpc.ServiceCall $call, $async.Future<$0.PollOptions> $request) async* {
    yield* pollToggleAvailability($call, await $request);
  }

  $async.Stream<$0.ToggleAvailability> pollToggleAvailability($grpc.ServiceCall call, $0.PollOptions request);
}
