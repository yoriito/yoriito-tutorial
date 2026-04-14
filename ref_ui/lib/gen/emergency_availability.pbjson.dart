//
//  Generated code. Do not modify.
//  source: emergency_availability.proto
//
// @dart = 3.3

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use pollOptionsDescriptor instead')
const PollOptions$json = {
  '1': 'PollOptions',
  '2': [
    {'1': 'interval_ms', '3': 1, '4': 1, '5': 13, '10': 'intervalMs'},
  ],
};

/// Descriptor for `PollOptions`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List pollOptionsDescriptor = $convert.base64Decode(
    'CgtQb2xsT3B0aW9ucxIfCgtpbnRlcnZhbF9tcxgBIAEoDVIKaW50ZXJ2YWxNcw==');

@$core.Deprecated('Use toggleAvailabilityDescriptor instead')
const ToggleAvailability$json = {
  '1': 'ToggleAvailability',
  '2': [
    {'1': 'is_available', '3': 1, '4': 1, '5': 8, '10': 'isAvailable'},
    {'1': 'message', '3': 2, '4': 1, '5': 9, '10': 'message'},
  ],
};

/// Descriptor for `ToggleAvailability`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List toggleAvailabilityDescriptor = $convert.base64Decode(
    'ChJUb2dnbGVBdmFpbGFiaWxpdHkSIQoMaXNfYXZhaWxhYmxlGAEgASgIUgtpc0F2YWlsYWJsZR'
    'IYCgdtZXNzYWdlGAIgASgJUgdtZXNzYWdl');

