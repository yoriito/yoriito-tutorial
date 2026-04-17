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

SRC="$1"
OUT_DIR="splash_png"
SIZES=(1536 1024 768 512 256)

mkdir -p "$OUT_DIR"
BASE="$(basename "${SRC%.*}")"

for PX in "${SIZES[@]}"; do
  OUT="$OUT_DIR/${BASE}-${PX}.png"
  rsvg-convert -w "$PX" "$SRC" | \
    convert png:- -trim +repage "$OUT"
done
