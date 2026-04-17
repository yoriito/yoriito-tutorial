#!/usr/bin/env bash
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

set -euo pipefail

export VSS_JSON_PATH="/etc/yoriito/viss/vss.json"
export VSS_TO_UNITY_MAP_JS_PATH="./config/vssToUnityMap.js"
export OUTPUT_CSV_FILE_PATH="./config/vss_signal_settings.csv"

python3 tools/generate_vss_signal_settings.py \
  --vss-json "$VSS_JSON_PATH" \
  --vss-to-unity-map "$VSS_TO_UNITY_MAP_JS_PATH" \
  --output-csv "$OUTPUT_CSV_FILE_PATH"

node - <<'NODE' 
'use strict';
const { runVssNodeSetup } = require('./viss/vssNodeSetup/runVssNodeSetup');

(async () => {
  const csvFilePath = process.env.OUTPUT_CSV_FILE_PATH;
  await runVssNodeSetup({ csvFilePath });
})();
NODE
