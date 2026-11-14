// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

#include "../DisplayDrivers.h"

#if defined(USE_EPD_PAINTER) || defined(USE_GXEPD2)

#include <algorithm>
#include <cmath>

tft_sprite::tft_sprite(tft_display *parent) : _display(parent) {}

void *tft_sprite::createSprite(int16_t w, int16_t h, uint8_t frames) {
    (void)frames;
    if (w <= 0 || h <= 0) return nullptr;
    _width = w;
    _height = h;
    _buffer.assign(static_cast<size_t>(w) * static_cast<size_t>(h), 0);
    return _buffer.data();
}

void tft_sprite::deleteSprite() {
    _buffer.clear();
    _buffer.shrink_to_fit();
    _width = _height = 0;
}

void tft_sprite::fillScreen(uint32_t color) { fillRect(0, 0, _width, _height, color); }

void tft_sprite::setColorDepth(uint8_t depth) { _colorDepth = depth; }

void tft_sprite::setCursor(int16_t x, int16_t y) {
    _cursorX = x;
    _cursorY = y;
}

void tft_sprite::setTextColor(uint16_t c) { _textColor = c; }

void tft_sprite::setTextColor(uint16_t c, uint16_t b) {
    _textColor = c;
    _textBgColor = b;
}

void tft_sprite::setTextSize(uint8_t s) { _textSize = s ? s : 1; }

void tft_sprite::setTextDatum(uint8_t d) { _textDatum = d; }

void tft_sprite::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (!_hasBuffer()) return;
    if (w <= 0 || h <= 0) return;
    int32_t x0 = std::max<int32_t>(0, x);
    int32_t y0 = std::max<int32_t>(0, y);
    int32_t x1 = std::min<int32_t>(_width, x + w);
    int32_t y1 = std::min<int32_t>(_height, y + h);
    uint16_t c = static_cast<uint16_t>(color);
    for (int32_t row = y0; row < y1; ++row) {
        uint16_t *line = &_buffer[static_cast<size_t>(row) * static_cast<size_t>(_width)];
        std::fill(line + x0, line + x1, c);
    }
}

void tft_sprite::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {
    fillRect(x, y, 1, h, color);
}

void tft_sprite::fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    if (!_hasBuffer() || r < 0) return;
    uint16_t c = static_cast<uint16_t>(color);
    for (int32_t y = -r; y <= r; ++y) {
        int32_t dx = static_cast<int32_t>(std::sqrt(static_cast<float>(r * r - y * y)));
        drawHLine(x0 - dx, y0 + y, dx * 2, c);
    }
}

void tft_sprite::drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    if (!_hasBuffer() || r < 0) return;
    int32_t f = 1 - r;
    int32_t ddF_x = 1;
    int32_t ddF_y = -2 * r;
    int32_t x = 0;
    int32_t y = r;
    auto plot = [&](int32_t px, int32_t py) { setPixel(px, py, static_cast<uint16_t>(color)); };
    plot(x0, y0 + r);
    plot(x0, y0 - r);
    plot(x0 + r, y0);
    plot(x0 - r, y0);
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        plot(x0 + x, y0 + y);
        plot(x0 - x, y0 + y);
        plot(x0 + x, y0 - y);
        plot(x0 - x, y0 - y);
        plot(x0 + y, y0 + x);
        plot(x0 - y, y0 + x);
        plot(x0 + y, y0 - x);
        plot(x0 - y, y0 - x);
    }
}

void tft_sprite::fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
    if (!_hasBuffer() || rx < 0 || ry < 0) return;
    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t x = 0;
    int32_t y = ry;
    int64_t px = 0;
    int64_t py = 2LL * rx2 * y;
    int64_t p = std::llround(ry2 - (rx2 * ry) + (0.25 * rx2));
    while (px < py) {
        ++x;
        px += 2LL * ry2;
        if (p < 0) p += ry2 + px;
        else {
            --y;
            py -= 2LL * rx2;
            p += ry2 + px - py;
        }
        drawHLine(x0 - x, y0 + y, x * 2, color);
        drawHLine(x0 - x, y0 - y, x * 2, color);
    }
    p = std::llround(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);
    while (y >= 0) {
        drawHLine(x0 - x, y0 + y, x * 2, color);
        drawHLine(x0 - x, y0 - y, x * 2, color);
        --y;
        py -= 2LL * rx2;
        if (p > 0) p += rx2 - py;
        else {
            ++x;
            px += 2LL * ry2;
            p += rx2 - py + px;
        }
    }
}

void tft_sprite::fillTriangle(
    int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color
) {
    if (!_hasBuffer()) return;
    uint16_t c = static_cast<uint16_t>(color);
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }

    auto drawSpan = [&](int32_t y, int32_t xa, int32_t xb) {
        if (y < 0 || y >= _height) return;
        if (xa > xb) std::swap(xa, xb);
        xa = std::max<int32_t>(0, xa);
        xb = std::min<int32_t>(_width - 1, xb);
        drawHLine(xa, y, xb - xa + 1, c);
    };

    auto edge = [](int32_t y0, int32_t x0, int32_t y1, int32_t x1, int32_t y) -> int32_t {
        if (y1 == y0) return x0;
        return x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    };

    for (int32_t y = y0; y <= y1; ++y) {
        int32_t xa = edge(y0, x0, y2, x2, y);
        int32_t xb = edge(y0, x0, y1, x1, y);
        drawSpan(y, xa, xb);
    }
    for (int32_t y = y1; y <= y2; ++y) {
        int32_t xa = edge(y0, x0, y2, x2, y);
        int32_t xb = edge(y1, x1, y2, x2, y);
        drawSpan(y, xa, xb);
    }
}

