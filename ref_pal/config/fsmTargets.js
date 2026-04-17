// config/fsmTargets.js
'use strict';

const { POS_CLOSED } = require('./constants');

// =======================================================
// FSM Targets (Door/Trunk + Window + Mirror)
// =======================================================
const TARGETS = [
  // --- Door Row1 ---
  {
    id: "door1",
    kind: "doorTrunk",
    entity: "row1_door1",
    paths: {
      isLocked: "Vehicle.Cabin.Door.Row1.DriverSide.IsLocked",
      isOpen: "Vehicle.Cabin.Door.Row1.DriverSide.IsOpen",
      position: "Vehicle.Cabin.Door.Row1.DriverSide.Position",
    },
    unity_skip_paths: new Set([
      "Vehicle.Cabin.Door.Row1.DriverSide.IsLocked",
    ]),
  },
  {
    id: "door2",
    kind: "doorTrunk",
    entity: "row1_door2",
    paths: {
      isLocked: "Vehicle.Cabin.Door.Row1.PassengerSide.IsLocked",
      isOpen: "Vehicle.Cabin.Door.Row1.PassengerSide.IsOpen",
      position: "Vehicle.Cabin.Door.Row1.PassengerSide.Position",
    },
    unity_skip_paths: new Set([
      "Vehicle.Cabin.Door.Row1.PassengerSide.IsLocked",
    ]),
  },

  // --- Door Row2 ---
  {
    id: "door3",
    kind: "doorTrunk",
    entity: "row2_door3",
    paths: {
      isLocked: "Vehicle.Cabin.Door.Row2.DriverSide.IsLocked",
      isOpen: "Vehicle.Cabin.Door.Row2.DriverSide.IsOpen",
      position: "Vehicle.Cabin.Door.Row2.DriverSide.Position",
    },
    unity_skip_paths: new Set([
      "Vehicle.Cabin.Door.Row2.DriverSide.IsLocked",
    ]),
  },
  {
    id: "door4",
    kind: "doorTrunk",
    entity: "row2_door4",
    paths: {
      isLocked: "Vehicle.Cabin.Door.Row2.PassengerSide.IsLocked",
      isOpen: "Vehicle.Cabin.Door.Row2.PassengerSide.IsOpen",
      position: "Vehicle.Cabin.Door.Row2.PassengerSide.Position",
    },
    unity_skip_paths: new Set([
      "Vehicle.Cabin.Door.Row2.PassengerSide.IsLocked",
    ]),
  },

  // --- Trunk Rear / Front ---
  {
    id: "trunk_rear",
    kind: "doorTrunk",
    entity: "trunk_rear",
    paths: {
      isLocked: "Vehicle.Body.Trunk.Rear.IsLocked",
      isOpen: "Vehicle.Body.Trunk.Rear.IsOpen",
      position: "Vehicle.Body.Trunk.Rear.Position",
    },
    unity_skip_paths: new Set([
      "Vehicle.Body.Trunk.Rear.IsLocked",
    ]),
  },
  {
    id: "trunk_front",
    kind: "doorTrunk",
    entity: "trunk_front",
    paths: {
      isLocked: "Vehicle.Body.Trunk.Front.IsLocked",
      isOpen: "Vehicle.Body.Trunk.Front.IsOpen",
      position: "Vehicle.Body.Trunk.Front.Position",
    },
    unity_skip_paths: new Set([
      "Vehicle.Body.Trunk.Front.IsLocked",
    ]),
  },

  // --- Windows (W0/W1) ---
  {
    id: "window1",
    kind: "window",
    entity: "window_1",
    paths: {
      isOpen: "Vehicle.Cabin.Door.Row1.DriverSide.Window.IsOpen",
      position: "Vehicle.Cabin.Door.Row1.DriverSide.Window.Position",
    },
  },
  {
    id: "window2",
    kind: "window",
    entity: "window_2",
    paths: {
      isOpen: "Vehicle.Cabin.Door.Row1.PassengerSide.Window.IsOpen",
      position: "Vehicle.Cabin.Door.Row1.PassengerSide.Window.Position",
    },
  },
  {
    id: "window3",
    kind: "window",
    entity: "window_3",
    paths: {
      isOpen: "Vehicle.Cabin.Door.Row2.DriverSide.Window.IsOpen",
      position: "Vehicle.Cabin.Door.Row2.DriverSide.Window.Position",
    },
  },
  {
    id: "window4",
    kind: "window",
    entity: "window_4",
    paths: {
      isOpen: "Vehicle.Cabin.Door.Row2.PassengerSide.Window.IsOpen",
      position: "Vehicle.Cabin.Door.Row2.PassengerSide.Window.Position",
    },
  },

  // --- Mirrors (M0/M1/M2/M3) ---
  {
    id: "mirror_left",
    kind: "mirror",
    entity: "mirror_left",
    paths: {
      isFolded: "Vehicle.Body.Mirrors.DriverSide.IsFolded",
      isLocked: "Vehicle.Body.Mirrors.DriverSide.IsLocked",
    },
    // For safety, do not forward the lock signal to Unity (update Current only),
    // similar to Door/Trunk handling.
    unity_skip_paths: new Set([
      "Vehicle.Body.Mirrors.DriverSide.IsLocked",
    ]),
  },
  {
    id: "mirror_right",
    kind: "mirror",
    entity: "mirror_right",
    paths: {
      isFolded: "Vehicle.Body.Mirrors.PassengerSide.IsFolded",
      isLocked: "Vehicle.Body.Mirrors.PassengerSide.IsLocked",
    },
    // For safety, do not forward the lock signal to Unity (update Current only),
    // similar to Door/Trunk handling.
    unity_skip_paths: new Set([
      "Vehicle.Body.Mirrors.PassengerSide.IsLocked",
    ]),
  },
];

// Map: target.id -> target definition
const TARGET_BY_ID = new Map(TARGETS.map(t => [t.id, t]));

// Map: VSS path -> target.id (register all paths automatically)
const PATH_TO_TARGET_ID = new Map();
for (const t of TARGETS) {
  for (const k of Object.keys(t.paths)) {
    PATH_TO_TARGET_ID.set(t.paths[k], t.id);
  }
}
function initialCurForTarget(target) {
  if (target.kind === "doorTrunk") {
    return { isLocked: false, isOpen: false, position: POS_CLOSED }; // S0
  }
  if (target.kind === "window") {
    return { isOpen: false, position: POS_CLOSED }; // W0
  }
  if (target.kind === "mirror") {
    return { isLocked: false, isFolded: false }; // M0
  }
  return {};
}

const targetCur = new Map();
for (const t of TARGETS) {
  targetCur.set(t.id, initialCurForTarget(t));
}

module.exports = {
  TARGETS,
  TARGET_BY_ID,
  PATH_TO_TARGET_ID,
  targetCur,
  initialCurForTarget,
};
