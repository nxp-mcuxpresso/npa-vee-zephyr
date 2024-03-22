#!/bin/bash
set -x
#
# BASH
#
# Copyright 2021 MicroEJ Corp. All rights reserved.
# This library is provided in source code for use, modification and test, subject to license terms.
# Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

# 'run.sh' implementation for zephyr project.

# 'run.sh' is responsible for flashing the executable file onto the target device 
# then resetting target device

# Save application current directory and jump one level above scripts
CURRENT_DIRECTORY=$(pwd)
cd $(dirname "$0")/../../..

#. "scripts/set_project_env.sh"

##west flash -d "$ZEPHYR_BUILD_DIR"

west flash --runner linkserver

# Restore application directory
cd $CURRENT_DIRECTORY

echo "run.sh script terminating."
