// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// Lilygo T-LoraPager
// ST7796 over Arduino_HWSPI, 222x480.
//
// Launcher env: lilygo-t-lora-pager

#ifndef DISPLAY_DEVICE_LILYGO_T_LORA_PAGER_H
#define DISPLAY_DEVICE_LILYGO_T_LORA_PAGER_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry and window offsets
#define TFT_WIDTH 222
#define TFT_HEIGHT 480
#define TFT_IPS 1
#define TFT_COL_OFS1 49
#define TFT_ROW_OFS1 0
#define TFT_COL_OFS2 49
#define TFT_ROW_OFS2 0

// Pins
#define TFT_CS 38
#define TFT_DC 37
#define TFT_RST -1
#define TFT_SCLK 35
#define TFT_MOSI 34
#define TFT_MISO 33
#define TFT_BL 42

// Panel quirks
#define TFT_BRIGHT_FREQ 10000

// ---------------------------------------------------------------------------
// The same panel in each graphics library's vocabulary. Only the branch for
// the backend you selected is compiled.
// ---------------------------------------------------------------------------
#if defined(USE_ARDUINO_GFX)
    #define TFT_DATABUS_N 0 // Arduino_HWSPI
    #define TFT_DISPLAY_DRIVER_N 2 // Arduino_ST7796

#elif defined(USE_LOVYANGFX)
    #define LOVYAN_PANEL Panel_ST7796
    #define LOVYAN_BUS Bus_SPI

#elif defined(USE_TFT_ESPI)
    // TFT_eSPI is configured through macros instead of User_Setup.h.
    #define USER_SETUP_LOADED 1
    #define ST7796_DRIVER 1
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

#endif // DISPLAY_DEVICE_LILYGO_T_LORA_PAGER_H
