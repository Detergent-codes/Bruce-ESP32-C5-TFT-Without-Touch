// GxEPD2 panel class for the Xteink X4 / X4 Pro when the glass behind it is a
// UC8179 — the controller newer production batches ship instead of the
// SSD1677 the stock GxEPD2_426_GDEQ0426T82 class drives.
//
// Same 800x480 GDEQ0426T82 glass, same pinout, different silicon. The UC8179 is
// an UltraChip part in the KW command family (0x00 panel setting, 0x61
// resolution, 0x10/0x13 data planes, 0x12 trigger), BUSY is active LOW, and its
// gates scan bottom to top. It runs the factory OTP waveforms — the MTP holds
// temperature-compensated LUT sets — so nothing is uploaded here; what it does
// need, and what the SSD1677 path never sends, is an explicit PLL/booster/VCOM
// bring-up. On OTP defaults alone a UC8179 accepts every command and never
// develops an image, which is the failure mode a newer-batch unit shows when
// flashed with a build that only knows the SSD1677.
//
// The register recipe is transcribed from the FreeInk SDK's Uc8179Driver
// (recovered from the X4 Pro OEM firmware, UC8179_800x480 init FUN_4214dff8 /
// full-update FUN_4214e584):
//   https://github.com/Free-Ink/freeink-sdk
//
// Two things worth knowing about the geometry: the panel is addressed as
// 800x600 even though 480 rows are visible — the OTP waveform is tuned for the
// full 600-gate scan — so every plane write is padded white to 600 rows. And
// the fast/partial path is a real PTIN/PTOUT partial with a different forced
// temperature (0xE5), which is the lever that makes it shorter than a full.
//
// Untested on hardware — nobody here has a UC8179 unit. This is a faithful
// transcription of code that is known to drive the panel, but transcription is
// not verification. The FreeInk driver itself is marked pending hardware
// validation.
//
// GxEPD2 is GPL-3.0 and this file is derived from it, so it carries the same
// licence — as does any firmware built with it.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _GxEPD2_X4_800x480_UC8179_H_
#define _GxEPD2_X4_800x480_UC8179_H_

#include "GxEPD2_Xteink_UC.h"

class GxEPD2_X4_800x480_UC8179 : public GxEPD2_Xteink_UC {
public:
    // attributes
    static const uint16_t WIDTH = 800;
    static const uint16_t WIDTH_VISIBLE = WIDTH;
    static const uint16_t HEIGHT = 480;
    // The controller addresses 600 gates; only these 480 carry pixels.
    static const uint16_t GATE_TOTAL = 600;
    static const uint16_t GATE_OFFSET = 0;
    static const GxEPD2::Panel panel = GxEPD2::GDEQ0426T82;
    static const bool hasColor = false;
    static const bool hasPartialUpdate = true;
    static const bool usePartialUpdateWindow = false;
    static const bool hasFastPartialUpdate = true;
    static const uint16_t power_on_time = 150;        // ms
    static const uint16_t power_off_time = 150;       // ms
    static const uint16_t full_refresh_time = 3000;   // ms
    static const uint16_t partial_refresh_time = 800; // ms

    GxEPD2_X4_800x480_UC8179(int16_t cs, int16_t dc, int16_t rst, int16_t busy);

protected:
    void _initController();
    void _refreshFrame(bool full_sync);
};

#endif // _GxEPD2_X4_800x480_UC8179_H_
