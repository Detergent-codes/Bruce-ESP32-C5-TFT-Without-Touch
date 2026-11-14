// See GxEPD2_X4_800x480_UC8279.h for provenance and licence.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../DisplayDrivers.h"

#if defined(USE_GXEPD2)

#include "GxEPD2_X4_800x480_UC8279.h"

namespace {
// Register values, from the FreeInk SDK's uc8279X4DefaultConfig() —
// cross-checked against crosspoint-reader's Uc8279X4Driver.cpp, which is
// field-validated on a real X4 Pro UC8279 (LUT_VER=0x02) unit.
constexpr uint8_t PSR0 = 0x37;       // REG=1 at init (the AA path uses it as is)
constexpr uint8_t PSR1 = 0x4D;       //
constexpr uint8_t PSR0_OTP = 0x17;   // PSR0 & 0xDF — REG=0, run the OTP waveforms
constexpr uint8_t PFS = 0x20;        // 0x03 power-off sequence
constexpr uint8_t PLL = 0x0E;        // 0x30 frame rate
constexpr uint8_t GATE_SCAN = 0x02;  // 0xE1
constexpr uint8_t CCSET = 0x02;      // 0xE0
constexpr uint8_t TSSET_FULL = 0x1E; // 0xE5 forced temperature, full refresh
constexpr uint8_t TSSET_FAST = 0x5A; // 0xE5, fast/partial
// CDI (0x50, single byte on this part). The confirmed reference DOES write
// this on every B/W refresh — full and partial each get their own value —
// contradicting this file's earlier "no CDI on the B/W path" assumption.
constexpr uint8_t CDI_BW_FULL = 0x97;
constexpr uint8_t CDI_BW_FAST = 0xD7;
} // namespace

GxEPD2_X4_800x480_UC8279::GxEPD2_X4_800x480_UC8279(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
    : GxEPD2_Xteink_UC(
          cs, dc, rst, busy, 10000000, WIDTH, HEIGHT, GATE_OFFSET, GATE_TOTAL, panel
      ) {}

void GxEPD2_X4_800x480_UC8279::_initController() {
    _writeCommand(0x00); // PSR panel setting
    _writeData(PSR0);
    _writeData(PSR1);

    _writeCommand(0x61); // TRES resolution
    _writeData((uint8_t)(WIDTH >> 8));
    _writeData((uint8_t)(WIDTH & 0xFF));
    _writeData((uint8_t)(GATE_TOTAL >> 8));
    _writeData((uint8_t)(GATE_TOTAL & 0xFF));

    _writeCommand(0x65); // GSST gate/source start
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0x03); // PFS power-off sequence
    _writeData(PFS);

    _writeCommand(0x30); // PLL — this part programs the frame rate, not the booster
    _writeData(PLL);

    _writeCommand(0xE1); // gate scan selection
    _writeData(GATE_SCAN);
}

void GxEPD2_X4_800x480_UC8279::_refreshFrame(bool full_sync) {
    // The plane writes pad 120 white rows before the image and none after
    // (120 + 480 = 600); the base class does that from GATE_OFFSET.
    _sendPlane(0x13, false);

    if (full_sync) _fillPlane(0x10, 0xFF);
    // Otherwise 0x10 still holds the previously displayed frame.

    // CDI (0x50): the confirmed reference writes this 1-byte register on
    // EVERY B/W refresh, full and partial each with their own value —
    // corrected from this file's earlier "no CDI on the B/W path" assumption.
    _writeCommand(0x50);
    _writeData(full_sync ? CDI_BW_FULL : CDI_BW_FAST);

    _writeCommand(0xE0); // CCSET
    _writeData(CCSET);
    _writeCommand(0xE5); // TSSET forced temperature
    _writeData(full_sync ? TSSET_FULL : TSSET_FAST);

    if (!full_sync) {
        _writeCommand(0x03);
        _writeData(PFS);
        _writeCommand(0xE1);
        _writeData(GATE_SCAN);
    }

    _PowerOn();

    if (!full_sync) {
        // PTIN + PTL, re-sent on every partial refresh: the confirmed reference
        // notes stock never issues PTIN without a PTL window -- with PTL unset
        // the DU scans but develops nothing (this is what "menu stops drawing,
        // panel still flashes" was). The window covers only the visible rows in
        // gate space (the 120-gate offset), not the white padding gates.
        constexpr uint16_t xEnd = WIDTH - 1;
        constexpr uint16_t yStart = GATE_OFFSET;
        constexpr uint16_t yEnd = GATE_OFFSET + HEIGHT - 1;
        _writeCommand(0x91); // PTIN
        _writeCommand(0x90); // PTL
        _writeData(0x00);
        _writeData(0x00); // x start, byte-aligned
        _writeData((uint8_t)(xEnd >> 8));
        _writeData((uint8_t)(xEnd | 0x07)); // x end, byte-aligned up
        _writeData((uint8_t)(yStart >> 8));
        _writeData((uint8_t)(yStart & 0xFF));
        _writeData((uint8_t)(yEnd >> 8));
        _writeData((uint8_t)(yEnd & 0xFF));
        _writeData(0x01);
    }

    // PSR rewrite happens AFTER PowerOn(), not before: the confirmed reference
    // notes PON reloads the controller's MTP defaults, so a PSR write issued
    // earlier in this function never actually latches.
    _writeCommand(0x00); // PSR, REG cleared so the OTP waveforms run
    _writeData(PSR0_OTP);
    _writeData(PSR1);

    _writeCommand(0x12); // DRF
    _waitWhileBusy("_Update", full_sync ? full_refresh_time : partial_refresh_time);
    if (!full_sync) _writeCommand(0x92); // PTOUT

    // Sync OLD with what is now on the glass, so the next partial diffs against
    // the real screen contents.
    _sendPlane(0x10, false);
}

#endif // USE_GXEPD2
