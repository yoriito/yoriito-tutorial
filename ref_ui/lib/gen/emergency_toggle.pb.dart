//
//  Generated code. Do not modify.
//  source: emergency_toggle.proto
//
// @dart = 3.3

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

import 'google/protobuf/empty.pb.dart' as $2;

export 'package:protobuf/protobuf.dart' show GeneratedMessageGenericExtensions;

/// Toggle state request.
class ToggleRequest extends $pb.GeneratedMessage {
  factory ToggleRequest({
    $core.bool? isOn,
  }) {
    final $result = create();
    if (isOn != null) {
      $result.isOn = isOn;
    }
    return $result;
  }
  ToggleRequest._() : super();
  factory ToggleRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory ToggleRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'ToggleRequest', package: const $pb.PackageName(_omitMessageNames ? '' : 'emrgency'), createEmptyInstance: create)
    ..aOB(1, _omitFieldNames ? '' : 'isOn')
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  ToggleRequest clone() => ToggleRequest()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  ToggleRequest copyWith(void Function(ToggleRequest) updates) => super.copyWith((message) => updates(message as ToggleRequest)) as ToggleRequest;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static ToggleRequest create() => ToggleRequest._();
  ToggleRequest createEmptyInstance() => create();
  static $pb.PbList<ToggleRequest> createRepeated() => $pb.PbList<ToggleRequest>();
  @$core.pragma('dart2js:noInline')
  static ToggleRequest getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<ToggleRequest>(create);
  static ToggleRequest? _defaultInstance;

  @$pb.TagNumber(1)
  $core.bool get isOn => $_getBF(0);
  @$pb.TagNumber(1)
  set isOn($core.bool v) { $_setBool(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasIsOn() => $_has(0);
  @$pb.TagNumber(1)
  void clearIsOn() => $_clearField(1);
}

/// empty response.
class ToggleResponse extends $pb.GeneratedMessage {
  factory ToggleResponse({
    $2.Empty? empty,
  }) {
    final $result = create();
    if (empty != null) {
      $result.empty = empty;
    }
    return $result;
  }
  ToggleResponse._() : super();
  factory ToggleResponse.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory ToggleResponse.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'ToggleResponse', package: const $pb.PackageName(_omitMessageNames ? '' : 'emrgency'), createEmptyInstance: create)
    ..aOM<$2.Empty>(1, _omitFieldNames ? '' : 'empty', subBuilder: $2.Empty.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  ToggleResponse clone() => ToggleResponse()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  ToggleResponse copyWith(void Function(ToggleResponse) updates) => super.copyWith((message) => updates(message as ToggleResponse)) as ToggleResponse;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static ToggleResponse create() => ToggleResponse._();
  ToggleResponse createEmptyInstance() => create();
  static $pb.PbList<ToggleResponse> createRepeated() => $pb.PbList<ToggleResponse>();
  @$core.pragma('dart2js:noInline')
  static ToggleResponse getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<ToggleResponse>(create);
  static ToggleResponse? _defaultInstance;

  @$pb.TagNumber(1)
  $2.Empty get empty => $_getN(0);
  @$pb.TagNumber(1)
  set empty($2.Empty v) { $_setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasEmpty() => $_has(0);
  @$pb.TagNumber(1)
  void clearEmpty() => $_clearField(1);
  @$pb.TagNumber(1)
  $2.Empty ensureEmpty() => $_ensure(0);
}


const _omitFieldNames = $core.bool.fromEnvironment('protobuf.omit_field_names');
const _omitMessageNames = $core.bool.fromEnvironment('protobuf.omit_message_names');
