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

cmake_minimum_required(VERSION 3.28)
project(pocket-pal-cc LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/yoriito_common.cmake)

find_library(
  DATABROKER_PRODUCER_LIB
  NAMES databroker_producer
  HINTS ${INTERFACE_LIBRARY_DIR})
if(NOT DATABROKER_PRODUCER_LIB)
  message(FATAL_ERROR
          "databroker_producer library not found under ${INTERFACE_LIBRARY_DIR}")
endif()

find_library(
  LOGGER_LIB
  NAMES logger
  HINTS ${INTERFACE_LIBRARY_DIR})
if(NOT LOGGER_LIB)
  message(FATAL_ERROR
          "logger library not found under ${INTERFACE_LIBRARY_DIR}")
endif()

set(TARGET pocket-pal)
add_executable(${TARGET} main.cpp)
target_include_directories(${TARGET} PRIVATE ${INTERFACE_INCLUDE_DIRS})
target_link_libraries(${TARGET} PRIVATE ${DATABROKER_PRODUCER_LIB}
                                        ${LOGGER_LIB})
