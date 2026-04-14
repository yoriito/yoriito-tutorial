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

# ── Resolve paths ────────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
PROTO_DIR="${SCRIPT_DIR}"

# List of target .proto files
PROTO_FILES=(
  "${PROTO_DIR}/emergency_toggle.proto"
  "${PROTO_DIR}/emergency_availability.proto"
)

# Output directories
CPP_OUT_DIR="${REPO_ROOT}/ref_app/gen"
DART_OUT_DIR="${REPO_ROOT}/ref_ui/lib/gen"

# ── Tools (Yocto devshell assumed) ───────────────────────────────────────────
need() { command -v "$1" >/dev/null 2>&1 || { echo "Error: $1 not found." >&2; exit 1; }; }
need protoc
need grpc_cpp_plugin

# Resolve include dir for well-known types (empty if not found; optional)
PROTOBUF_INCLUDE="$(pkg-config --variable=includedir protobuf 2>/dev/null || echo "")"

# Print tool paths/versions to ensure we are using Yocto-managed ones
echo "protoc: $(command -v protoc) ($(protoc --version))"
echo "grpc_cpp_plugin: $(command -v grpc_cpp_plugin)"
[[ -n "${PROTOBUF_INCLUDE}" ]] && echo "protobuf include: ${PROTOBUF_INCLUDE}"

# ── Sanity checks ────────────────────────────────────────────────────────────
for f in "${PROTO_FILES[@]}"; do
  [[ -f "${f}" ]] || { echo "Error: ${f} not found." >&2; exit 1; }
done

# Build include flags (order: proto first, then well-known types)
INCLUDES=(-I "${PROTO_DIR}")
[[ -n "${PROTOBUF_INCLUDE}" ]] && INCLUDES+=(-I "${PROTOBUF_INCLUDE}")

# ── Generation (C++) ─────────────────────────────────────────────────────────
echo "-> Generating C++ / gRPC into ${CPP_OUT_DIR}"
mkdir -p "${CPP_OUT_DIR}"
protoc "${INCLUDES[@]}" \
  --cpp_out="${CPP_OUT_DIR}" \
  --grpc_out="${CPP_OUT_DIR}" \
  --plugin=protoc-gen-grpc="$(command -v grpc_cpp_plugin)" \
  "${PROTO_FILES[@]}"

# ── Generation (Dart) ────────────────────────────────────────────────────────
# protoc-gen-dart is usually not available inside Yocto; run only if found
if command -v protoc-gen-dart >/dev/null 2>&1; then
  echo "-> Generating Dart / gRPC into ${DART_OUT_DIR}"
  mkdir -p "${DART_OUT_DIR}"
  # NOTE: Do not pass well-known types as inputs; import resolution via -I is sufficient and more robust
  protoc "${INCLUDES[@]}" \
    --dart_out="grpc:${DART_OUT_DIR}" \
    "${PROTO_FILES[@]}"
else
  echo "Skipping Dart file generation."
fi

# ── Summary ──────────────────────────────────────────────────────────────────
echo -e "\n✅ Generated C++ files:"
ls -1 "${CPP_OUT_DIR}"/Emergency_*.{h,cc} 2>/dev/null | sed 's/^/  • /' || true

if [[ -d "${DART_OUT_DIR}" ]]; then
  echo -e "\n✅ Generated Dart files:"
  ls -1 "${DART_OUT_DIR}"/Emergency_*.dart 2>/dev/null | sed 's/^/  • /' || true
fi
echo -e "\nDone."
