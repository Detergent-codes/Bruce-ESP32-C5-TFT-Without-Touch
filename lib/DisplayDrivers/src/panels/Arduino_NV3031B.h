// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// NV3031B 240x320 QSPI LCD (Seeed Wio Tracker L2) for Arduino_GFX.
//
// The NV3031B speaks the same QSPI framing as the NV3041A Arduino_GFX already
// drives (0x02 00 cmd 00 for commands, 0x32 for quad pixel data), so it rides
// Arduino_ESP32QSPI unchanged and only differs in its init table and in how it
// wakes up. The init table and the wake-up quirk come from LovyanGFX's
// Panel_NV3031B (FreeBSD licence, contributed by mverch67 for Meshtastic).
//
// Selected as TFT_DISPLAY_DRIVER_N=51.

#ifndef _DD_ARDUINO_NV3031B_H_
#define _DD_ARDUINO_NV3031B_H_

#include <Arduino_GFX_Library.h>

#define NV3031B_TFTWIDTH 240
#define NV3031B_TFTHEIGHT 320

#define NV3031B_RST_DELAY 120
#define NV3031B_SLPIN_DELAY 120
#define NV3031B_SLPOUT_DELAY 100

#define NV3031B_SWRESET 0x01
#define NV3031B_SLPIN 0x10
#define NV3031B_SLPOUT 0x11
#define NV3031B_INVOFF 0x20
#define NV3031B_INVON 0x21
#define NV3031B_DISPOFF 0x28
#define NV3031B_DISPON 0x29
#define NV3031B_CASET 0x2A
#define NV3031B_RASET 0x2B
#define NV3031B_RAMWR 0x2C
#define NV3031B_MADCTL 0x36

#define NV3031B_MADCTL_MY 0x80
#define NV3031B_MADCTL_MX 0x40
#define NV3031B_MADCTL_MV 0x20
#define NV3031B_MADCTL_RGB 0x00

class Arduino_NV3031B : public Arduino_TFT {
public:
    Arduino_NV3031B(
        Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false,
        int16_t w = NV3031B_TFTWIDTH, int16_t h = NV3031B_TFTHEIGHT, uint8_t col_offset1 = 0,
        uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0
    );

    bool begin(int32_t speed = GFX_NOT_DEFINED) override;
    void setRotation(uint8_t r) override;
    void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;
    void invertDisplay(bool) override;
    void displayOn() override;
    void displayOff() override;

protected:
    void tftInit() override;

private:
    void runInit();
    bool _inverted = false;
};

#endif // _DD_ARDUINO_NV3031B_H_
