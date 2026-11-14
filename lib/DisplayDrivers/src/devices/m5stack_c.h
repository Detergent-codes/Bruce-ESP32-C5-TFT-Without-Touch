// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// M5Stack StickC
//
// Launcher env: m5stack-c

#ifndef DISPLAY_DEVICE_M5STACK_C_H
#define DISPLAY_DEVICE_M5STACK_C_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry and window offsets
#define TFT_WIDTH 80
#define TFT_HEIGHT 160

// ---------------------------------------------------------------------------
// This board carries its panel configuration inside its graphics library
// (a TFT_eSPI fork with its own User_Setup), not in macros, so there is
// nothing to translate for the other backends. Driving it with anything
// else means filling in the driver, bus and pins yourself.
// ---------------------------------------------------------------------------
#if !defined(USE_TFT_ESPI)
    #error "This device is only described for TFT_eSPI. See devices/README.md."
#endif

// clang-format on

#endif // DISPLAY_DEVICE_M5STACK_C_H
