#ifndef _PICO_INTERFACES_BOARD_H
#define _PICO_INTERFACES_BOARD_H

//#include "pins.h"
#include "board_config.h"

#define HID_CMD_SIZE 64
#define HID_RESPONSE_SIZE 64

// namespace Pin {
//     enum ID {
//         // GPIO
//         GP_2 = 2,
//         GP_3 = 3,
//         GP_6 = 6,
//         GP_9 = 9,
//         GP_7 = 7,
//         GP_8 = 8,
//         GP_21 = 21,
//         GP_22 = 22,
//         GP_28 = 28,

//         // Analog
//         GP26_ADC0 = 26,
//         GP27_ADC1 = 27,

//         // UART0
//         GP0_UART0_TX = U2IF_UART0_TX,
//         GP1_UART0_RX = U2IF_UART0_RX,

//         // SPI0
//         GP18_SPI0_CK = U2IF_SPI0_CK,
//         GP19_SPI0_MOSI = U2IF_SPI0_MOSI,
//         GP16_SPI0_MISO = U2IF_SPI0_MISO,
//         GP17_SPI0_CS1 = 17,
//         GP20_SPI0_CS2 = 20,

//         // SPI1
//         GP10_SPI1_CK = U2IF_SPI1_CK,
//         GP11_SPI1_MOSI = U2IF_SPI1_MOSI,
//         GP12_SPI1_MISO = U2IF_SPI1_MISO,
//         GP13_SPI1_CS1 = 13,

//         // I2C0
//         GP4_I2C0_SDA = U2IF_I2C0_SDA,
//         GP5_I2C0_SCL = U2IF_I2C0_SCL,

//         // I2C1
//         GP14_I2C1_SDA = U2IF_I2C1_SDA,
//         GP15_I2C1_SCL = U2IF_I2C1_SCL
//     };
// }

enum CmdStatus {
    OK = 0x01,
    NOK = 0x02,
    NOT_FINISHED = 0xFE, // INTERNAL
    NOT_CONCERNED = 0xFF
};

enum IRQ_EVENT {
    EVENT_NONE = 0x00,
    EVENT_RISING = 0x01,
    EVENT_FALLING = 0x02,
};

// Each report size is is 64 bytes.
// In general reports return | Report::ID | CmdStatus::OK or CmdStatus::NOK |
namespace Report {
    enum ID {
        // SYSTEM
        // | RESET | => | RESET | CmdStatus::OK | then system reset
        SYS_RESET = 0x10,
        // | SYS_GET_SN | => | SYS_GET_SN | CmdStatus::OK | SN * 8 bytes |
        SYS_GET_SN = 0x11,
        // | SYS_GET_VN | => | SYS_GET_VN | CmdStatus::OK | MAJOR VERSION | MINOR VERSION | PATCH VERSION |
        SYS_GET_VN = 0x12,

        // GPIO
        // | GPIO_INIT_PIN | GP NUMBER | DIRECTION (0=INPUT; 1=OUTPUT) | PULL (0=NONE; 1=PULLUP; 2=PULLDOWN)
        GPIO_INIT_PIN = 0x20,
        // | GPIO_SET_VALUE | GP NUMBER | VALUE (0=LOW; 1=HIGH) |
        GPIO_SET_VALUE = 0x21,
        // | GPIO_GET_VALUE | GP NUMBER | => | GPIO_GET_VALUE | CmdStatus::OK | GP NUMBER | VALUE (0=LOW; 1=HIGH) |
        GPIO_GET_VALUE = 0x22,
        // | GPIO_SET_IRQ | GP NUMBER | EVENT (EVENT_NONE or (EVENT_RISING | EVENT_FALLING)) | DEBOUNCED (0:False; 1:True)
        GPIO_SET_IRQ = 0x23,
        // | GPIO_GET_IRQ | => | GPIO_GET_IRQ | CmdStatus::OK | IRQ_NUMBER | IRQ(GP NUMBER[0..5] | EVENT (EVENT_RISING | EVENT_FALLING) [6..7]) * IRQ_NUMBER|
        GPIO_GET_IRQ = 0x24,

