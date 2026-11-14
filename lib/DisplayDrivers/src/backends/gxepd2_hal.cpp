// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This backend subclasses GxEPD2_BW from GxEPD2 (Jean-Marc Zingg), which is
// GPL-3.0. Both this file pair and any firmware built with -DUSE_GXEPD2 are
// derivative works of it. The rest of DisplayDrivers stays MIT.

#include "../DisplayDrivers.h"

#if defined(USE_GXEPD2)

// The panel is built in begin(), not here: everything that describes it lives
// in displayConfig, and the point of that struct is that a board gets to change
// it — after the display object exists, before it is brought up.
tft_display::tft_display(int16_t _W, int16_t _H) : _height(_H), _width(_W) {}

tft_display::~tft_display() {
    delete _gfx;
    _gfx = nullptr;
}

void tft_display::begin(uint32_t speed) {
    (void)speed;
    if (!_gfx) {
        const int16_t cs = displayConfig.cs;
        const int16_t dc = displayConfig.dc;
        const int16_t rst = displayConfig.rst;
        const int16_t busy = displayConfig.busy;
        // clang-format off
        switch (displayConfig.driver) {
        case 0:
            _gfx = new GXEPD2_PANEL_OF(GXEPD2_PANEL)(cs, dc, rst, busy);
            break;
#if defined(GXEPD2_PANEL_ALT1)
        case 1:
            _gfx = new GXEPD2_PANEL_OF(GXEPD2_PANEL_ALT1)(cs, dc, rst, busy);
            break;
#endif
#if defined(GXEPD2_PANEL_ALT2)
        case 2:
            _gfx = new GXEPD2_PANEL_OF(GXEPD2_PANEL_ALT2)(cs, dc, rst, busy);
            break;
#endif
#if defined(GXEPD2_PANEL_ALT3)
        case 3:
            _gfx = new GXEPD2_PANEL_OF(GXEPD2_PANEL_ALT3)(cs, dc, rst, busy);
            break;
#endif
#if defined(GXEPD2_PANEL_ALT4)
        case 4:
            _gfx = new GXEPD2_PANEL_OF(GXEPD2_PANEL_ALT4)(cs, dc, rst, busy);
            break;
#endif
        default:
            // Asked for a panel this binary was not built with. Leave the
            // display unbuilt rather than drive the glass with the wrong
            // controller; every call below is null-guarded.
            return;
        }
        // clang-format on
    }
    if (!_gfx) return;
    if (displayConfig.beginSpi) {
        SPI.begin(displayConfig.sclk, displayConfig.miso, displayConfig.mosi, displayConfig.cs);
    }
    _gfx->init(
        GXEPD2_DIAG_BAUD, GXEPD2_INITIAL, displayConfig.resetDuration, displayConfig.pulldownRst
    );
    _gfx->setFullWindow();
    _needsFullRefresh = true;
    _width = _gfx->width();
    _height = _gfx->height();
    setTextSize(1);
    setTextColor(GxEPD_BLACK, GxEPD_WHITE);
}

void tft_display::init(uint8_t tc) {
    (void)tc;
    begin();
}

void tft_display::setRotation(uint8_t r) {
    if (_gfx) _gfx->setRotation(r);
    _rotation = r;
    if (_gfx) {
        _width = _gfx->width();
        _height = _gfx->height();
    }
}

void tft_display::display(bool fullRefresh) {
    if (!_gfx) return;
    bool useFullRefresh = fullRefresh || _needsFullRefresh;
#if GXEPD2_FULL_REFRESH_INTERVAL
    if (!useFullRefresh && ++_partialCount >= GXEPD2_FULL_REFRESH_INTERVAL) { useFullRefresh = true; }
#endif
    if (useFullRefresh) _partialCount = 0;
    _gfx->display(!useFullRefresh);
    _needsFullRefresh = false;
}

void tft_display::hibernate() {
    if (_gfx) _gfx->hibernate();
}

void tft_display::drawPixel(int32_t x, int32_t y, uint32_t color) {
    if (_gfx) _gfx->drawPixel(x, y, color);
}

void tft_display::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
    if (_gfx) _gfx->drawLine(x0, y0, x1, y1, color);
}

void tft_display::drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {
    if (_gfx) _gfx->drawFastHLine(x, y, w, color);
}

void tft_display::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {
    if (_gfx) _gfx->drawFastVLine(x, y, h, color);
}

