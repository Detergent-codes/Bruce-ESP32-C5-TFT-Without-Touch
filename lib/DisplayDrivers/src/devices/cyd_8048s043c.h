// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// CYD 8048S043C
// RGB panel over Arduino_ESP32RGBPanel, 800x480.
//
// Launcher envs: CYD-8048S043C, CYD-8048W550C

#ifndef DISPLAY_DEVICE_CYD_8048S043C_H
#define DISPLAY_DEVICE_CYD_8048S043C_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry and window offsets
#define TFT_WIDTH 800
#define TFT_HEIGHT 480

// Pins
#define TFT_DE 40
#define TFT_VSYNC 41
#define TFT_HSYNC 39
#define TFT_PCLK 42
#define TFT_R0 45
#define TFT_R1 48
#define TFT_R2 47
#define TFT_R3 21
#define TFT_R4 14
#define TFT_G0 5
#define TFT_G1 6
#define TFT_G2 7
#define TFT_G3 15
#define TFT_G4 16
#define TFT_G5 4
#define TFT_B0 8
#define TFT_B1 3
#define TFT_B2 46
#define TFT_B3 9
#define TFT_B4 1
#define TFT_BL 2

// RGB timings
#define TFT_HSYNC_POL 0
#define TFT_HSYNC_FRONT_PORCH 8
#define TFT_HSYNC_PULSE_WIDTH 4
#define TFT_HSYNC_BACK_PORCH 8
#define TFT_VSYNC_POL 0
#define TFT_VSYNC_FRONT_PORCH 8
#define TFT_VSYNC_PULSE_WIDTH 4
#define TFT_VSYNC_BACK_PORCH 8
#define TFT_PCLK_ACTIVE_NEG true
#define TFT_PREF_SPEED 16000000

// ---------------------------------------------------------------------------
// The same panel in each graphics library's vocabulary. Only the branch for
// the backend you selected is compiled.
// ---------------------------------------------------------------------------
#if defined(USE_ARDUINO_GFX)
    #define TFT_DATABUS_N 3 // Arduino_ESP32RGBPanel
    #define TFT_DISPLAY_DRIVER_N 49 // Arduino_RGB_panel

#elif defined(USE_LOVYANGFX)
    #error "LovyanGFX has no RGB panel/Arduino_ESP32RGBPanel combination in this wrapper; build this device with USE_ARDUINO_GFX."

#elif defined(USE_TFT_ESPI)
    #error "TFT_eSPI cannot drive a RGB panel on Arduino_ESP32RGBPanel; build this device with USE_ARDUINO_GFX."

#elif defined(USE_M5GFX)
    // M5Unified detects the hardware and configures the panel itself, so
    // the macros above are ignored. It only knows M5Stack units.

#endif

// clang-format on

#endif // DISPLAY_DEVICE_CYD_8048S043C_H
