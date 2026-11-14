// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// Lilygo T-5 S3 Pro
//
// Launcher env: lilygo-t5-epaper-s3-pro

#ifndef DISPLAY_DEVICE_LILYGO_T5_EPAPER_S3_PRO_H
#define DISPLAY_DEVICE_LILYGO_T5_EPAPER_S3_PRO_H

// These are plain defines: pass the same macro as a build flag and the
// compiler will report a redefinition. Edit the profile instead.
// clang-format off

// The panel is described unrotated: TFT_ROTATION is the mounting offset
// baked into the driver, and a profile has no business guessing which way
// round you hold the board. Rotate at runtime with tft.setRotation(n).
#define TFT_ROTATION 0

// Panel geometry
#define TFT_WIDTH 540
#define TFT_HEIGHT 960

// Parallel e-paper driven by EPD_Painter. The panel, its pins and the I2C
// bus all come from the preset, which the library probes at runtime unless
// one is named here (EPD_PAINTER_PRESET_*).

#define E_PAPER_DISPLAY 1

#if !defined(USE_EPD_PAINTER)
    #error "This device needs -DUSE_EPD_PAINTER=1"
#endif

// clang-format on

#endif // DISPLAY_DEVICE_LILYGO_T5_EPAPER_S3_PRO_H
