// viss/vssNodeSetup/availabilitySetup.js
'use strict';

const { nowEpochSeconds, setCurrentAsync, buildSetCurrentRequest } = require('./vissGrpcHelpers');

const DEFAULT_ENTRIES_PER_REQUEST = 200;

function buildSetCurrentRequestMulti(entryList) {
  const dataList = [];

  for (const entry of entryList) {
    dataList.push({
      path: entry.pathString,
      dp: [entry.dataPointCurrent],
    });
  }

  return {
    data: {
      data: dataList,
    },
  };
}

function getNodePathFromSignalSetting(signalSetting) {
  if (signalSetting && typeof signalSetting.nodePath === 'string' && signalSetting.nodePath.length > 0) {
    return signalSetting.nodePath;
  }
  if (signalSetting && typeof signalSetting.basePath === 'string' && signalSetting.basePath.length > 0) {
    return signalSetting.basePath;
  }
  if (signalSetting && typeof signalSetting.path === 'string' && signalSetting.path.length > 0) {
    return signalSetting.path;
  }
  return '';
}

function snakeToCamelCase(snakeCaseText) {
  const text = String(snakeCaseText || '').trim();
  if (text.indexOf('_') < 0) return text;

  const parts = text.split('_');
  let camel = parts[0];
  for (let i = 1; i < parts.length; i += 1) {
    const part = parts[i];
    if (!part) continue;
    camel += part[0].toUpperCase() + part.slice(1);
  }
  return camel;
}

function parseBoolText(text) {
  const normalized = String(text || '').trim().toLowerCase();
  if (normalized === 'true' || normalized === '1') return true;
  return false;
}

function parseNumberTextAsInt(text) {
  const parsed = parseInt(String(text || '0'), 10);
  if (Number.isNaN(parsed)) return 0;
  return parsed;
}

function parseNumberTextAsFloat(text) {
  const parsed = parseFloat(String(text || '0'));
  if (Number.isNaN(parsed)) return 0;
  return parsed;
}

function parseJsonArrayText(text) {
  if (!text) return [];
  try {
    const parsed = JSON.parse(text);
    if (Array.isArray(parsed)) return parsed;
    return [];
  } catch (_error) {
    return [];
  }
}

function parseDefaultValueByType(defaultValueType, defaultValueText) {
  const typeKey = String(defaultValueType || '').trim();
  if (!typeKey) return null;

  if (typeKey.endsWith('_array')) {
    return parseJsonArrayText(defaultValueText);
  }

  if (typeKey === 'bool') {
    return parseBoolText(defaultValueText);
  }

  if (
    typeKey === 'int8' ||
    typeKey === 'int16' ||
    typeKey === 'int32' ||
    typeKey === 'int64' ||
    typeKey === 'uint8' ||
    typeKey === 'uint16' ||
    typeKey === 'uint32' ||
    typeKey === 'uint64'
  ) {
    return parseNumberTextAsInt(defaultValueText);
  }

  if (typeKey === 'float' || typeKey === 'double') {
    return parseNumberTextAsFloat(defaultValueText);
  }

  if (typeKey === 'string') {
    return String(defaultValueText || '');
  }

  return null;
}

function buildValueObject(defaultValueType, defaultValueText) {
  const parsedValue = parseDefaultValueByType(defaultValueType, defaultValueText);
  if (parsedValue === null) return null;

  const grpcValueFieldKey = snakeToCamelCase(defaultValueType);
  const valueObject = {};
  valueObject[grpcValueFieldKey] = parsedValue;
  return valueObject;
}

function buildAvailabilityDataPoint(signalSetting, producedTimestampSeconds) {
  const availabilitySensorValue = signalSetting.availabilitySensor === true;
  const availabilityActuatorValue = signalSetting.availabilityActuator === true;

  const valueObject = buildValueObject(signalSetting.defaultValueType, signalSetting.defaultValue);

  const safeValueObject = valueObject || {};

  if (!valueObject) {
    console.log(
      '[vssNodeSetup] WARN: default value is missing/unknown:',
      getNodePathFromSignalSetting(signalSetting),
      'type=' + String(signalSetting.defaultValueType || ''),
      'value=' + String(signalSetting.defaultValue || '')
    );
  }

  return {
    value: safeValueObject,
    isAvailableSensor: availabilitySensorValue,
    isAvailableActuator: availabilityActuatorValue,
  };
}

