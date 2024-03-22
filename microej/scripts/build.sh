#!/bin/bash
set -x
#
# BASH
#
# Copyright 2021 MicroEJ Corp. All rights reserved.
# This library is provided in source code for use, modification and test, subject to license terms.
# Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

# 'build.sh' implementation for zephyr project.

# 'build.sh' is responsible for producing the executable file 
# then copying this executable file to the current directory where it has been executed to a file named 'microej.out'

# Save application current directory and jump one level above scripts
CURRENT_DIRECTORY=$(pwd)
cd $(dirname "$0")/../../..

west build

cp build/zephyr/zephyr.elf  "$CURRENT_DIRECTORY/application.out"

# Restore application directory
cd $CURRENT_DIRECTORY

echo "build.sh script terminating."
