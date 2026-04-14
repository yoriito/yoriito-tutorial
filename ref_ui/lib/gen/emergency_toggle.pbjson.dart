//
//  Generated code. Do not modify.
//  source: emergency_toggle.proto
//
// @dart = 3.3

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use toggleRequestDescriptor instead')
const ToggleRequest$json = {
  '1': 'ToggleRequest',
  '2': [
    {'1': 'is_on', '3': 1, '4': 1, '5': 8, '10': 'isOn'},
  ],
};

/// Descriptor for `ToggleRequest`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List toggleRequestDescriptor = $convert.base64Decode(
    'Cg1Ub2dnbGVSZXF1ZXN0EhMKBWlzX29uGAEgASgIUgRpc09u');

@$core.Deprecated('Use toggleResponseDescriptor instead')
const ToggleResponse$json = {
  '1': 'ToggleResponse',
  '2': [
    {'1': 'empty', '3': 1, '4': 1, '5': 11, '6': '.google.protobuf.Empty', '10': 'empty'},
  ],
};

/// Descriptor for `ToggleResponse`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List toggleResponseDescriptor = $convert.base64Decode(
    'Cg5Ub2dnbGVSZXNwb25zZRIsCgVlbXB0eRgBIAEoCzIWLmdvb2dsZS5wcm90b2J1Zi5FbXB0eV'
    'IFZW1wdHk=');

