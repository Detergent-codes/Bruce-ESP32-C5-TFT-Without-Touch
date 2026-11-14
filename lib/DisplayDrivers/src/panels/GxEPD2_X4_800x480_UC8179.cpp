// See GxEPD2_X4_800x480_UC8179.h for provenance and licence.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../DisplayDrivers.h"

#if defined(USE_GXEPD2)

#include "GxEPD2_X4_800x480_UC8179.h"

namespace {
// Register values, from the FreeInk SDK's uc8179DefaultConfig().
constexpr uint8_t PSR0 = 0x3F;      // REG=1 at init; refresh re-asserts it with REG cleared
constexpr uint8_t PSR1 = 0x0A;      //
constexpr uint8_t PSR0_OTP = 0x1F;  // PSR0 & 0xDF — REG=0, run the OTP waveforms
constexpr uint8_t PFS = 0x20;       // 0x03 power-off sequence
constexpr uint8_t GATE_SCAN = 0x02; // 0xE1
constexpr uint8_t CCSET = 0x02;     // 0xE0
constexpr uint8_t TSSET_FULL = 0x1E; // 0xE5 forced temperature, full refresh
constexpr uint8_t TSSET_FAST = 0x5A; // 0xE5 — the frame-rate lever that shortens a partial
constexpr uint8_t CDI_ACTIVE = 0x29; // 0x50 byte0 during a refresh
constexpr uint8_t CDI_IDLE = 0xA9;   // 0x50 byte0 restored afterwards
constexpr uint8_t CDI_INTERVAL = 0x07; // 0x50 byte1, constant
constexpr uint8_t POWER_SAVE = 0x22;   // 0xE3 — VCOM 2 lines, source 2 * 660 ns
} // namespace

GxEPD2_X4_800x480_UC8179::GxEPD2_X4_800x480_UC8179(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
    : GxEPD2_Xteink_UC(
          cs, dc, rst, busy, 10000000, WIDTH, HEIGHT, GATE_OFFSET, GATE_TOTAL, panel
      ) {}

void GxEPD2_X4_800x480_UC8179::_initController() {
    _writeCommand(0x00); // PSR panel setting
    _writeData(PSR0);
    _writeData(PSR1);

    _writeCommand(0x61); // TRES resolution: HRES then VRES, both 16-bit big endian
    _writeData((uint8_t)(WIDTH >> 8));
    _writeData((uint8_t)(WIDTH & 0xFF));
    _writeData((uint8_t)(GATE_TOTAL >> 8)); // the addressed gate count, not the visible height
    _writeData((uint8_t)(GATE_TOTAL & 0xFF));

    _writeCommand(0x65); // GSST gate/source start — vendor reference writes four zeroes
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0x03); // PFS power-off sequence
    _writeData(PFS);

    _writeCommand(0x06); // BTST booster soft start
    _writeData(0x25);
    _writeData(0x25);
    _writeData(0x3C);
    _writeData(0x25);

    _writeCommand(0xE1); // gate scan selection
    _writeData(GATE_SCAN);

    // PWS. GxEPD2 added this UC8179 setting specifically to stop dithered
    // bitmaps from tearing; the OEM stream carries it too.
    _writeCommand(0xE3);
    _writeData(POWER_SAVE);
}

void GxEPD2_X4_800x480_UC8179::_refreshFrame(bool full_sync) {
    // NEW plane. In KW mode the (old,new) pair picks the per-pixel waveform, so
    // this is always the frame being displayed.
    _sendPlane(0x13, false);

    if (full_sync) {
        // Seed OLD white: every pixel then classifies as a transition from
        // white and the OTP clearing waveform runs over the whole panel.
        _fillPlane(0x10, 0xFF);
    }
    // Otherwise 0x10 still holds the previously displayed frame, put there at
    // the end of the last refresh. That is what lets the KW (black->white)
    // waveform actually erase last page's text instead of ghosting it.

    _writeCommand(0x50); // CDI
    _writeData(CDI_ACTIVE);
    _writeData(CDI_INTERVAL);
    _writeCommand(0xE0); // CCSET
    _writeData(CCSET);
    _writeCommand(0xE5); // TSSET forced temperature — selects the OTP frame count
    _writeData(full_sync ? TSSET_FULL : TSSET_FAST);
    _writeCommand(0x00); // PSR again, REG cleared so the OTP waveforms run
    _writeData(PSR0_OTP);
    _writeData(PSR1);

    if (!full_sync) {
        // Fast only. Without these the OTP waveform runs at the full frame
        // count: same duration as a full refresh, and garbled.
        _writeCommand(0x03);
        _writeData(PFS);
        _writeCommand(0xE1);
        _writeData(GATE_SCAN);
    }

    _PowerOn();

    if (!full_sync) _writeCommand(0x91); // PTIN — whole-panel partial, no 0x90 window
    _writeCommand(0x12);                 // DRF
    _waitWhileBusy("_Update", full_sync ? full_refresh_time : partial_refresh_time);
    if (!full_sync) _writeCommand(0x92); // PTOUT

    _writeCommand(0x50); // restore the idle border, as the OEM does
    _writeData(CDI_IDLE);
    _writeData(CDI_INTERVAL);

    // Sync OLD with what is now on the glass so the next partial has a real
    // baseline. This is the piece that makes fast page turns clean.
    _sendPlane(0x10, false);
}

#endif // USE_GXEPD2
