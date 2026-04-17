'use strict';

const path = require('path');

const { connectVissServer } = require('../startViss');
const { DEFAULT_VISS_SERVER_ADDRESS, DEFAULT_CSV_FILE_PATH } = require('../../config/constants');

const { readVssSignalSettingsCsv } = require('./readVssSignalSettingsCsv');

const { runDeletePhase } = require('./deleteSetup');
const { runCapabilityPhase } = require('./capabilitySetup');
const { runAvailabilityPhase } = require('./availabilitySetup');

function getDefaultCsvFilePath() {
  return path.join(__dirname, DEFAULT_CSV_FILE_PATH);
}

async function runVssNodeSetup(options) {
  const optionObject = options || {};
  const serverAddress =
    optionObject.serverAddress || process.env.VISS_ADDR || DEFAULT_VISS_SERVER_ADDRESS;
  const csvFilePath = optionObject.csvFilePath || getDefaultCsvFilePath();

  let vissClient = null;

  try {
    const signalSettingsList = readVssSignalSettingsCsv(csvFilePath);
    console.log('[vssNodeSetup] CSV loaded:', csvFilePath, 'rows=', signalSettingsList.length);

    vissClient = connectVissServer({ serverAddress: serverAddress });

    // ---- Phase 1: Delete node (noop)----
    await runDeletePhase(vissClient, signalSettingsList);

    // ---- Phase 2: Set capability at node (noop) ----
    await runCapabilityPhase(vissClient, signalSettingsList);

    // ---- Phase 3: Set availability at node (X_Availability_Sensor/Actuator) and data points ----
    await runAvailabilityPhase(vissClient, signalSettingsList);

    console.log('[vssNodeSetup] Completed.');
  } catch (error) {
    console.log('[vssNodeSetup] Unexpected error:', error.message || error);
  } finally {
    if (vissClient && typeof vissClient.close === 'function') {
      try {
        vissClient.close();
        console.log('[vssNodeSetup] Closed.');
      } catch (_error) {
      }
    }
  }
}

module.exports = { runVssNodeSetup };
