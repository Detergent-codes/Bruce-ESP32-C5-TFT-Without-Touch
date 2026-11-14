// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

#ifndef LIB_HAL_DUMMY_H
#define LIB_HAL_DUMMY_H
#include <pins_arduino.h>

#if defined(USE_DUMMY_TFT)

#include <SPI.h>
#include <WString.h>

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "tft_defines.h"

// ---------------------------------------------------------------------------
// Headless backend — the whole API, none of the pixels.
//
// For firmware that runs on boards with and without a panel: the display code
// compiles unchanged and every draw call becomes nothing. Only the state a
// caller can read back is kept, so layout maths still works and code that
// branches on width(), height(), fontHeight() or getTextColor() behaves the
// same as it would on a real panel.
//
// No graphics library is needed, and none is pulled in. Everything is inline,
// so the calls disappear at -Os instead of turning into empty function calls.
//
// The panel it pretends to be is TFT_WIDTH x TFT_HEIGHT; both default to a
// 240x320 portrait panel when the board declares no size at all.
// ---------------------------------------------------------------------------

#if !defined(TFT_WIDTH)
#define TFT_WIDTH 240
#endif
#if !defined(TFT_HEIGHT)
#define TFT_HEIGHT 320
#endif

// Metrics reported by textWidth()/fontHeight(), in pixels at text size 1.
// The defaults are the classic 6x8 GLCD cell every other backend uses for
// font 1. They are never rendered — they only keep layout maths sane.
#if !defined(DUMMY_TFT_CHAR_WIDTH)
#define DUMMY_TFT_CHAR_WIDTH 6
#endif
#if !defined(DUMMY_TFT_CHAR_HEIGHT)
#define DUMMY_TFT_CHAR_HEIGHT 8
#endif

class tft_display {
public:
    explicit tft_display(int16_t _W = TFT_WIDTH, int16_t _H = TFT_HEIGHT)
        : _width(static_cast<uint16_t>(_W)), _height(static_cast<uint16_t>(_H)) {}
    friend class tft_sprite;
    friend class tft_logger;

    void begin(uint32_t speed = 0) { (void)speed; }
    void init(uint8_t tc = 0) { (void)tc; }
    void setRotation(uint8_t r) { _rotation = static_cast<uint8_t>(r & 3); }

