// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// DisplayDrivers configuration.
//
// Two things have to be decided before anything compiles: which graphics
// library drives the panel, and which panel it is. Either edit this file, or
// pass both as build flags and leave it alone — see src/devices/README.md.
//
// DisplayDrivers.h pulls this in before it decides anything, so whatever lands
// here reaches every translation unit, including the library's own .cpp files.
#ifndef DISPLAYDRIVERS_SETUP_H
#define DISPLAYDRIVERS_SETUP_H
// clang-format off

// ---------------------------------------------------------------------------
// 0. A project of your own.
//
// This file lives inside the library, so it is found before anything in your
// project and it is lost every time the library is updated. To keep your
// configuration in your own tree instead, put a DisplayDrivers_User_Setup.h
// anywhere on the include path (PlatformIO: include/, plus `-I include`) —
// it is read first, and every macro below is guarded, so yours win.
// ---------------------------------------------------------------------------
#if defined(__has_include)
    #if __has_include(<DisplayDrivers_User_Setup.h>)
        #include <DisplayDrivers_User_Setup.h>
    #endif
#endif

// ---------------------------------------------------------------------------
// 1. The graphics library. Uncomment one, or pass it as a build flag.
//    Arduino_GFX is the default when nothing is selected.
// ---------------------------------------------------------------------------
// #define USE_ARDUINO_GFX 1  // LCD / OLED / AMOLED / RGB / DSI
// #define USE_LOVYANGFX   1  // LCD / OLED
// #define USE_TFT_ESPI    1  // LCD
// #define USE_M5GFX       1  // any M5Stack unit, auto-detected
// #define USE_EPD_PAINTER 1  // parallel e-paper
// #define USE_GXEPD2      1  // SPI e-paper  (GPL-3.0, see the Licensing section)
// #define USE_DUMMY_TFT   1  // no panel at all, every draw call is a no-op
// #define USE_NATIVE_SDL  1  // desktop SDL2 window, PlatformIO `platform = native` only
//                            // (no Arduino core — see examples/NativeUI)

// Selected here so the device profiles below can branch on it. DisplayDrivers.h
// makes the same choice; doing it early only moves it in front of the profiles.
#if !defined(USE_ARDUINO_GFX) && !defined(USE_LOVYANGFX) && !defined(USE_TFT_ESPI) && !defined(USE_M5GFX) && !defined(USE_EPD_PAINTER) && !defined(USE_GXEPD2) && !defined(USE_NATIVE_SDL) && !defined(USE_DUMMY_TFT)
    #define USE_ARDUINO_GFX 1
#endif

// ---------------------------------------------------------------------------
// 2. The device. Uncomment one, or pass -DDISPLAY_DEVICE_<DEVICE> as a build
//    flag. Leave every one commented out to describe the panel yourself with
//    plain TFT_* macros, the way the README documents.
// ---------------------------------------------------------------------------

