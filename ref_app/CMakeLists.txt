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

# ref_app/CMakeLists.txt

cmake_minimum_required(VERSION 3.28)
project(ref_app LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Import shared interface/install dirs defined at repo root:
# INTERFACE_INCLUDE_DIR / INTERFACE_LIBRARY_DIR
# INCLUDE_INSTALL_DIR / LIBRARY_INSTALL_DIR / BIN_INSTALL_DIR
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/yoriito_common.cmake)

# Dependencies (fail early if not found)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(Threads REQUIRED)

# Sources (our code + generated gRPC stubs)
set(REF_APP_SRC
  ${CMAKE_CURRENT_LIST_DIR}/src/main.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/ref_app.cc
  ${CMAKE_CURRENT_LIST_DIR}/gen/emergency_toggle.pb.cc
  ${CMAKE_CURRENT_LIST_DIR}/gen/emergency_toggle.grpc.pb.cc
  ${CMAKE_CURRENT_LIST_DIR}/gen/emergency_availability.pb.cc
  ${CMAKE_CURRENT_LIST_DIR}/gen/emergency_availability.grpc.pb.cc
)
add_executable(ref_app ${REF_APP_SRC})

# Public headers and generated code include paths
target_include_directories(ref_app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/inc
  ${CMAKE_CURRENT_LIST_DIR}/gen
  ${INTERFACE_INCLUDE_DIR}
  ${INTERFACE_INCLUDE_DIR}/databroker
)

# Resolve YoRiito shared libraries from common locations
set(_YORIITO_HINTS
  "${INTERFACE_LIBRARY_DIR}"
  "${LIBRARY_INSTALL_DIR}"
  "/usr/lib/yoriito"
  "/usr/local/lib/yoriito"
  "/opt/vehicle-api/build"
)
set(_YORIITO_HINTS_EXISTING "")
foreach(d IN LISTS _YORIITO_HINTS)
  if(EXISTS "${d}")
    list(APPEND _YORIITO_HINTS_EXISTING "${d}")
  endif()
endforeach()
if(NOT _YORIITO_HINTS_EXISTING)
  set(_YORIITO_HINTS_EXISTING ${_YORIITO_HINTS})
endif()

# Find concrete .so files by canonical names
find_library(YORIITO_LOGGER         NAMES logger              HINTS ${_YORIITO_HINTS_EXISTING})
find_library(YORIITO_DATABROKER_LIB NAMES databroker_consumer HINTS ${_YORIITO_HINTS_EXISTING})
find_library(YORIITO_BODY           NAMES yoriito_vehicle_api_body  HINTS ${_YORIITO_HINTS_EXISTING})
find_library(YORIITO_CABIN          NAMES yoriito_vehicle_api_cabin HINTS ${_YORIITO_HINTS_EXISTING})

# Compute RPATH directories from found libs (+ well-known prefixes)
get_filename_component(_DIR_LOGGER ${YORIITO_LOGGER} DIRECTORY)
get_filename_component(_DIR_DB     ${YORIITO_DATABROKER_LIB} DIRECTORY)
get_filename_component(_DIR_BODY   ${YORIITO_BODY} DIRECTORY)
get_filename_component(_DIR_CABIN  ${YORIITO_CABIN} DIRECTORY)

set(_BUILD_RPATH_DIRS ${_DIR_LOGGER} ${_DIR_DB} ${_DIR_BODY} ${_DIR_CABIN})
set(_INSTALL_RPATH_DIRS ${LIBRARY_INSTALL_DIR})

list(REMOVE_DUPLICATES _BUILD_RPATH_DIRS)

# Embed RPATH for runtime resolution without LD_LIBRARY_PATH
set_target_properties(ref_app PROPERTIES
  BUILD_RPATH   "${_BUILD_RPATH_DIRS}"
  INSTALL_RPATH "${_INSTALL_RPATH_DIRS}"
)
# To avoid the contamination between BUILD_RPATH and INSTALL_RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)

# Keep direct links even if they appear unused from the linker's POV
target_link_options(ref_app PRIVATE LINKER:--no-as-needed)

# Ensure transitive DT_NEEDED libraries are kept on the link line (as requested)
target_link_options(ref_app PRIVATE LINKER:--copy-dt-needed-entries)

# Use RPATH so that indirect dependencies can be resolved correctly.
target_link_options(ref_app PRIVATE LINKER:--disable-new-dtags)

# Add RPATH directories to the link options
foreach(_dir IN LISTS _BUILD_RPATH_DIRS)
  # Add each RPATH directory to the link options
  target_link_options(ref_app PRIVATE LINKER:-rpath-link,${_dir})
endforeach()

# Link required libraries
target_link_libraries(ref_app PRIVATE
  ${YORIITO_LOGGER}
  ${YORIITO_DATABROKER_LIB}
  ${YORIITO_BODY}
  ${YORIITO_CABIN}
  gRPC::grpc++
  protobuf::libprotobuf
  Threads::Threads
)

# Install the executable into the configured bin directory
install(TARGETS ref_app RUNTIME DESTINATION ${BIN_INSTALL_DIR})

# Debug output
message(STATUS "logger: ${YORIITO_LOGGER}")
message(STATUS "databroker_consumer: ${YORIITO_DATABROKER_LIB}")
message(STATUS "yoriito_vehicle_api_body: ${YORIITO_BODY}")
message(STATUS "yoriito_vehicle_api_cabin: ${YORIITO_CABIN}")
message(STATUS "RPATH: ${_RPATH_DIRS}")
