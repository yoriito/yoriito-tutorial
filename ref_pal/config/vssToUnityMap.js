// config/vssToUnityMap.js
'use strict';

const propertyMap = {
  "Vehicle.Cabin.Door.Row1.DriverSide.IsOpen": {
    unity_path: "row1_door1",
    option_name: "row1_door1_open",
  },
  "Vehicle.Cabin.Door.Row1.DriverSide.Position": {
    unity_path: "row1_door1",
    option_name: "door1_position",
    action_name: 'position',
  },
  "Vehicle.Cabin.Door.Row1.DriverSide.IsLocked": {
    unity_path: "row1_door1",
    option_name: "door1_locked",
  },
  "Vehicle.Cabin.Door.Row1.PassengerSide.IsOpen": {
    unity_path: "row1_door2",
    option_name: "row1_door2_open",
  },
  "Vehicle.Cabin.Door.Row1.PassengerSide.Position": {
    unity_path: "row1_door2",
    option_name: "door2_position",
    action_name: 'position',
  },
  "Vehicle.Cabin.Door.Row1.PassengerSide.IsLocked": {
    unity_path: "row1_door2",
    option_name: "door2_locked",
  },
  "Vehicle.Body.Lights.Beam.High.IsOn": {
    unity_path: "beam_high",
    option_name: "beam_high_open",
  },
  "Vehicle.Body.Lights.Beam.Low.IsOn": {
    unity_path: "beam_low",
    option_name: "beam_low_open",
  },
  "Vehicle.Body.Trunk.Rear.IsOpen": {
    unity_path: "trunk_rear",
    option_name: "trunk_rear_open",
  },
  "Vehicle.Body.Trunk.Rear.Position": {
    unity_path: "trunk_rear",
    option_name: "trunk_rear_position",
    action_name: "position",
  },
  "Vehicle.Body.Trunk.Rear.IsLocked": {
    unity_path: "trunk_rear",
    option_name: "trunk_rear_islocked",
  },
  "Vehicle.Body.Trunk.Rear.IsLightOn": {
    unity_path: "rear_light",
    option_name: "rear_light",
  },
  "Vehicle.Body.Trunk.Front.Position": {
    unity_path: "trunk_front",
    option_name: "trunk_front_position",
    action_name: "position",
  },
  "Vehicle.Body.Trunk.Front.IsOpen": {
    unity_path: "trunk_front",
    option_name: "trunk_front_isopen",
  },
  "Vehicle.Body.Trunk.Front.IsLocked": {
    unity_path: "trunk_front",
    option_name: "trunk_front_islocked",
  },
  "Vehicle.Body.Trunk.Front.IsLightOn": {
    unity_path: "front_light",
    option_name: "front_light",
  },
  "Vehicle.Body.Lights.Brake.IsActive": {
    unity_path: "light_brake",
    option_name: "light_brake_active",
    action_map: {
      ACTIVE: "turn_on_brake",
      INACTIVE: "turn_off_brake",
      ADAPTIVE: "turn_on_brake",
    },
  },
  "Vehicle.Cabin.Seat.Row1.DriverSide.Height": {
    unity_path: "seat1",
    option_name: "seat1_height",
    action_name: "seat_height",
  },
  "Vehicle.Cabin.Seat.Row1.DriverSide.Position": {
    unity_path: "seat1",
    option_name: "seat1_position",
    action_name: "seat_position",
  },
  "Vehicle.Body.Lights.Hazard.IsSignaling": {
    unity_path: "hazard_signaling",
    option_name: "hazard_signaling",
    action_map: {
      "true": "turn_on_signal",
      "false": "turn_off_signal",
    },
  },
  "Vehicle.Cabin.Seat.Row1.PassengerSide.Position": {
    unity_path: "seat2",
    option_name: "seat2_position",
    action_name: "seat_position",
  },
  "Vehicle.Cabin.Seat.Row1.PassengerSide.Height": {
    unity_path: "seat2",
    option_name: "seat2_height",
    action_name: "seat_height",
  },
  "Vehicle.Cabin.Seat.Row2.DriverSide.Position": {
    unity_path: "seat3",
    option_name: "seat3_position",
    action_name: "seat_position",
  },
  "Vehicle.Cabin.Seat.Row2.DriverSide.Height": {
    unity_path: "seat3",
    option_name: "seat3_height",
    action_name: "seat_height",
  },
  "Vehicle.Cabin.Seat.Row2.PassengerSide.Position": {
    unity_path: "seat4",
    option_name: "seat4_position",
    action_name: "seat_position",
  },
  "Vehicle.Cabin.Seat.Row2.PassengerSide.Height": {
    unity_path: "seat4",
    option_name: "seat4_height",
    action_name: "seat_height",
  },
  "Vehicle.Cabin.Door.Row2.DriverSide.IsOpen": {
    unity_path: "row2_door3",
    option_name: "row2_door3_open",
  },
  "Vehicle.Cabin.Door.Row2.DriverSide.Position": {
    unity_path: "row2_door3",
    option_name: "door3_position",
    action_name: 'position',
  },
  "Vehicle.Cabin.Door.Row2.DriverSide.IsLocked": {
    unity_path: "row2_door3",
    option_name: "door3_locked",
  },
  "Vehicle.Cabin.Door.Row2.PassengerSide.IsOpen": {
    unity_path: "row2_door4",
    option_name: "row2_door4_open",
  },
  "Vehicle.Cabin.Door.Row2.PassengerSide.Position": {
    unity_path: "row2_door4",
    option_name: "door4_position",
    action_name: 'position',
  },
  "Vehicle.Cabin.Door.Row2.PassengerSide.IsLocked": {
    unity_path: "row2_door4",
    option_name: "door4_locked",
  },
  "Vehicle.Body.Windshield.Front.Wiping.Mode": {
    unity_path: "wipper_front",
    option_name: null,
  },
  "Vehicle.Body.Windshield.Rear.Wiping.Mode": {
    unity_path: "wipper_rear",
    option_name: null,
  },

  // ---- Mirrors ----
  "Vehicle.Body.Mirrors.DriverSide.IsFolded": {
    unity_path: "mirror_left",
    option_name: "mirror_left_isfolded",
  },
  "Vehicle.Body.Mirrors.DriverSide.IsLocked": {
    unity_path: "mirror_left",
    option_name: "mirror_left_islocked",
  },
  "Vehicle.Body.Mirrors.PassengerSide.IsFolded": {
    unity_path: "mirror_right",
    option_name: "mirror_right_isfolded",
  },
  "Vehicle.Body.Mirrors.PassengerSide.IsLocked": {
    unity_path: "mirror_right",
    option_name: "mirror_right_islocked",
  },

  // ---- Windows ----
  "Vehicle.Cabin.Door.Row1.DriverSide.Window.IsOpen": {
    unity_path: "window_1",
    option_name: "window_isopen",
  },
  "Vehicle.Cabin.Door.Row1.DriverSide.Window.Position": {
    unity_path: "window_1",
    option_name: "window1_position",
    action_name: "position",
  },
  "Vehicle.Cabin.Door.Row1.PassengerSide.Window.IsOpen": {
    unity_path: "window_2",
    option_name: "window2_isopen",
  },
  "Vehicle.Cabin.Door.Row1.PassengerSide.Window.Position": {
    unity_path: "window_2",
    option_name: "window2_position",
    action_name: "position",
  },
  "Vehicle.Cabin.Door.Row2.DriverSide.Window.IsOpen": {
    unity_path: "window_3",
    option_name: "window3_isopen",
  },
  "Vehicle.Cabin.Door.Row2.DriverSide.Window.Position": {
    unity_path: "window_3",
    option_name: "window3_position",
    action_name: "position",
  },
  "Vehicle.Cabin.Door.Row2.PassengerSide.Window.IsOpen": {
    unity_path: "window_4",
    option_name: "window4_isopen",
  },
  "Vehicle.Cabin.Door.Row2.PassengerSide.Window.Position": {
    unity_path: "window_4",
    option_name: "window4_position",
    action_name: "position",
  },

  "Vehicle.Cabin.Light.AmbientLight.Row1.DriverSide.Color": {
    unity_path: "ambientlight_1",
    option_name: "ambientlight_1_color",
    action_name: "set_color",
  },
  "Vehicle.Cabin.Light.AmbientLight.Row1.DriverSide.IsLightOn": {
    unity_path: "ambientlight_1",
    option_name: "ambientlight_1_ison",
  },
  "Vehicle.Cabin.Light.AmbientLight.Row1.PassengerSide.Color": {
    unity_path: "ambientlight_2",
    option_name: "ambientlight_2_color",
    action_name: "set_color",
  },
  "Vehicle.Cabin.Light.AmbientLight.Row1.PassengerSide.IsLightOn": {
    unity_path: "ambientlight_2",
    option_name: "ambientlight_2_ison",
  },
  "Vehicle.AverageSpeed": {
    unity_path: "wheels",
    option_name: "wheels",
    action_name: "speed",
  },
  "Vehicle.Body.Lights.LicensePlate.IsOn": {
    unity_path: "license_plate",
    option_name: "license_plate_light",
  },
  "Vehicle.Body.Horn.IsActive": {
    unity_path: "air_bag",
    option_name: "air_bag_light",
  },
};

module.exports = { propertyMap };
