// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// M5Stack StickCPlus2
// ST7789 over Arduino_HWSPI, 135x240.
//
// Launcher env: m5stack-cplus2

#ifndef DISPLAY_DEVICE_M5STACK_CPLUS2_H
#define DISPLAY_DEVICE_M5STACK_CPLUS2_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry and window offsets
#define TFT_WIDTH 135
#define TFT_HEIGHT 240
#define TFT_IPS 1
#define TFT_COL_OFS1 52
#define TFT_ROW_OFS1 40
#define TFT_COL_OFS2 53
#define TFT_ROW_OFS2 40

// Pins
#define TFT_CS 5
#define TFT_DC 14
#define TFT_RST 12
#define TFT_SCLK 13
#define TFT_MOSI 15
#define TFT_MISO -1
#define TFT_BL 27

// ---------------------------------------------------------------------------
// The same panel in each graphics library's vocabulary. Only the branch for
// the backend you selected is compiled.
// ---------------------------------------------------------------------------
#if defined(USE_ARDUINO_GFX)
    #define TFT_DATABUS_N 0 // Arduino_HWSPI
    #define TFT_DISPLAY_DRIVER_N 1 // Arduino_ST7789

#elif defined(USE_LOVYANGFX)
    #define LOVYAN_PANEL Panel_ST7789
    #define LOVYAN_BUS Bus_SPI

#elif defined(USE_TFT_ESPI)
    // TFT_eSPI is configured through macros instead of User_Setup.h.
    #define USER_SETUP_LOADED 1
    #define ST7789_DRIVER 1
    #define SPI_FREQUENCY 40000000
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

#endif // DISPLAY_DEVICE_M5STACK_CPLUS2_H