// #define DISPLAY_DEVICE_ARDUINO_NESSO_N1 1
// #define DISPLAY_DEVICE_CYD_2432S022C 1
// #define DISPLAY_DEVICE_CYD_2432S024R 1
// #define DISPLAY_DEVICE_CYD_2432S028 1
// #define DISPLAY_DEVICE_CYD_2432S032C 1
// #define DISPLAY_DEVICE_CYD_2432W328C 1
// #define DISPLAY_DEVICE_CYD_2_USB 1
// #define DISPLAY_DEVICE_CYD_3248S035C 1
// #define DISPLAY_DEVICE_CYD_3248W535C 1
// #define DISPLAY_DEVICE_CYD_4827S043R 1
// #define DISPLAY_DEVICE_CYD_8048S043C 1
// #define DISPLAY_DEVICE_ELECROW_24B 1
// #define DISPLAY_DEVICE_ELECROW_35B 1
// #define DISPLAY_DEVICE_ELECROW_35BV2_2 1
// #define DISPLAY_DEVICE_HEADLESS_ESP32_4MB 1
// #define DISPLAY_DEVICE_LILYGO_T5_EPAPER_S3_PRO 1
// #define DISPLAY_DEVICE_LILYGO_T_DECK 1
// #define DISPLAY_DEVICE_LILYGO_T_DECK_PRO 1
// #define DISPLAY_DEVICE_LILYGO_T_DISPLAY_C5 1
// #define DISPLAY_DEVICE_LILYGO_T_DISPLAY_P4 1
// #define DISPLAY_DEVICE_LILYGO_T_DISPLAY_P4_AMOLED 1
// #define DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_AMOLED 1
// #define DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_AMOLED_PLUS 1
// #define DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_PRO 1
// #define DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_TOUCH 1
// #define DISPLAY_DEVICE_LILYGO_T_DONGLE_C5_TFT 1
// #define DISPLAY_DEVICE_LILYGO_T_DONGLE_S3_TFT 1
// #define DISPLAY_DEVICE_LILYGO_T_EMBED 1
// #define DISPLAY_DEVICE_LILYGO_T_EMBED_CC1101 1
// #define DISPLAY_DEVICE_LILYGO_T_HMI 1
// #define DISPLAY_DEVICE_LILYGO_T_LORA_PAGER 1
// #define DISPLAY_DEVICE_LILYGO_T_WATCH_S3 1
// #define DISPLAY_DEVICE_LILYGO_T_WATCH_ULTRA 1
// #define DISPLAY_DEVICE_M5STACK_C 1
// #define DISPLAY_DEVICE_M5STACK_CARDPUTER 1
// #define DISPLAY_DEVICE_M5STACK_CORE 1
// #define DISPLAY_DEVICE_M5STACK_CORE2 1
// #define DISPLAY_DEVICE_M5STACK_CORES3 1
// #define DISPLAY_DEVICE_M5STACK_CPLUS1_1 1
// #define DISPLAY_DEVICE_M5STACK_CPLUS2 1
// #define DISPLAY_DEVICE_M5STACK_DINMETER 1
// #define DISPLAY_DEVICE_M5STACK_PAPER 1
// #define DISPLAY_DEVICE_M5STACK_PAPER_COLOR 1
// #define DISPLAY_DEVICE_M5STACK_PAPER_S3 1
// #define DISPLAY_DEVICE_M5STACK_STICKS3 1
// #define DISPLAY_DEVICE_M5STACK_TAB5 1
// #define DISPLAY_DEVICE_MARAUDER_MINI 1
// #define DISPLAY_DEVICE_MARAUDER_V4_V6 1
// #define DISPLAY_DEVICE_MARAUDER_V7 1
// #define DISPLAY_DEVICE_MARAUDER_V8 1
// #define DISPLAY_DEVICE_NM_CYD_C5 1
// #define DISPLAY_DEVICE_PANCAKE 1
// #define DISPLAY_DEVICE_PHANTOM_S024R 1
// #define DISPLAY_DEVICE_REAPER 1
// #define DISPLAY_DEVICE_SMOOCHIEE_BOARD 1
// #define DISPLAY_DEVICE_WAVESHARE_ESP32_S3_LCD_147 1
// #define DISPLAY_DEVICE_XUEERSI_XIAOMIAO 1

#if defined(DISPLAY_DEVICE_ARDUINO_NESSO_N1)
    #include "devices/arduino_nesso_n1.h"
#elif defined(DISPLAY_DEVICE_CYD_2432S022C)
    #include "devices/cyd_2432s022c.h"
#elif defined(DISPLAY_DEVICE_CYD_2432S024R)
    #include "devices/cyd_2432s024r.h"
#elif defined(DISPLAY_DEVICE_CYD_2432S028)
    #include "devices/cyd_2432s028.h"
#elif defined(DISPLAY_DEVICE_CYD_2432S032C)
    #include "devices/cyd_2432s032c.h"
#elif defined(DISPLAY_DEVICE_CYD_2432W328C)
    #include "devices/cyd_2432w328c.h"
#elif defined(DISPLAY_DEVICE_CYD_2_USB)
    #include "devices/cyd_2_usb.h"
#elif defined(DISPLAY_DEVICE_CYD_3248S035C)
    #include "devices/cyd_3248s035c.h"
#elif defined(DISPLAY_DEVICE_CYD_3248W535C)
    #include "devices/cyd_3248w535c.h"
#elif defined(DISPLAY_DEVICE_CYD_4827S043R)
    #include "devices/cyd_4827s043r.h"
#elif defined(DISPLAY_DEVICE_CYD_8048S043C)
    #include "devices/cyd_8048s043c.h"
#elif defined(DISPLAY_DEVICE_ELECROW_24B)
    #include "devices/elecrow_24b.h"
#elif defined(DISPLAY_DEVICE_ELECROW_35B)
    #include "devices/elecrow_35b.h"
#elif defined(DISPLAY_DEVICE_ELECROW_35BV2_2)
    #include "devices/elecrow_35bv2_2.h"
