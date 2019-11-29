################################################################################
# CMakeLists.txt
#
# Copyright (C) 2019 Florian Kurpicz <florian.kurpicz@tu-dortmund.de>
#
# All rights reserved. Published under the BSD-2 license in the LICENSE file.
################################################################################

cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/misc/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project (lcp-test)

add_compile_options("-Wall" "-Wextra" "-Wpedantic"
  "$<$<CONFIG:RELEASE>:-O3>"
  "$<$<CONFIG:RELEASE>:-funroll-loops>"
  "$<$<CONFIG:RELEASE>:-march=native>")

# Add TLX command line parser
add_library(tlx_command_line
  extlib/tlx/tlx/cmdline_parser.cpp
  extlib/tlx/tlx/string/parse_si_iec_units.cpp)

target_include_directories(tlx_command_line PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extlib/tlx/>
  $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extlib/tlx/>
)

# Add sais-lite-lcp
add_library(sais_lite_lcp
  extlib/sais-lite-lcp/sais.c)

target_include_directories(sais_lite_lcp PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extlib/sais-lite-lcp>
  $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extlib/sais-lite-lcp>
)

# Add divsufsort-lcp
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Do not build a shared library for libdivsufsort")
set(BUILD_EXAMPLES OFF CACHE BOOL "Do not build libdivsufsort example")
set(BUILD_DIVSUFSORT64 OFF CACHE BOOL "Build libdivsufsort in 64-bits mode")
add_subdirectory(extlib/libdivsufsort)

# SDSL is required (because we compare with lots of LCP algorithms only available there)
find_package(SDSL REQUIRED)

add_subdirectory(src)

################################################################################
