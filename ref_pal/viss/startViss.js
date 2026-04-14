// viss/startViss.js
'use strict';

const path = require('path');
const grpc = require('@grpc/grpc-js');
const protoLoader = require('@grpc/proto-loader');

const { propertyMap } = require('../config/vssToUnityMap');
const { subscribeTarget } = require('./subscriptions');
const { DEFAULT_VISS_SERVER_ADDRESS, DEFAULT_CSV_FILE_PATH } = require('../config/constants');
const { readVssSignalSettingsCsv } = require('./vssNodeSetup/readVssSignalSettingsCsv');

function connectVissServer({ serverAddress }) {
  const baseDir = path.join(__dirname, '..');
  const protoDir = path.join(baseDir, 'proto');
  const protoPath = path.join(protoDir, 'yoriito/viss/v1/producer.proto');

  const options = { includeDirs: [protoDir] };
  const packageDef = protoLoader.loadSync(protoPath, options);
  const grpcObject = grpc.loadPackageDefinition(packageDef);

  const VISSClient = grpcObject.yoriito.viss.v1.producer.VISS;
  // NOTE:
  // This client uses grpc.credentials.createInsecure(), which is only appropriate
  // for localhost / development usage. If you need to connect to a non-local
  // address or use this in production, configure TLS and use secure credentials
  // instead of createInsecure().
  return new VISSClient(serverAddress, grpc.credentials.createInsecure());
}

function buildAvailabilityMapByPath(signalSettingsList) {
  const availabilityMapByPath = new Map();

  for (let index = 0; index < signalSettingsList.length; index += 1) {
    const signalSetting = signalSettingsList[index];
    availabilityMapByPath.set(signalSetting.basePath, {
      availabilitySensor: signalSetting.availabilitySensor,
      availabilityActuator: signalSetting.availabilityActuator,
    });
  }

  return availabilityMapByPath;
}

function applyAvailabilityFromCsvToDataPoint(dataPoint, pathStr, availabilityMapByPath) {
  const availability = availabilityMapByPath.get(pathStr);
  if (!availability) {
    return dataPoint;
  }

  return {
    ...dataPoint,
    isAvailableSensor: availability.availabilitySensor,
    isAvailableActuator: availability.availabilityActuator,
  };
}

function setCurrentSingleUsingTemplate(client, pathStr, templateDp, valueObj, availabilityMapByPath) {
  // Keep templateDp metadata (e.g., timestamp) and replace only value
  const dataPointBase = { ...templateDp, value: valueObj };
  const dataPointWithAvailability = applyAvailabilityFromCsvToDataPoint(
    dataPointBase,
    pathStr,
    availabilityMapByPath
  );

  const setRequest = {
    data: {
      data: [
        {
          path: pathStr,
          dp: [dataPointWithAvailability],
        },
      ],
    },
  };

  client.SetCurrent(setRequest, function (err) {
    if (err) {
      console.error('[setCurrentSingle] SetCurrent error:', err, 'path=', pathStr);
      return;
    }
    console.log('[setCurrentSingle] SetCurrent OK for path', pathStr);
  });
}

function setCurrent(client, dataPackage, availabilityMapByPath) {
  if (!dataPackage || !dataPackage.dp || dataPackage.dp.length === 0) {
    console.warn('[setCurrent] DataPackage has no DataPoint');
    return;
  }

  const dataPointList = dataPackage.dp;
  const dataPointListWithAvailability = [];

  for (let index = 0; index < dataPointList.length; index += 1) {
    const dataPoint = dataPointList[index];
    const updatedDataPoint = applyAvailabilityFromCsvToDataPoint(
      dataPoint,
      dataPackage.path,
      availabilityMapByPath
    );
    dataPointListWithAvailability.push(updatedDataPoint);
  }

  const setRequest = {
    data: {
      data: [
        {
          path: dataPackage.path,
          dp: dataPointListWithAvailability,
        },
      ],
    },
  };

  client.SetCurrent(setRequest, function (err) {
    if (err) {
      console.error('[setCurrent] SetCurrent error:', err);
      return;
    }
    console.log('[setCurrent] SetCurrent OK for path', dataPackage.path);
  });
}

function startViss({ mainWindow, serverAddress = DEFAULT_VISS_SERVER_ADDRESS } = {}) {
  const client = connectVissServer({ serverAddress });

  const baseDir = path.join(__dirname, '..');
  const csvFilePath = path.resolve(baseDir, DEFAULT_CSV_FILE_PATH);

  let availabilityMapByPath = new Map();
  try {
    const signalSettingsList = readVssSignalSettingsCsv(csvFilePath);
    availabilityMapByPath = buildAvailabilityMapByPath(signalSettingsList);
    console.log('[startViss] CSV loaded:', csvFilePath, 'rows=', signalSettingsList.length);
  } catch (error) {
    console.log('[startViss] CSV load failed (skip availability merge):', error.message || error);
  }

  function setCurrentWithCsvAvailability(vissClient, dataPackage) {
    setCurrent(vissClient, dataPackage, availabilityMapByPath);
  }

  function setCurrentSingleUsingTemplateWithCsvAvailability(vissClient, pathStr, templateDp, valueObj) {
    setCurrentSingleUsingTemplate(vissClient, pathStr, templateDp, valueObj, availabilityMapByPath);
  }

  Object.keys(propertyMap).forEach((key) => {
    console.log('-----------------------------------------');
    console.log(key);
    console.log('-----------------------------------------');
    subscribeTarget({
      client: client,
      targetPath: key,
      mainWindow: mainWindow,
      propertyMap: propertyMap,
      setCurrent: setCurrentWithCsvAvailability,
      setCurrentSingleUsingTemplate: setCurrentSingleUsingTemplateWithCsvAvailability,
    });
  });

  return { client };
}

module.exports = { startViss, connectVissServer };
