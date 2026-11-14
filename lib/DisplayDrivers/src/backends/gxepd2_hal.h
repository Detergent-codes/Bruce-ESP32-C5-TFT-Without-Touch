// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This backend subclasses GxEPD2_BW from GxEPD2 (Jean-Marc Zingg), which is
// GPL-3.0. Both this file pair and any firmware built with -DUSE_GXEPD2 are
// derivative works of it. The rest of DisplayDrivers stays MIT.

#ifndef LIB_HAL_GXEPD2_HAL_H
#define LIB_HAL_GXEPD2_HAL_H
#include <pins_arduino.h>

#if defined(USE_GXEPD2)

// GxEPD2_BW is a template over its panel class, so a binary that can drive more
// than one panel needs a common base to hold them in. GxEPD2 has one --
// GxEPD2_GFX -- but only builds it when asked. Turning it on costs 2.7 kB of
// flash once (vtable and thunks for its ~35 virtuals), no RAM, and nothing per
// panel; the alternative, a leaner base of our own, would have to redeclare the
// same surface to save a fraction of that.
//
// Safe against ODR trouble: everything the macro changes lives in the GxEPD2_BW
// template, which is header-only and only ever instantiated here. GxEPD2's one
// compiled unit, GxEPD2_EPD.cpp, does not read it.
#define ENABLE_GxEPD2_GFX 1

#include <GxEPD2_BW.h>
#include <SPI.h>

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <memory>

#include "swsprite.h"
#include "tft_defines.h"

// Panels this library carries itself, for glass GxEPD2 has no class for. They
// are ordinary GxEPD2 panel classes, so GXEPD2_PANEL names them exactly like a
// stock one.
#include "../panels/GxEPD2_X3_792x528.h"
#include "../panels/GxEPD2_X3_792x528_UC8279.h"
#include "../panels/GxEPD2_426_SEEED_GDEQ0426T82.h"
#include "../panels/GxEPD2_426_XTEINK_GDEQ0426T82.h"
#include "../panels/GxEPD2_X4_800x480_UC8179.h"
#include "../panels/GxEPD2_X4_800x480_UC8279.h"

// clang-format off
// ---------------------------------------------------------------------------
// Panel selection
// ---------------------------------------------------------------------------
#if !defined(GXEPD2_PANEL)
    #error "Please define GXEPD2_PANEL with the GxEPD2 panel class of this board,\n \
    e.g. -DGXEPD2_PANEL=GxEPD2_310_GDEQ031T10 . The list of stock ones lives in\n \
    the epd/ folder of the GxEPD2 library, and this library adds a few of its own\n \
    under src/panels/; only black/white panels are wired up here."
#endif

// Panels the board can also come with. The Xteink X4 shipped as SSD1677, then
// UC8179, then UC8279 behind the same case and the same pinout, so one binary
// that picks at boot beats three builds. Board code writes the index into
// displayConfig.driver before begin(): 0 is GXEPD2_PANEL, 1 is _ALT1, and so on.
// Each one declared links its panel class in; declaring none costs nothing.
#if !defined(GXEPD2_PANEL_ALT1) && defined(GXEPD2_PANEL_ALT2)
    #error "GXEPD2_PANEL_ALT2 without GXEPD2_PANEL_ALT1; the alternates are numbered in order"
#endif
#if !defined(GXEPD2_PANEL_ALT2) && defined(GXEPD2_PANEL_ALT3)
    #error "GXEPD2_PANEL_ALT3 without GXEPD2_PANEL_ALT2; the alternates are numbered in order"
#endif
#if !defined(GXEPD2_PANEL_ALT3) && defined(GXEPD2_PANEL_ALT4)
    #error "GXEPD2_PANEL_ALT4 without GXEPD2_PANEL_ALT3; the alternates are numbered in order"
#endif

