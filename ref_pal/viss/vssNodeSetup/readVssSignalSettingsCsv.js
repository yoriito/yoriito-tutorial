'use strict';

const fs = require('fs');

function parseBoolean(text) {
  const normalized = String(text || '').trim().toLowerCase();
  if (normalized === 'true' || normalized === '1') return true;
  return false;
}

function parseCsvLine(line) {
  return line.split(',').map((cell) => String(cell).trim());
}

function readVssSignalSettingsCsv(csvFilePath) {
  const csvText = fs.readFileSync(csvFilePath, 'utf-8');
  const lines = csvText.split(/\r?\n/);

  if (lines.length === 0) {
    return [];
  }

  const headerCells = parseCsvLine(lines[0]);
  const expectedHeader = [
    'base_path',
    'capability',
    'availability_sensor',
    'availability_actuator',
    'default_value_type',
    'default_value',
  ];

  const basePathIndex = headerCells.indexOf('base_path');
  const capabilityIndex = headerCells.indexOf('capability');
  const availabilitySensorIndex = headerCells.indexOf('availability_sensor');
  const availabilityActuatorIndex = headerCells.indexOf('availability_actuator');
  const defaultValueTypeIndex = headerCells.indexOf('default_value_type');
  const defaultValueIndex = headerCells.indexOf('default_value');

  if (
    basePathIndex < 0 ||
    capabilityIndex < 0 ||
    availabilitySensorIndex < 0 ||
    availabilityActuatorIndex < 0 ||
    defaultValueTypeIndex < 0 ||
    defaultValueIndex < 0
  ) {
    throw new Error(
      `CSV header must contain: ${expectedHeader.join(', ')} (csvFilePath=${csvFilePath})`
    );
  }

  const signalSettingsList = [];

  for (let lineIndex = 1; lineIndex < lines.length; lineIndex += 1) {
    const rawLine = String(lines[lineIndex] || '').trim();
    if (rawLine.length === 0) continue;
    if (rawLine.startsWith('#')) continue;

    const cells = parseCsvLine(rawLine);
    const basePath = String(cells[basePathIndex] || '').trim();
    if (basePath.length === 0) continue;

    const capability = parseBoolean(cells[capabilityIndex]);
    const availabilitySensor = parseBoolean(cells[availabilitySensorIndex]);
    const availabilityActuator = parseBoolean(cells[availabilityActuatorIndex]);

    const defaultValueType = String(cells[defaultValueTypeIndex] || '').trim();
    const defaultValue = String(cells[defaultValueIndex] || '').trim();

    signalSettingsList.push({
      basePath: basePath,
      capability: capability,
      availabilitySensor: availabilitySensor,
      availabilityActuator: availabilityActuator,
      defaultValueType: defaultValueType,
      defaultValue: defaultValue,
    });
  }

  return signalSettingsList;
}

module.exports = { readVssSignalSettingsCsv };
