// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// Lilygo T-Display P4
// DSI panel over Arduino_ESP32DSIPanel, 568x1232.
//
// Launcher env: lilygo-t-display-p4 -- the Launcher builds one binary for both
// P4 panels and probes at boot, so it seeds the IPS profile and rewrites
// displayConfig for this one. Pick this profile directly when you are building
// for a unit you already know is the AMOLED.

#ifndef DISPLAY_DEVICE_LILYGO_T_DISPLAY_P4_AMOLED_H
#define DISPLAY_DEVICE_LILYGO_T_DISPLAY_P4_AMOLED_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry and window offsets
#define TFT_WIDTH 568
#define TFT_HEIGHT 1232
#define TFT_IPS 1
#define TFT_COL_OFS1 0
#define TFT_ROW_OFS1 0
#define TFT_COL_OFS2 0
#define TFT_ROW_OFS2 0

// DSI timings
#define TFT_HSYNC_PULSE_WIDTH 50
#define TFT_HSYNC_BACK_PORCH 150
#define TFT_HSYNC_FRONT_PORCH 50
#define TFT_VSYNC_PULSE_WIDTH 40
#define TFT_VSYNC_BACK_PORCH 120
#define TFT_VSYNC_FRONT_PORCH 80
#define TFT_PREF_SPEED 60000000
#define TFT_DSI_INIT rm69a10_amoled_init_operations
#define TFT_RST -1

// ---------------------------------------------------------------------------
// The same panel in each graphics library's vocabulary. Only the branch for
// the backend you selected is compiled.
// ---------------------------------------------------------------------------
#if defined(USE_ARDUINO_GFX)
    #define TFT_DATABUS_N 4 // Arduino_ESP32DSIPanel
    #define TFT_DISPLAY_DRIVER_N 50 // Arduino_DSI_panel

#elif defined(USE_LOVYANGFX)
    #error "LovyanGFX has no DSI panel/Arduino_ESP32DSIPanel combination in this wrapper; build this device with USE_ARDUINO_GFX."

#elif defined(USE_TFT_ESPI)
    #error "TFT_eSPI cannot drive a DSI panel on Arduino_ESP32DSIPanel; build this device with USE_ARDUINO_GFX."

#elif defined(USE_M5GFX)
    // M5Unified detects the hardware and configures the panel itself, so
    // the macros above are ignored. It only knows M5Stack units.

#endif

// clang-format on

#endif // DISPLAY_DEVICE_LILYGO_T_DISPLAY_P4_AMOLED_H
