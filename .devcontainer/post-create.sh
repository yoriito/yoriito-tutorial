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

apt-get update
apt-get install -y npm libnspr4 libnss3 dbus dbus-x11
rm -rf /var/lib/apt/lists/*

dart pub global activate protoc_plugin
if ! grep -q '.pub-cache/bin' /root/.bashrc 2>/dev/null; then
  echo 'export PATH="$HOME/.pub-cache/bin:$PATH"' >> /root/.bashrc
fi

if [ -d /usr/lib/wsl/lib ]; then
  if ! grep -q 'GALLIUM_DRIVER=d3d12' /root/.bashrc 2>/dev/null; then
    cat <<'EOF' >> /root/.bashrc

export GALLIUM_DRIVER=d3d12
export LD_LIBRARY_PATH=/usr/lib/wsl/lib:$LD_LIBRARY_PATH
EOF
  fi
fi

if ! grep -q 'dbus-daemon --system' /root/.bashrc 2>/dev/null; then
  cat <<'EOF' >> /root/.bashrc

if [ "$(id -u)" -eq 0 ]; then
  export XDG_RUNTIME_DIR=/run/user/0
  mkdir -p "$XDG_RUNTIME_DIR"
fi

if command -v dbus-daemon >/dev/null 2>&1 && [ ! -S /run/dbus/system_bus_socket ]; then
  mkdir -p /run/dbus
  dbus-daemon --system
fi

if command -v dbus-launch >/dev/null 2>&1 && [ -z "$DBUS_SESSION_BUS_ADDRESS" ]; then
  eval "$(dbus-launch --sh-syntax)"
fi
EOF
fi
