cmake_minimum_required(VERSION 3.20.0)

project(microej_comp C)

SET(PROJDIRPATH ${CMAKE_CURRENT_SOURCE_DIR})

add_library(microej_java dummy.c)

add_dependencies(microej_java FP CONFIGU JAVA_APPS)

SET(FP ${PROJDIRPATH}/front-panel)
SET(MOCK ${PROJDIRPATH}/mock)
SET(CONF ${PROJDIRPATH}/vee-port-configuration)
SET(PLAT ${PROJDIRPATH}/plat/source)
SET(VAL ${PROJDIRPATH}/validation)
SET(PLATFORMS ${PROJDIRPATH}/plats)

SET(ECLIPSE_HOME $ENV{ECLIPSE_HOME_VAR})
SET(MODULE_REPOSITORY_SETTINGS_FILE $ENV{MODULE_REPOSITORY_SETTINGS_FILE_VAR})
SET(MICROEJ_BUILDKIT_PATH $ENV{MICROEJ_BUILDKIT_PATH_VAR})

if (NOT DEFINED MODULE_REPOSITORY_SETTINGS_FILE)
    SET(MODULE_REPOSITORY_SETTINGS_FILE_SUB "")
else()
    SET(MODULE_REPOSITORY_SETTINGS_FILE_SUB "--module-repository-settings-file=${MODULE_REPOSITORY_SETTINGS_FILE}")
endif()

if (NOT DEFINED JUSAGE)
    SET(JUSAGE "eval")
endif()
message("USAGE: ${JUSAGE}")

if (NOT DEFINED VERB)
    SET(QUIET "--quiet")
else()
    SET(QUIET "")
endif()

if (NOT DEFINED JMAIN)
    if (CONFIG_TENSORFLOW_LITE_MICRO)
        SET(JMAIN "com.nxp.aiSample.AiMain")
        SET(APP ${PROJDIRPATH}/apps/AiSample)
    elseif (CONFIG_DISPLAY)
        SET(JMAIN "com.nxp.simpleGFX.SimpleGFX")
        SET(APP ${PROJDIRPATH}/apps/SimpleGFX)
    else()
        SET(JMAIN "com.nxp.HelloWorld.HelloWorld")
        SET(APP ${PROJDIRPATH}/apps/HelloWorld)
    endif()
endif()
message("MAIN: ${JMAIN}")

if (DEFINED JPATH)
    SET(APP ${JPATH})
    message("APP: ${JPATH}")
endif()

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

if (CONFIG_DISPLAY)
  SET(FP_FILE "")
  SET(FP_SKIN_FILE "")

  if (DEFINED SHIELD)
      SET(FP_FILE ${FP}/src/main/resources/front-panels/${SHIELD}.fp)
      SET(FP_SKIN_FILE ${FP}/src/main/resources/front-panels/${SHIELD}.jpeg)
  elseif (DEFINED BOARD)
      SET(FP_FILE ${FP}/src/main/resources/front-panels/${BOARD}.fp)
      SET(FP_SKIN_FILE ${FP}/src/main/resources/front-panels/${BOARD}.jpeg)
  endif()

  if (EXISTS ${FP_FILE})
      message("Configuring simulation FrontPanel with: ${FP_FILE}")
      configure_file(${FP_FILE} ${FP}/src/main/resources/platform.fp COPYONLY)
      configure_file(${FP_SKIN_FILE} ${FP}/src/main/resources/skin.jpeg COPYONLY)
  else()
      message(WARNING "Configuring simulation FrontPanel missing config file for '${FP_FILE}'")
      message(WARNING "Please add relevant file in ${FP}/src/main/resources/front-panels")
  endif()
endif()

if (${CONFIG_CPU_CORTEX_M7})
    configure_file(${CONF}/arch/CM7.platform ${CONF}/NXP.platform COPYONLY)
    configure_file(${CONF}/arch/module.properties.CM7 ${CONF}/module.properties COPYONLY)
elseif (${CONFIG_CPU_CORTEX_M33})
    configure_file(${CONF}/arch/CM33.platform ${CONF}/NXP.platform COPYONLY)
    configure_file(${CONF}/arch/module.properties.CM33 ${CONF}/module.properties COPYONLY)
elseif (${CONFIG_CPU_CORTEX_M4})
    configure_file(${CONF}/arch/CM4.platform ${CONF}/NXP.platform COPYONLY)
    configure_file(${CONF}/arch/module.properties.CM4 ${CONF}/module.properties COPYONLY)
else()
    message(FATAL_ERROR "CPU NOT supported")
endif()

