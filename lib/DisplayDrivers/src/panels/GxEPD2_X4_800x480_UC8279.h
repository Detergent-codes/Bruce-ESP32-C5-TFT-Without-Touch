// GxEPD2 panel class for the Xteink X4 / X4 Pro when the glass behind it is a
// UC8279 — the second UltraChip variant, distinct from both the original
// SSD1677 and the UC8179 of the batch before it.
//
// Same 800x480 GDEQ0426T82 glass and the same KW command family as the UC8179,
// and like it the B/W waveforms come from OTP. Three things differ and each one
// is enough to leave the panel dark or scrambled if the UC8179 path is used:
//
//   * PSR is 0x37/0x4D, not 0x3F/0x0A, and the PLL (0x30) is programmed at init
//     instead of the booster.
//   * The 480 visible rows do not start at gate 0. The panel is addressed as
//     600 gates with the window offset by 120, so every plane write pads 120
//     white rows before the image as well as after it.
//   * The B/W path writes no CDI at all. 0x50 belongs to the grayscale pass on
//     this part; sending the UC8179's 0x29/0xA9 here is not harmless.
//
// Identified by the VER register (0x70): byte 2 reads 0x02 or 0x68.
//
// Transcribed from the FreeInk SDK's Uc8279X4Driver:
//   https://github.com/Free-Ink/freeink-sdk
//
// Untested on hardware. This is a faithful transcription of code that is known
// to drive the panel, but transcription is not verification.
//
// GxEPD2 is GPL-3.0 and this file is derived from it, so it carries the same
// licence — as does any firmware built with it.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _GxEPD2_X4_800x480_UC8279_H_
#define _GxEPD2_X4_800x480_UC8279_H_

#include "GxEPD2_Xteink_UC.h"

class GxEPD2_X4_800x480_UC8279 : public GxEPD2_Xteink_UC {
public:
    // attributes
    static const uint16_t WIDTH = 800;
    static const uint16_t WIDTH_VISIBLE = WIDTH;
    static const uint16_t HEIGHT = 480;
    // 600 addressed gates with the visible window starting at gate 120.
    static const uint16_t GATE_TOTAL = 600;
    static const uint16_t GATE_OFFSET = 120;
    static const GxEPD2::Panel panel = GxEPD2::GDEQ0426T82;
    static const bool hasColor = false;
    static const bool hasPartialUpdate = true;
    static const bool usePartialUpdateWindow = false;
    static const bool hasFastPartialUpdate = true;
    static const uint16_t power_on_time = 150;        // ms
    static const uint16_t power_off_time = 150;       // ms
    static const uint16_t full_refresh_time = 3000;   // ms
    static const uint16_t partial_refresh_time = 800; // ms

    GxEPD2_X4_800x480_UC8279(int16_t cs, int16_t dc, int16_t rst, int16_t busy);

protected:
    void _initController();
    void _refreshFrame(bool full_sync);
};

#endif // _GxEPD2_X4_800x480_UC8279_H_
