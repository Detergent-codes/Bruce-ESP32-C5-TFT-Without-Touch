// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

// TFT_DISPLAY_DRIVER_N number -> Arduino_GFX class name.
//
// The includer sets DD_DRV_ID; this leaves the class name in DD_DRV_CLASS.
// Split out of ardgfx_driver.inc so the header can resolve the board's primary
// driver into the TFT_DISPLAY_DRIVER alias from the same table the factory
// builds from. Deliberately not #pragma once.

#if !defined(DD_DRV_ID)
    #error "ardgfx_driver_class.inc requires DD_DRV_ID to be defined by the includer"
#endif

// clang-format off
#undef DD_DRV_CLASS
#if DD_DRV_ID == 0
    #define DD_DRV_CLASS Arduino_ST7735
#elif DD_DRV_ID == 1
    #define DD_DRV_CLASS Arduino_ST7789
#elif DD_DRV_ID == 2
    #define DD_DRV_CLASS Arduino_ST7796
#elif DD_DRV_ID == 3
    #define DD_DRV_CLASS Arduino_ST77916
#elif DD_DRV_ID == 4
    #define DD_DRV_CLASS Arduino_ILI9341
#elif DD_DRV_ID == 5
    #define DD_DRV_CLASS Arduino_GC9A01
#elif DD_DRV_ID == 6
    #define DD_DRV_CLASS Arduino_GC9C01
#elif DD_DRV_ID == 7
    #define DD_DRV_CLASS Arduino_GC9D01
#elif DD_DRV_ID == 8
    #define DD_DRV_CLASS Arduino_GC9106
#elif DD_DRV_ID == 9
    #define DD_DRV_CLASS Arduino_GC9107
#elif DD_DRV_ID == 10
    #define DD_DRV_CLASS Arduino_HX8347C
#elif DD_DRV_ID == 11
    #define DD_DRV_CLASS Arduino_HX8347D
#elif DD_DRV_ID == 12
    #define DD_DRV_CLASS Arduino_HX8352C
#elif DD_DRV_ID == 13
    #define DD_DRV_CLASS Arduino_HX8369A
#elif DD_DRV_ID == 14
    #define DD_DRV_CLASS Arduino_NT35310
#elif DD_DRV_ID == 15
    #define DD_DRV_CLASS Arduino_NT35510
#elif DD_DRV_ID == 16
    #define DD_DRV_CLASS Arduino_NT39125
#elif DD_DRV_ID == 17
    #define DD_DRV_CLASS Arduino_NV3007
#elif DD_DRV_ID == 18
    #define DD_DRV_CLASS Arduino_NV3023
#elif DD_DRV_ID == 19
    #define DD_DRV_CLASS Arduino_NV3041A
#elif DD_DRV_ID == 20
    #define DD_DRV_CLASS Arduino_OTM8009A
#elif DD_DRV_ID == 21
    #define DD_DRV_CLASS Arduino_JBT6K71
#elif DD_DRV_ID == 22
    #define DD_DRV_CLASS Arduino_AXS15231B
#elif DD_DRV_ID == 23
    #define DD_DRV_CLASS Arduino_ILI9331
#elif DD_DRV_ID == 24
    #define DD_DRV_CLASS Arduino_ILI9342
#elif DD_DRV_ID == 25
    #define DD_DRV_CLASS Arduino_ILI9481_18bit
#elif DD_DRV_ID == 26
    #define DD_DRV_CLASS Arduino_ILI9486
#elif DD_DRV_ID == 27
    #define DD_DRV_CLASS Arduino_ILI9486_18bit
#elif DD_DRV_ID == 28
    #define DD_DRV_CLASS Arduino_ILI9488
#elif DD_DRV_ID == 29
    #define DD_DRV_CLASS Arduino_ILI9488_18bit
#elif DD_DRV_ID == 30
    #define DD_DRV_CLASS Arduino_ILI9488_3bit
#elif DD_DRV_ID == 31
    #define DD_DRV_CLASS Arduino_ILI9806
#elif DD_DRV_ID == 32
    #define DD_DRV_CLASS Arduino_HX8357A
#elif DD_DRV_ID == 33
    #define DD_DRV_CLASS Arduino_HX8357B
#elif DD_DRV_ID == 34
    #define DD_DRV_CLASS Arduino_R61529
#elif DD_DRV_ID == 35
    #define DD_DRV_CLASS Arduino_RM67162
#elif DD_DRV_ID == 36
    #define DD_DRV_CLASS Arduino_SSD1283A
#elif DD_DRV_ID == 37
    #define DD_DRV_CLASS Arduino_SSD1331
#elif DD_DRV_ID == 38
    #define DD_DRV_CLASS Arduino_SSD1351
#elif DD_DRV_ID == 39
    #define DD_DRV_CLASS Arduino_SH8601
#elif DD_DRV_ID == 40
    #define DD_DRV_CLASS Arduino_RM690B0
#elif DD_DRV_ID == 41
    #define DD_DRV_CLASS Arduino_CO5300
#elif DD_DRV_ID == 42
    #define DD_DRV_CLASS Arduino_JD9613
#elif DD_DRV_ID == 43
    #define DD_DRV_CLASS Arduino_SEPS525
#elif DD_DRV_ID == 44
    #define DD_DRV_CLASS Arduino_ILI9225
#elif DD_DRV_ID == 45
    #define DD_DRV_CLASS Arduino_SPD2010
#elif DD_DRV_ID == 46
    #define DD_DRV_CLASS Arduino_WEA2012
#elif DD_DRV_ID >= 47 && DD_DRV_ID <= 48
    #error "SSD1306 (47) and SH1106 (48) derive from Arduino_G, not Arduino_GFX: they are\n \
    output sinks with no drawing API of their own, and have to be driven through an\n \
    Arduino_Canvas_Mono. That canvas takes no rotation and uses verticalByte instead,\n \
    so it does not fit the USE_CANVAS path here and is not wired up yet.\n \
    Use the LovyanGFX backend (Panel_SSD1306 / Panel_SH1106) for these panels."
#elif DD_DRV_ID == 49
    #define DD_DRV_CLASS Arduino_RGB_Display
#elif DD_DRV_ID == 50
    #define DD_DRV_CLASS Arduino_DSI_Display
#elif DD_DRV_ID == 51
    #define DD_DRV_CLASS Arduino_NV3031B // src/panels/, not part of Arduino_GFX
#elif DD_DRV_ID == 52
    #define DD_DRV_CLASS Arduino_ST77922_1L4 // src/panels/, not part of Arduino_GFX
#else
    #error "Invalid driver id in TFT_DISPLAY_DRIVER_N / TFT_DISPLAY_DRIVER_N_ALT*"
#endif
// clang-format on

