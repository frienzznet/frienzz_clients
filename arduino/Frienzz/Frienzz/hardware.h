#ifndef HARDWARE_H
#define HARDWARE_H

#include "Adafruit_GFX.h"
#include "systemversion.h"

// some help construct to compile the right display and touchscreen settings depending on the ESP USB connector direction in the final housing position
#define NORTH_UP 1
#define SOUTH_UP 2
#define WEST_UP 3
#define EAST_UP 4

// ------------------------------------ HW_NODEMCU32 -----------------

#if HARDWARE_ID == HW_NODEMCU32
// Code to include if HW_NODEMCU32 is defined

#include "Adafruit_GC9A01A.h"

#define TFT_DIAMETER 240
#define TFT_ROUND true
#define TFT_ROTATION 0

#define HAS_ROTARY_ENCODER

typedef Adafruit_GC9A01A DISPLAY_CONSTRUCTOR_TYPE;
extern DISPLAY_CONSTRUCTOR_TYPE *tft;
// Nodemcu ESP32

/* Pin from https://lastminuteengineers.com/esp32-pinout-reference/ for the VSPI- SPI Pins*/

#define TFT_CS 5  // D5  // CS  (Chip Select)
#define TFT_RST 4 // D4 // RES
#define TFT_DC 15 // D15 // DC

#define TFT_BL 2 // Backlight control

#define DISPLAY_INIT_SEQUENCE                           \
    tft = new DISPLAY_CONSTRUCTOR_TYPE(TFT_CS, TFT_DC); \
    tft->begin();                                       \
    Serial.println("TFT begun");                        \
    tft->setSPISpeed(40000000);

#define ROTARY_ENCODER_A_PIN 19
#define ROTARY_ENCODER_B_PIN 21
#define ROTARY_ENCODER_BUTTON_PIN 22
#define EXT_ROTARY_PULLUPS true // true if you have external pullups on rotary encoder pins

#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

// it is wierd, but the ADAFRUIT_GFX library needs this constant to initialize the global declared canvas object correctly,
// as this cant be set correctly at runtime based on the display dimensions

// try to do some speed ajustments for the display because of th software SPI
#define MAIN_DISPLAY_DELAY 60
#define CHANGE_STATE_BUTTON_TIMEOUT 3
#define SCROLL_STEP_WIDTH 1

#endif

// ------------------------------------ HW_ESP32_C3_LCDKIT -----------------

#if HARDWARE_ID == HW_ESP32_C3_LCDKIT
// Code to include if HW_ESP32_C3_LCDKIT is defined

#include "Adafruit_GC9A01A.h"

#define TFT_DIAMETER 240
#define TFT_ROUND true
#define TFT_ROTATION 2

#define HAS_ROTARY_ENCODER

// it is wierd, but the ADAFRUIT_GFX library needs this constant to initialize the global declared canvas object correctly,
// as this cant be set correctly at runtime based on the display dimensions
typedef Adafruit_GC9A01A DISPLAY_CONSTRUCTOR_TYPE;
extern DISPLAY_CONSTRUCTOR_TYPE *tft;

// =====================
// SPI Settings
// =====================
#define SPI_CLOCK_SPEED 40000000 // 40 MHz → schnell, meist stabil

#define TFT_CS 7
#define TFT_RST -1
#define TFT_DC 2
#define TFT_SDA_MOSI 0
#define TFT_SCLK 1

#define TFT_BL 5 // Backlight control

#define DISPLAY_INIT_SEQUENCE           \
    SPI.begin(                          \
        TFT_SCLK,                       \
        -1,                             \
        TFT_SDA_MOSI,                   \
        TFT_CS);                        \
    tft = new DISPLAY_CONSTRUCTOR_TYPE( \
        TFT_CS,                         \
        TFT_DC,                         \
        TFT_RST);                       \
    tft->begin();                       \
    Serial.println("TFT begun");        \
    tft->setSPISpeed(SPI_CLOCK_SPEED);

#define ROTARY_ENCODER_A_PIN 10
#define ROTARY_ENCODER_B_PIN 6
#define ROTARY_ENCODER_BUTTON_PIN 9

#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define EXT_ROTARY_PULLUPS false  // true if you have external pullups on rotary encoder pins

// try to do some speed ajustments for the display because of th software SPI
#define MAIN_DISPLAY_DELAY 30
#define CHANGE_STATE_BUTTON_TIMEOUT 5
#define SCROLL_STEP_WIDTH 1
/*
More information about the ESP32-C3-LCDKit display module:
https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c3/esp32-c3-lcdkit/user_guide.html#id7
https://de.aliexpress.com/item/1005007654726169.html?ug_edm_item_id=1005007654726169&edm_log_data=gmod-edm-item-list-three-columns.track-edm-item-list-three-columns-log-link&tracelog=rowan&rowan_id1=lg_local_carrier_202503270152_1_en_US_2026-01-16&rowan_msg_id=6833306681292165%24be70c1556a68464e9cfa703d0af9ebe9&ck=in_edm_other&mem_info=%20Gt%206jHVb2YbFB%2Fd5BJapQ%3D%3D-1002-lg_local_carrier_202503270152-hZcSlpen6HB%2F5A%20xQKPxn8mtOZk5P6fAiNPUKs13ejc%3D&gatewayAdapt=glo2deu


 */
