cmake_minimum_required(VERSION 3.14)
project(gtpc-datagen)

# Don't try to contact github everytime.
set (FETCHCONTENT_FULLY_DISCONNECTED "OFF")

# Set the searching location for cmake 'include' locations
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Require C++20
include(c++-standards)
cxx_20()

#---------------------------------------------------------------------------

# C++ compiler flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated -O3 -Wsign-compare")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated -O0 -g -Wsign-compare")
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs -Wno-#pragma-messages")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-unused")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd488 -wd597")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused -Wno-uninitialized")

if(CMAKE_BUILD_TYPE MATCHES Release)
  # this disables asserts for release builds
  add_definitions("-DNDEBUG")
endif()

#-----------------------------------------------------------------------------------------

include(FetchContent)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG        v2.2.0
)

FetchContent_MakeAvailable(cli11)

if(NOT DEFINED CLI11_DIR)
set (CLI11_DIR "${PROJECT_BINARY_DIR}/_deps/cli11-src")
endif()

#-----------------------------------------------------------------------------------------

include_directories("${CLI11_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")

#-----------------------------------------------------------------------------------------
#
# Data generator for the GPTC benchmark.
#

add_executable(gtpc_datagen
  src/gtpc_main.cpp
  src/csv_writer.cpp
  src/data_source.cpp
  src/generator.cpp
)

target_link_libraries(gtpc_datagen
)
