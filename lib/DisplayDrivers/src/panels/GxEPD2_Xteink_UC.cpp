// See GxEPD2_Xteink_UC.h for provenance and licence.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// This file is built for every environment, so it has to compile away to
// nothing unless GxEPD2 is the selected backend — the library's headers are
// not on the include path otherwise.
#include "../DisplayDrivers.h"

#if defined(USE_GXEPD2)

#include "GxEPD2_Xteink_UC.h"

GxEPD2_Xteink_UC::GxEPD2_Xteink_UC(
    int16_t cs, int16_t dc, int16_t rst, int16_t busy, uint32_t busy_timeout, uint16_t w, uint16_t h,
    uint16_t gate_offset, uint16_t gate_total, GxEPD2::Panel p
)
    // BUSY is active LOW on every UltraChip part in this family, unlike the
    // X4's original SSD1677.
    : GxEPD2_EPD(cs, dc, rst, busy, LOW, busy_timeout, w, h, p, false, true, true), _w(w), _h(h),
      _wb(w / 8), _gate_offset(gate_offset), _gate_total(gate_total), _pending(nullptr),
      _pending_fill(0xFF), _pending_pgm(false), _pending_invert(false), _pending_mirror(false),
      _old_ram_valid(false), _panel_configured(false) {}

void GxEPD2_Xteink_UC::_recordPlane(
    const uint8_t *bitmap, uint8_t fill, bool invert, bool mirror_y, bool pgm
) {
    _pending = bitmap;
    _pending_fill = fill;
    _pending_invert = invert;
    _pending_mirror = mirror_y;
    _pending_pgm = pgm;
}

void GxEPD2_Xteink_UC::_sendPlane(uint8_t command, bool invert) {
    _writeCommand(command);
    _startTransfer();
    // Gates below the visible window. White, always: this is off-panel and the
    // waveform only needs it to hold a defined level.
    for (uint16_t y = 0; y < _gate_offset; y++) {
        for (uint16_t xb = 0; xb < _wb; xb++) _transfer(0xFF);
    }
    for (uint16_t y = 0; y < _h; y++) {
        // Confirmed on X4 Pro hardware (UC8279, index 2, ROTATION=3): the
        // panel is physically landscape but driven rotated to portrait, so
        // this class's own "X" (source/SHL, PSR0 — left as the SDK default,
        // see GxEPD2_X4_800x480_UC8279.cpp) is what the user sees as the
        // vertical axis, and this row/gate order is what the user sees as
        // the horizontal axis. The baseline (row order reversed by default)
        // showed correct Y but mirrored X on screen; feeding rows in natural
        // top-to-bottom order fixes that, without touching PSR0.
        const uint16_t srcY = _pending_mirror ? (uint16_t)(_h - 1 - y) : y;
        for (uint16_t xb = 0; xb < _wb; xb++) {
            uint8_t data = _pending_fill;
            if (_pending != nullptr) {
                const uint32_t idx = (uint32_t)srcY * _wb + xb;
                data = _pending_pgm ? pgm_read_byte(&_pending[idx]) : _pending[idx];
            }
            if (_pending_invert != invert) data = ~data;
            _transfer(data);
        }
    }
    for (uint16_t y = (uint16_t)(_gate_offset + _h); y < _gate_total; y++) {
        for (uint16_t xb = 0; xb < _wb; xb++) _transfer(0xFF);
    }
    _endTransfer();
}

void GxEPD2_Xteink_UC::_fillPlane(uint8_t command, uint8_t value) {
    _writeCommand(command);
    _startTransfer();
    for (uint16_t y = 0; y < _gate_total; y++) {
        for (uint16_t xb = 0; xb < _wb; xb++) _transfer(value);
    }
    _endTransfer();
}

void GxEPD2_Xteink_UC::_writeBank(const uint8_t (*bank)[43]) {
    for (uint8_t t = 0; t < 5; t++) {
        _writeCommand(pgm_read_byte(&bank[t][0]));
        _writeDataPGM(&bank[t][1], 42);
    }
}

void GxEPD2_Xteink_UC::_InitDisplay() {
    if (_hibernating) {
        _reset();
        delay(50); // the SDK settles for 50ms after reset on these panels
        _panel_configured = false;
        _old_ram_valid = false;
    }
    if (_panel_configured) return;

    _initController();

    _panel_configured = true;
    _power_is_on = false;
}

void GxEPD2_Xteink_UC::_PowerOn() {
    if (!_power_is_on) {
        _writeCommand(0x04);
        _waitWhileBusy("_PowerOn", 150);
    }
    _power_is_on = true;
}

void GxEPD2_Xteink_UC::_PowerOff() {
    if (_power_is_on) {
        _writeCommand(0x02);
        _waitWhileBusy("_PowerOff", 150);
    }
    _power_is_on = false;
}

