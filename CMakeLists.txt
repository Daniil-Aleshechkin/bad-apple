cmake_minimum_required(VERSION 3.12)
###############################################################################
# arm-none-eabi toolchain
include(./gcc-arm-none-eabi.cmake)
###############################################################################
# Project configuration
# set(PROJECT_NAME firmware)
project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
###############################################################################
# FreeRTOS configuration
set(FREERTOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS")
set(FREERTOS_CONFIG_FILE_DIRECTORY "${FREERTOS_DIR}")
set(FREERTOS_HEAP "4")
set(FREERTOS_PORT "GCC_ARM_CM3")
add_subdirectory("${FREERTOS_DIR}/Source")
###############################################################################
# STM32 specific configuration
set(MCU_MODEL STM32F103xB)
add_compile_definitions(STM32F1xx)

# Compiler and linker settings
set(CPU_PARAMETERS
    -mcpu=cortex-m3
    -mthumb
    -DSTM32F103xB
    -DF_CPU=72000000L
)
set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/core/startup_stm32f103xb.S)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/core/STM32F103XB_FLASH.ld)
###############################################################################
# Source files
file(GLOB_RECURSE DEVICE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/core/src/*.c)
file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch]pp)
file(GLOB_RECURSE LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.[ch]pp)

# Executable
add_executable(${PROJECT_NAME}
    ${DEVICE_SOURCES}
    ${PROJECT_SOURCES}
    ${LIB_SOURCES}
    ${STARTUP_SCRIPT}
)

# Include directories
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/usart
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/cli
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/com-14646
    ${FREERTOS_DIR}/Source/include
    ${FREERTOS_DIR}
)

# FreeRTOS link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE freertos_kernel)
###############################################################################
# Compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wdouble-promotion
    -Wformat=2 -Wformat-truncation
    -Wundef
    -fno-common
    -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wconversion
        -Wno-volatile
        -Wold-style-cast
        -Wuseless-cast
        -Wsuggest-override>
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>
    $<$<CONFIG:Release>:-Os -g0 -ffunction-sections -fdata-sections>)

# Linker options
target_link_options(${PROJECT_NAME} PRIVATE
    -T${MCU_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    # -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -Wl,-Map=${PROJECT_NAME}.map
    $<$<VERSION_GREATER:$<C_COMPILER_VERSION>,10.3.1>:-Wl,--no-warn-rwx-segments>
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -Wl,--end-group
    -Wl,--print-memory-usage)

message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
###############################################################################
# Produce .bin .map .hex files
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin)
