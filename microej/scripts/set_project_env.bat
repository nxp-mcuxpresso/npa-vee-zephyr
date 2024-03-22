@echo off

REM BAT
REM Copyright 2021 MicroEJ Corp. All rights reserved.
REM This library is provided in source code for use, modification and test, subject to license terms.
REM Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

REM 'set_project_env.bat' implementation for zephyr project.

REM 'set_project_env' is responsible for
REM - checking the availability of required environment variables 
REM - setting project local variables for 'build.bat' and 'run.bat' 

REM Load local settings if the file exists.  See set_local_env.bat.tpl for an example.
SET LOCAL_ENV_SCRIPT="scripts\set_local_env.bat"
IF EXIST "%LOCAL_ENV_SCRIPT%" (
   ECHO "Load %LOCAL_ENV_SCRIPT%"
   CALL "%LOCAL_ENV_SCRIPT%"
   IF %ERRORLEVEL% NEQ 0 (
      EXIT /B %ERRORLEVEL%
   )
)

SET "PROJECT_ROOT_DIR=%~dp0.."

REM Set ZEPHYR_BASE to default value if the variable is NOT DEFINED.
SET "ZEPHYR_BASE_DEFAULT=%USERPROFILE%\zephyrproject\zephyr"
IF NOT DEFINED ZEPHYR_BASE (
	ECHO "Environment variable 'ZEPHYR_BASE' used to define zephyr installation directory is set to default value ('%ZEPHYR_BASE_DEFAULT%')"
    SET "ZEPHYR_BASE=%ZEPHYR_BASE_DEFAULT%"
) ELSE ECHO "Environment variable 'ZEPHYR_BASE' used to define zephyr installation directory is set to '%ZEPHYR_BASE%' value"

REM Set GNUARMEMB_TOOLCHAIN_PATH to default value if the variable is NOT DEFINED.
SET "GNUARMEMB_TOOLCHAIN_PATH_DEFAULT=C:\Program Files (x86)\GNU Arm Embedded Toolchain"
IF NOT DEFINED GNUARMEMB_TOOLCHAIN_PATH (
	ECHO "Environment variable 'GNUARMEMB_TOOLCHAIN_PATH' used to define GNU ARM toolchain directory is set to default value ('%GNUARMEMB_TOOLCHAIN_PATH_DEFAULT%')"
    SET "GNUARMEMB_TOOLCHAIN_PATH=%GNUARMEMB_TOOLCHAIN_PATH_DEFAULT%"
) ELSE ECHO "Environment variable 'GNUARMEMB_TOOLCHAIN_PATH' used to define GNU ARM toolchain directory is set to '%GNUARMEMB_TOOLCHAIN_PATH%' value"

REM Set OPENOCD_EXEC_PATH to default value if the variable is NOT DEFINED.
SET "OPENOCD_EXEC_PATH_DEFAULT=C:\Program Files\OpenOCD\bin"
IF NOT DEFINED OPENOCD_EXEC_PATH (
	ECHO "Environment variable 'OPENOCD_EXEC_PATH' used to define GNU ARM toolchain directory is set to default value ('%OPENOCD_EXEC_PATH_DEFAULT%')"
    SET "OPENOCD_EXEC_PATH=%OPENOCD_EXEC_PATH_DEFAULT%"
) ELSE ECHO "Environment variable 'OPENOCD_EXEC_PATH' used to define GNU ARM toolchain directory is set to '%OPENOCD_EXEC_PATH%' value"

REM Set OPENOCD_EXEC_PATH to default value if the variable is NOT DEFINED.
SET "ZEPHYR_BUILD_DIR_DEFAULT=%PROJECT_ROOT_DIR%\build"
IF NOT DEFINED ZEPHYR_BUILD_DIR (
	ECHO "Environment variable 'ZEPHYR_BUILD_DIR' used to define GNU ARM toolchain directory is set to default value ('%ZEPHYR_BUILD_DIR_DEFAULT%')"
    SET ZEPHYR_BUILD_DIR="%ZEPHYR_BUILD_DIR_DEFAULT%"
) ELSE ECHO "Environment variable 'ZEPHYR_BUILD_DIR' used to define GNU ARM toolchain directory is set to '%ZEPHYR_BUILD_DIR%' value"

SET "ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb"

SET "BOARD=stm32f746g_disco"