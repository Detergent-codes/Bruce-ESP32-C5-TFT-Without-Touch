// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

// DisplayDrivers — one display API (tft_display / tft_sprite) over several
// graphics libraries. Pick a backend with a USE_* build flag; see README.md.
//
// This is the only header meant to be included by user code. Everything under
// backends/ is internal and deliberately kept off the include path, so names
// like tft.h or m5gfx.h cannot shadow a backend library's own headers.
#ifndef DISPLAY_DRIVERS_H
#define DISPLAY_DRIVERS_H

// Configuration. Two ways in, and the difference matters:
//
//   1. Build flags (-DUSE_LOVYANGFX=1 -DTFT_CS=5 …). Reach every translation
//      unit, including this library's own .cpp files. Use these when your build
//      system has somewhere to put them (PlatformIO build_flags, ESP-IDF
//      COMPILE_OPTIONS).
//
//   2. A DisplayDrivers_Setup.h of your own, anywhere on the include path
//      (PlatformIO: the project's include/ folder). It is pulled in below,
//      before anything is decided, so it also reaches every translation unit.
//
// Defining the macros in your sketch before #include <DisplayDrivers.h> does
// NOT work: this library compiles its backends as separate translation units,
// which never see your sketch's macros. They would fall back to the default
// backend and fail to link against the one your sketch declared.
#if defined(__has_include)
#if __has_include(<DisplayDrivers_Setup.h>)
#include <DisplayDrivers_Setup.h>
#endif
#endif

// USE_NATIVE_SDL targets PlatformIO's `platform = native`, which has no
// Arduino core at all — pins_arduino.h does not exist there, and that
// backend brings its own String/SPIClass instead of SPI.h's.
#if !defined(USE_NATIVE_SDL)
#include <pins_arduino.h>
#endif

// USE_DUMMY_TFT wins over any panel flag the build inherited: a board that says
// it has no display has no display. Every backend .cpp includes this header
// before testing its own flag, so clearing them here disables all of them —
// nothing is left to pull in a graphics library that is not even installed.
#if defined(USE_DUMMY_TFT)
#undef USE_ARDUINO_GFX
#undef USE_LOVYANGFX
#undef USE_TFT_ESPI
#undef USE_M5GFX
#undef USE_EPD_PAINTER
#undef USE_GXEPD2
#undef USE_NATIVE_SDL
#undef USE_CANVAS
#endif

#if !defined(USE_ARDUINO_GFX) && !defined(USE_LOVYANGFX) && !defined(USE_TFT_ESPI) && !defined(USE_M5GFX) &&  \
    !defined(USE_EPD_PAINTER) && !defined(USE_GXEPD2) && !defined(USE_NATIVE_SDL) && !defined(USE_DUMMY_TFT)
#define USE_ARDUINO_GFX
#endif

#if !defined(USE_NATIVE_SDL)
class TFT_eSPI;
class tft_sprite;
class tft_logger;
#include <SPI.h>
#endif

// The mutable description of the panel, seeded from the macros above. Backends
// read it in begin(), so anything running before that can retarget the display.
#include "DisplayConfig.h"

#if defined(USE_DUMMY_TFT)
#include "backends/dummy.h"

#elif defined(USE_EPD_PAINTER)
#include "backends/epdpainter.h"

#elif defined(USE_GXEPD2)
#include "backends/gxepd2_hal.h"

#elif defined(USE_TFT_ESPI)
#include "backends/tftespi.h"

#elif defined(USE_LOVYANGFX)
#include "backends/lovyan.h"

#elif defined(USE_M5GFX)
#include "backends/m5gfx_hal.h"

#elif defined(USE_NATIVE_SDL)
#include "backends/native_sdl.h"

#elif defined(USE_ARDUINO_GFX)
#include "backends/ardgfx.h"

#endif
#endif // DISPLAY_DRIVERS_H