if (${CONFIG_PARTIAL_BUFFER})
    configure_file(${CONF}/dropins/scripts/init-partial-buffer/partial-buffer-init.xml.ON ${CONF}/dropins/scripts/init-partial-buffer/partial-buffer-init.xml COPYONLY)
else()
    configure_file(${CONF}/dropins/scripts/init-partial-buffer/partial-buffer-init.xml.OFF ${CONF}/dropins/scripts/init-partial-buffer/partial-buffer-init.xml COPYONLY)
endif()

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/clean.guard"
                  COMMAND ${CMAKE_COMMAND} -E rm -rf ${PROJDIRPATH}/plat
                  COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_BINARY_DIR}/clean.guard")
add_custom_target(CLEAN_PLAT
                  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/clean.guard")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/fp.guard" WORKING_DIRECTORY "${FP}"
                   COMMAND "${MICROEJ_BUILDKIT_PATH}/bin/mmm" "publish" ${QUIET}
                   "-Declipse.home=${ECLIPSE_HOME}"
                   "-Dcom.microej.platformbuilder.architecture.usage=${JUSAGE}"
                   "${MODULE_REPOSITORY_SETTINGS_FILE_SUB}"
                   DEPENDS CLEAN_PLAT
                   COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_BINARY_DIR}/fp.guard")

add_custom_target(FP DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/fp.guard")


add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/mock.guard" WORKING_DIRECTORY "${MOCK}"
                   COMMAND "${MICROEJ_BUILDKIT_PATH}/bin/mmm" "publish" ${QUIET}
                   "-Declipse.home=${ECLIPSE_HOME}"
                   "-Dcom.microej.platformbuilder.architecture.usage=${JUSAGE}"
                   "${MODULE_REPOSITORY_SETTINGS_FILE_SUB}"
                   DEPENDS FP
                   COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_BINARY_DIR}/mock.guard")

add_custom_target(MOCK DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/mock.guard")



add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/configuration.guard" WORKING_DIRECTORY "${CONF}"
                   COMMAND "${MICROEJ_BUILDKIT_PATH}/bin/mmm" "publish" ${QUIET}
                   "-Declipse.home=${ECLIPSE_HOME}"
                   "-Dcom.microej.platformbuilder.architecture.usage=${JUSAGE}"
                   "${MODULE_REPOSITORY_SETTINGS_FILE_SUB}"
                   DEPENDS ${CONF}/module.ivy  MOCK
                   COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_BINARY_DIR}/configuration.guard")
add_custom_target(CONFIGU DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/configuration.guard")

file(GLOB_RECURSE JAVA_SRCS ${APP}/*.java)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/apps.guard" WORKING_DIRECTORY "${APP}"
                   COMMAND "${MICROEJ_BUILDKIT_PATH}/bin/mmm" "publish" ${QUIET}
                   "-Declipse.home=${ECLIPSE_HOME}"
                   "${MODULE_REPOSITORY_SETTINGS_FILE_SUB}"
                   "-Dcom.microej.platformbuilder.architecture.usage=${JUSAGE}"
                   "-Dplatform-loader.target.platform.dir=${PLAT}"
                   "-Dplatform-launcher.platform.dir=${PLAT}"
                   "-Dapplication.main.class=${JMAIN}"
                   DEPENDS  ${JAVA_SRCS} ${APP}/module.ivy CONFIGU
                   COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_BINARY_DIR}/apps.guard")
add_custom_target(JAVA_APPS DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/apps.guard")
add_custom_target(microejapp.o DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/apps.guard")
add_custom_target(microejruntime.a DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/apps.guard")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/jrun.guard" WORKING_DIRECTORY "${APP}"
                   COMMAND "${MICROEJ_BUILDKIT_PATH}/bin/mmm" "run" ${QUIET}
                   "-Declipse.home=${ECLIPSE_HOME}"
                   "${MODULE_REPOSITORY_SETTINGS_FILE_SUB}"
                   "-Dcom.microej.platformbuilder.architecture.usage=${JUSAGE}"
                   "-Dplatform-loader.target.platform.dir=${PLAT}"
                   "-Dplatform-launcher.platform.dir=${PLAT}"
                   "-Dapplication.main.class=${JMAIN}")

add_custom_target(java-run DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/apps.guard" "${CMAKE_CURRENT_BINARY_DIR}/jrun.guard")


set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${FP}/fp/target~)
set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${APP}/target~)
set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${CONF}/target~)
set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${PROJDIRPATH}/../bsp/platform/)
set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${PROJDIRPATH}/plat/)

set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${CONF}/dropins/scripts/init-partial-buffer/partial-buffer-init.xml)
set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${CONF}/NXP.platform)
set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${CONF}/module.properties)
set_property(TARGET microej_java APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${PROJDIRPATH}/plat)
