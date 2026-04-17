// viss/subscriptions.js
const { buildUnityMsgFromDp } = require('../utils/unityMsg');
const { handleEvent } = require('../fsm/fsmHandler');

/**
 * Start SubscribeTarget
 * - If handled by FSM, finish here
 * - Otherwise, send to Unity & SetCurrent as usual
 */
function subscribeTarget({
  client,
  targetPath,
  mainWindow,
  propertyMap,
  setCurrent,
  setCurrentSingleUsingTemplate,
}) {
  const subscribeRequest = { path: targetPath };
  const call = client.SubscribeTarget(subscribeRequest);

  const sendToUnity = (msg) => {
    if (!mainWindow || !mainWindow.webContents) {
      console.warn('[viss] sendToUnity called before window is ready');
      return;
    }
    mainWindow.webContents.send('send-to-unity', msg);
  };

  call.on('data', (response) => {
    if ('success' in response) {
      console.log(response.success);
      return;
    }

    if (!('event' in response)) return;

    const event = response.event.data.data[0];
    const dp0 = event.dp[0];

    console.log('[subscribeTarget] event path =', event.path, 'dp =', dp0);

    const handledByFsm = handleEvent({
      client,
      event,
      path: event.path,
      dp0,
      templateDp: dp0,
      propertyMap,
      sendToUnity,
      setCurrentSingleUsingTemplate,
    });

    if (handledByFsm) {
      return; // Finish here for FSM-managed targets
    }

    // ===== Existing behavior (unchanged) =====
    const message = buildUnityMsgFromDp(propertyMap, event.path, dp0);
    if (message == null) {
      console.warn('[subscribeTarget] message is null for path', event.path);
      return;
    }

    console.log('[subscribeTarget] send-to-unity:', message);
    sendToUnity(message);
    setCurrent(client, event);
  });

  call.on('error', (err) => {
    console.error('gRPC SubscribeTarget error:', err);
  });
  call.on('end', () => {
    console.log('gRPC SubscribeTarget stream ended');
  });

  return call;
}

module.exports = { subscribeTarget };
