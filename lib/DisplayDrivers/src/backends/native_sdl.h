// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

#ifndef LIB_HAL_NATIVE_SDL_H
#define LIB_HAL_NATIVE_SDL_H

#if defined(USE_NATIVE_SDL)

// This backend targets PlatformIO's `platform = native` (a desktop g++/clang
// build, no Arduino core at all) so a UI can be exercised in an SDL2 window on
// a PC. Every other DisplayDrivers backend assumes Arduino (pins_arduino.h,
// WString.h, SPIClass) is present; native builds have none of that, so this
// header brings its own minimal, source-compatible String and SPIClass rather
// than pulling in Arduino. It is not meant to run on a device.
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/sdl/Panel_sdl.hpp>

#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

#include "tft_defines.h"

#if !defined(TFT_WIDTH)
#define TFT_WIDTH 320
#endif
#if !defined(TFT_HEIGHT)
#define TFT_HEIGHT 240
#endif

// ---------------------------------------------------------------------------
// Arduino String, just enough of it: construction from the usual scalar
// types, concatenation and c_str(). Anything pasted in from device code that
// only uses these stays source-compatible.
// ---------------------------------------------------------------------------
class String {
public:
    String() = default;
    String(const char *s) : _s(s ? s : "") {}
    String(const std::string &s) : _s(s) {}
    String(char c) : _s(1, c) {}
    // Base defaults to 10 so plain String(intValue) call sites (the vast
    // majority) are unaffected; only String(x, 16) and similar pick a radix.
    // base takes the same param type (int) as the float/double decimals
    // overloads below on purpose: a mismatched (unsigned char) type here
    // ties with those on one argument and loses on the other, making every
    // 2-argument String(byteValue, N) call ambiguous.
    String(unsigned char v, int base = 10) { _assignBase(v, static_cast<unsigned char>(base)); }
    String(int v) : _s(std::to_string(v)) {}
    String(unsigned int v) : _s(std::to_string(v)) {}
    String(long v) : _s(std::to_string(v)) {}
    String(unsigned long v) : _s(std::to_string(v)) {}
    String(float v, int decimals = 2) { _assignFloat(v, decimals); }
    String(double v, int decimals = 2) { _assignFloat(v, decimals); }

    const char *c_str() const { return _s.c_str(); }
    size_t length() const { return _s.length(); }
    bool isEmpty() const { return _s.empty(); }
    char charAt(size_t i) const { return i < _s.size() ? _s[i] : '\0'; }
    char operator[](size_t i) const { return _s[i]; }
    void reserve(size_t n) { _s.reserve(n); }

    // Overloads taking const char* directly (rather than going through the
    // String(const char*) constructor) so `s == "literal"` picks exactly one
    // candidate instead of being ambiguous between that conversion and the
    // operator const char*() below.
    bool operator==(const String &o) const { return _s == o._s; }
    bool operator!=(const String &o) const { return _s != o._s; }
    bool operator==(const char *s) const { return _s == (s ? s : ""); }
    bool operator!=(const char *s) const { return _s != (s ? s : ""); }
    bool operator<(const String &o) const { return _s < o._s; }
    String operator+(const String &o) const { return String((_s + o._s).c_str()); }
    String &operator+=(const String &o) {
        _s += o._s;
        return *this;
    }
    operator const char *() const { return _s.c_str(); }

