// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// CYD 2432S022C
// ST7789 over Arduino_ESP32PAR8, 240x320.
//
// Launcher envs: CYD-2432S022C, CYD-2432S022C-lovyan

#ifndef DISPLAY_DEVICE_CYD_2432S022C_H
#define DISPLAY_DEVICE_CYD_2432S022C_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry and window offsets
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_IPS 0
#define TFT_COL_OFS1 0
#define TFT_ROW_OFS1 0
#define TFT_COL_OFS2 0
#define TFT_ROW_OFS2 0

// Pins
#define TFT_CS 17
#define TFT_DC 16
#define TFT_RST GPIO_NUM_NC
#define TFT_WR 4
#define TFT_RD 2
#define TFT_D0 15
#define TFT_D1 13
#define TFT_D2 12
#define TFT_D3 14
#define TFT_D4 27
#define TFT_D5 25
#define TFT_D6 33
#define TFT_D7 32
#define TFT_BL 0

// Panel quirks
#define TFT_INVERSION_OFF
#define TFT_INVERTED 0
#define TFT_BUS_SHARED 0

// ---------------------------------------------------------------------------
// The same panel in each graphics library's vocabulary. Only the branch for
// the backend you selected is compiled.
// ---------------------------------------------------------------------------
#if defined(USE_ARDUINO_GFX)
    #define TFT_DATABUS_N 5 // Arduino_ESP32PAR8
    #define TFT_DISPLAY_DRIVER_N 1 // Arduino_ST7789

#elif defined(USE_LOVYANGFX)
    #define LOVYAN_PANEL Panel_ST7789
    #define LOVYAN_BUS Bus_Parallel8

#elif defined(USE_TFT_ESPI)
    // TFT_eSPI is configured through macros instead of User_Setup.h.
    #define USER_SETUP_LOADED 1
    #define ST7789_DRIVER 1
    #define TFT_PARALLEL_8_BIT 1

#elif defined(USE_M5GFX)
    // M5Unified detects the hardware and configures the panel itself, so
    // the macros above are ignored. It only knows M5Stack units.

#endif

// clang-format on

#endif // DISPLAY_DEVICE_CYD_2432S022C_H
