// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

#ifndef LIB_HAL_SWSPRITE_H
#define LIB_HAL_SWSPRITE_H
#include <pins_arduino.h>

#if defined(USE_EPD_PAINTER) || defined(USE_GXEPD2)

#include <WString.h>
#include <cstdint>
#include <vector>

#include "tft_defines.h"

class tft_display;

// Generic RGB565 software sprite, shared by the backends whose native library
// has no sprite/canvas of its own (EPD Painter and GxEPD2 both draw straight
// into their own single framebuffer). The buffer is kept in RGB565 so callers
// see the same colour space as on the colour backends; the conversion to panel
// levels happens once, when the sprite is pushed to the display.
class tft_sprite {
public:
    explicit tft_sprite(tft_display *parent);
    ~tft_sprite() = default;

    void *createSprite(int16_t w, int16_t h, uint8_t frames = 1);
    void deleteSprite();

    void fillScreen(uint32_t color);
    void setColorDepth(uint8_t depth);
    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t b);
    void setTextSize(uint8_t s);
    void setTextDatum(uint8_t d);

    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

    void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);

    void fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
    void drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);

    void fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);

    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);

    void pushSprite(int32_t x, int32_t y, uint32_t transparent = TFT_TRANSPARENT);

    void pushToSprite(tft_sprite *dest, int32_t x, int32_t y, uint32_t transparent = TFT_TRANSPARENT);

    int16_t width() const;
    int16_t height() const;
    int16_t fontHeight(int16_t font = 1) const;
    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
    void drawPixel(int32_t x, int32_t y, uint32_t color);
    void drawXBitmap(
        int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg = 0
    );
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data);
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, bool bpp8, uint16_t *cmap);
    void
    pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data, bool bpp8, uint16_t *cmap);
    void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
    void fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
    int16_t drawString(const String &string, int32_t x, int32_t y, uint8_t font = 1);

private:
    bool _hasBuffer() const;

    void setPixel(int32_t x, int32_t y, uint16_t color);

    void drawHLine(int32_t x, int32_t y, int32_t w, uint16_t color);

    tft_display *_display = nullptr;
    int16_t _width = 0;
    int16_t _height = 0;
    std::vector<uint16_t> _buffer;
    uint8_t _colorDepth = 16;
    int16_t _cursorX = 0;
    int16_t _cursorY = 0;
    uint32_t _textColor = TFT_WHITE;
    uint32_t _textBgColor = TFT_BLACK;
    uint8_t _textSize = 1;
    uint8_t _textDatum = 0;
};

#endif
#endif // LIB_HAL_SWSPRITE_H
