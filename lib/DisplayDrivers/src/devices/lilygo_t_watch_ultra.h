// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// Lilygo T-Watch Ultra
// CO5300 over Arduino_ESP32QSPI, 502x410.
//
// Launcher env: lilygo-t-watch-ultra

#ifndef DISPLAY_DEVICE_LILYGO_T_WATCH_ULTRA_H
#define DISPLAY_DEVICE_LILYGO_T_WATCH_ULTRA_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry and window offsets
#define TFT_WIDTH 502
#define TFT_HEIGHT 410
#define TFT_IPS 1
#define TFT_COL_OFS1 0
#define TFT_ROW_OFS1 22
#define TFT_COL_OFS2 0
#define TFT_ROW_OFS2 22

// Pins
#define TFT_CS 41
#define TFT_RST 37
#define TFT_SCLK 40
#define TFT_D0 38
#define TFT_D1 39
#define TFT_D2 42
#define TFT_D3 45
#define TFT_TE 6

// Panel quirks
#define TFT_TE 6

// ---------------------------------------------------------------------------
// The same panel in each graphics library's vocabulary. Only the branch for
// the backend you selected is compiled.
// ---------------------------------------------------------------------------
#if defined(USE_ARDUINO_GFX)
    #define TFT_DATABUS_N 1 // Arduino_ESP32QSPI
    #define TFT_DISPLAY_DRIVER_N 41 // Arduino_CO5300
    #define USE_CANVAS 1
    #define TFT_CANVAS_ROTATE_OUTPUT 1

#elif defined(USE_LOVYANGFX)
    #error "LovyanGFX has no CO5300/Arduino_ESP32QSPI combination in this wrapper; build this device with USE_ARDUINO_GFX."

#elif defined(USE_TFT_ESPI)
    #error "TFT_eSPI cannot drive a CO5300 on Arduino_ESP32QSPI; build this device with USE_ARDUINO_GFX."

#elif defined(USE_M5GFX)
    // M5Unified detects the hardware and configures the panel itself, so
    // the macros above are ignored. It only knows M5Stack units.

#endif

// clang-format on

#endif // DISPLAY_DEVICE_LILYGO_T_WATCH_ULTRA_H
