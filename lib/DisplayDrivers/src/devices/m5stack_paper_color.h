// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// M5Stack Paper Color
//
// Launcher env: m5stack-paper-color

#ifndef DISPLAY_DEVICE_M5STACK_PAPER_COLOR_H
#define DISPLAY_DEVICE_M5STACK_PAPER_COLOR_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// M5Unified auto-detects the panel: there is nothing to declare. The
// geometry below is only what the launcher's layout code reads back.

#define TFT_WIDTH 400
#define TFT_HEIGHT 600

// Draw into a full-screen M5Canvas and push it on display(). On the
// e-paper units that is one panel refresh per frame instead of one
// per primitive.
#define USE_CANVAS 1

#define E_PAPER_DISPLAY 1

#if !defined(USE_M5GFX)
    #error "This device is an M5Stack unit: build it with -DUSE_M5GFX=1"
#endif

// clang-format on

#endif // DISPLAY_DEVICE_M5STACK_PAPER_COLOR_H
