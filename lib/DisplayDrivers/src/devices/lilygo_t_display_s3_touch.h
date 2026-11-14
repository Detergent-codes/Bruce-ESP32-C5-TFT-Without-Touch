// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// Lilygo T-Display S3
// ST7789 over Arduino_ESP32PAR8Q, 170x320.
//
// Launcher env: lilygo-t-display-S3-touch

#ifndef DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_TOUCH_H
#define DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_TOUCH_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry and window offsets
#define TFT_WIDTH 170
#define TFT_HEIGHT 320
#define TFT_IPS 1
#define TFT_COL_OFS1 35
#define TFT_ROW_OFS1 0
#define TFT_COL_OFS2 35
#define TFT_ROW_OFS2 0

// Pins
#define TFT_CS 6
#define TFT_DC 7
#define TFT_RST 5
#define TFT_WR 8
#define TFT_RD 9
#define TFT_D0 39
#define TFT_D1 40
#define TFT_D2 41
#define TFT_D3 42
#define TFT_D4 45
#define TFT_D5 46
#define TFT_D6 47
#define TFT_D7 48
#define TFT_BL 38

// Panel quirks
#define TFT_BRIGHT_FREQ 10000

// ---------------------------------------------------------------------------
// The same panel in each graphics library's vocabulary. Only the branch for
// the backend you selected is compiled.
// ---------------------------------------------------------------------------
#if defined(USE_ARDUINO_GFX)
    #define TFT_DATABUS_N 2 // Arduino_ESP32PAR8Q
    #define TFT_DISPLAY_DRIVER_N 1 // Arduino_ST7789

#elif defined(USE_LOVYANGFX)
    #define LOVYAN_PANEL Panel_ST7789
    #define LOVYAN_BUS Bus_Parallel8

#elif defined(USE_TFT_ESPI)
    // TFT_eSPI is configured through macros instead of User_Setup.h.
    #define USER_SETUP_LOADED 1
    #define ST7789_DRIVER 1
    #define TFT_PARALLEL_8_BIT 1
    // IPS panels are inverted
    #define TFT_INVERSION_ON 1
    // TFT_eSPI has no window-offset macros: the offsets above are
    // baked into its driver. A panel that needs them may come out
    // shifted — check before trusting this branch.
    #define CGRAM_OFFSET 1

#elif defined(USE_M5GFX)
    // M5Unified detects the hardware and configures the panel itself, so
    // the macros above are ignored. It only knows M5Stack units.

#endif

// clang-format on

#endif // DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_TOUCH_H
