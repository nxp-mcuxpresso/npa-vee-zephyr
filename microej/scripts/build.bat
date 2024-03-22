@echo off

REM BAT
REM Copyright 2021 MicroEJ Corp. All rights reserved.
REM This library is provided in source code for use, modification and test, subject to license terms.
REM Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

REM 'build.bat' implementation for zephyr project.

REM 'build.bat' is responsible for producing the executable file 
REM then copying this executable file to the current directory where it has been executed to a file named 'microej.out'

REM Reset ERRORLEVEL between multiple run in the same shell
SET ERRORLEVEL=0

REM Save application current directory and jump one level above scripts
SET CURRENT_DIRECTORY=%CD%
CD "%~dp0\..\..\.."

west build
COPY "build\zephyr\zephyr.elf" "%CURRENT_DIRECTORY%\application.out"

REM Restore application directory
CD "%CURRENT_DIRECTORY%"

ECHO "build.bat script terminating."
