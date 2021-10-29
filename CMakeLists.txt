cmake_minimum_required(VERSION 3.20)

include(pico_sdk_import.cmake)

project(multicore_test)

pico_sdk_init()

add_executable(multicore 
    multicore.c
)

target_link_libraries(multicore pico_stdlib 
    hardware_uart 
    hardware_gpio 
    hardware_pwm
    pico_multicore)

pico_enable_stdio_usb(multicore 1)
pico_enable_stdio_uart(multicore 0)

pico_add_extra_outputs(multicore)