void tft_sprite::pushSprite(int32_t x, int32_t y, uint32_t transparent) {
    if (!_hasBuffer() || !_display) return;
    if (transparent == TFT_TRANSPARENT) {
        for (int32_t j = 0; j < _height; ++j) {
            const uint16_t *row = &_buffer[static_cast<size_t>(j) * static_cast<size_t>(_width)];
            for (int32_t i = 0; i < _width; ++i) {
                uint16_t color = row[i];
                if (color != static_cast<uint16_t>(transparent)) { _display->drawPixel(x + i, y + j, color); }
            }
        }
    } else {
        _display->pushImage(x, y, _width, _height, _buffer.data());
    }
}

void tft_sprite::pushToSprite(tft_sprite *dest, int32_t x, int32_t y, uint32_t transparent) {
    if (!dest || !_hasBuffer() || !dest->_hasBuffer()) return;
    for (int32_t j = 0; j < _height; ++j) {
        for (int32_t i = 0; i < _width; ++i) {
            uint16_t color =
                _buffer[static_cast<size_t>(j) * static_cast<size_t>(_width) + static_cast<size_t>(i)];
            if (color == static_cast<uint16_t>(transparent)) continue;
            dest->setPixel(x + i, y + j, color);
        }
    }
}

int16_t tft_sprite::width() const { return _width; }

int16_t tft_sprite::height() const { return _height; }

int16_t tft_sprite::fontHeight(int16_t font) const {
    (void)font;
    return static_cast<int16_t>(_textSize * 8);
}

void tft_sprite::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
    if (!_hasBuffer()) return;
    int32_t dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int32_t dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int32_t err = dx + dy;
    while (true) {
        setPixel(x0, y0, static_cast<uint16_t>(color));
        if (x0 == x1 && y0 == y1) break;
        int32_t e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void tft_sprite::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    drawLine(x, y, x + w - 1, y, color);
    drawLine(x, y, x, y + h - 1, color);
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
}

void tft_sprite::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
    (void)r;
    drawRect(x, y, w, h, color);
}

void tft_sprite::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
    (void)r;
    fillRect(x, y, w, h, color);
}

void tft_sprite::drawPixel(int32_t x, int32_t y, uint32_t color) {
    setPixel(x, y, static_cast<uint16_t>(color));
}

void tft_sprite::drawXBitmap(
    int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg
) {
    if (!_hasBuffer() || !bitmap) return;
    for (int16_t j = 0; j < h; ++j) {
        for (int16_t i = 0; i < w; ++i) {
            uint8_t byte = bitmap[(j * ((w + 7) / 8)) + (i >> 3)];
            bool bit = byte & (0x80 >> (i & 7));
            setPixel(x + i, y + j, bit ? color : bg);
        }
    }
}

void tft_sprite::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data) {
    if (!_hasBuffer() || !data) return;
    for (int32_t row = 0; row < h; ++row) {
        for (int32_t col = 0; col < w; ++col) { setPixel(x + col, y + row, data[row * w + col]); }
    }
}

void tft_sprite::pushImage(
    int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, bool bpp8, uint16_t *cmap
) {
    if (!_hasBuffer() || !data || !bpp8 || !cmap) return;
    for (int32_t row = 0; row < h; ++row) {
        for (int32_t col = 0; col < w; ++col) {
            uint8_t idx = data[row * w + col];
            setPixel(x + col, y + row, cmap[idx]);
        }
    }
}

void tft_sprite::pushImage(
    int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data, bool bpp8, uint16_t *cmap
) {
    pushImage(x, y, w, h, const_cast<uint8_t *>(data), bpp8, cmap);
}

void tft_sprite::fillRectHGradient(
    int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2
) {
    (void)color2;
    fillRect(x, y, w, h, color1);
}

void tft_sprite::fillRectVGradient(
    int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2
) {
    (void)color2;
    fillRect(x, y, w, h, color1);
}

int16_t tft_sprite::drawString(const String &string, int32_t x, int32_t y, uint8_t font) {
    (void)font;
    setCursor(x, y);
    return string.length();
}

bool tft_sprite::_hasBuffer() const { return !_buffer.empty() && _width > 0 && _height > 0; }

void tft_sprite::setPixel(int32_t x, int32_t y, uint16_t color) {
    if (x < 0 || y < 0 || x >= _width || y >= _height) return;
    _buffer[static_cast<size_t>(y) * static_cast<size_t>(_width) + static_cast<size_t>(x)] = color;
}

void tft_sprite::drawHLine(int32_t x, int32_t y, int32_t w, uint16_t color) {
    if (y < 0 || y >= _height || w <= 0) return;
    int32_t x0 = std::max<int32_t>(0, x);
    int32_t x1 = std::min<int32_t>(_width, x + w);
    if (x0 >= x1) return;
    std::fill(
        _buffer.begin() + static_cast<size_t>(y) * static_cast<size_t>(_width) + x0,
        _buffer.begin() + static_cast<size_t>(y) * static_cast<size_t>(_width) + x1,
        color
    );
}

#endif
