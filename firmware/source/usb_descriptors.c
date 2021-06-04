/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/*
 * 2021: Modification by execuc for this project.
 */

#include "tusb.h"
#include "pico/unique_id.h"
#include "board_config.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )

#if BOARD == FEATHER
  #define USB_MFG "Adafruit"
  #define USB_PRD "Feather RP2040 U2IF"
  #define USB_VID 0x239A
  #define USB_PID 0x00F1
#elif BOARD == ITSYBITSY
  #define USB_MFG "Adafruit"
  #define USB_PRD "ItsyBitsy RP2040 U2IF"
  #define USB_VID 0x239A
  #define USB_PID 0x00FD
#elif BOARD == QTPY
  #define USB_MFG "Adafruit"
  #define USB_PRD "QT Py RP2040 U2IF"
  #define USB_VID 0x239A
  #define USB_PID 0x00F7
#elif BOARD == QT2040_TRINKEY
  #define USB_MFG "Adafruit"
  #define USB_PRD "QT2040 Trinkey U2IF"
  #define USB_VID 0x239A
  #define USB_PID 0x0109
#elif BOARD == PICO
  #define USB_MFG "Pico"
  #define USB_PRD "U2IF"
  #define USB_VID 0xCAFE
  #define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
                            _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4) )
#else
  #warning "Please define board type"
#endif

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
        {
                .bLength            = sizeof(tusb_desc_device_t),
                .bDescriptorType    = TUSB_DESC_DEVICE,
                .bcdUSB             = 0x0200,
                /*.bDeviceClass       = 0x00,
                .bDeviceSubClass    = 0x00,
                .bDeviceProtocol    = 0x00,*/
                // test
                .bDeviceClass       = TUSB_CLASS_MISC,
                .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
                .bDeviceProtocol    = MISC_PROTOCOL_IAD,
                
                .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

                .idVendor           = USB_VID,
                .idProduct          = USB_PID,
                .bcdDevice          = 0x0100,

                .iManufacturer      = 0x01,
                .iProduct           = 0x02,
                .iSerialNumber      = 0x03,

                .bNumConfigurations = 0x01
        };

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void) {
    return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

uint8_t const desc_hid_report[] =
        {
                TUD_HID_REPORT_DESC_GENERIC_INOUT(CFG_TUD_HID_BUFSIZE)
        };

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(void) {
    return desc_hid_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
/*
enum {
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};*/
enum {
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

// Ajout
#define EPNUM_CDC_NOTIF   0x81
#define EPNUM_CDC_OUT     0x02
#define EPNUM_CDC_IN      0x82



#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_INOUT_DESC_LEN)

#define EPNUM_HID   0x03

uint8_t const desc_configuration[] =
        {
                // Config number, interface count, string index, total length, attribute, power in mA
                TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, /*100*/250),

                // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
                TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64),

                // Interface number, string index, protocol, report descriptor len, EP In & Out address, size & polling interval
                TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 5, HID_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_HID,
                                         0x80 | EPNUM_HID, CFG_TUD_HID_BUFSIZE, /*10*/ 1)
        };

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void) index; // for multiple configurations
    return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const *string_desc_arr[] =
        {
                (const char[]) {0x09, 0x04}, // 0: is supported language is English (0x0409)
                USB_MFG,                     // 1: Manufacturer
                USB_PRD,              // 2: Product
                "",                      // 3: Serials, should use chip ID
                "CDC Streamed data channel"  ,           // 4: CDC Interface
                "HID Command channel",                     // 5: HID
        };

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid;

    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else if (index == 3) {
        pico_unique_board_id_t board_id;
        pico_get_unique_board_id(&board_id);
        chr_count = 0;
        _desc_str[1 + chr_count++] = '0';
        _desc_str[1 + chr_count++] = 'x';
        for (uint i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; ++i) {
            static char tmp[1];
            sprintf(tmp, "%02X", board_id.id[i]);
            _desc_str[1 + chr_count++] = tmp[0];
            _desc_str[1 + chr_count++] = tmp[1];
        }
    } else {
        // Convert ASCII string into UTF-16

        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0]))) return NULL;

        const char *str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        if (chr_count > 31) chr_count = 31;

        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return _desc_str;
}
