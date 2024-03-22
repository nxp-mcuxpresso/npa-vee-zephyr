#!/bin/bash
#
# BASH
#
# Copyright 2021 MicroEJ Corp. All rights reserved.
# This library is provided in source code for use, modification and test, subject to license terms.
# Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

# 'set_project_env.sh' implementation for zephyr project.

# 'set_project_env' is responsible for
# - checking the availability of required environment variables 
# - setting project local variables for 'build.sh' and 'run.sh' 

# Set "stm32f746g_zephyr-bsp/projects/microej" as project root directory
export PROJECT_ROOT_DIR="$( cd -- "$(dirname "$0")/.." >/dev/null 2>&1 ; pwd -P )"

# Set default value for 'ZEPHYR_BASE' environment variable (see https://docs.zephyrproject.org/2.5.0/application/index.html#important-build-system-variables)
export ZEPHYR_BASE_DEFAULT="~/zephyrproject/zephyr"

# Set default value for 'ZEPHYR_BUILD_DIR' environment variable
export ZEPHYR_BUILD_DIR_DEFAULT="$PROJECT_ROOT_DIR/build"

# Set Zephyr 'BOARD' environment variable (see https://docs.zephyrproject.org/2.5.0/application/index.html#important-build-system-variables) 
export BOARD=stm32f746g_disco

LOCAL_ENV_SCRIPT="scripts/set_local_env.sh"
if [[ -e "$LOCAL_ENV_SCRIPT" ]]; then
    echo "Load $LOCAL_ENV_SCRIPT"
    . "$LOCAL_ENV_SCRIPT"
fi

# Set ZEPHYR_BASE to default value if the variable is NOT DEFINED.
if [ -z "$ZEPHYR_BASE" ]; then
	echo "Environment variable 'ZEPHYR_BASE' used to define zephyr installation directory is set to default value ('$ZEPHYR_BASE_DEFAULT')"
    export ZEPHYR_BASE="$ZEPHYR_BASE_DEFAULT"
else
	echo "Environment variable 'ZEPHYR_BASE' used to define zephyr installation directory is set to '$ZEPHYR_BASE' value"
fi

# Set ZEPHYR_BUILD_DIR to default value if the variable is NOT DEFINED.
if [ -z "$ZEPHYR_BUILD_DIR" ]; then
	echo "Environment variable 'ZEPHYR_BUILD_DIR' used to define zephyr installation directory is set to default value ('$ZEPHYR_BUILD_DIR_DEFAULT')"
    export ZEPHYR_BUILD_DIR="$ZEPHYR_BUILD_DIR_DEFAULT"
else
	echo "Environment variable 'ZEPHYR_BUILD_DIR' used to define zephyr installation directory is set to '$ZEPHYR_BUILD_DIR' value"
fi
