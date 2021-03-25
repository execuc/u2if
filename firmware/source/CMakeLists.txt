cmake_minimum_required(VERSION 3.12)

# Set PICO_SDK_PATH to parent directory
get_filename_component(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../pico-sdk" ABSOLUTE)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(u2if VERSION "0.1.0")

# initialize the Pico SDK
pico_sdk_init()

# Enable wanted peripherals
set(I2C0_ENABLE 1)
set(I2C1_ENABLE 1)
set(SPI0_ENABLE 1)
set(SPI1_ENABLE 1)
set(UART_ENABLE 1)
set(PWM_ENABLE 1)
set(ADC_ENABLE 1)
set(WS2812_SIZE 1000) #0 to disable
configure_file("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_SOURCE_DIR}/config.h")

FILE(GLOB InterfaceSources interfaces/*.cpp)

add_executable(u2if
        main.cpp
        usb_descriptors.c
	ModeActivity.cpp
	${InterfaceSources}
        )

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(u2if ${CMAKE_CURRENT_LIST_DIR}/interfaces/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/interfaces/)

target_include_directories(u2if PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(u2if PRIVATE pico_stdlib pico_unique_id tinyusb_device tinyusb_board 
hardware_i2c hardware_watchdog hardware_pwm hardware_adc hardware_spi
hardware_pio hardware_dma)

pico_enable_stdio_usb(u2if 0)
pico_enable_stdio_uart(u2if 0) # Set to 1, call stdio_init_all(); to make printf for debugging puroposes

pico_add_extra_outputs(u2if)

# add url via pico_set_program_url
#example_auto_set_url(u2if)
