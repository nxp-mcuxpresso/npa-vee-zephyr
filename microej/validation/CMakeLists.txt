cmake_minimum_required(VERSION 3.2)

project(microej_comp C)

SET(PROJDIRPATH ${CMAKE_CURRENT_SOURCE_DIR})

add_library(microej_java dummy.c)

SET(FP ${PROJDIRPATH}/../front-panel)
SET(CONF ${PROJDIRPATH}/../vee-port-configuration)
SET(PLAT ${PROJDIRPATH}/../plat/source)
SET(VAL ${PROJDIRPATH}/../validation)

SET(VAL_CORE ${VAL}/core/java-testsuite-runner-core)
SET(VAL_UI ${VAL}/ui/ui3/java-testsuite-runner-ui3)
SET(VAL_FS ${VAL}/fs/java-testsuite-runner-fs)
SET(VAL_NET ${VAL}/net/java-testsuite-runner-net)
SET(VAL_SECURITY ${VAL}/security/java-testsuite-runner-security)
SET(VAL_SSL ${VAL}/ssl/java-testsuite-runner-ssl)

SET(ECLIPSE_HOME $ENV{ECLIPSE_HOME_VAR})
SET(MODULE_REPOSITORY_SETTINGS_FILE $ENV{MODULE_REPOSITORY_SETTINGS_FILE_VAR})
SET(MICROEJ_BUILDKIT_PATH $ENV{MICROEJ_BUILDKIT_PATH_VAR})


if (NOT DEFINED MODULE_REPOSITORY_SETTINGS_FILE)
    SET(MODULE_REPOSITORY_SETTINGS_FILE_SUB "")
else()
    SET(MODULE_REPOSITORY_SETTINGS_FILE_SUB "--module-repository-settings-file=${MODULE_REPOSITORY_SETTINGS_FILE}")
endif()

if (NOT DEFINED CUR_VAL)
SET(CUR_VAL CORE_VAL)
endif()

add_dependencies(microej_java ${CUR_VAL})

if (NOT DEFINED JUSAGE)
SET(JUSAGE "eval")
endif()
message("USAGE: ${JUSAGE}")

if (NOT DEFINED TTY)
SET(TTY "/dev/ttyACM0")
endif()

if (NOT DEFINED VERB)
SET(QUIET "--quiet")
else()
SET(QUIET "")
endif()

file(GLOB_RECURSE VAL_CORE_SRCS ${VAL_CORE}/*.java)
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/CORE.guard" WORKING_DIRECTORY "${VAL_CORE}"
                   COMMAND "${MICROEJ_BUILDKIT_PATH}/bin/mmm" "publish" ${QUIET}
                   "-Declipse.home=${ECLIPSE_HOME}"
                   "${MODULE_REPOSITORY_SETTINGS_FILE_SUB}"
                   "-Dcom.microej.platformbuilder.architecture.usage=${JUSAGE}"
                   "-Dplatform-loader.target.platform.dir=${PLAT}"
                   "-Dplatform-launcher.platform.dir=${PLAT}"
                   DEPENDS  ${VAL_CORE_SRCS} ${VAL_CORE}/module.ivy )
add_custom_target(CORE_VAL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/CORE.guard")


file(GLOB_RECURSE VAL_UI_SRCS ${VAL_UI}/*.java)
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/UI.guard" WORKING_DIRECTORY "${VAL_UI}"
                   COMMAND "${MICROEJ_BUILDKIT_PATH}/bin/mmm" "publish" ${QUIET}
                   "-Declipse.home=${ECLIPSE_HOME}"
                   "${MODULE_REPOSITORY_SETTINGS_FILE_SUB}"
                   "-Dcom.microej.platformbuilder.architecture.usage=${JUSAGE}"
                   "-Dplatform-loader.target.platform.dir=${PLAT}"
                   "-Dplatform-launcher.platform.dir=${PLAT}"
                   DEPENDS  ${VAL_UI_SRCS} ${VAL_UI}/module.ivy )
add_custom_target(UI_VAL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/UI.guard")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/s2s.guard" WORKING_DIRECTORY "${PLAT}"
                   COMMAND "java" "-Djava.library.path=resources/os/${CMAKE_SYSTEM_NAME}64"
                   "-cp"
                   "'tools/*'"
                   "com.is2t.serialsockettransmitter.SerialToSocketTransmitterBatch"
                   "-port" "${TTY}" "-baudrate" "115200" "-databits" "8"
                   "-parity" "none" "-stopbits" "1" "-hostPort" "5555"
                   COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_BINARY_DIR}/s2s.guard")
add_custom_target(S2S DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/s2s.guard")

set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${FP}/fp/target~)
set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${CONF}/target~)
set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${PROJDIRPATH}/../bsp/platform/)
