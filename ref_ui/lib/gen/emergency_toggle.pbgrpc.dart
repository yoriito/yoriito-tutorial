//
//  Generated code. Do not modify.
//  source: emergency_toggle.proto
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

import 'emergency_toggle.pb.dart' as $1;

export 'emergency_toggle.pb.dart';

@$pb.GrpcServiceName('emrgency.EmergencyService')
class EmergencyServiceClient extends $grpc.Client {
  static final _$setToggleState = $grpc.ClientMethod<$1.ToggleRequest, $1.ToggleResponse>(
      '/emrgency.EmergencyService/SetToggleState',
      ($1.ToggleRequest value) => value.writeToBuffer(),
      ($core.List<$core.int> value) => $1.ToggleResponse.fromBuffer(value));

  EmergencyServiceClient(super.channel, {super.options, super.interceptors});

  $grpc.ResponseFuture<$1.ToggleResponse> setToggleState($1.ToggleRequest request, {$grpc.CallOptions? options}) {
    return $createUnaryCall(_$setToggleState, request, options: options);
  }
}

@$pb.GrpcServiceName('emrgency.EmergencyService')
abstract class EmergencyServiceBase extends $grpc.Service {
  $core.String get $name => 'emrgency.EmergencyService';

  EmergencyServiceBase() {
    $addMethod($grpc.ServiceMethod<$1.ToggleRequest, $1.ToggleResponse>(
        'SetToggleState',
        setToggleState_Pre,
        false,
        false,
        ($core.List<$core.int> value) => $1.ToggleRequest.fromBuffer(value),
        ($1.ToggleResponse value) => value.writeToBuffer()));
  }

  $async.Future<$1.ToggleResponse> setToggleState_Pre($grpc.ServiceCall $call, $async.Future<$1.ToggleRequest> $request) async {
    return setToggleState($call, await $request);
  }

  $async.Future<$1.ToggleResponse> setToggleState($grpc.ServiceCall call, $1.ToggleRequest request);
}
