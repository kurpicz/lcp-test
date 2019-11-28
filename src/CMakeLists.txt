################################################################################
# src/CMakeLists.txt
#
# Copyright (C) 2019 Florian Kurpicz <florian.kurpicz@tu-dortmund.de>
#
# All rights reserved. Published under the BSD-2 license in the LICENSE file.
################################################################################

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark
  stdc++fs tlx_command_line divsufsort divsufsort64 sais_lite_lcp sdsl)
target_include_directories(benchmark PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
)

################################################################################
