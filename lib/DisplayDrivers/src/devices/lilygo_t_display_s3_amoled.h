// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// Lilygo T-Display AMOLED
// RM67162 over Arduino_ESP32QSPI, 240x536.
//
// Launcher env: lilygo-t-display-S3-amoled

#ifndef DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_AMOLED_H
#define DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_AMOLED_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry and window offsets
#define TFT_WIDTH 240
#define TFT_HEIGHT 536
#define TFT_IPS 0
#define TFT_COL_OFS1 0
#define TFT_ROW_OFS1 0
#define TFT_COL_OFS2 0
#define TFT_ROW_OFS2 0

// Pins
#define TFT_CS 6
#define TFT_RST 17
#define TFT_SCLK 47
#define TFT_D0 18
#define TFT_D1 7
#define TFT_D2 48
#define TFT_D3 5

// ---------------------------------------------------------------------------
// The same panel in each graphics library's vocabulary. Only the branch for
// the backend you selected is compiled.
// ---------------------------------------------------------------------------
#if defined(USE_ARDUINO_GFX)
    #define TFT_DATABUS_N 1 // Arduino_ESP32QSPI
    #define TFT_DISPLAY_DRIVER_N 35 // Arduino_RM67162

#elif defined(USE_LOVYANGFX)
    #error "LovyanGFX has no RM67162/Arduino_ESP32QSPI combination in this wrapper; build this device with USE_ARDUINO_GFX."

#elif defined(USE_TFT_ESPI)
    #error "TFT_eSPI cannot drive a RM67162 on Arduino_ESP32QSPI; build this device with USE_ARDUINO_GFX."

#elif defined(USE_M5GFX)
    // M5Unified detects the hardware and configures the panel itself, so
    // the macros above are ignored. It only knows M5Stack units.

#endif

// clang-format on

#endif // DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_AMOLED_H
