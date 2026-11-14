// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// CYD 2432S028
// ILI9341 over Arduino_HWSPI, 240x320.
//
// Launcher env: CYD-2432S028

#ifndef DISPLAY_DEVICE_CYD_2432S028_H
#define DISPLAY_DEVICE_CYD_2432S028_H

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
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST -1
#define TFT_SCLK 14
#define TFT_MOSI 13
#define TFT_MISO 12
#define TFT_BL 21

// ---------------------------------------------------------------------------
// The same panel in each graphics library's vocabulary. Only the branch for
// the backend you selected is compiled.
// ---------------------------------------------------------------------------
#if defined(USE_ARDUINO_GFX)
    #define TFT_DATABUS_N 0 // Arduino_HWSPI
    #define TFT_DISPLAY_DRIVER_N 4 // Arduino_ILI9341

#elif defined(USE_LOVYANGFX)
    #define LOVYAN_PANEL Panel_ILI9341
    #define LOVYAN_BUS Bus_SPI

#elif defined(USE_TFT_ESPI)
    // TFT_eSPI is configured through macros instead of User_Setup.h.
    #define USER_SETUP_LOADED 1
    #define ILI9341_DRIVER 1
    #define SPI_FREQUENCY 40000000

#elif defined(USE_M5GFX)
    // M5Unified detects the hardware and configures the panel itself, so
    // the macros above are ignored. It only knows M5Stack units.

#endif

// clang-format on

#endif // DISPLAY_DEVICE_CYD_2432S028_H
