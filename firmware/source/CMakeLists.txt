cmake_minimum_required(VERSION 3.12)

# Set PICO_SDK_PATH to parent directory
get_filename_component(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../pico-sdk" ABSOLUTE)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(u2if VERSION "0.6.0")

# initialize the Pico SDK
pico_sdk_init()


set(BOARD "PICO" CACHE STRING "BaseName chosen by the user at CMake configure time")
set_property(CACHE BOARD PROPERTY STRINGS PICO FEATHER ITSYBITSY QTPY QT2040_TRINKEY MACROPAD)
message(STATUS "Selected board:'${BOARD}'")

# might help for feather?
#add_compile_definitions(PICO_DEFAULT_BOOT_STAGE2_FILE=${CMAKE_CURRENT_SOURCE_DIR}/../pico-sdk/src/rp2_common/boot_stage2/boot2_generic_03h.S)

if (NOT DEFINED ADC_ENABLED)
        set(ADC_ENABLED 1)
endif()

if (NOT DEFINED PWM_ENABLED)
        set(PWM_ENABLED 1)
endif()

if (NOT DEFINED I2S_ALLOW)
        set(I2S_ALLOW 0)
endif()

if (NOT DEFINED HUB75_ALLOW)
        set(HUB75_ALLOW 0)
endif()

if (NOT DEFINED WS2812_SIZE)
        set(WS2812_SIZE 1000)
endif()

if (NOT DEFINED WS2812_ENABLED)
        set(WS2812_ENABLED 1)
endif()

if (NOT DEFINED HUB75_MAX_LEDS)
        set(HUB75_MAX_LEDS 128*64)
endif()


configure_file("${PROJECT_SOURCE_DIR}/board_config.h.in" "${PROJECT_SOURCE_DIR}/board_config.h")

FILE(GLOB InterfaceSources interfaces/*.cpp)

add_executable(u2if
        main.cpp
        usb_descriptors.c
	ModeActivity.cpp
	${InterfaceSources}
        )


# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(u2if ${CMAKE_CURRENT_LIST_DIR}/interfaces/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/interfaces/)
pico_generate_pio_header(u2if ${CMAKE_CURRENT_LIST_DIR}/interfaces/audio_i2s.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/interfaces/)
pico_generate_pio_header(u2if ${CMAKE_CURRENT_LIST_DIR}/interfaces/hub75.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/interfaces/)

target_include_directories(u2if PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(u2if PRIVATE pico_stdlib pico_unique_id tinyusb_device tinyusb_board 
hardware_i2c hardware_watchdog hardware_pwm hardware_adc hardware_spi
hardware_pio hardware_dma pico_sync pico_multicore)

pico_enable_stdio_usb(u2if 0)
pico_enable_stdio_uart(u2if 0) # Set to 1, call stdio_init_all(); to make printf for debugging puroposes

pico_add_extra_outputs(u2if)

# add url via pico_set_program_url
#example_auto_set_url(u2if)
