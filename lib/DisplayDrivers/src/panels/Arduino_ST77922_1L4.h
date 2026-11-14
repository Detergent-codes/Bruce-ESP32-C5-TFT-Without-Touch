// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

// Arduino_GFX driver for the ST77922 over QSPI, id 52
// (TFT_DATABUS_N=6 / TFT_DISPLAY_DRIVER_N=52).
//
// Arduino_GFX's own Arduino_ESP32QSPI sends every command on all 4 QSPI lines
// ("4-4-4": see its writeCommand(), which sets SPI_TRANS_MULTILINE_CMD |
// SPI_TRANS_MULTILINE_ADDR unconditionally). This controller wants register
// writes on a single line and only the pixel data on four, so that bus cannot
// drive it. Like Arduino_RGB_Display/Arduino_DSI_Display (ids 49/50, see
// ardgfx.cpp), this class therefore owns its bus instead of taking an
// Arduino_DataBus.
//
// The transfer code below follows the board vendor's own Arduino example for
// this panel (LCDwiki ES3C35P, github.com/ydedox/st77922, Example_01) closely,
// because that is the known-good reference:
//   - register write : opcode 0x02, 24-bit address = (register << 8), params on
//                      one line;
//   - pixel write    : opcode 0x32, address 0x003C00 ("write memory continue"),
//                      data on four lines, after the window is set with
//                      0x2A / 0x2B;
//   - no software reset, no RAMWR (0x2C) per write;
//   - the window's X start and width are multiples of 4.
//
// The panel itself does not rotate (its MADCTL does not remap addressing), so
// rotation is done in software: Arduino_Canvas hands this driver native
// portrait frames, and the per-pixel/per-rect paths transform coordinates.
#ifndef ARDUINO_ST77922_1L4_H
#define ARDUINO_ST77922_1L4_H

#include <Arduino_GFX_Library.h>

#include <driver/spi_master.h>

#include "st77922_1l4/st77922_1l4_init_table.h"

class Arduino_ST77922_1L4 : public Arduino_GFX {
public:
    Arduino_ST77922_1L4(
        int16_t w, int16_t h, int8_t cs, int8_t sclk, int8_t d0, int8_t d1, int8_t d2, int8_t d3, int8_t rst,
        uint8_t rotation
    )
        : Arduino_GFX(w, h), _cs(cs), _sclk(sclk), _d0(d0), _d1(d1), _d2(d2), _d3(d3) {
        (void)rst; // the panel's reset line is tied to the chip's own reset on this board
        setRotation(rotation);
    }

    // Pixels per SPI transaction; the vendor's own value.
    static const int32_t CHUNK_PIXELS = 0x4000;

    bool begin(int32_t speed = 80000000) override {
        // Callers reach this through an Arduino_GFX*, so the default above is
        // never applied: they pass the base class's "not defined" (-1).
        if (speed <= 0) speed = 80000000;

        // Internal, DMA-capable RAM for pixel data and for register parameters.
        _buf = (uint16_t *)heap_caps_malloc(CHUNK_PIXELS * 2, MALLOC_CAP_DMA);
        _cmd = (uint8_t *)heap_caps_malloc(64, MALLOC_CAP_DMA);
        if (!_buf || !_cmd) return false;

        spi_bus_config_t buscfg = {};
        buscfg.data0_io_num = _d0;
        buscfg.data1_io_num = _d1;
        buscfg.sclk_io_num = _sclk;
        buscfg.data2_io_num = _d2;
        buscfg.data3_io_num = _d3;
        buscfg.max_transfer_sz = CHUNK_PIXELS * 2 + 8;
        buscfg.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_IOMUX_PINS | SPICOMMON_BUSFLAG_QUAD;
        if (spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO) != ESP_OK) return false;

        spi_device_interface_config_t devcfg = {};
        devcfg.command_bits = 0;
        devcfg.address_bits = 0;
        devcfg.mode = 0;
        devcfg.clock_speed_hz = speed;
        devcfg.spics_io_num = _cs;
        devcfg.flags = SPI_DEVICE_HALFDUPLEX;
        devcfg.queue_size = 17;
        if (spi_bus_add_device(SPI2_HOST, &devcfg, &_dev) != ESP_OK) return false;

        const size_t n = sizeof(es3c35p_lcd_init_cmds) / sizeof(es3c35p_lcd_init_cmds[0]);
        for (size_t i = 0; i < n; i++) {
            writeReg((uint8_t)es3c35p_lcd_init_cmds[i].cmd, es3c35p_lcd_init_cmds[i].data,
                     es3c35p_lcd_init_cmds[i].data_bytes);
            if (es3c35p_lcd_init_cmds[i].delay_ms) delay(es3c35p_lcd_init_cmds[i].delay_ms);
        }