        // GROUP GPIO: pins must be initialized separately
        // | GROUP_GPIO_SET_VALUES | GP MASK[4] L.Endian | VALUES[4] (0=LOW; 1=HIGH) L.Endian |
        GROUP_GPIO_SET_VALUES = 0x28,
        // | GROUP_GPIO_GET_ALL_VALUES | => | GPIO_GET_VALUE | CmdStatus::OK | VALUES[4] (0=LOW; 1=HIGH) L.Endian |
        GROUP_GPIO_GET_ALL_VALUES = 0x29,

        // PWM
        // | PWM_INIT_PIN | GP NUMBER | => | PWM_INIT_PIN |  CmdStatus::OK|NOK | GP NUMBER | SLICE NUMBER | CHANNEL | err : 0x01: Already used slice
        PWM_INIT_PIN = 0x30,
        // | PWM_DEINIT_PIN | GP NUMBER |
        PWM_DEINIT_PIN = 0x31,
        // | PWM_SET_FREQ | GP NUMBER | FREQ[4] L.Endian | => | PWM_SET_FREQ | CmdStatus::OK|NOK | GP NUMBER | err: 0x01 : Different freq on same slice; 0x02: Freq too low; 0x03:Fre too high|
        PWM_SET_FREQ = 0x32,
        // | PWM_GET_FREQ |GP NUMBER |  => | PWM_GET_FREQ |  CmdStatus::OK | GP NUMBER | FREQ[4] L.Endian  |
        PWM_GET_FREQ = 0x33,
        // | PWM_SET_DUTY_U16 | GP NUMBER | DUTY | DUTY >> 8 | => | PWM_SET_DUTY_U16 | CmdStatus::OK|NOK |
        PWM_SET_DUTY_U16 = 0x34,
        // | PWM_GET_DUTY_U16 | GP NUMBER => | PWM_GET_DUTY_U16 | CmdStatus::OK|NOK | GP NUMBER | DUTY | DUTY >> 8 |
        PWM_GET_DUTY_U16 = 0x35,
        // | PWM_SET_DUTY_NS | GP NUMBER | DUTY[4] L.Endian | => | PWM_SET_DUTY_NS | CmdStatus::OK|NOK | GP NUMBER | err: 0x01 = Too large duty |
        PWM_SET_DUTY_NS = 0x36,
        // | PWM_GET_DUTY_NS | GP NUMBER | => | PWM_GET_DUTY_NS | CmdStatus::OK|NOK | GP NUMBER | DUTY[4] L.Endian |
        PWM_GET_DUTY_NS = 0x37,

        // ADC
        // | ADC_INIT_PIN | GP NUMBER |
        ADC_INIT_PIN = 0x40,
        // | ADC_GET_VALUE | GP NUMBER | => | ADC_GET_VALUE | CmdStatus::OK|NOK | GP NUMBER | VALUE[2] L.Endian (12bits=4096) |
        ADC_GET_VALUE = 0x41,

        // UART0
        // | UART0_INIT | MODE (NOT USED) | BAUDRATE[4] L.Endian |
        UART0_INIT = 0x50,
        // | UART0_DEINIT |
        UART0_DEINIT = 0x51,
        // | UART0_WRITE | NB_BYTES[1] | PAYLOAD |=> First | UART_WRITE | CmdStatus::OK |
        UART0_WRITE = 0x52,
        // | UART0_READ | => First | UART_READ_FROM_UART | CmdStatus::OK | NB_BYTES[1] | PAYLOAD |
        UART0_READ = 0x53,

        // UART1: 0xCX
        UART0_UART1_OFFSET = 0x70,
        UART1_INIT = UART0_INIT + UART0_UART1_OFFSET,
        UART1_DEINIT = UART0_DEINIT + UART0_UART1_OFFSET,
        UART1_WRITE = UART0_WRITE + UART0_UART1_OFFSET,
        UART1_READ = UART0_READ + UART0_UART1_OFFSET,

        // SPI0
        // | SPI0_INIT | MODE (To implement) | BAUDRATE[4] L.Endian |
        SPI0_INIT = 0x60,
        // | SPI0_DEINIT |
        SPI0_DEINIT = 0x61,
        // | SPI0_WRITE | NB_BYTES[1] | PAYLOAD |
        SPI0_WRITE = 0x62,
        // | I2C0_READ | WRITE_BYTE | NB_BYTES[1] | => | SPI0_READ | CmdStatus::OK | PAYLOAD |
        SPI0_READ = 0x63,
        // | SPI0_WRITE_FROM_UART | NB_BYTES[4] L.Endian | => First | SPI0_WRITE_FROM_UART | CmdStatus::OK | and after the CDC stream | SPI0_WRITE_FROM_UART | CmdStatus::OK |
        SPI0_WRITE_FROM_UART = 0x64,

