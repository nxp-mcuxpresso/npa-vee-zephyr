# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(nvee)

add_subdirectory(microej)
add_subdirectory(bsp)

add_dependencies(app JAVA_APPS)
