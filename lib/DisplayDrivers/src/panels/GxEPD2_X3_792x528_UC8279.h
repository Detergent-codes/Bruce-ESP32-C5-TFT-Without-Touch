// GxEPD2 panel class for the Xteink X3 when the glass behind it is a UC8279d —
// the controller Xteink moved to in July 2026, replacing the UC8253 that
// GxEPD2_X3_792x528 drives.
//
// Same 792x528 glass and the same KW command family as its predecessor, and it
// is the odd one of this family in exactly one way: these modules ship with a
// BLANK MTP. Address 0x000 does not hold 0xA5, there are no factory command
// defaults and no per-temperature waveforms, so the host has to supply
// everything — the drive voltages (PWR/VDCS), the booster, the PLL, the active
// window, and the waveform LUT banks themselves. A driver that assumes OTP
// waveforms brings the panel up with no drive rails and leaves it completely
// dark; that is what the first UC8279d field units did.
//
// Two structural differences from the rest of this family:
//
//   * There is no TRES. The active area is set by a full-panel PTL window
//     (0x90) entered with PTIN (0x91), and EVERY plane write and refresh has to
//     run inside it — normal mode addresses the controller's 800x600 frame in
//     100-byte rows and would misalign these 99-byte rows.
//   * The waveform is uploaded per refresh: a "GC" bank for a clearing update
//     and a "DU" bank for a differential one. Both diff the new frame against
//     the real previous frame in 0x10; GC is simply the stronger waveform, not
//     a paint-from-white. Seeding 0x10 white on a GC is what made a black
//     splash pixel that is white in the next frame read old==new==white, select
//     the white-stays table, and ghost the splash through — so it is seeded
//     only on the very first paint, when no previous frame exists.
//
// The register recipe and both waveform banks are transcribed from the FreeInk
// SDK's Uc8279Driver and lut/Uc8279X3Luts.h, reverse-engineered from the stock
// X3 firmware (init FUN_42014ad4, GC FUN_42015786, DU FUN_4201580a):
//   https://github.com/Free-Ink/freeink-sdk
//
// Untested on hardware, and the FreeInk driver it comes from is itself marked
// pending validation on a UC8279d unit.
//
// GxEPD2 is GPL-3.0 and this file is derived from it, so it carries the same
// licence — as does any firmware built with it.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _GxEPD2_X3_792x528_UC8279_H_
#define _GxEPD2_X3_792x528_UC8279_H_

#include "GxEPD2_Xteink_UC.h"

class GxEPD2_X3_792x528_UC8279 : public GxEPD2_Xteink_UC {
public:
    // attributes
    static const uint16_t WIDTH = 792;
    static const uint16_t WIDTH_VISIBLE = WIDTH;
    static const uint16_t HEIGHT = 528;
    // The PTL window is the visible area exactly, so no gate padding.
    static const uint16_t GATE_TOTAL = HEIGHT;
    static const uint16_t GATE_OFFSET = 0;
    // No enumerator of our own: GxEPD2::Panel lives in the library and this
    // board has no business editing it. The value is informational.
    static const GxEPD2::Panel panel = GxEPD2::GDEW075T7;
    static const bool hasColor = false;
    static const bool hasPartialUpdate = true;
    static const bool usePartialUpdateWindow = false;
    static const bool hasFastPartialUpdate = true;
    static const uint16_t power_on_time = 150;        // ms
    static const uint16_t power_off_time = 150;       // ms
    static const uint16_t full_refresh_time = 3000;   // ms
    static const uint16_t partial_refresh_time = 800; // ms

    GxEPD2_X3_792x528_UC8279(int16_t cs, int16_t dc, int16_t rst, int16_t busy);

protected:
    void _initController();
    void _refreshFrame(bool full_sync);

private:
    // PTIN plus the full-panel PTL window. Every RAM write and every refresh
    // has to happen inside it — see the note at the top of this file.
    void _windowIn();

    // The controller has never been told anything since reset, so the first
    // refresh uses a different border setting than the ones after it.
    bool _first_refresh;

    // 5 tables of 43 bytes; byte 0 of each is the LUT register (0x20..0x24) and
    // the remaining 42 are its data. Only the leading bytes are non-zero — the
    // rest is the aggregate initialiser's zero fill, which is what the stock
    // firmware sends.
    static const uint8_t lut_bw_gc[5][43];
    static const uint8_t lut_bw_du[5][43];
};

#endif // _GxEPD2_X3_792x528_UC8279_H_