        return true;
    }

    void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override {
        writeFillRectPreclipped(x, y, 1, 1, color);
    }

    // Fills a rectangle given in rotated (logical) coordinates. The panel needs
    // X start and width in multiples of 4, so the rectangle is widened to the
    // next multiple (the vendor's own point-drawing does the same). Whole-frame
    // output goes through draw16bitRGBBitmap() instead.
    void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override {
        if (!_dev) return;
        int32_t nx, ny, nw, nh;
        toNative(x, y, w, h, nx, ny, nw, nh);
        fillNative(nx, ny, nx + nw, ny + nh, color);
    }

    // Whole-bitmap push for Arduino_Canvas::flush(). The canvas owns rotation
    // and hands over a native-portrait bitmap; anything this path can't take
    // (rotated driver, X or width not a multiple of 4) returns false so the
    // caller falls back to the generic per-pixel path.
    bool pushBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
        if (!_dev || (_rotation & 3) != 0) return false;
        if (x < 0 || y < 0 || w <= 0 || h <= 0 || x + w > WIDTH || y + h > HEIGHT) return false;
        if ((x & 3) || (w & 3)) return false;
        setWindow(x, y, x + w, y + h);
        size_t total = (size_t)w * h;
        const uint16_t *src = bitmap;
        while (total) {
            size_t n = total > CHUNK_PIXELS ? CHUNK_PIXELS : total;
            for (size_t i = 0; i < n; i++) _buf[i] = swap565(src[i]);
            writePixels(n);
            src += n;
            total -= n;
        }
        return true;
    }

    void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override {
        if (!pushBitmap(x, y, bitmap, w, h)) Arduino_GFX::draw16bitRGBBitmap(x, y, bitmap, w, h);
    }

    void draw16bitRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) override {
        if (!pushBitmap(x, y, bitmap, w, h)) Arduino_GFX::draw16bitRGBBitmap(x, y, bitmap, w, h);
    }

private:
    // Logical (rotated) rect -> native portrait rect. A solid rectangle rotated
    // by a multiple of 90 degrees is still a solid rectangle, so this closed
    // form covers single pixels and whole rectangles alike.
    void toNative(int16_t lx, int16_t ly, int16_t lw, int16_t lh, int32_t &nx, int32_t &ny, int32_t &nw, int32_t &nh) {
        switch (_rotation & 3) {
            default: // case 0
                nx = lx; ny = ly; nw = lw; nh = lh;
                break;
            case 1:
                nx = WIDTH - ly - lh; ny = lx; nw = lh; nh = lw;
                break;
            case 2:
                nx = WIDTH - lx - lw; ny = HEIGHT - ly - lh; nw = lw; nh = lh;
                break;
            case 3:
                nx = ly; ny = HEIGHT - lx - lw; nw = lh; nh = lw;
                break;
        }
    }

    // The controller is little-endian-hostile: 16bpp goes out MSB first.
    static inline uint16_t swap565(uint16_t c) { return (c >> 8) | (c << 8); }

    // One register write: opcode 0x02, address (reg << 8), params on one line.
    void writeReg(uint8_t reg, const void *data, size_t len) {
        spi_transaction_ext_t t = {};
        t.base.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
        t.base.cmd = 0x02;
        t.base.addr = (uint32_t)reg << 8;
        t.command_bits = 8;
        t.address_bits = 24;
        if (len && len <= 64) {
            memcpy(_cmd, data, len);
            t.base.tx_buffer = _cmd;
            t.base.length = 8 * len;
        }
        spi_device_polling_transmit(_dev, (spi_transaction_t *)&t);
    }

    // x1 / y1 are exclusive.
    void setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
        uint8_t cx[4] = {(uint8_t)(x0 >> 8), (uint8_t)x0, (uint8_t)((x1 - 1) >> 8), (uint8_t)(x1 - 1)};
        uint8_t cy[4] = {(uint8_t)(y0 >> 8), (uint8_t)y0, (uint8_t)((y1 - 1) >> 8), (uint8_t)(y1 - 1)};
        writeReg(0x2A, cx, 4);
        writeReg(0x2B, cy, 4);
    }

    // n pixels from _buf (already byte-swapped): opcode 0x32, address 0x003C00,
    // data on four lines.
    void writePixels(size_t n) {
        spi_transaction_ext_t t = {};
        t.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
        t.base.cmd = 0x32;
        t.base.addr = 0x3C << 8;
        t.command_bits = 8;
        t.address_bits = 24;
        t.base.tx_buffer = _buf;
        t.base.length = n * 16;
        spi_device_polling_transmit(_dev, (spi_transaction_t *)&t);
    }

    // Solid fill of a native-portrait rectangle (x1 / y1 exclusive), widened to
    // the panel's multiple-of-4 X alignment.
    void fillNative(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) {
        if (x0 < 0) x0 = 0;
        if (y0 < 0) y0 = 0;
        if (x1 > WIDTH) x1 = WIDTH;
        if (y1 > HEIGHT) y1 = HEIGHT;
        if (x1 <= x0 || y1 <= y0) return;
        x0 &= ~3;
        x1 = (x1 + 3) & ~3;
        if (x1 > WIDTH) x1 = WIDTH;
        setWindow(x0, y0, x1, y1);
        size_t total = (size_t)(x1 - x0) * (y1 - y0);
        size_t first = total > CHUNK_PIXELS ? CHUNK_PIXELS : total;
        uint16_t sc = swap565(color);
        for (size_t i = 0; i < first; i++) _buf[i] = sc;
        while (total) {
            size_t n = total > CHUNK_PIXELS ? CHUNK_PIXELS : total;
            writePixels(n);
            total -= n;
        }
    }

    int8_t _cs, _sclk, _d0, _d1, _d2, _d3;
    spi_device_handle_t _dev = nullptr;
    uint16_t *_buf = nullptr;
    uint8_t *_cmd = nullptr;
};

#endif // ARDUINO_ST77922_1L4_H
