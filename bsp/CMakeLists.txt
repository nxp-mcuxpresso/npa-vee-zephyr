# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Git)

SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

file (STRINGS "${ProjDirPath}/../nvee_version.txt" VEE_VERSION)

target_compile_options(app PRIVATE -DSDK_BOARD_ID=${CONFIG_BOARD})

execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --always --abbrev=8 --dirty
  WORKING_DIRECTORY "${ProjDirPath}"
  OUTPUT_VARIABLE GIT_SHA_1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file("${ProjDirPath}/src/tree_version.c.in"
               "${CMAKE_CURRENT_BINARY_DIR}/tree_version.c" @ONLY)

if (CONFIG_TENSORFLOW_LITE_MICRO)
SET(AI_SRC src/model_cifarnet_ops.cpp)
endif()

target_sources(app PRIVATE src/main.c
                src/simple_gfx_app_imp.c
                src/core_test_natives.c
                ${AI_SRC}
               "${CMAKE_CURRENT_BINARY_DIR}/tree_version.c")

target_include_directories(app PRIVATE
   src
    ${CMAKE_CURRENT_BINARY_DIR}/../platform/inc)

set(CMAKE_MODULE_PATH
    ${ProjDirPath}/microej/ai
    ${ProjDirPath}/microej/core
    ${ProjDirPath}/microej/ui)

include(microej_core)

if (CONFIG_DISPLAY)
include(microej_ui)
endif()

if (CONFIG_TENSORFLOW_LITE_MICRO)
include(nxpvee_ai)
endif()

if (CONFIG_SHELL)
target_sources(app PRIVATE src/shell.c)
endif()

target_link_libraries(app PUBLIC
${CMAKE_CURRENT_BINARY_DIR}/../microejapp.o
${CMAKE_CURRENT_BINARY_DIR}/../microejruntime.a)
