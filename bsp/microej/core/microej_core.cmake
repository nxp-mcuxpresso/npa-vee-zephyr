include_guard()
message("microej/core component is included.")

target_sources(app PRIVATE
${CMAKE_CURRENT_LIST_DIR}/src/device_natives.c
${CMAKE_CURRENT_LIST_DIR}/src/LLBSP_NXP.c
${CMAKE_CURRENT_LIST_DIR}/src/LLMJVM_ZephyrOS.c
${CMAKE_CURRENT_LIST_DIR}/src/microej_main.c
${CMAKE_CURRENT_LIST_DIR}/src/microej_time.c
)

target_include_directories(app PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
