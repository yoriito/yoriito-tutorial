#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
#
# Copyright 2026 JASPAR
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# -*- coding: utf-8 -*-

import argparse
import csv
import json
import re
from pathlib import Path
from typing import Any, Dict, List, Set, Tuple


CSV_HEADER = [
    "base_path",
    "capability",
    "availability_sensor",
    "availability_actuator",
    "default_value_type",
    "default_value",
]


def load_vss_json(vss_json_path: Path) -> Dict[str, Any]:
    return json.loads(vss_json_path.read_text(encoding="utf-8"))


def load_unity_target_vss_paths(vss_to_unity_map_js_path: Path) -> Set[str]:
    js_text = vss_to_unity_map_js_path.read_text(encoding="utf-8")

    # ex) "Vehicle.Body.Lights.Beam.High.IsOn": { ... }
    unity_target_paths = re.findall(r'^\s*"([^"]+)"\s*:\s*\{', js_text, flags=re.MULTILINE)
    return set(unity_target_paths)


def extract_leaf_nodes_with_datatype(vss_root: Dict[str, Any]) -> List[Tuple[str, Dict[str, Any]]]:
    if "Vehicle" not in vss_root:
        raise ValueError("vss.json must have 'Vehicle' as the top-level key.")

    leaf_nodes: List[Tuple[str, Dict[str, Any]]] = []

    def walk(node_dict: Dict[str, Any], current_path_parts: List[str]) -> None:
        if "datatype" in node_dict:
            full_path = ".".join(current_path_parts)
            leaf_nodes.append((full_path, node_dict))

        children_dict = node_dict.get("children")
        if isinstance(children_dict, dict):
            for child_name, child_node in children_dict.items():
                if isinstance(child_node, dict):
                    walk(child_node, current_path_parts + [child_name])

    walk(vss_root["Vehicle"], ["Vehicle"])
    return leaf_nodes


def is_availability_node(vss_path: str) -> bool:
    return vss_path.endswith("_Availability_Sensor") or vss_path.endswith("_Availability_Actuator")


def get_base_signal_path_for_availability_node(availability_node_path: str) -> str:
    if availability_node_path.endswith("_Availability_Sensor"):
        return availability_node_path[: -len("_Availability_Sensor")]
    if availability_node_path.endswith("_Availability_Actuator"):
        return availability_node_path[: -len("_Availability_Actuator")]
    return availability_node_path


def to_lower_bool_string(value: bool) -> str:
    return "true" if value else "false"


def is_body_or_cabin_path(vss_path: str) -> bool:
    return vss_path.startswith("Vehicle.Body.") or vss_path.startswith("Vehicle.Cabin.")


def map_vss_datatype_to_csv_default_value_type(vss_datatype: str) -> str:
    datatype_map = {
        "boolean": "bool",
        "string": "string",
        "int8": "int8",
        "int16": "int16",
        "int32": "int32",
        "int64": "int64",
        "uint8": "uint8",
        "uint16": "uint16",
        "uint32": "uint32",
        "uint64": "uint64",
        "float": "float",
        "double": "double",
    }

    if vss_datatype.startswith("Types."):
        if vss_datatype.endswith("[]"):
            return "value_object_array"
        return "value_object"

    if vss_datatype.endswith("[]"):
        element_datatype = vss_datatype[:-2]
        element_value_type = datatype_map.get(element_datatype)
        if element_value_type is None:
            raise ValueError(f"Unsupported VSS array datatype: {vss_datatype}")
        return f"{element_value_type}_array"

    value_type = datatype_map.get(vss_datatype)
    if value_type is None:
        raise ValueError(f"Unsupported VSS datatype: {vss_datatype}")
    return value_type


def get_default_value_for_normal_node(csv_value_type: str) -> str:
    if csv_value_type == "bool":
        return "false"
    if csv_value_type in {
        "int8", "int16", "int32", "int64",
        "uint8", "uint16", "uint32", "uint64",
        "float", "double",
    }:
        return "0"
    if csv_value_type.endswith("_array"):
        return "[]"
    if csv_value_type == "value_object":
        return "{}"
    return ""