    bool equals(const String &o) const { return _s == o._s; }
    bool equalsIgnoreCase(const String &o) const {
        if (_s.size() != o._s.size()) return false;
        for (size_t i = 0; i < _s.size(); ++i) {
            if (::tolower((unsigned char)_s[i]) != ::tolower((unsigned char)o._s[i])) return false;
        }
        return true;
    }
    bool startsWith(const String &prefix) const { return _s.rfind(prefix._s, 0) == 0; }
    bool endsWith(const String &suffix) const {
        return _s.size() >= suffix._s.size() &&
               _s.compare(_s.size() - suffix._s.size(), suffix._s.size(), suffix._s) == 0;
    }
    int indexOf(const String &needle, size_t from = 0) const {
        auto pos = _s.find(needle._s, from);
        return pos == std::string::npos ? -1 : static_cast<int>(pos);
    }
    int indexOf(char c, size_t from = 0) const {
        auto pos = _s.find(c, from);
        return pos == std::string::npos ? -1 : static_cast<int>(pos);
    }
    int lastIndexOf(const String &needle) const {
        auto pos = _s.rfind(needle._s);
        return pos == std::string::npos ? -1 : static_cast<int>(pos);
    }
    int lastIndexOf(char c) const {
        auto pos = _s.rfind(c);
        return pos == std::string::npos ? -1 : static_cast<int>(pos);
    }
    String substring(size_t from) const { return from >= _s.size() ? String() : String(_s.substr(from)); }
    String substring(size_t from, size_t to) const {
        if (from >= _s.size() || to <= from) return String();
        return String(_s.substr(from, to - from));
    }
    void remove(size_t index) {
        if (index < _s.size()) _s.erase(index);
    }
    void remove(size_t index, size_t count) {
        if (index < _s.size()) _s.erase(index, count);
    }
    void replace(const String &from, const String &to) {
        if (from._s.empty()) return;
        size_t pos = 0;
        while ((pos = _s.find(from._s, pos)) != std::string::npos) {
            _s.replace(pos, from._s.size(), to._s);
            pos += to._s.size();
        }
    }
    void trim() {
        size_t a = _s.find_first_not_of(" \t\r\n");
        size_t b = _s.find_last_not_of(" \t\r\n");
        _s = (a == std::string::npos) ? "" : _s.substr(a, b - a + 1);
    }
    void toUpperCase() {
        for (auto &c : _s) c = static_cast<char>(::toupper((unsigned char)c));
    }
    void toLowerCase() {
        for (auto &c : _s) c = static_cast<char>(::tolower((unsigned char)c));
    }
    bool concat(const String &o) {
        _s += o._s;
        return true;
    }
    int toInt() const { return _s.empty() ? 0 : atoi(_s.c_str()); }
    float toFloat() const { return _s.empty() ? 0.f : static_cast<float>(atof(_s.c_str())); }
    int compareTo(const String &o) const { return _s.compare(o._s); }

private:
    void _assignFloat(double v, int decimals) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%.*f", decimals, v);
        _s = buf;
    }
    // Mirrors Arduino String's String(byte value, base) constructor: base 10
    // is the common decimal case, anything else (typically 16) renders
    // unsigned digit-by-digit like utoa does on real hardware.
    void _assignBase(unsigned char v, unsigned char base) {
        if (base == 10) {
            _s = std::to_string(v);
            return;
        }
        if (base < 2 || base > 36) base = 10;
        static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
        char buf[8 * sizeof(unsigned long) + 1];
        char *p = buf + sizeof(buf) - 1;
        *p = '\0';
        do {
            *--p = digits[v % base];
            v /= base;
        } while (v);
        _s = p;
    }
    std::string _s;
};

// So `"literal" == s` and `"literal" + s` also resolve to exactly one
// candidate instead of relying on String's implicit operator const char*().
inline bool operator==(const char *a, const String &b) { return b == a; }
inline bool operator!=(const char *a, const String &b) { return b != a; }
inline String operator+(const char *a, const String &b) { return String(a) + b; }

// Arduino's global no-op, occasionally called from loops ported from device
// code that expect cooperative multitasking to happen there.
inline void yield() {}

// ArduinoJson has built-in support for Arduino's own String only when built
// under the Arduino core; this backend's String is a different type, so it
// needs its own Converter (the extension point ArduinoJson's docs describe
// for custom string types) to (de)serialize through JsonVariant.
#if __has_include(<ArduinoJson.h>)
#include <ArduinoJson.h>
namespace ArduinoJson {
template <> struct Converter<String> {
    static void toJson(const String &src, JsonVariant dst) { dst.set(src.c_str()); }
    static String fromJson(JsonVariantConst src) {
        const char *s = src.as<const char *>();
        return String(s ? s : "");
    }
    static bool checkJson(JsonVariantConst src) { return src.is<const char *>(); }
};
} // namespace ArduinoJson
#endif

