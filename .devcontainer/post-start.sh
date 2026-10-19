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

if ! command -v dbus-daemon >/dev/null 2>&1; then
  exit 0
fi

mkdir -p /run/dbus
install -d -m 700 /run/user/0

if ! pgrep -f 'dbus-daemon --system' >/dev/null 2>&1; then
  rm -f /run/dbus/system_bus_socket /run/dbus/pid
  XDG_RUNTIME_DIR=/run/user/0 dbus-daemon --system
fi

if ! pgrep -f 'dbus-daemon --session' >/dev/null 2>&1; then
  rm -f /run/dbus/session_bus_socket
  XDG_RUNTIME_DIR=/run/user/0 dbus-daemon --session --fork --address=unix:path=/run/dbus/session_bus_socket
fi
