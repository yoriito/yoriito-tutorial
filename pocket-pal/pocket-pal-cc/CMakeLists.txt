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

cmake_minimum_required(VERSION 3.10)
project(pocket-pal-cc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(YORIITO "yoriito")
set(LIBRARY_INSTALL_DIR ${SDKTARGETSYSROOT}/usr/lib/${YORIITO})
find_library(DATABROKER_PRODUCER_LIB NAMES databroker_producer PATHS ${LIBRARY_INSTALL_DIR})
find_library(LOGGER_LIB NAMES logger PATHS ${LIBRARY_INSTALL_DIR})
set(TARGET pocket-pal)
add_executable(
    ${TARGET}
    main.cpp
)
target_compile_options(${TARGET}  PRIVATE "-g" "-O0")
target_include_directories(
    ${TARGET}
    PRIVATE
    /usr/include/yoriito
)
target_link_libraries(
    ${TARGET}
    PRIVATE
    ${DATABROKER_PRODUCER_LIB}
    ${LOGGER_LIB}
)
