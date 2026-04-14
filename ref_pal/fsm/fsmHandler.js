// fsm/fsmHandler.js
const { TARGET_BY_ID, PATH_TO_TARGET_ID, targetCur } = require('../config/fsmTargets');
const { convertValueToString } = require('../utils/valueConversion');

const { fsmState } = require('./classifyState');
const { classifyEvent } = require('./classifyEvent');
const { reduce } = require('./nextState');
const { applyTargetNext } = require('./applyNext');

function handleEvent({ client, path, dp0, sendToUnity, setCurrentSingleUsingTemplate }) {
  try {
    const targetId = PATH_TO_TARGET_ID.get(path);
    if (!targetId) return false;

    const target = TARGET_BY_ID.get(targetId);
    const cur = targetCur.get(targetId);

    const rawValue = convertValueToString(dp0.value);
    const ev = classifyEvent(target, path, rawValue);

    if (!ev) {
      console.warn(`[FSM:${targetId}] Unknown event for path`, path, 'raw=', rawValue);
      return true;
    }

    const next = reduce(target, cur, ev);

    console.log(
      `[FSM:${targetId}] kind=${target.kind}`,
      'cur=', cur,
      'state=', fsmState(target, cur),
      'ev=', ev,
      'next=', next
    );

    if (next) {
      applyTargetNext(client, target, next, dp0, sendToUnity, setCurrentSingleUsingTemplate);
    } else {
      console.log(`[FSM:${targetId}] rejected or no-op. keep=`, cur);
    }

    return true;
  } catch (e) {
    console.error('[FSM] handleEvent failed:', e);
    return false;
  }
}

module.exports = {
  handleEvent,
};
