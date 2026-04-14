// utils/unityMsg.js
'use strict';

const { convertValueToString } = require('./valueConversion');

function buildUnityMsg(propertyMap, pathStr, rawValue) {
  const entry = propertyMap[pathStr];
  if (!entry) {
    console.warn("[UnityMsg] Unknown VSS path:", pathStr);
    return null;
  }

  const name = entry.unity_path;

  let action;
  let options;

  if (entry.action_map) {
    action = entry.action_map[rawValue] ?? rawValue;
    options = entry.option_name ?? rawValue;
  } else if (entry.action_name) {
    action = entry.action_name;
    options = rawValue;
  } else {
    action = rawValue;
    options = entry.option_name;
    if (options === null) options = rawValue;
  }

  const msg = { name, action, options };
  console.log('[UnityMsg] path=', pathStr, 'raw=', rawValue, 'msg=', msg);

  return JSON.stringify(msg);
}

// convert dp.value to string and call buildUnityMsg
function buildUnityMsgFromDp(propertyMap, pathStr, dp) {
  const rawValue = convertValueToString(dp.value);
  return buildUnityMsg(propertyMap, pathStr, rawValue);
}

module.exports = {
  buildUnityMsg,
  buildUnityMsgFromDp,
};