def determine_availability_flags_for_normal_node(is_unity_target: bool) -> Tuple[bool, bool]:
    availability_sensor = is_unity_target
    availability_actuator = is_unity_target
    return availability_sensor, availability_actuator


def determine_availability_flags_for_availability_node() -> Tuple[bool, bool]:
    return True, False


def determine_default_value_for_availability_node(
    availability_node_path: str,
    unity_target_vss_paths: Set[str],
) -> str:
    base_signal_path = get_base_signal_path_for_availability_node(availability_node_path)
    return to_lower_bool_string(base_signal_path in unity_target_vss_paths)


def build_csv_row(
    vss_path: str,
    vss_node_dict: Dict[str, Any],
    unity_target_vss_paths: Set[str],
) -> Dict[str, str]:
    capability_value = is_body_or_cabin_path(vss_path)

    if is_availability_node(vss_path):
        availability_sensor, availability_actuator = determine_availability_flags_for_availability_node()
        default_value_type = "bool"
        default_value = determine_default_value_for_availability_node(vss_path, unity_target_vss_paths)
    else:
        is_unity_target = vss_path in unity_target_vss_paths
        availability_sensor, availability_actuator = determine_availability_flags_for_normal_node(is_unity_target)
        vss_datatype = str(vss_node_dict.get("datatype", "string"))
        default_value_type = map_vss_datatype_to_csv_default_value_type(vss_datatype)
        default_value = get_default_value_for_normal_node(default_value_type)

    return {
        "base_path": vss_path,
        "capability": to_lower_bool_string(capability_value),
        "availability_sensor": to_lower_bool_string(availability_sensor),
        "availability_actuator": to_lower_bool_string(availability_actuator),
        "default_value_type": default_value_type,
        "default_value": default_value,
    }


def write_vss_signal_settings_csv(output_csv_path: Path, csv_rows: List[Dict[str, str]]) -> None:
    output_csv_path.parent.mkdir(parents=True, exist_ok=True)
    with output_csv_path.open("w", encoding="utf-8", newline="") as file_handle:
        csv_writer = csv.DictWriter(file_handle, fieldnames=CSV_HEADER)
        csv_writer.writeheader()
        csv_writer.writerows(csv_rows)


def generate_vss_signal_settings_csv(
    vss_json_path: Path,
    vss_to_unity_map_js_path: Path,
    output_csv_path: Path,
) -> None:
    vss_root = load_vss_json(vss_json_path)
    unity_target_vss_paths = load_unity_target_vss_paths(vss_to_unity_map_js_path)

    leaf_nodes = extract_leaf_nodes_with_datatype(vss_root)

    csv_rows: List[Dict[str, str]] = []
    for vss_path, vss_node_dict in leaf_nodes:
        csv_rows.append(build_csv_row(vss_path, vss_node_dict, unity_target_vss_paths))

    csv_rows.sort(key=lambda row: row["base_path"])

    write_vss_signal_settings_csv(output_csv_path, csv_rows)


def parse_command_line_arguments() -> Tuple[Path, Path, Path]:
    parser = argparse.ArgumentParser(
        description=(
            "Generate a VSS signal settings CSV from a VSS JSON file and a VSS-to-Unity map JS file."
        )
    )
    parser.add_argument(
        "--vss-json",
        required=True,
        type=Path,
        dest="vss_json_path",
        help="Path to the VSS JSON file (e.g., vss.json).",
    )
    parser.add_argument(
        "--vss-to-unity-map",
        required=True,
        type=Path,
        dest="vss_to_unity_map_js_path",
        help="Path to the VSS-to-Unity map JS file (e.g., vssToUnityMap.js).",
    )
    parser.add_argument(
        "--output-csv",
        required=True,
        type=Path,
        dest="output_csv_path",
        help="Output path for the generated CSV file.",
    )

    parsed_args = parser.parse_args()
    return (
        parsed_args.vss_json_path,
        parsed_args.vss_to_unity_map_js_path,
        parsed_args.output_csv_path,
    )


if __name__ == "__main__":
    vss_json_path, vss_to_unity_map_js_path, output_csv_path = parse_command_line_arguments()
    generate_vss_signal_settings_csv(vss_json_path, vss_to_unity_map_js_path, output_csv_path)
    print(f"Generated: {output_csv_path}")