        // SPI1: 0x7X
        SPI0_SPI1_OFFSET = 0x10,
        SPI1_INIT = SPI0_INIT + SPI0_SPI1_OFFSET,
        SPI1_DEINIT = SPI0_DEINIT + SPI0_SPI1_OFFSET,
        SPI1_WRITE = SPI0_WRITE + SPI0_SPI1_OFFSET,
        SPI1_READ = SPI0_READ + SPI0_SPI1_OFFSET,
        SPI1_WRITE_FROM_UART = SPI0_WRITE_FROM_UART + SPI0_SPI1_OFFSET,

        // I2C0
        // | I2C0_INIT | PULLUP(1=True) | BAUDRATE[4] L.Endian |
        I2C0_INIT = 0x80,
        // | I2C0_DEINIT |
        I2C0_DEINIT = 0x81,
        // | I2C0_WRITE | ADDR | SEND_STOP | NB_BYTES[4] L.Endian | PAYLOAD |
        I2C0_WRITE = 0x82,
        // | I2C0_READ | ADDR | SEND_STOP | NB_BYTES => | I2C0_READ | CmdStatus::OK | PAYLOAD |
        I2C0_READ = 0x83,
        // | I2C0_WRITE_FROM_UART | ADDR | NB_BYTES[4] L.Endian | => First | I2C0_WRITE_FROM_UART | CmdStatus::OK | and after the CDC stream | I2C0_WRITE_FROM_UART | CmdStatus::OK |
        I2C0_WRITE_FROM_UART = 0x84,

        // I2C1: 0x9X
        I2C0_I2C1_OFFSET = 0x10,
        I2C1_INIT = I2C0_INIT + I2C0_I2C1_OFFSET,
        I2C1_DEINIT = I2C0_DEINIT + I2C0_I2C1_OFFSET,
        I2C1_WRITE = I2C0_WRITE + I2C0_I2C1_OFFSET,
        I2C1_READ = I2C0_READ + I2C0_I2C1_OFFSET,
        I2C1_WRITE_FROM_UART = I2C0_WRITE_FROM_UART + I2C0_I2C1_OFFSET,

        // WS2812B (LED)
        // | WS2812B_INIT |
        WS2812B_INIT = 0xA0,
        // | WS2812B_DEINIT |
        WS2812B_DEINIT = 0xA1,
        // | WS2812B_WRITE | NB_BYTES[4] L.Endian (== NB_LED *4)| => First | WS2812B_WRITE | CmdStatus::OK|NOK | err:0x01 (Too many led for the firmware), err:0x02 (Transfer already in progress) |
        // ... and after the CDC stream (when transfer to led starting) | WS2812B_WRITE | CmdStatus::OK |
        WS2812B_WRITE = 0xA2,

        // I2S
        // |I2S_INIT | MODE (0x01: mono, 0x02: stereo) => Mode not implemented,only stereo 16bit/channel |
        I2S_INIT = 0xB0,
        // |I2S_DEINIT|
        I2S_DEINIT = 0xB1,
        // |I2S_SET_FREQ | FREQ[4] L.Endian |
        I2S_SET_FREQ = 0xB2,
        // | I2S_WRITE_BUFFER | BUFFER_SIZE[4] L.Endian (MAX=4000bytes)| => First | I2S_WRITE_BUFFER | CmdStatus::OK | and after the CDC stream | I2S_WRITE_BUFFER | CmdStatus::OK |
        I2S_WRITE_BUFFER = 0xB3,

        // HUB75: 0xDX
        // | HUB75_INIT | WIDTH | HEIGHT |
        HUB75_INIT = 0xD0,
        // | HUB75_DEINIT |
        HUB75_DEINIT = 0xD1,
        // | HUB75_WRITE | NB_BYTES[4] L.Endian (WIDTH * HEIGHT)| => First | HUB75_WRITE | CmdStatus::OK|NOK | 
        // ... and after the CDC stream (when transfer to led starting) | HUB75_WRITE | CmdStatus::OK |
        HUB75_WRITE = 0xD2,
    };
}


#endif

