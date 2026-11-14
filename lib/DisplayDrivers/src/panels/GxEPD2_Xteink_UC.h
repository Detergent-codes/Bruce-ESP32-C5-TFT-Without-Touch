// Shared base for the Xteink UltraChip panels that GxEPD2 has no class for.
//
// The X3 and the X4 changed panel controller mid-production and each new
// controller is an UltraChip part driven in KW mode: 1 bpp, command 0x10 holds
// the OLD plane, 0x13 the NEW one, and the pair (old,new) selects which of the
// five waveform tables drives each pixel. Everything above that — the register
// bring-up, whether the waveforms come from OTP or have to be uploaded, and how
// the visible rows sit inside the controller's addressed gate count — differs
// per controller, so those are the two hooks a subclass fills in.
//
// What lives here is the part GxEPD2 makes every panel class repeat: the dozen
// writeImage/drawImage overloads, the deferred plane write, power on/off and
// hibernate. It is a base rather than a copy because there are three of these
// classes and they only disagree about two functions.
//
// Deferred plane write: GxEPD2 hands over the image first and only says what
// kind of refresh it wants afterwards. These controllers need that answer
// before the planes go out — the waveform bank, the data polarity and whether
// the OLD plane is seeded all depend on it — so the write is recorded and
// replayed from refresh(). The pointer is borrowed; GxEPD2_BW's page buffer
// outlives every call.
//
// Sub-screen writes are not implemented. The launcher paints a full page buffer
// and calls display(); a windowed write would mean guessing at 0x90/0x91/0x92
// geometry on panels nobody here can test, so the part variants promote to a
// full-screen write when the region covers the screen and are dropped
// otherwise — the same choice GxEPD2_X3_792x528 already makes.
//
// GxEPD2 is GPL-3.0 and this file is derived from it, so it carries the same
// licence — as does any firmware built with it.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _GxEPD2_XTEINK_UC_H_
#define _GxEPD2_XTEINK_UC_H_

#include <GxEPD2_EPD.h>

class GxEPD2_Xteink_UC : public GxEPD2_EPD {
public:
    // gate_offset / gate_total describe where the visible rows sit inside the
    // controller's addressed gate count. The UC8279 X4 panel, for instance, is
    // addressed as 600 gates with the 480 visible rows starting at gate 120;
    // the rest is padded white on every plane write. A panel whose addressed
    // height is its visible height passes 0 and h.
    GxEPD2_Xteink_UC(
        int16_t cs, int16_t dc, int16_t rst, int16_t busy, uint32_t busy_timeout, uint16_t w,
        uint16_t h, uint16_t gate_offset, uint16_t gate_total, GxEPD2::Panel p
    );

    void clearScreen(uint8_t value = 0xFF);
    void writeScreenBuffer(uint8_t value = 0xFF);

    void writeImage(
        const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false,
        bool mirror_y = false, bool pgm = false
    );
    void writeImageToPrevious(
        const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false,
        bool mirror_y = false, bool pgm = false
    );
    void writeImage(
        const uint8_t *black, const uint8_t *color, int16_t x, int16_t y, int16_t w, int16_t h,
        bool invert = false, bool mirror_y = false, bool pgm = false
    );
    void writeNative(
        const uint8_t *data1, const uint8_t *data2, int16_t x, int16_t y, int16_t w, int16_t h,
        bool invert = false, bool mirror_y = false, bool pgm = false
    );
    void drawImage(
        const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false,
        bool mirror_y = false, bool pgm = false
    );
    void drawImage(
        const uint8_t *black, const uint8_t *color, int16_t x, int16_t y, int16_t w, int16_t h,
        bool invert = false, bool mirror_y = false, bool pgm = false
    );
    void drawNative(
        const uint8_t *data1, const uint8_t *data2, int16_t x, int16_t y, int16_t w, int16_t h,
        bool invert = false, bool mirror_y = false, bool pgm = false
    );

    // The refresh itself syncs 0x10 to the frame it just displayed, so there is
    // nothing for the caller to do to keep the two planes equal.
    void writeImageAgain(
        const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false,
        bool mirror_y = false, bool pgm = false
    ) {}
    void writeImagePartAgain(
        const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false,
        bool pgm = false
    ) {}

    void writeImagePart(
        const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false,
        bool pgm = false
    );
    void writeImagePartToPrevious(
        const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false,
        bool pgm = false
    );
    void writeImagePart(
        const uint8_t *black, const uint8_t *color, int16_t x_part, int16_t y_part,
        int16_t w_bitmap, int16_t h_bitmap, int16_t x, int16_t y, int16_t w, int16_t h,
        bool invert = false, bool mirror_y = false, bool pgm = false
    );
    void drawImagePart(
        const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false,
        bool pgm = false
    );
    void drawImagePart(
        const uint8_t *black, const uint8_t *color, int16_t x_part, int16_t y_part,
        int16_t w_bitmap, int16_t h_bitmap, int16_t x, int16_t y, int16_t w, int16_t h,
        bool invert = false, bool mirror_y = false, bool pgm = false
    );

    void refresh(bool partial_update_mode = false);
    void refresh(int16_t x, int16_t y, int16_t w, int16_t h);
    void powerOff();
    void hibernate();

protected:
    // Register bring-up after reset. Called once per power cycle; a subclass
    // must leave the controller ready to take planes and a refresh.
    virtual void _initController() = 0;
    // One refresh: planes, waveform selection, PON, DRF, and whatever the
    // controller wants afterwards. full_sync is true for a clearing update and
    // false for a differential one.
    virtual void _refreshFrame(bool full_sync) = 0;
    // Deep-sleep command. UltraChip's is 0x07 + 0xA5 across the family, but the
    // subclass owns it so a part that disagrees can say so.
    virtual void _deepSleep();

    void _recordPlane(const uint8_t *bitmap, uint8_t fill, bool invert, bool mirror_y, bool pgm);
    // Write the recorded plane into RAM command `command`. Rows go out last
    // first: the gates scan bottom to top on all of these panels. Gate padding
    // is always white, whatever `invert` says — it is off-panel and only has to
    // be a defined level.
    void _sendPlane(uint8_t command, bool invert);
    // Fill the whole addressed gate count with one byte, for seeding a plane.
    void _fillPlane(uint8_t command, uint8_t value);
    // Send a 5 x 43 waveform bank whose first byte per row is the LUT register.
    void _writeBank(const uint8_t (*bank)[43]);
    void _InitDisplay();
    void _PowerOn();
    void _PowerOff();

    const uint16_t _w, _h, _wb;
    const uint16_t _gate_offset, _gate_total;

    const uint8_t *_pending;
    uint8_t _pending_fill; // used when _pending is null (constant plane)
    bool _pending_pgm;
    bool _pending_invert;
    bool _pending_mirror;
    // 0x10 holds a frame the controller actually displayed, so a differential
    // update has something real to diff against.
    bool _old_ram_valid;
    bool _panel_configured;
};

#endif // _GxEPD2_XTEINK_UC_H_
