// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// Lilygo T-Deck Pro
//
// Launcher env: lilygo-t-deck-pro

#ifndef DISPLAY_DEVICE_LILYGO_T_DECK_PRO_H
#define DISPLAY_DEVICE_LILYGO_T_DECK_PRO_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry
#define TFT_WIDTH 240
#define TFT_HEIGHT 320

// SPI e-paper driven by GxEPD2. Note that GxEPD2 is GPL-3.0, so anything
// you build with this profile is too — see the library's Licensing section.

#define GXEPD2_PANEL GxEPD2_310_GDEQ031T10
#define TFT_CS 34
#define TFT_DC 35
#define TFT_RST -1
#define TFT_BUSY 37
#define TFT_SCLK 36
#define TFT_MOSI 33

#define E_PAPER_DISPLAY 1

#if !defined(USE_GXEPD2)
    #error "This device needs -DUSE_GXEPD2=1"
#endif

// clang-format on

#endif // DISPLAY_DEVICE_LILYGO_T_DECK_PRO_H