void tft_display::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (_gfx) _gfx->drawRect(x, y, w, h, color);
}

void tft_display::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (_gfx) _gfx->fillRect(x, y, w, h, color);
}

void tft_display::fillRectHGradient(
    int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2
) {
    (void)color2;
    fillRect(x, y, w, h, color1);
}

void tft_display::fillRectVGradient(
    int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2
) {
    (void)color2;
    fillRect(x, y, w, h, color1);
}

void tft_display::fillScreen(uint32_t color) {
    if (!_gfx) return;
    // Repainting everything means the next flush has to be a full refresh, or
    // the ghost of the previous screen stays on the panel.
    _needsFullRefresh = true;
    if (_gfx) _gfx->fillScreen(color);
}

void tft_display::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
    if (_gfx) _gfx->drawRoundRect(x, y, w, h, r, color);
}

void tft_display::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
    if (_gfx) _gfx->fillRoundRect(x, y, w, h, r, color);
}

void tft_display::drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    if (_gfx) _gfx->drawCircle(x0, y0, r, color);
}

void tft_display::fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    if (_gfx) _gfx->fillCircle(x0, y0, r, color);
}

void tft_display::drawTriangle(
    int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color
) {
    if (_gfx) _gfx->drawTriangle(x0, y0, x1, y1, x2, y2, color);
}

void tft_display::fillTriangle(
    int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color
) {
    if (_gfx) _gfx->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

void tft_display::drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
    // Adafruit_GFX has no ellipse primitive: plot the four symmetric quadrants.
    if (!_gfx || rx < 0 || ry < 0) return;
    if (rx == 0 || ry == 0) {
        _gfx->drawLine(x0 - rx, y0 - ry, x0 + rx, y0 + ry, color);
        return;
    }
    const float frx = static_cast<float>(rx);
    const float fry = static_cast<float>(ry);
    for (int32_t x = -rx; x <= rx; ++x) {
        const float t = static_cast<float>(x) / frx;
        const int32_t y = static_cast<int32_t>(std::lround(fry * std::sqrt(std::max(0.0f, 1.0f - t * t))));
        _gfx->drawPixel(x0 + x, y0 + y, color);
        _gfx->drawPixel(x0 + x, y0 - y, color);
    }
    for (int32_t y = -ry; y <= ry; ++y) {
        const float t = static_cast<float>(y) / fry;
        const int32_t x = static_cast<int32_t>(std::lround(frx * std::sqrt(std::max(0.0f, 1.0f - t * t))));
        _gfx->drawPixel(x0 + x, y0 + y, color);
        _gfx->drawPixel(x0 - x, y0 + y, color);
    }
}

void tft_display::fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
    if (!_gfx || rx < 0 || ry < 0) return;
    const float frx = static_cast<float>(rx);
    const float fry = static_cast<float>(ry);
    if (fry == 0.0f) return;
    for (int32_t y = -ry; y <= ry; ++y) {
        const float t = static_cast<float>(y) / fry;
        const int32_t x = static_cast<int32_t>(std::lround(frx * std::sqrt(std::max(0.0f, 1.0f - t * t))));
        _gfx->drawFastHLine(x0 - x, y0 + y, x * 2 + 1, color);
    }
}

void tft_display::drawArc(
    int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color,
    uint32_t bg_color, bool smoothArc
) {
    (void)bg_color;
    (void)smoothArc;
    if (!_gfx || r <= 0) return;
    // Same angle convention the other backends use: the caller's angles are
    // shifted by +90 before being measured from 3 o'clock, clockwise.
    const float start = static_cast<float>(startAngle + 90);
    float sweep = static_cast<float>(endAngle) - static_cast<float>(startAngle);
    if (sweep <= 0.0f) sweep += 360.0f;
    if (sweep > 360.0f) sweep = 360.0f;

    const int32_t outer2 = r * r;
    const int32_t inner2 = ir > 0 ? ir * ir : 0;
    for (int32_t dy = -r; dy <= r; ++dy) {
        for (int32_t dx = -r; dx <= r; ++dx) {
            const int32_t d2 = dx * dx + dy * dy;
            if (d2 > outer2 || d2 < inner2) continue;
            float a = std::atan2(static_cast<float>(dy), static_cast<float>(dx)) * 180.0f / 3.14159265f;
            float rel = a - start;
            while (rel < 0.0f) rel += 360.0f;
            while (rel >= 360.0f) rel -= 360.0f;
            if (rel <= sweep) _gfx->drawPixel(x + dx, y + dy, fg_color);
        }
    }
}

