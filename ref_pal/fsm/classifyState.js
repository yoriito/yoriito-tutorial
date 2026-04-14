// fsm/classifyState.js
const { POS_CLOSED } = require('../config/constants');

function fsmState(target, cur) {
  if (target.kind === "doorTrunk") {
    const L = !!cur.isLocked;
    const O = !!cur.isOpen;
    const P = Number(cur.position) || POS_CLOSED;

    if (!L && !O && P === POS_CLOSED) return "S0"; // unlocked & closed
    if (!L && O && P > POS_CLOSED) return "S3";    // unlocked & open
    if (L && !O && P === POS_CLOSED) return "S4";  // locked & closed
    return "UNSTABLE";
  }

  if (target.kind === "window") {
    const O = !!cur.isOpen;
    const P = Number(cur.position) || POS_CLOSED;

    if (!O && P === POS_CLOSED) return "W0"; // closed
    if (O && P > POS_CLOSED) return "W1";    // open
    return "UNSTABLE";
  }

  if (target.kind === "mirror") {
    const L = !!cur.isLocked;
    const F = !!cur.isFolded;

    if (!L && !F) return "M0"; // unlocked & unfolded
    if (!L && F)  return "M1"; // unlocked & folded
    if (L && !F)  return "M2"; // locked & unfolded
    if (L && F)   return "M3"; // locked & folded
  }

  return "UNKNOWN";
}

module.exports = {
  fsmState,
};
