// See GxEPD2_X3_792x528_UC8279.h for provenance and licence.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../DisplayDrivers.h"

#if defined(USE_GXEPD2)

#include "GxEPD2_X3_792x528_UC8279.h"

namespace {
constexpr uint8_t CDI_FIRST = 0x97; // 0x50, first refresh after init
constexpr uint8_t CDI_LATER = 0xD7; // 0x50, every refresh after that
} // namespace

// Waveform banks, byte-exact from the stock X3 firmware. Each row is
// {LUT register, 42 data bytes}; everything past the values written here is
// zero, which is what the firmware sends.
// clang-format off
const uint8_t GxEPD2_X3_792x528_UC8279::lut_bw_gc[5][43] PROGMEM = {
    {0x20, 0x01, 0x1A, 0x1A, 0x01, 0x00, 0x01, 0x01},
    {0x21, 0x01, 0x5A, 0x9A, 0x01, 0x00, 0x01, 0x01},
    {0x22, 0x01, 0x1A, 0x9A, 0x01, 0x00, 0x01, 0x01},
    {0x23, 0x01, 0x1A, 0x5A, 0x01, 0x00, 0x01, 0x01},
    {0x24, 0x01, 0x9A, 0x5A, 0x01, 0x00, 0x01, 0x01},
};

const uint8_t GxEPD2_X3_792x528_UC8279::lut_bw_du[5][43] PROGMEM = {
    {0x20, 0x01, 0x07, 0x01, 0x06, 0x06, 0x01, 0x01, 0x01, 0x06, 0x01, 0x00, 0x00, 0x01},
    {0x21, 0x01, 0x07, 0x81, 0x06, 0x06, 0x01, 0x01, 0x01, 0x06, 0x01, 0x00, 0x00, 0x01},
    {0x22, 0x01, 0x87, 0x81, 0x86, 0x86, 0x01, 0x01, 0x01, 0x86, 0x01, 0x00, 0x00, 0x01},
    {0x23, 0x01, 0x47, 0x41, 0x46, 0x46, 0x01, 0x01, 0x01, 0x46, 0x01, 0x00, 0x00, 0x01},
    {0x24, 0x01, 0x07, 0x01, 0x06, 0x06, 0x01, 0x01, 0x01, 0x06, 0x01, 0x00, 0x00, 0x01},
};
// clang-format on

GxEPD2_X3_792x528_UC8279::GxEPD2_X3_792x528_UC8279(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
    : GxEPD2_Xteink_UC(cs, dc, rst, busy, 10000000, WIDTH, HEIGHT, GATE_OFFSET, GATE_TOTAL, panel),
      _first_refresh(true) {}

void GxEPD2_X3_792x528_UC8279::_windowIn() {
    _writeCommand(0x91); // PTIN
    _writeCommand(0x90); // PTL: X 0..791, Y 0..527 in gate space, PT_SCAN = 1
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x03);
    _writeData(0x17);
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x02);
    _writeData(0x0F);
    _writeData(0x01);
}

void GxEPD2_X3_792x528_UC8279::_initController() {
    _writeCommand(0x00); // PSR — REG=1, the waveform comes from the banks below
    _writeData(0x3F);
    _writeData(0x4A);

    _windowIn(); // PTL stands in for TRES on this part

    _writeCommand(0x03); // PFS power-off sequence
    _writeData(0x20);

    _writeCommand(0x01); // PWR drive voltages — a blank MTP supplies none of these
    _writeData(0x43);
    _writeData(0x00);
    _writeData(0x78);
    _writeData(0x78);
    _writeData(0x17);

    _writeCommand(0x82); // VDCS, VCOM_DC
    _writeData(0x24);

    _writeCommand(0x06); // BTST booster soft start
    _writeData(0x25);
    _writeData(0x25);
    _writeData(0x3C);

    _writeCommand(0x30); // PLL
    _writeData(0x0F);

    _writeCommand(0xE1); // gate scan selection
    _writeData(0x02);

    _first_refresh = true;
}

void GxEPD2_X3_792x528_UC8279::_refreshFrame(bool full_sync) {
    _windowIn();

    // Seed OLD white only when there is genuinely no previous frame. On a GC the
    // bank still resolves the true old->new transition, so overwriting a valid
    // 0x10 here would make an unchanged white pixel select the white-stays
    // table and leave whatever was under it on the glass.
    if (!_old_ram_valid) {
        _fillPlane(0x10, 0xFF);
        _writeCommand(0x11); // DSP, data stop
    }

    _sendPlane(0x13, false); // NEW plane
    _writeCommand(0x11);

    _writeCommand(0x50); // CDI
    _writeData(_first_refresh ? CDI_FIRST : CDI_LATER);

    _writeBank(full_sync ? lut_bw_gc : lut_bw_du);

    _PowerOn();
    _writeCommand(0x12); // DRF
    _waitWhileBusy("_Update", full_sync ? full_refresh_time : partial_refresh_time);

    _writeCommand(0x50); // restore the border hold
    _writeData(CDI_LATER);

    // Sync OLD with the frame just displayed. This has to happen while still
    // inside the PTL window: the NEW plane above was written windowed, so an
    // unwindowed 0x10 would land on different gates and the next differential
    // update would drive garbage.
    _sendPlane(0x10, false);
    _writeCommand(0x11);

    _writeCommand(0x92); // PTOUT

    _first_refresh = false;
}

#endif // USE_GXEPD2