void GxEPD2_Xteink_UC::_deepSleep() {
    _writeCommand(0x07);
    _writeData(0xA5);
}

void GxEPD2_Xteink_UC::writeScreenBuffer(uint8_t value) {
    _initial_write = false;
    _recordPlane(nullptr, value, false, false, false);
}

void GxEPD2_Xteink_UC::clearScreen(uint8_t value) {
    _recordPlane(nullptr, value, false, false, false);
    _old_ram_valid = false; // a clear is always a full sync
    refresh(false);
}

void GxEPD2_Xteink_UC::writeImage(
    const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y,
    bool pgm
) {
    // Only whole-screen writes reach the panel; see the header.
    if ((x != 0) || (y != 0) || (w != int16_t(_w)) || (h != int16_t(_h))) return;
    _recordPlane(bitmap, 0xFF, invert, mirror_y, pgm);
}

void GxEPD2_Xteink_UC::writeImageToPrevious(
    const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y,
    bool pgm
) {
    // refresh() syncs 0x10 to the frame it displayed, so seeding it by hand
    // would only fight the controller.
    (void)bitmap;
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)invert;
    (void)mirror_y;
    (void)pgm;
}

void GxEPD2_Xteink_UC::writeImage(
    const uint8_t *black, const uint8_t *color, int16_t x, int16_t y, int16_t w, int16_t h,
    bool invert, bool mirror_y, bool pgm
) {
    (void)color; // monochrome panel
    writeImage(black, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_Xteink_UC::writeNative(
    const uint8_t *data1, const uint8_t *data2, int16_t x, int16_t y, int16_t w, int16_t h,
    bool invert, bool mirror_y, bool pgm
) {
    (void)data2;
    writeImage(data1, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_Xteink_UC::drawImage(
    const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y,
    bool pgm
) {
    writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
    refresh(true);
}

void GxEPD2_Xteink_UC::drawImage(
    const uint8_t *black, const uint8_t *color, int16_t x, int16_t y, int16_t w, int16_t h,
    bool invert, bool mirror_y, bool pgm
) {
    writeImage(black, color, x, y, w, h, invert, mirror_y, pgm);
    refresh(true);
}

void GxEPD2_Xteink_UC::drawNative(
    const uint8_t *data1, const uint8_t *data2, int16_t x, int16_t y, int16_t w, int16_t h,
    bool invert, bool mirror_y, bool pgm
) {
    writeNative(data1, data2, x, y, w, h, invert, mirror_y, pgm);
    refresh(true);
}

// --- sub-screen variants: promote to full screen or drop, never guess ---

void GxEPD2_Xteink_UC::writeImagePart(
    const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
    int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm
) {
    if ((x_part == 0) && (y_part == 0) && (w_bitmap == int16_t(_w)) && (h_bitmap == int16_t(_h))) {
        writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
    }
}

void GxEPD2_Xteink_UC::writeImagePartToPrevious(
    const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
    int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm
) {
    (void)bitmap;
    (void)x_part;
    (void)y_part;
    (void)w_bitmap;
    (void)h_bitmap;
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)invert;
    (void)mirror_y;
    (void)pgm;
}

void GxEPD2_Xteink_UC::writeImagePart(
    const uint8_t *black, const uint8_t *color, int16_t x_part, int16_t y_part, int16_t w_bitmap,
    int16_t h_bitmap, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y,
    bool pgm
) {
    (void)color;
    writeImagePart(black, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_Xteink_UC::drawImagePart(
    const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
    int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm
) {
    writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
    refresh(true);
}

void GxEPD2_Xteink_UC::drawImagePart(
    const uint8_t *black, const uint8_t *color, int16_t x_part, int16_t y_part, int16_t w_bitmap,
    int16_t h_bitmap, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y,
    bool pgm
) {
    writeImagePart(
        black, color, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm
    );
    refresh(true);
}

void GxEPD2_Xteink_UC::refresh(int16_t x, int16_t y, int16_t w, int16_t h) {
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    refresh(true);
}

void GxEPD2_Xteink_UC::refresh(bool partial_update_mode) {
    _InitDisplay();

    // A differential update is only meaningful once 0x10 holds a frame the
    // controller put there itself.
    const bool full_sync = !partial_update_mode || !_old_ram_valid || _initial_refresh;

    _refreshFrame(full_sync);

    _old_ram_valid = true;
    _initial_refresh = false;
    _using_partial_mode = !full_sync;
}

void GxEPD2_Xteink_UC::powerOff() { _PowerOff(); }

void GxEPD2_Xteink_UC::hibernate() {
    _PowerOff();
    if (_rst >= 0) {
        _deepSleep();
        _hibernating = true;
        _panel_configured = false;
        _old_ram_valid = false;
    }
}

#endif // USE_GXEPD2