void tft_display::drawWideLine(
    float ax, float ay, float bx, float by, float wd, uint32_t fg_color, uint32_t bg_color
) {
    (void)bg_color;
    drawWideLineFallback(ax, ay, bx, by, wd, fg_color);
}

void tft_display::drawWideLineFallback(float ax, float ay, float bx, float by, float wd, uint32_t color) {
    if (!_gfx) return;
    float dx = bx - ax;
    float dy = by - ay;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len == 0) {
        _gfx->drawPixel(static_cast<int32_t>(ax), static_cast<int32_t>(ay), color);
        return;
    }
    float nx = -dy / len;
    float ny = dx / len;
    int32_t half = static_cast<int32_t>(std::ceil(wd / 2.0f));
    for (int32_t i = -half; i <= half; ++i) {
        float ox = nx * static_cast<float>(i);
        float oy = ny * static_cast<float>(i);
        _gfx->drawLine(
            static_cast<int32_t>(ax + ox),
            static_cast<int32_t>(ay + oy),
            static_cast<int32_t>(bx + ox),
            static_cast<int32_t>(by + oy),
            color
        );
    }
}

void tft_display::drawXBitmap(
    int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color
) {
    if (!_gfx || !bitmap) return;
    _gfx->drawXBitmap(x, y, bitmap, w, h, color);
}

void tft_display::drawXBitmap(
    int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg
) {
    if (!_gfx || !bitmap) return;
    for (int16_t j = 0; j < h; ++j) {
        for (int16_t i = 0; i < w; ++i) {
            uint8_t byte = bitmap[(j * ((w + 7) / 8)) + (i >> 3)];
            bool bit = byte & (0x80 >> (i & 7));
            _gfx->drawPixel(x + i, y + j, bit ? color : bg);
        }
    }
}

void tft_display::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data) {
    pushImageFallback(x, y, w, h, data);
}

void tft_display::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data) {
    pushImageFallback(x, y, w, h, data);
}

void tft_display::pushImage(
    int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, bool bpp8, uint16_t *cmap
) {
    if (!_gfx || !data || !bpp8 || !cmap) return;
    for (int32_t row = 0; row < h; ++row) {
        for (int32_t col = 0; col < w; ++col) {
            uint8_t idx = data[row * w + col];
            _gfx->drawPixel(x + col, y + row, cmap[idx]);
        }
    }
}

void tft_display::pushImage(
    int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data, bool bpp8, uint16_t *cmap
) {
    pushImage(x, y, w, h, const_cast<uint8_t *>(data), bpp8, cmap);
}

void tft_display::invertDisplay(bool i) { (void)i; }

void tft_display::sleep(bool value) {
    if (_gfx && value) _gfx->hibernate();
}

void tft_display::setSwapBytes(bool swap) { _swapBytes = swap; }

bool tft_display::getSwapBytes() const { return _swapBytes; }

