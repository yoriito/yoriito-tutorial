// fsm/applyNext.js
const { targetCur } = require('../config/fsmTargets');
const { propertyMap } = require('../config/vssToUnityMap');
const { buildUnityMsg } = require('../utils/unityMsg');

/**
 * Apply next state to:
 * - Unity (send-to-unity)
 * - VISS (SetCurrent)
 * - Local cache (targetCur)
 */
function applyTargetNext(client, target, next, templateDp, sendToUnity, setCurrentSingleUsingTemplate) {
  const cur = targetCur.get(target.id);
  if (!cur) {
    console.warn('[FSM] missing cur for target', target.id);
    return;
  }

  const updates = [];

  // Create diffs based on target.paths keys
  for (const key of Object.keys(target.paths)) {
    const pathStr = target.paths[key];

    const curVal = cur[key];
    const nextVal = next[key];

    // Skip if next does not define the key (safety)
    if (typeof nextVal === "undefined") continue;

    // Skip if no change
    if (String(curVal) === String(nextVal)) continue;

    // Build SetCurrent value object
    let valueObj;

    // Booleans
    if (key === "isLocked" || key === "isOpen" || key === "isFolded") {
      valueObj = { bool: Boolean(nextVal) };
    }
    // Integers
    else if (key === "position") {
      valueObj = { int32: Number(nextVal) };
    } else {
      // Unexpected key (not used in current targets)
      continue;
    }

    updates.push({
      key,
      path: pathStr,
      raw: String(nextVal),
      value: valueObj,
    });
  }

  if (updates.length === 0) return;

  // Update cache first to keep FSM consistent
  targetCur.set(target.id, { ...cur, ...next });

  for (const u of updates) {
    // Apply per-target Unity forwarding skip rules
    if (!target.unity_skip_paths || !target.unity_skip_paths.has(u.path)) {
      const msg = buildUnityMsg(propertyMap, u.path, u.raw);
      if (msg) {
        console.log(`[FSM:${target.id}] send-to-unity:`, msg);
        sendToUnity(msg);
      }
    } else {
      console.log(`[FSM:${target.id}] skip send-to-unity for path:`, u.path, 'raw=', u.raw);
    }

    // Always update Current on VISS side
    setCurrentSingleUsingTemplate(client, u.path, templateDp, u.value);
  }
}

module.exports = {
  applyTargetNext,
};
