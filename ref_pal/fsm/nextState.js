// fsm/nextState.js
const { POS_CLOSED, POS_FULL } = require('../config/constants');
const { fsmState } = require('./classifyState');

// =================================================
// State definitions (Sx/Wx/Mx): see README
// =================================================

function reduce(target, cur, ev) {
  if (target.kind === "doorTrunk") {
    let state = fsmState(target, cur);

    // Minimal implementation: if unstable, fall back to a conservative stable state
    // using a fixed priority order (locked → open → closed).
    if (state === "UNSTABLE") {
      if (cur.isLocked) state = "S4";
      else if (cur.isOpen || (Number(cur.position) || POS_CLOSED) > POS_CLOSED) state = "S3";
      else state = "S0";
    }

    switch (state) {
      case "S0": { // unlocked & closed
        if (ev.type === "LOCK") return { isLocked: true, isOpen: false, position: POS_CLOSED };
        if (ev.type === "OPEN_FULL") return { isLocked: false, isOpen: true, position: POS_FULL };
        if (ev.type === "SET_POS") return { isLocked: false, isOpen: true, position: ev.p };
        return null;
      }

      case "S3": { // unlocked & open
        if (ev.type === "LOCK") return null; // reject
        if (ev.type === "CLOSE") return { isLocked: false, isOpen: false, position: POS_CLOSED };
        if (ev.type === "OPEN_FULL") return { isLocked: false, isOpen: true, position: POS_FULL };
        if (ev.type === "SET_POS") return { isLocked: false, isOpen: true, position: ev.p };
        return null;
      }

      case "S4": { // locked & closed
        if (ev.type === "UNLOCK") return { isLocked: false, isOpen: false, position: POS_CLOSED };
        return null;
      }

      default:
        return null;
    }
  }

  if (target.kind === "window") {
    let state = fsmState(target, cur);

    // Minimal implementation: if unstable, fall back to a stable state by prioritizing "open" (W1) over "closed" (W0)
    if (state === "UNSTABLE") {
      if (cur.isOpen || (Number(cur.position) || POS_CLOSED) > POS_CLOSED) state = "W1";
      else state = "W0";
    }

    switch (state) {
      case "W0": { // closed
        if (ev.type === "OPEN_FULL") return { isOpen: true, position: POS_FULL };
        if (ev.type === "SET_POS") return { isOpen: true, position: ev.p };
        return null;
      }

      case "W1": { // open
        if (ev.type === "CLOSE") return { isOpen: false, position: POS_CLOSED };
        if (ev.type === "OPEN_FULL") return { isOpen: true, position: POS_FULL };
        if (ev.type === "SET_POS") return { isOpen: true, position: ev.p };
        return null;
      }

      default:
        return null;
    }
  }

  if (target.kind === "mirror") {
    let state = fsmState(target, cur);

    // If unstable, determine state based on current flags
    if (state === "UNSTABLE") {
      state = (cur.isLocked ? (cur.isFolded ? "M3" : "M2") : (cur.isFolded ? "M1" : "M0"));
    }

    switch (state) {
      case "M0": { // unlocked & unfolded
        if (ev.type === "LOCK") return { isLocked: true, isFolded: false }; // -> M2
        if (ev.type === "FOLD") return { isLocked: false, isFolded: true }; // -> M1
        return null; // UNLOCK/UNFOLD are no-ops here
      }

      case "M1": { // unlocked & folded
        if (ev.type === "LOCK") return { isLocked: true, isFolded: true }; // -> M3
        if (ev.type === "UNFOLD") return { isLocked: false, isFolded: false }; // -> M0
        return null;
      }

      case "M2": { // locked & unfolded
        if (ev.type === "UNLOCK") return { isLocked: false, isFolded: false }; // -> M0
        // While locked, reject fold/unfold commands (keep current)
        return null;
      }

      case "M3": { // locked & folded
        if (ev.type === "UNLOCK") return { isLocked: false, isFolded: true }; // -> M1
        // While locked, reject fold/unfold commands (keep current)
        return null;
      }

      default:
        return null;
    }
  }

  return null;
}

module.exports = {
  reduce,
};