// Rows kept in RAM. The default holds the whole panel, which is what every
// board in this project can afford; drop it to page the panel in bands.
//
// A board that states it pins every panel to that height; left alone, each
// panel gets its own full height, which is what the alternates want -- they are
// usually the same glass behind a different controller, but nothing guarantees
// their HEIGHT constants agree.
#if defined(GXEPD2_PAGE_HEIGHT)
    #define GXEPD2_PANEL_OF(cls) gxepd2_panel<cls, GXEPD2_PAGE_HEIGHT>
#else
    #define GXEPD2_PANEL_OF(cls) gxepd2_panel<cls>
    // Only so DisplayConfig.cpp can record what the primary came up with.
    #define GXEPD2_PAGE_HEIGHT GXEPD2_PANEL::HEIGHT
#endif

// ---------------------------------------------------------------------------
// Pins — taken from the usual TFT_* names unless overridden
// ---------------------------------------------------------------------------
#if !defined(GXEPD2_CS)
    #if !defined(TFT_CS)
        #error "Missing definition for TFT_CS (or GXEPD2_CS)"
    #endif
    #define GXEPD2_CS TFT_CS
#endif

#if !defined(GXEPD2_DC)
    #if !defined(TFT_DC)
        #error "Missing definition for TFT_DC (or GXEPD2_DC)"
    #endif
    #define GXEPD2_DC TFT_DC
#endif

#if !defined(GXEPD2_BUSY)
    #if defined(TFT_BUSY)
        #define GXEPD2_BUSY TFT_BUSY
    #elif defined(TFT_BUSY_PIN)
        #define GXEPD2_BUSY TFT_BUSY_PIN
    #else
        #error "Missing definition for TFT_BUSY (or GXEPD2_BUSY)"
    #endif
#endif

#if !defined(GXEPD2_RST)
    #if defined(TFT_RST)
        #define GXEPD2_RST TFT_RST
    #else
        #define GXEPD2_RST -1
    #endif
#endif

// ---------------------------------------------------------------------------
// init() parameters (see GxEPD2_EPD::init)
// ---------------------------------------------------------------------------
#if !defined(GXEPD2_DIAG_BAUD)
    #define GXEPD2_DIAG_BAUD 115200
#endif
#if !defined(GXEPD2_INITIAL)
    #define GXEPD2_INITIAL true
#endif
// Reset pulse in ms. 2 is the value Waveshare boards with a "clever" reset
// circuit need; 10 is the GxEPD2 default.
#if !defined(GXEPD2_RESET_DURATION)
    #define GXEPD2_RESET_DURATION 2
#endif
#if !defined(GXEPD2_PULLDOWN_RST)
    #define GXEPD2_PULLDOWN_RST false
#endif

// Define GXEPD2_BEGIN_SPI (and TFT_SCLK / TFT_MOSI / TFT_MISO) to have the HAL
// bring the SPI bus up itself. Leave it undefined when the bus is shared with
// the SD card and started elsewhere.
#if defined(GXEPD2_BEGIN_SPI)
    #if !defined(TFT_SCLK) || !defined(TFT_MOSI)
        #error "GXEPD2_BEGIN_SPI needs TFT_SCLK and TFT_MOSI"
    #endif
    #if !defined(TFT_MISO)
        #define TFT_MISO -1
    #endif
#endif

// Ordered dithering of the RGB565 colours the launcher draws with down to the
// panel's two levels. Set to 0 to threshold at 50% grey instead.
#if !defined(GXEPD2_DITHER)
    #define GXEPD2_DITHER 1
#endif

// Partial (fast waveform) updates never fully settle every pixel, so unchanged
// areas drift darker with each one. Force a full (flashing) refresh every this
// many partial ones to clear that drift. Set to 0 to disable.
#if !defined(GXEPD2_FULL_REFRESH_INTERVAL)
    #define GXEPD2_FULL_REFRESH_INTERVAL 10
#endif
// clang-format on

