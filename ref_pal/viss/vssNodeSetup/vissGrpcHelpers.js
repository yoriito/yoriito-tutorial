// viss/vssNodeSetup/vissGrpcHelpers.js
'use strict';

function nowEpochSeconds() {
  return Math.floor(Date.now() / 1000);
}

function sleepMilliseconds(milliseconds) {
  return new Promise((resolve) => setTimeout(resolve, milliseconds));
}

function getSetCurrentResponseError(response) {
  if (!response || !response.error) {
    return null;
  }

  const error = response.error;
  const description = error.description || 'unknown error';
  return new Error(
    `VISS SetCurrent failed: number=${error.number}, reason=${error.reason}, description=${description}`
  );
}

function setCurrentAsync(vissClient, setCurrentRequest) {
  return new Promise((resolve, reject) => {
    vissClient.SetCurrent(setCurrentRequest, (error, response) => {
      if (error) {
        reject(error);
        return;
      }

      const responseError = getSetCurrentResponseError(response);
      if (responseError) {
        reject(responseError);
        return;
      }

      resolve(response);
    });
  });
}

function deleteAsync(vissClient, deleteRequest) {
  return new Promise((resolve, reject) => {
    vissClient.Delete(deleteRequest, (error, response) => {
      if (error) {
        reject(error);
        return;
      }
      resolve(response);
    });
  });
}

function buildSetCurrentRequest(pathString, dataPointCurrent) {
  return {
    data: {
      data: [
        {
          path: pathString,
          dp: [dataPointCurrent],
        },
      ],
    },
  };
}

/**
 * SetCurrent of multi path 
 *
 * entries: [{ pathString: string, dataPointCurrent: object }, ...]
 */
function buildSetCurrentRequestMulti(entries) {
  const requestDataList = [];

  for (const entry of entries) {
    requestDataList.push({
      path: entry.pathString,
      dp: [entry.dataPointCurrent],
    });
  }

  return {
    data: {
      data: requestDataList,
    },
  };
}

module.exports = {
  getSetCurrentResponseError,
  nowEpochSeconds,
  sleepMilliseconds,
  setCurrentAsync,
  deleteAsync,
  buildSetCurrentRequest,
  buildSetCurrentRequestMulti,
};