#endif

// ------------------------------------ ESP32_2424S012 -----------------

#if HARDWARE_ID == ESP32_2424S012
// Code to include if ESP32_2424S012 is defined

#include "Adafruit_GC9A01A.h"

#define TFT_DIAMETER 240
#define TFT_ROUND true

#if ORIENTATION == NORTH_UP
#define TFT_ROTATION 2
#elif ORIENTATION == SOUTH_UP
#define TFT_ROTATION 0
#elif ORIENTATION == EAST_UP
#define TFT_ROTATION 1
#elif ORIENTATION == WEST_UP
#define TFT_ROTATION 3
#else
#define TFT_ROTATION 0
#endif

#define HAS_TOUCHSCREEN

// it is wierd, but the ADAFRUIT_GFX library needs this constant to initialize the global declared canvas object correctly,
// as this cant be set correctly at runtime based on the display dimensions
typedef Adafruit_GC9A01A DISPLAY_CONSTRUCTOR_TYPE;
extern DISPLAY_CONSTRUCTOR_TYPE *tft;

// =====================
// SPI Settings
// =====================
#define SPI_CLOCK_SPEED 40000000 // 40 MHz → schnell, meist stabil

#define TFT_CS 10
#define TFT_RST -1
#define TFT_DC 2
#define TFT_SDA_MOSI 7
#define TFT_SCLK 6

#define TFT_BL 3 // Backlight control

// touchscreen pins for cst816d touch controller, which is used in the ESP32_2424S012 display module
#define TOUCH_ADDRESS 0x15
#define TOUCH_I2C_SCL 5
#define TOUCH_I2C_SDA 4
#define TOUCH_INT 0
#define TOUCH_RST 1

#if ORIENTATION == NORTH_UP
#define TOUCH_ROTATION 2
#elif ORIENTATION == SOUTH_UP
#define TOUCH_ROTATION 0
#elif ORIENTATION == EAST_UP
#define TOUCH_ROTATION 1

#elif ORIENTATION == WEST_UP
#define TOUCH_ROTATION 3
#else

#define TOUCH_ROTATION 0
#endif

// touchscreen settings
#define TOUCH_BUTTON_DETECTION_RADIUS 50 // radius around the center of the display where touch will be detected as button press, in pixels. This is used to avoid detecting touches on the edge of the display as button presses, since the display is round and we want to have a circular button area in the middle of the display.
#define TOUCH_ANGLE_INNER_RADIUS 70      // inner radius of the circular area where touch will be detected as angle change, in pixels. This is used to avoid detecting touches in the middle of the display as angle changes, since we want to have a circular area in the middle of the display where touch will be detected as button presses, and an annular area around it where touch will be detected as angle changes.
#define TOUCH_ANGLE_OUTER_RADIUS 120     // outer radius of the circular area where touch will

#define DISPLAY_INIT_SEQUENCE           \
    SPI.begin(                          \
        TFT_SCLK,                       \
        -1,                             \
        TFT_SDA_MOSI,                   \
        TFT_CS);                        \
    tft = new DISPLAY_CONSTRUCTOR_TYPE( \
        TFT_CS,                         \
        TFT_DC,                         \
        TFT_RST);                       \
    tft->begin();                       \
    Serial.println("TFT begun");        \
    tft->setSPISpeed(SPI_CLOCK_SPEED);

#define ROTARY_ENCODER_A_PIN 10
#define ROTARY_ENCODER_B_PIN 6
#define ROTARY_ENCODER_BUTTON_PIN 9

#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define EXT_ROTARY_PULLUPS false  // true if you have external pullups on rotary encoder pins

// try to do some speed ajustments for the display because of th software SPI
#define MAIN_DISPLAY_DELAY 30
#define CHANGE_STATE_BUTTON_TIMEOUT 2
#define SCROLL_STEP_WIDTH 1

#endif

// Arduino UNO
// #define TFT_CS       10
// #define TFT_RST       9
// #define TFT_DC        8

// Wemos D1 Mini
// #define TFT_CS       D8
// #define TFT_RST      D1
// #define TFT_DC       D2

// XIAO
// #define TFT_CS       D7
// #define TFT_RST      D1
// #define TFT_DC       D2

// Arduino Nano ESP 32
// #define TFT_CS       10
// #define TFT_RST       9
// #define TFT_DC        8

// ESP32-C6
// #define TFT_CS       18
// #define TFT_RST       3
// #define TFT_DC        2

// ESP32-WROOM
// #define TFT_CS        5
// #define TFT_RST       4
// #define TFT_DC        2

#endif