#elif defined(DISPLAY_DEVICE_HEADLESS_ESP32_4MB)
    #include "devices/headless_esp32_4mb.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T5_EPAPER_S3_PRO)
    #include "devices/lilygo_t5_epaper_s3_pro.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DECK)
    #include "devices/lilygo_t_deck.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DECK_PRO)
    #include "devices/lilygo_t_deck_pro.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DISPLAY_C5)
    #include "devices/lilygo_t_display_c5.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DISPLAY_P4)
    #include "devices/lilygo_t_display_p4.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DISPLAY_P4_AMOLED)
    #include "devices/lilygo_t_display_p4_amoled.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_AMOLED)
    #include "devices/lilygo_t_display_s3_amoled.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_AMOLED_PLUS)
    #include "devices/lilygo_t_display_s3_amoled_plus.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_PRO)
    #include "devices/lilygo_t_display_s3_pro.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_TOUCH)
    #include "devices/lilygo_t_display_s3_touch.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DONGLE_C5_TFT)
    #include "devices/lilygo_t_dongle_c5_tft.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_DONGLE_S3_TFT)
    #include "devices/lilygo_t_dongle_s3_tft.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_EMBED)
    #include "devices/lilygo_t_embed.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_EMBED_CC1101)
    #include "devices/lilygo_t_embed_cc1101.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_HMI)
    #include "devices/lilygo_t_hmi.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_LORA_PAGER)
    #include "devices/lilygo_t_lora_pager.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_WATCH_S3)
    #include "devices/lilygo_t_watch_s3.h"
#elif defined(DISPLAY_DEVICE_LILYGO_T_WATCH_ULTRA)
    #include "devices/lilygo_t_watch_ultra.h"
#elif defined(DISPLAY_DEVICE_M5STACK_C)
    #include "devices/m5stack_c.h"
#elif defined(DISPLAY_DEVICE_M5STACK_CARDPUTER)
    #include "devices/m5stack_cardputer.h"
#elif defined(DISPLAY_DEVICE_M5STACK_CORE)
    #include "devices/m5stack_core.h"
#elif defined(DISPLAY_DEVICE_M5STACK_CORE2)
    #include "devices/m5stack_core2.h"
#elif defined(DISPLAY_DEVICE_M5STACK_CORES3)
    #include "devices/m5stack_cores3.h"
#elif defined(DISPLAY_DEVICE_M5STACK_CPLUS1_1)
    #include "devices/m5stack_cplus1_1.h"
#elif defined(DISPLAY_DEVICE_M5STACK_CPLUS2)
    #include "devices/m5stack_cplus2.h"
#elif defined(DISPLAY_DEVICE_M5STACK_DINMETER)
    #include "devices/m5stack_dinmeter.h"
#elif defined(DISPLAY_DEVICE_M5STACK_PAPER)
    #include "devices/m5stack_paper.h"
#elif defined(DISPLAY_DEVICE_M5STACK_PAPER_COLOR)
    #include "devices/m5stack_paper_color.h"
#elif defined(DISPLAY_DEVICE_M5STACK_PAPER_S3)
    #include "devices/m5stack_paper_s3.h"
#elif defined(DISPLAY_DEVICE_M5STACK_STICKS3)
    #include "devices/m5stack_sticks3.h"
#elif defined(DISPLAY_DEVICE_M5STACK_TAB5)
    #include "devices/m5stack_tab5.h"
#elif defined(DISPLAY_DEVICE_MARAUDER_MINI)
    #include "devices/marauder_mini.h"
#elif defined(DISPLAY_DEVICE_MARAUDER_V4_V6)
    #include "devices/marauder_v4_v6.h"
#elif defined(DISPLAY_DEVICE_MARAUDER_V7)
    #include "devices/marauder_v7.h"
#elif defined(DISPLAY_DEVICE_MARAUDER_V8)
    #include "devices/marauder_v8.h"
#elif defined(DISPLAY_DEVICE_NM_CYD_C5)
    #include "devices/nm_cyd_c5.h"
#elif defined(DISPLAY_DEVICE_PANCAKE)
    #include "devices/pancake.h"
#elif defined(DISPLAY_DEVICE_PHANTOM_S024R)
    #include "devices/phantom_s024r.h"
#elif defined(DISPLAY_DEVICE_REAPER)
    #include "devices/reaper.h"
#elif defined(DISPLAY_DEVICE_SMOOCHIEE_BOARD)
    #include "devices/smoochiee_board.h"
#elif defined(DISPLAY_DEVICE_WAVESHARE_ESP32_S3_LCD_147)
    #include "devices/waveshare_esp32_s3_lcd_147.h"
#elif defined(DISPLAY_DEVICE_XUEERSI_XIAOMIAO)
    #include "devices/xueersi_xiaomiao.h"
#endif

// ---------------------------------------------------------------------------
// Anything no profile covers goes here: describe the panel with plain TFT_*
// macros, exactly as the README documents. A profile's macros are not guarded,
// so do not restate one it already sets — edit the profile itself instead.
// ---------------------------------------------------------------------------

// clang-format on
#endif // DISPLAYDRIVERS_SETUP_H