// Reducing the launcher's RGB565 palette to the panel's two levels. Kept off
// the panel class: callers dither text colours whether or not a panel exists,
// and there is nothing per-panel about the arithmetic.
struct gxepd2_dither {
    static uint8_t rgb565ToLuma(uint16_t color) {
        const uint16_t r = ((color >> 11) & 0x1F) * 255U / 31U;
        const uint16_t g = ((color >> 5) & 0x3F) * 255U / 63U;
        const uint16_t b = (color & 0x1F) * 255U / 31U;
        return static_cast<uint8_t>((r * 299U + g * 587U + b * 114U) / 1000U);
    }

    static uint16_t ditherColor(int16_t x, int16_t y, uint16_t color) {
        if (color == GxEPD_BLACK || color == GxEPD_WHITE) return color;

        const uint8_t luma = rgb565ToLuma(color);
#if GXEPD2_DITHER
        static const uint8_t bayer4x4[4][4] = {
            {0,  8,  2,  10},
            {12, 4,  14, 6 },
            {3,  11, 1,  9 },
            {15, 7,  13, 5 },
        };
        const uint8_t threshold = static_cast<uint8_t>(bayer4x4[y & 0x3][x & 0x3] * 16U);
#else
        const uint8_t threshold = 128;
#endif
        return (luma > threshold) ? GxEPD_WHITE : GxEPD_BLACK;
    }

    // Text is rendered glyph by glyph, so a per-pixel dither would smear the
    // characters; snap to the nearer of the two panel levels against a fixed
    // mid-grey threshold instead.
    static uint16_t snapColor(uint16_t color) {
        if (color == GxEPD_BLACK || color == GxEPD_WHITE) return color;
        return (rgb565ToLuma(color) > 128U) ? GxEPD_WHITE : GxEPD_BLACK;
    }
};

// One GxEPD2 panel class, with every drawing primitive routed through a
// dithering drawPixel so the launcher's RGB565 palette survives on 1bpp glass.
// A template, because the panel is chosen at boot: the backend holds whichever
// instantiation the board asked for through their shared GxEPD2_GFX base.
template <typename PANEL, uint16_t PAGE_H = PANEL::HEIGHT>
class gxepd2_panel : public GxEPD2_BW<PANEL, PAGE_H> {
    typedef GxEPD2_BW<PANEL, PAGE_H> Base;

public:
    // Pins come from displayConfig, not from the macros, so a board can probe
    // its hardware and retarget the panel before begin() builds this.
    gxepd2_panel(int16_t cs, int16_t dc, int16_t rst, int16_t busy) : Base(PANEL(cs, dc, rst, busy)) {}

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        Base::drawPixel(x, y, gxepd2_dither::ditherColor(x, y, color));
    }

    // Base::fillScreen memsets the buffer and treats anything non-zero as
    // white, which would flatten every mid tone; dither those by hand.
    void fillScreen(uint16_t color) override {
        if (color == GxEPD_BLACK || color == GxEPD_WHITE) {
            Base::fillScreen(color);
            return;
        }
        for (int16_t py = 0; py < this->height(); ++py) {
            for (int16_t px = 0; px < this->width(); ++px) { drawPixel(px, py, color); }
        }
    }
};

class tft_display {
public:
    explicit tft_display(int16_t _W = TFT_WIDTH, int16_t _H = TFT_HEIGHT);
    ~tft_display();
    friend class tft_sprite;
    friend class tft_logger;