uint16_t tft_display::color565(uint8_t r, uint8_t g, uint8_t b) const {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

int16_t tft_display::textWidth(const String &s, uint8_t font) const { return textWidth(s.c_str(), font); }

int16_t tft_display::textWidth(const char *s, uint8_t font) const {
    (void)font;
    if (!_gfx || !s) return 0;
    int16_t x1, y1;
    uint16_t w, h;
    _gfx->getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
    return static_cast<int16_t>(w);
}

void tft_display::setCursor(int16_t x, int16_t y) {
    if (_gfx) _gfx->setCursor(x, y);
}

int16_t tft_display::getCursorX() const { return _gfx ? _gfx->getCursorX() : 0; }

int16_t tft_display::getCursorY() const { return _gfx ? _gfx->getCursorY() : 0; }

void tft_display::setTextSize(uint8_t s) {
    _textSize = s ? s : 1;
    if (_gfx) _gfx->setTextSize(_textSize);
}

void tft_display::setTextColor(uint16_t c) {
    _textColor = c;
    if (_gfx) _gfx->setTextColor(gxepd2_dither::snapColor(c));
}

void tft_display::setTextColor(uint16_t c, uint16_t b, bool bgfill) {
    (void)bgfill;
    _textColor = c;
    _textBgColor = b;
    // Text is rendered glyph by glyph, so a per-pixel dither would smear the
    // characters; pick the nearest of the two panel levels instead.
    if (_gfx) _gfx->setTextColor(gxepd2_dither::snapColor(c), gxepd2_dither::snapColor(b));
}

void tft_display::setTextDatum(uint8_t d) { _textDatum = d; }

uint8_t tft_display::getTextDatum() const { return _textDatum; }

void tft_display::setTextFont(uint8_t f) { _textFont = f; }

void tft_display::setTextWrap(bool wrapX, bool wrapY) {
    (void)wrapY;
    if (_gfx) _gfx->setTextWrap(wrapX);
}

int16_t tft_display::drawString(const String &string, int32_t x, int32_t y, uint8_t font) {
    (void)font;
    return drawAlignedString(string, x, y, _textDatum);
}

int16_t tft_display::drawCentreString(const String &string, int32_t x, int32_t y, uint8_t font) {
    (void)font;
    return drawAlignedString(string, x, y, TC_DATUM);
}

int16_t tft_display::drawRightString(const String &string, int32_t x, int32_t y, uint8_t font) {
    (void)font;
    return drawAlignedString(string, x, y, TR_DATUM);
}

int16_t tft_display::drawAlignedString(const String &s, int32_t x, int32_t y, uint8_t datum) {
    if (!_gfx) return 0;
    int16_t x1, y1;
    uint16_t w = 0, h = 0;
    _gfx->getTextBounds(s.c_str(), 0, 0, &x1, &y1, &w, &h);
    int32_t cx = x;
    int32_t cy = y;
    switch (datum) {
        case TC_DATUM: cx -= w / 2; break;
        case TR_DATUM: cx -= w; break;
        case MC_DATUM:
            cx -= w / 2;
            cy -= h / 2;
            break;
        case MR_DATUM:
            cx -= w;
            cy -= h / 2;
            break;
        case BC_DATUM:
            cx -= w / 2;
            cy -= h;
            break;
        case BR_DATUM:
            cx -= w;
            cy -= h;
            break;
        case BL_DATUM: cy -= h; break;
        default: break;
    }
    // Print at the requested spot, then hand the cursor back untouched — the
    // launcher interleaves drawString() with print() and relies on this.
    const int16_t oldX = _gfx->getCursorX();
    const int16_t oldY = _gfx->getCursorY();
    _gfx->setCursor(cx, cy);
    _gfx->print(s);
    _gfx->setCursor(oldX, oldY);
    return static_cast<int16_t>(w);
}

size_t tft_display::write(uint8_t c) { return _gfx ? _gfx->write(c) : 0; }

size_t tft_display::write(const uint8_t *buffer, size_t size) {
    if (!_gfx || !buffer) return 0;
    size_t written = 0;
    for (size_t i = 0; i < size; ++i) { written += _gfx->write(buffer[i]); }
    return written;
}

size_t tft_display::println() { return _gfx ? _gfx->println() : 0; }

size_t tft_display::printf(const char *fmt, ...) {
    if (!_gfx || !fmt) return 0;
    va_list args;
    va_start(args, fmt);
    char stackBuf[128];
    int len = vsnprintf(stackBuf, sizeof(stackBuf), fmt, args);
    va_end(args);
    if (len < 0) return 0;
    if (static_cast<size_t>(len) < sizeof(stackBuf)) return _gfx->print(stackBuf);
    std::unique_ptr<char[]> buf(new char[len + 1]);
    va_start(args, fmt);
    vsnprintf(buf.get(), len + 1, fmt, args);
    va_end(args);
    return _gfx->print(buf.get());
}

int16_t tft_display::width() const { return _gfx ? _gfx->width() : 0; }

int16_t tft_display::height() const { return _gfx ? _gfx->height() : 0; }

SPIClass &tft_display::getSPIinstance() const { return SPI; }

void tft_display::writecommand(uint8_t c) { (void)c; }

uint32_t tft_display::getTextColor() const { return _textColor; }

uint32_t tft_display::getTextBgColor() const { return _textBgColor; }

uint8_t tft_display::getTextSize() const { return _textSize; }

uint8_t tft_display::getRotation() const { return _rotation; }

int16_t tft_display::fontHeight(int16_t font) const {
    (void)font;
    return static_cast<int16_t>(_textSize * 8);
}

GxEPD2_GFX *tft_display::native() { return _gfx; }

#endif
