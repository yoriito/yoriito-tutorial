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

ARG YORIITO_DEVCONTAINER
ARG YORIITO_VERSION

FROM ${YORIITO_DEVCONTAINER}:${YORIITO_VERSION}

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        clangd \
        cmake-format \
        curl \
        ca-certificates \
        libnspr4 \
        libnss3 \
        dbus \
        dbus-x11 \
    && DEBIAN_FRONTEND=noninteractive apt-get purge -y npm nodejs libnode-dev || true \
    && curl -fsSL https://deb.nodesource.com/setup_22.x -o /tmp/nodesource_setup.sh \
    && bash /tmp/nodesource_setup.sh \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends nodejs \
    && node --version \
    && npm --version \
    && rm -f /tmp/nodesource_setup.sh \
    && rm -rf /var/lib/apt/lists/*

RUN dart pub global activate protoc_plugin

ENV PATH="/root/.pub-cache/bin:${PATH}"