// The public tft_display API returns SPIClass& on every backend so portable
// code can call getSPIinstance() unconditionally. Nothing here ever talks to
// real SPI hardware, so an empty type is enough to satisfy the signature.
class SPIClass {
public:
    void begin(int sck = -1, int miso = -1, int mosi = -1, int cs = -1) {
        (void)sck;
        (void)miso;
        (void)mosi;
        (void)cs;
    }
};

class tft_sprite;
class tft_logger;

class tft_display : private lgfx::LGFX_Device {
public:
    explicit tft_display(int16_t _W = TFT_WIDTH, int16_t _H = TFT_HEIGHT);
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
    size_t print(const char *val) { return val ? (lgfx::LGFX_Device::print(val)) : 0; }
    size_t println(const char *val) { return val ? (lgfx::LGFX_Device::println(val)) : 0; }
    template <typename T> size_t print(const T &val) { return lgfx::LGFX_Device::print(val); }
    template <typename T> size_t println(const T &val) { return lgfx::LGFX_Device::println(val); }
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
    lgfx::LGFX_Device *native();

    inline void display(bool fullRefresh = false) { (void)fullRefresh; }

private:
    int16_t drawAlignedString(const String &s, int32_t x, int32_t y, uint8_t datum);

    lgfx::Panel_sdl _panel_instance;
    uint16_t _height = TFT_HEIGHT;
    uint16_t _width = TFT_WIDTH;
    bool _swapBytes = false;
    uint32_t _textColor = 0xFFFF;
    uint32_t _textBgColor = 0x0000;
    uint8_t _textSize = 1;
    uint8_t _textDatum = 0;
    uint8_t _textFont = 1;
    uint8_t _rotation = 0;
};

class tft_sprite : private lgfx::LGFX_Sprite {
public:
    explicit tft_sprite(tft_display *parent);
    ~tft_sprite() = default;

    using lgfx::LGFX_Sprite::drawCircle;
    using lgfx::LGFX_Sprite::drawLine;
    using lgfx::LGFX_Sprite::drawPixel;
    using lgfx::LGFX_Sprite::drawRect;
    using lgfx::LGFX_Sprite::drawRoundRect;
    using lgfx::LGFX_Sprite::drawXBitmap;
    using lgfx::LGFX_Sprite::fillCircle;
    using lgfx::LGFX_Sprite::fillRect;
    using lgfx::LGFX_Sprite::fillRoundRect;
    using lgfx::LGFX_Sprite::pushImage;
    using lgfx::LGFX_Sprite::setCursor;
    using lgfx::LGFX_Sprite::setTextColor;
    using lgfx::LGFX_Sprite::setTextDatum;
    using lgfx::LGFX_Sprite::setTextSize;

    void *createSprite(int16_t w, int16_t h, uint8_t frames = 1);
    void deleteSprite();
    void setColorDepth(uint8_t depth);

    void fillScreen(uint32_t color);
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color);
    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
    void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);

    void pushSprite(int32_t x, int32_t y, uint32_t transparent = TFT_TRANSPARENT);
    void pushToSprite(tft_sprite *dest, int32_t x, int32_t y, uint32_t transparent = TFT_TRANSPARENT);
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, bool bpp8, uint16_t *cmap);
    void
    pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data, bool bpp8, uint16_t *cmap);

    void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
    void fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);

    int16_t drawString(const String &string, int32_t x, int32_t y, uint8_t font = 1);

    int16_t width() const;
    int16_t height() const;
    int16_t fontHeight(int16_t font = 1) const;
};

#endif
#endif // LIB_HAL_NATIVE_SDL_H