    void drawPixel(int32_t x, int32_t y, uint32_t color) {
        (void)x;
        (void)y;
        (void)color;
    }
    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
        (void)x0;
        (void)y0;
        (void)x1;
        (void)y1;
        (void)color;
    }
    void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {
        (void)x;
        (void)y;
        (void)w;
        (void)color;
    }
    void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {
        (void)x;
        (void)y;
        (void)h;
        (void)color;
    }
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)color;
    }
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)color;
    }
    void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)color1;
        (void)color2;
    }
    void fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)color1;
        (void)color2;
    }
    void fillScreen(uint32_t color) { (void)color; }
    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)r;
        (void)color;
    }
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)r;
        (void)color;
    }
    void drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
        (void)x0;
        (void)y0;
        (void)r;
        (void)color;
    }
    void fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
        (void)x0;
        (void)y0;
        (void)r;
        (void)color;
    }
    void drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
        (void)x0;
        (void)y0;
        (void)x1;
        (void)y1;
        (void)x2;
        (void)y2;
        (void)color;
    }
    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
        (void)x0;
        (void)y0;
        (void)x1;
        (void)y1;
        (void)x2;
        (void)y2;
        (void)color;
    }
    void drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
        (void)x0;
        (void)y0;
        (void)rx;
        (void)ry;
        (void)color;
    }
    void fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
        (void)x0;
        (void)y0;
        (void)rx;
        (void)ry;
        (void)color;
    }
    void drawArc(
        int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle,
        uint32_t fg_color, uint32_t bg_color, bool smoothArc = true
    ) {
        (void)x;
        (void)y;
        (void)r;
        (void)ir;
        (void)startAngle;
        (void)endAngle;
        (void)fg_color;
        (void)bg_color;
        (void)smoothArc;
    }
    void drawWideLine(
        float ax, float ay, float bx, float by, float wd, uint32_t fg_color, uint32_t bg_color = 0x00FFFFFF
    ) {
        (void)ax;
        (void)ay;
        (void)bx;
        (void)by;
        (void)wd;
        (void)fg_color;
        (void)bg_color;
    }
    void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
        (void)x;
        (void)y;
        (void)bitmap;
        (void)w;
        (void)h;
        (void)color;
    }
    void drawXBitmap(
        int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg
    ) {
        (void)x;
        (void)y;
        (void)bitmap;
        (void)w;
        (void)h;
        (void)color;
        (void)bg;
    }
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)data;
    }
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)data;
    }
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, bool bpp8, uint16_t *cmap) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)data;
        (void)bpp8;
        (void)cmap;
    }
    void
    pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data, bool bpp8, uint16_t *cmap) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)data;
        (void)bpp8;
        (void)cmap;
    }

    void invertDisplay(bool i) { (void)i; }
    void sleep(bool value) { (void)value; }
    void setSwapBytes(bool swap) { _swapBytes = swap; }
    bool getSwapBytes() const { return _swapBytes; }

    // Pure arithmetic, so callers that pack their own colours keep working.
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b) const {
        return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
    }

    int16_t textWidth(const String &s, uint8_t font = 1) const { return textWidth(s.c_str(), font); }
    int16_t textWidth(const char *s, uint8_t font = 1) const {
        (void)font;
        if (!s) return 0;
        size_t n = strlen(s);
        return static_cast<int16_t>(n * DUMMY_TFT_CHAR_WIDTH * (_textSize ? _textSize : 1));
    }

    void setCursor(int16_t x, int16_t y) {
        _cursorX = x;
        _cursorY = y;
    }
    int16_t getCursorX() const { return _cursorX; }
    int16_t getCursorY() const { return _cursorY; }
    void setTextSize(uint8_t s) { _textSize = s; }
    void setTextColor(uint16_t c) { _textColor = c; }
    void setTextColor(uint16_t c, uint16_t b, bool bgfill = false) {
        (void)bgfill;
        _textColor = c;
        _textBgColor = b;
    }
    void setTextDatum(uint8_t d) { _textDatum = d; }
    uint8_t getTextDatum() const { return _textDatum; }
    void setTextFont(uint8_t f) { _textFont = f; }
    void setTextWrap(bool wrapX, bool wrapY = false) {
        (void)wrapX;
        (void)wrapY;
    }
    int16_t drawString(const String &string, int32_t x, int32_t y, uint8_t font = 1) {
        (void)x;
        (void)y;
        return textWidth(string, font);
    }
    int16_t drawCentreString(const String &string, int32_t x, int32_t y, uint8_t font = 1) {
        return drawString(string, x, y, font);
    }
    int16_t drawRightString(const String &string, int32_t x, int32_t y, uint8_t font = 1) {
        return drawString(string, x, y, font);
    }

    // Print-compatible returns: the byte count is "accepted", so callers that
    // treat a short write as an error do not see one.
    size_t write(uint8_t c) {
        (void)c;
        return 1;
    }
    size_t write(const uint8_t *buffer, size_t size) {
        (void)buffer;
        return buffer ? size : 0;
    }
    // C strings get their own overloads: a plain char buffer (a VLA in
    // particular) cannot bind to the template's const T& parameter.
    size_t print(const char *val) { return val ? strlen(val) : 0; }
    size_t println(const char *val) { return val ? strlen(val) + 1 : 0; }
    template <typename T> size_t print(const T &val) {
        (void)val;
        return 0;
    }
    template <typename T> size_t println(const T &val) {
        (void)val;
        return 0;
    }
    size_t println() { return 0; }

    size_t printf(const char *fmt, ...) {
        if (!fmt) return 0;
        va_list args;
        va_start(args, fmt);
        const int n = vsnprintf(nullptr, 0, fmt, args);
        va_end(args);
        return n > 0 ? static_cast<size_t>(n) : 0;
    }

    int16_t width() const { return static_cast<int16_t>((_rotation & 1) ? _height : _width); }
    int16_t height() const { return static_cast<int16_t>((_rotation & 1) ? _width : _height); }
    SPIClass &getSPIinstance() const { return SPI; }
    void writecommand(uint8_t c) { (void)c; }

    uint32_t getTextColor() const { return _textColor; }
    uint32_t getTextBgColor() const { return _textBgColor; }
    uint8_t getTextSize() const { return _textSize; }
    uint8_t getRotation() const { return _rotation; }
    int16_t fontHeight(int16_t font = 1) const {
        (void)font;
        return static_cast<int16_t>(DUMMY_TFT_CHAR_HEIGHT * (_textSize ? _textSize : 1));
    }

    // There is no panel object to hand out. Present so portable code that
    // guards on native() != nullptr compiles and takes the "no panel" path.
    void *native() { return nullptr; }

    // Present on every backend so callers do not have to know whether they are
    // driving a framebuffered/e-paper panel. Nothing to flush here.
    void display(bool fullRefresh = false) { (void)fullRefresh; }