function buildAvailabilityEntries(signalSettingsList, producedTimestampSeconds) {
  const entryList = [];

  for (const signalSetting of signalSettingsList) {
    const nodePath = getNodePathFromSignalSetting(signalSetting);
    if (!nodePath) continue;

    entryList.push({
      pathString: nodePath,
      dataPointCurrent: buildAvailabilityDataPoint(signalSetting, producedTimestampSeconds),
    });
  }

  return entryList;
}

async function setAvailabilityValues(vissClient, signalSettingsList, producedTimestampSeconds) {
  const entries = buildAvailabilityEntries(signalSettingsList, producedTimestampSeconds);

  if (entries.length === 0) {
    return;
  }

  const request = buildSetCurrentRequestMulti(entries);

  try {
    await setCurrentAsync(vissClient, request);
    console.log('[vssNodeSetup] Availability: SetCurrent OK. entries=', entries.length);
  } catch (error) {
    console.log(
      '[vssNodeSetup] Availability: SetCurrent NG. entries=',
      entries.length,
      error.message || error
    );
  }
}

async function setAvailabilityValuesChunked(
  vissClient,
  signalSettingsList,
  producedTimestampSeconds,
  entriesPerRequest
) {
  const entries = buildAvailabilityEntries(signalSettingsList, producedTimestampSeconds);

  if (entries.length === 0) {
    console.log('[vssNodeSetup] Availability: no entries. (skip)');
    return;
  }

  const perRequest =
    typeof entriesPerRequest === 'number' && entriesPerRequest > 0 ? entriesPerRequest : DEFAULT_ENTRIES_PER_REQUEST;

  if (perRequest >= entries.length) {
    await setAvailabilityValues(vissClient, signalSettingsList, producedTimestampSeconds);
    return;
  }

  console.log('[vssNodeSetup] Availability: SetCurrent start. entries=', entries.length, 'entriesPerRequest=', perRequest);

  for (let startIndex = 0; startIndex < entries.length; startIndex += 1) {
    const endIndex = Math.min(startIndex + perRequest, entries.length);
    const chunkEntries = entries.slice(startIndex, endIndex);

    const request = buildSetCurrentRequestMulti(chunkEntries);

    try {
      await setCurrentAsync(vissClient, request);
      console.log(
        '[vssNodeSetup] Availability: SetCurrent OK (chunk):',
        startIndex,
        '-',
        endIndex - 1,
        'entries=',
        chunkEntries.length
      );
    } catch (error) {
      console.log(
        '[vssNodeSetup] Availability: SetCurrent NG (chunk):',
        startIndex,
        '-',
        endIndex - 1,
        'entries=',
        chunkEntries.length,
        error.message || error
      );
    }

    startIndex = endIndex - 1;
  }
}

async function setAvailabilityValuesSequential(vissClient, signalSettingsList, producedTimestampSeconds) {
  const entries = buildAvailabilityEntries(signalSettingsList, producedTimestampSeconds);

  if (entries.length === 0) {
    console.log('[vssNodeSetup] Availability(sequential): no rows. (skip)');
    return;
  }

  for (const entry of entries) {
    const request = buildSetCurrentRequest(entry.pathString, entry.dataPointCurrent);

    try {
      await setCurrentAsync(vissClient, request);
      console.log('[vssNodeSetup] Availability: SetCurrent OK (sequential):', entry.pathString);
    } catch (error) {
      console.log(
        '[vssNodeSetup] Availability: SetCurrent NG (sequential):',
        entry.pathString,
        error.message || error
      );
    }
  }
}

async function runAvailabilityPhase(vissClient, signalSettingsList) {
  const timestampSeconds = nowEpochSeconds();
  await setAvailabilityValuesChunked(vissClient, signalSettingsList, timestampSeconds, DEFAULT_ENTRIES_PER_REQUEST);
}

module.exports = {
  runAvailabilityPhase,
};
