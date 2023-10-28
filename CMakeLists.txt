cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_PATH "/YOUR_PICO_SDK_PATH/pico-sdk")

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(modsample C CXX ASM)

# Initialise the Pico SDK
pico_sdk_init()

add_executable(modsample modsample.cpp)

pico_set_program_name(modsample "modsample")
pico_set_program_version(modsample "0.1")

pico_enable_stdio_uart(modsample 1)
pico_enable_stdio_usb(modsample 0)

target_link_libraries(modsample
        pico_stdlib
        hardware_dma
        hardware_pio
        hardware_timer
        hardware_clocks
        hardware_pwm
)

pico_add_extra_outputs(modsample)
