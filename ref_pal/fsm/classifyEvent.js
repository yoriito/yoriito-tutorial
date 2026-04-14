// fsm/classifyEvent.js
const { POS_CLOSED } = require('../config/constants');

function classifyEvent(target, pathStr, rawValue) {
  if (target.kind === "doorTrunk") {
    if (pathStr === target.paths.isLocked) {
      return rawValue === "true" ? { type: "LOCK" } : { type: "UNLOCK" };
    }
    if (pathStr === target.paths.isOpen) {
      return rawValue === "true" ? { type: "OPEN_FULL" } : { type: "CLOSE" };
    }
    if (pathStr === target.paths.position) {
      const p = Number(rawValue) || POS_CLOSED;
      return p > POS_CLOSED ? { type: "SET_POS", p } : { type: "CLOSE" };
    }
    return null;
  }

  if (target.kind === "window") {
    if (pathStr === target.paths.isOpen) {
      return rawValue === "true" ? { type: "OPEN_FULL" } : { type: "CLOSE" };
    }
    if (pathStr === target.paths.position) {
      const p = Number(rawValue) || POS_CLOSED;
      return p > POS_CLOSED ? { type: "SET_POS", p } : { type: "CLOSE" };
    }
    return null;
  }

  if (target.kind === "mirror") {
    if (pathStr === target.paths.isLocked) {
      return rawValue === "true" ? { type: "LOCK" } : { type: "UNLOCK" };
    }
    if (pathStr === target.paths.isFolded) {
      return rawValue === "true" ? { type: "FOLD" } : { type: "UNFOLD" };
    }
    return null;
  }

  return null;
}

module.exports = {
  classifyEvent,
};