private:
    uint16_t _width = TFT_WIDTH;
    uint16_t _height = TFT_HEIGHT;
    int16_t _cursorX = 0;
    int16_t _cursorY = 0;
    bool _swapBytes = false;
    uint32_t _textColor = TFT_WHITE;
    uint32_t _textBgColor = TFT_BLACK;
    uint8_t _textSize = 1;
    uint8_t _textDatum = 0;
    uint8_t _textFont = 1;
    uint8_t _rotation = 0;
};

// Same idea for sprites: the geometry is remembered so width()/height() answer
// what was asked for, and nothing is ever allocated or drawn.
class tft_sprite {
public:
    explicit tft_sprite(tft_display *parent) : _display(parent) {}
    ~tft_sprite() = default;

    // Returns nullptr, like any sprite whose allocation failed — there is no
    // buffer to hand out. Callers that check the result take their "no sprite"
    // path, which on a headless build draws nothing either way. Callers that
    // ignore it keep working: every call below is a no-op.
    void *createSprite(int16_t w, int16_t h, uint8_t frames = 1) {
        (void)frames;
        _width = w;
        _height = h;
        return nullptr;
    }
    void deleteSprite() {
        _width = 0;
        _height = 0;
    }

    void fillScreen(uint32_t color) { (void)color; }
    void setColorDepth(uint8_t depth) { _colorDepth = depth; }
    void setCursor(int16_t x, int16_t y) {
        _cursorX = x;
        _cursorY = y;
    }
    void setTextColor(uint16_t c) { _textColor = c; }
    void setTextColor(uint16_t c, uint16_t b) {
        _textColor = c;
        _textBgColor = b;
    }
    void setTextSize(uint8_t s) { _textSize = s; }
    void setTextDatum(uint8_t d) { _textDatum = d; }

    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)color;
    }
    void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {
        (void)x;
        (void)y;
        (void)h;
        (void)color;
    }
    void fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
        (void)x0;
        (void)y0;
        (void)r;
        (void)color;
    }
    void drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
        (void)x0;
        (void)y0;
        (void)r;
        (void)color;
    }
    void fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
        (void)x0;
        (void)y0;
        (void)rx;
        (void)ry;
        (void)color;
    }
    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
        (void)x0;
        (void)y0;
        (void)x1;
        (void)y1;
        (void)x2;
        (void)y2;
        (void)color;
    }
    void pushSprite(int32_t x, int32_t y, uint32_t transparent = TFT_TRANSPARENT) {
        (void)x;
        (void)y;
        (void)transparent;
    }
    void pushToSprite(tft_sprite *dest, int32_t x, int32_t y, uint32_t transparent = TFT_TRANSPARENT) {
        (void)dest;
        (void)x;
        (void)y;
        (void)transparent;
    }

    int16_t width() const { return _width; }
    int16_t height() const { return _height; }
    int16_t fontHeight(int16_t font = 1) const {
        (void)font;
        return static_cast<int16_t>(DUMMY_TFT_CHAR_HEIGHT * (_textSize ? _textSize : 1));
    }

    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
        (void)x0;
        (void)y0;
        (void)x1;
        (void)y1;
        (void)color;
    }
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)color;
    }
    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)r;
        (void)color;
    }
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)r;
        (void)color;
    }
    void drawPixel(int32_t x, int32_t y, uint32_t color) {
        (void)x;
        (void)y;
        (void)color;
    }
    void drawXBitmap(
        int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg = 0
    ) {
        (void)x;
        (void)y;
        (void)bitmap;
        (void)w;
        (void)h;
        (void)color;
        (void)bg;
    }
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)data;
    }
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, bool bpp8, uint16_t *cmap) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)data;
        (void)bpp8;
        (void)cmap;
    }
    void
    pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data, bool bpp8, uint16_t *cmap) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)data;
        (void)bpp8;
        (void)cmap;
    }
    void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)color1;
        (void)color2;
    }
    void fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)color1;
        (void)color2;
    }
    int16_t drawString(const String &string, int32_t x, int32_t y, uint8_t font = 1) {
        (void)x;
        (void)y;
        (void)font;
        return static_cast<int16_t>(string.length() * DUMMY_TFT_CHAR_WIDTH * (_textSize ? _textSize : 1));
    }

private:
    tft_display *_display = nullptr;
    int16_t _width = 0;
    int16_t _height = 0;
    uint8_t _colorDepth = 16;
    int16_t _cursorX = 0;
    int16_t _cursorY = 0;
    uint32_t _textColor = TFT_WHITE;
    uint32_t _textBgColor = TFT_BLACK;
    uint8_t _textSize = 1;
    uint8_t _textDatum = 0;
};

#endif
#endif // LIB_HAL_DUMMY_H
