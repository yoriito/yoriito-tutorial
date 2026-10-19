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
project(set LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/yoriito_common.cmake)

find_library(
  YORIITO_LOGGER_LIBRARY
  NAMES logger
  HINTS ${INTERFACE_LIBRARY_DIR} REQUIRED)

find_library(
  YORIITO_DATABROKER_CONSUMER_LIBRARY
  NAMES databroker_consumer
  HINTS ${INTERFACE_LIBRARY_DIR} REQUIRED)

find_library(
  YORIITO_VEHICLE_API_BODY_LIBRARY
  NAMES yoriito_vehicle_api_body
  HINTS ${INTERFACE_LIBRARY_DIR} REQUIRED)

add_executable(${PROJECT_NAME} src/main.cc)

target_include_directories(${PROJECT_NAME} PRIVATE ${INTERFACE_INCLUDE_DIRS})

target_link_options(${PROJECT_NAME} PRIVATE LINKER:--no-as-needed)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE ${YORIITO_VEHICLE_API_BODY_LIBRARY}
          ${YORIITO_DATABROKER_CONSUMER_LIBRARY} ${YORIITO_LOGGER_LIBRARY})

set_target_properties(
  ${PROJECT_NAME} PROPERTIES INSTALL_RPATH
                             "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}/yoriito")

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
