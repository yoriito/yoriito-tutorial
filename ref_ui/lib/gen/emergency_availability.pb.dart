//
//  Generated code. Do not modify.
//  source: emergency_availability.proto
//
// @dart = 3.3

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

export 'package:protobuf/protobuf.dart' show GeneratedMessageGenericExtensions;

/// Poll Option Request.
class PollOptions extends $pb.GeneratedMessage {
  factory PollOptions({
    $core.int? intervalMs,
  }) {
    final $result = create();
    if (intervalMs != null) {
      $result.intervalMs = intervalMs;
    }
    return $result;
  }
  PollOptions._() : super();
  factory PollOptions.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory PollOptions.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'PollOptions', package: const $pb.PackageName(_omitMessageNames ? '' : 'emrgency'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'intervalMs', $pb.PbFieldType.OU3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  PollOptions clone() => PollOptions()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  PollOptions copyWith(void Function(PollOptions) updates) => super.copyWith((message) => updates(message as PollOptions)) as PollOptions;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static PollOptions create() => PollOptions._();
  PollOptions createEmptyInstance() => create();
  static $pb.PbList<PollOptions> createRepeated() => $pb.PbList<PollOptions>();
  @$core.pragma('dart2js:noInline')
  static PollOptions getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<PollOptions>(create);
  static PollOptions? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get intervalMs => $_getIZ(0);
  @$pb.TagNumber(1)
  set intervalMs($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasIntervalMs() => $_has(0);
  @$pb.TagNumber(1)
  void clearIntervalMs() => $_clearField(1);
}

/// Toggle Availability Response.
class ToggleAvailability extends $pb.GeneratedMessage {
  factory ToggleAvailability({
    $core.bool? isAvailable,
    $core.String? message,
  }) {
    final $result = create();
    if (isAvailable != null) {
      $result.isAvailable = isAvailable;
    }
    if (message != null) {
      $result.message = message;
    }
    return $result;
  }
  ToggleAvailability._() : super();
  factory ToggleAvailability.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory ToggleAvailability.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'ToggleAvailability', package: const $pb.PackageName(_omitMessageNames ? '' : 'emrgency'), createEmptyInstance: create)
    ..aOB(1, _omitFieldNames ? '' : 'isAvailable')
    ..aOS(2, _omitFieldNames ? '' : 'message')
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  ToggleAvailability clone() => ToggleAvailability()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  ToggleAvailability copyWith(void Function(ToggleAvailability) updates) => super.copyWith((message) => updates(message as ToggleAvailability)) as ToggleAvailability;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static ToggleAvailability create() => ToggleAvailability._();
  ToggleAvailability createEmptyInstance() => create();
  static $pb.PbList<ToggleAvailability> createRepeated() => $pb.PbList<ToggleAvailability>();
  @$core.pragma('dart2js:noInline')
  static ToggleAvailability getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<ToggleAvailability>(create);
  static ToggleAvailability? _defaultInstance;

  @$pb.TagNumber(1)
  $core.bool get isAvailable => $_getBF(0);
  @$pb.TagNumber(1)
  set isAvailable($core.bool v) { $_setBool(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasIsAvailable() => $_has(0);
  @$pb.TagNumber(1)
  void clearIsAvailable() => $_clearField(1);

  @$pb.TagNumber(2)
  $core.String get message => $_getSZ(1);
  @$pb.TagNumber(2)
  set message($core.String v) { $_setString(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasMessage() => $_has(1);
  @$pb.TagNumber(2)
  void clearMessage() => $_clearField(2);
}


const _omitFieldNames = $core.bool.fromEnvironment('protobuf.omit_field_names');
const _omitMessageNames = $core.bool.fromEnvironment('protobuf.omit_message_names');
