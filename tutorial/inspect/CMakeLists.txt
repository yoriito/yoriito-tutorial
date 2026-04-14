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

# inspect/CMakeLists.txt

cmake_minimum_required(VERSION 3.28)
project(inspect LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Import shared interface/install dirs defined at repo root:
# INTERFACE_INCLUDE_DIR / INTERFACE_LIBRARY_DIR
# INCLUDE_INSTALL_DIR / LIBRARY_INSTALL_DIR / BIN_INSTALL_DIR
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/yoriito_common.cmake)

# Dependencies (fail early if not found)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(Threads REQUIRED)

# Sources (our code)
set(INSPECT_SRC
  ${CMAKE_CURRENT_LIST_DIR}/src/main.cc
)
add_executable(inspect ${INSPECT_SRC})

# Public headers and generated code include paths
target_include_directories(inspect PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/inc
  ${CMAKE_CURRENT_LIST_DIR}/gen
  ${INTERFACE_INCLUDE_DIR}
  ${INTERFACE_INCLUDE_DIR}/databroker
)

# Resolve yoriito shared libraries from common locations
set(_yoriito_HINTS
  "${INTERFACE_LIBRARY_DIR}"
  "${LIBRARY_INSTALL_DIR}"
  "/usr/lib/yoriito"
  "/usr/local/lib/yoriito"
  "/opt/yoriito-api-utility/build"
)
set(_yoriito_HINTS_EXISTING "")
foreach(d IN LISTS _yoriito_HINTS)
  if(EXISTS "${d}")
    list(APPEND _yoriito_HINTS_EXISTING "${d}")
  endif()
endforeach()
if(NOT _yoriito_HINTS_EXISTING)
  set(_yoriito_HINTS_EXISTING ${_yoriito_HINTS})
endif()

# Find concrete .so files by canonical names
find_library(yoriito_LOGGER         NAMES logger              HINTS ${_yoriito_HINTS_EXISTING})
find_library(yoriito_DATABROKER_LIB NAMES databroker_consumer HINTS ${_yoriito_HINTS_EXISTING})
find_library(yoriito_BODY           NAMES yoriito_vehicle_api_body     HINTS ${_yoriito_HINTS_EXISTING})
find_library(yoriito_CABIN          NAMES yoriito_vehicle_api_cabin    HINTS ${_yoriito_HINTS_EXISTING})

# Compute RPATH directories from found libs (+ well-known prefixes)
get_filename_component(_DIR_LOGGER ${yoriito_LOGGER} DIRECTORY)
get_filename_component(_DIR_DB     ${yoriito_DATABROKER_LIB} DIRECTORY)
get_filename_component(_DIR_BODY   ${yoriito_BODY} DIRECTORY)
get_filename_component(_DIR_CABIN  ${yoriito_CABIN} DIRECTORY)

set(_BUILD_RPATH_DIRS ${_DIR_LOGGER} ${_DIR_DB} ${_DIR_BODY} ${_DIR_CABIN})
set(_INSTALL_RPATH_DIRS ${LIBRARY_INSTALL_DIR})

list(REMOVE_DUPLICATES _BUILD_RPATH_DIRS)

# Embed RPATH for runtime resolution without LD_LIBRARY_PATH
set_target_properties(inspect PROPERTIES
  BUILD_RPATH   "${_BUILD_RPATH_DIRS}"
  INSTALL_RPATH "${_INSTALL_RPATH_DIRS}"
)
# To avoid the contamination between BUILD_RPATH and INSTALL_RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)

# Keep direct links even if they appear unused from the linker's POV
target_link_options(inspect PRIVATE LINKER:--no-as-needed)

# Ensure transitive DT_NEEDED libraries are kept on the link line (as requested)
target_link_options(inspect PRIVATE LINKER:--copy-dt-needed-entries)

# Use RPATH so that indirect dependencies can be resolved correctly.
target_link_options(inspect PRIVATE LINKER:--disable-new-dtags)

# Add RPATH directories to the link options
foreach(_dir IN LISTS _BUILD_RPATH_DIRS)
  # Add each RPATH directory to the link options
  target_link_options(inspect PRIVATE LINKER:-rpath-link,${_dir})
endforeach()

# Link required libraries
target_link_libraries(inspect PRIVATE
  ${yoriito_LOGGER}
  ${yoriito_DATABROKER_LIB}
  ${yoriito_BODY}
  ${yoriito_CABIN}
  gRPC::grpc++
  protobuf::libprotobuf
  Threads::Threads
)

# Install the executable into the configured bin directory
install(TARGETS inspect RUNTIME DESTINATION ${BIN_INSTALL_DIR})

# Debug output
message(STATUS "logger: ${yoriito_LOGGER}")
message(STATUS "databroker_consumer: ${yoriito_DATABROKER_LIB}")
message(STATUS "yoriito_vehicle_api_body: ${yoriito_BODY}")
message(STATUS "yoriito_vehicle_api_cabin: ${yoriito_CABIN}")
message(STATUS "RPATH: ${_RPATH_DIRS}")