    void begin(uint32_t speed = 0);
    void init(uint8_t tc = 0);
    void setRotation(uint8_t r);
    void drawPixel(int32_t x, int32_t y, uint32_t color);
    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
    void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
    void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
    void fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
    void fillScreen(uint32_t color);
    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
    void drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
    void fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
    void drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
    void drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);
    void fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);
    void drawArc(
        int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle,
        uint32_t fg_color, uint32_t bg_color, bool smoothArc = true
    );
    void drawWideLine(
        float ax, float ay, float bx, float by, float wd, uint32_t fg_color, uint32_t bg_color = 0x00FFFFFF
    );
    void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
    void drawXBitmap(
        int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg
    );
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data);
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data);
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, bool bpp8, uint16_t *cmap);
    void
    pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data, bool bpp8, uint16_t *cmap);
    void invertDisplay(bool i);
    void sleep(bool value);
    void setSwapBytes(bool swap);
    bool getSwapBytes() const;
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b) const;

    int16_t textWidth(const String &s, uint8_t font = 1) const;
    int16_t textWidth(const char *s, uint8_t font = 1) const;

    void setCursor(int16_t x, int16_t y);
    int16_t getCursorX() const;
    int16_t getCursorY() const;
    void setTextSize(uint8_t s);
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t b, bool bgfill = false);
    void setTextDatum(uint8_t d);
    uint8_t getTextDatum() const;
    void setTextFont(uint8_t f);
    void setTextWrap(bool wrapX, bool wrapY = false);
    int16_t drawString(const String &string, int32_t x, int32_t y, uint8_t font = 1);
    int16_t drawCentreString(const String &string, int32_t x, int32_t y, uint8_t font = 1);
    int16_t drawRightString(const String &string, int32_t x, int32_t y, uint8_t font = 1);

    size_t write(uint8_t c);
    size_t write(const uint8_t *buffer, size_t size);
    // C strings get their own overloads: a plain char buffer (a VLA in
    // particular) cannot bind to the template's const T& parameter.
    size_t print(const char *val) { return val ? (_gfx ? _gfx->print(val) : 0) : 0; }
    size_t println(const char *val) { return val ? (_gfx ? _gfx->println(val) : 0) : 0; }
    template <typename T> size_t print(const T &val) { return _gfx ? _gfx->print(val) : 0; }
    template <typename T> size_t println(const T &val) { return _gfx ? _gfx->println(val) : 0; }
    size_t println();

    size_t printf(const char *fmt, ...);

    int16_t width() const;
    int16_t height() const;
    SPIClass &getSPIinstance() const;
    void writecommand(uint8_t c);

    uint32_t getTextColor() const;
    uint32_t getTextBgColor() const;
    uint8_t getTextSize() const;
    uint8_t getRotation() const;
    int16_t fontHeight(int16_t font = 1) const;
    // The panel, through the base every gxepd2_panel<> shares. Cast it to
    // GXEPD2_PANEL_OF(GXEPD2_PANEL) (or the _ALTn the board selected) to reach
    // anything specific to one panel class.
    GxEPD2_GFX *native();

    // --- e-paper specific -------------------------------------------------
    // Flush the buffer to the panel. A full (flashing) refresh is used when
    // asked for, and also after anything that repainted the whole screen;
    // otherwise the panel does a fast partial update.
    void display(bool fullRefresh = false);

    // Put the panel controller to sleep; the next draw wakes it up again.
    void hibernate();

private:
    template <typename Ptr> void pushImageFallback(int32_t x, int32_t y, int32_t w, int32_t h, Ptr data) {
        if (!_gfx || !data) return;
        for (int32_t row = 0; row < h; ++row) {
            for (int32_t col = 0; col < w; ++col) {
                uint16_t color = data[row * w + col];
                if (_swapBytes) color = static_cast<uint16_t>((color >> 8) | (color << 8));
                _gfx->drawPixel(x + col, y + row, color);
            }
        }
    }

    void drawWideLineFallback(float ax, float ay, float bx, float by, float wd, uint32_t color);

    int16_t drawAlignedString(const String &s, int32_t x, int32_t y, uint8_t datum);

    GxEPD2_GFX *_gfx = nullptr;
    bool _needsFullRefresh = true;
    uint8_t _partialCount = 0;
    uint16_t _height = TFT_HEIGHT;
    uint16_t _width = TFT_WIDTH;
    bool _swapBytes = false;
    uint32_t _textColor = TFT_WHITE;
    uint32_t _textBgColor = TFT_BLACK;
    uint8_t _textSize = 1;
    uint8_t _textDatum = 0;
    uint8_t _textFont = 1;
    uint8_t _rotation = 0;
};

#endif
#endif // LIB_HAL_GXEPD2_HAL_H
