@echo off

REM BAT
REM Copyright 2021 MicroEJ Corp. All rights reserved.
REM This library is provided in source code for use, modification and test, subject to license terms.
REM Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

REM 'run.bat' implementation for zephyr project.

REM 'run.bat' is responsible for flashing the executable file onto the target device 
REM then resetting target device

REM Reset ERRORLEVEL between multiple run in the same shell
SET ERRORLEVEL=0

REM Save application current directory and jump one level above scripts
SET CURRENT_DIRECTORY=%CD%
CD "%~dp0\..\..\.."

CALL "scripts\set_project_env.bat"
IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

west flash --runner linkserver

REM Restore application directory
CD "%CURRENT_DIRECTORY%"

ECHO run.bat script terminating.

