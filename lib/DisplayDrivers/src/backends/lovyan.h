// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

#ifndef LIB_HAL_LOVYAN_H
#define LIB_HAL_LOVYAN_H
#include <pins_arduino.h>

#if !defined(LOVYAN_PANEL)
#error "You must define LOVYAN_PANEL:\n \
- Panel_ST7789\n \
- Panel_NV3031B (QSPI)\n \
- Panel_GC9A01\n \
- Panel_GDEW0154M09\n \
- Panel_HX8357B \n \
- Panel_HX8357D \n \
- Panel_ILI9163 \n \
- Panel_ILI9341 \n \
- Panel_ILI9342 \n \
- Panel_ILI9481 \n \
- Panel_ILI9486 \n \
- Panel_ILI9488 \n \
- Panel_IT8951  \n \
- Panel_RA8875  \n \
- Panel_SH1106  \n \
- Panel_SH1107  \n \
- Panel_SSD1306 \n \
- Panel_SSD1327 \n \
- Panel_SSD1331 \n \
- Panel_SSD1351 \n \
- Panel_SSD1357 \n \
- Panel_SSD1963 \n \
- Panel_ST7735  \n \
- Panel_ST7735S \n \
- Panel_ST7789  \n \
- Panel_ST7796"
#endif

#if !defined(LOVYAN_BUS)
#error "You must define LOVYAN_BUS: \n\
    - Bus_SPI\n \
    - Bus_Parallel8\n \
    - Bus_I2C "
#endif

#include <LovyanGFX.hpp>
#include <SPI.h>
#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <memory>

#include "tft_defines.h"

// clang-format off
// ---------------------------------------------------------------------------
// Bus selection
//
// The branch is derived from LOVYAN_BUS, so a board only states its bus once.
// LOVYAN_BUS is a type name, and every identifier the preprocessor does not
// know evaluates to 0 -- so `#if LOVYAN_BUS == Bus_SPI` would match *any* bus.
// Paste it into a per-bus id macro instead, which only resolves for the names
// we actually support.
// LOVYAN_SPI_BUS / LOVYAN_I2C_BUS / LOVYAN_8PARALLEL_BUS are still honoured
// when a board sets one explicitly.
// ---------------------------------------------------------------------------
#if !defined(LOVYAN_SPI_BUS) && !defined(LOVYAN_I2C_BUS) && !defined(LOVYAN_8PARALLEL_BUS)
    #define LOVYAN_BUS_ID_Bus_SPI       1
    #define LOVYAN_BUS_ID_Bus_I2C       2
    #define LOVYAN_BUS_ID_Bus_Parallel8 3
    #define LOVYAN_CAT_(a, b) a##b
    #define LOVYAN_CAT(a, b) LOVYAN_CAT_(a, b)
    #define LOVYAN_BUS_ID LOVYAN_CAT(LOVYAN_BUS_ID_, LOVYAN_BUS)

    #if LOVYAN_BUS_ID == 1
        #define LOVYAN_SPI_BUS 1
    #elif LOVYAN_BUS_ID == 2
        #define LOVYAN_I2C_BUS 1
    #elif LOVYAN_BUS_ID == 3
        #define LOVYAN_8PARALLEL_BUS 1
    #else
        #error "Unsupported LOVYAN_BUS. Use Bus_SPI, Bus_I2C or Bus_Parallel8, or select the\n \
        branch by hand with LOVYAN_SPI_BUS / LOVYAN_I2C_BUS / LOVYAN_8PARALLEL_BUS."
    #endif
#endif

// ---------------------------------------------------------------------------
// Panel configuration
//
// The project-wide TFT_* names are the source of truth; the LovyanGFX-specific
// names below stay available as per-board overrides.
// ---------------------------------------------------------------------------
#if !defined(TFT_CS) || !defined(TFT_RST) || !defined(TFT_WIDTH) || !defined(TFT_HEIGHT)
    #error "Missing Macro definitions of: TFT_CS, TFT_RST, TFT_WIDTH, TFT_HEIGHT"
#endif

#if !defined(TFT_OFFSET_X)
    #if defined(TFT_COL_OFS1)
        #define TFT_OFFSET_X TFT_COL_OFS1
    #else
        #define TFT_OFFSET_X 0
    #endif
#endif

#if !defined(TFT_OFFSET_Y)
    #if defined(TFT_ROW_OFS1)
        #define TFT_OFFSET_Y TFT_ROW_OFS1
    #else
        #define TFT_OFFSET_Y 0
    #endif
#endif

// Panel RAM. Only differs from the visible area on panels with a window offset.
#if !defined(TFT_MEM_WIDTH)
    #define TFT_MEM_WIDTH TFT_WIDTH
#endif
#if !defined(TFT_MEM_HEIGHT)
    #define TFT_MEM_HEIGHT TFT_HEIGHT
#endif

#if !defined(TFT_INVERTION)
    #if defined(TFT_INVERTED)
        #define TFT_INVERTION TFT_INVERTED
    #else
        #define TFT_INVERTION 0
    #endif
#endif

#if !defined(TFT_RGB_ORDER)
    #define TFT_RGB_ORDER 0 // 0 = BGR, 1 = RGB
#endif

#if !defined(TFT_BUSY_PIN)
    #define TFT_BUSY_PIN -1
#endif

// Panel mounting offset; setRotation() is applied on top of it.
#if !defined(TFT_ROTATION)
    #define TFT_ROTATION 0
#endif

// Whether the bus is shared with other devices (SD card, …).
#if !defined(TFT_BUS_SHARED)
    #define TFT_BUS_SHARED true
#endif

// ---------------------------------------------------------------------------
// Per-bus configuration
// ---------------------------------------------------------------------------
#if defined(LOVYAN_SPI_BUS)
    // Quad SPI (TFT_D0..TFT_D3) replaces MOSI/MISO, and QSPI panels carry the
    // command/data phase in the frame itself, so they have no DC line either.
    #if defined(TFT_D0) && defined(TFT_D1) && defined(TFT_D2) && defined(TFT_D3)
        #if !defined(TFT_SCLK)
            #error "To use a QSPI bus, need to define: TFT_SCLK, TFT_D0 .. TFT_D3"
        #endif
    #elif !defined(TFT_SCLK) || !defined(TFT_MOSI) || !defined(TFT_DC)
        #error "To use a SPI bus, need to define: TFT_SCLK, TFT_MOSI, TFT_DC (or TFT_D0 .. TFT_D3 for QSPI)"
    #endif
    #if !defined(TFT_MISO)
        #define TFT_MISO -1
    #endif
    #if !defined(TFT_SPI_HOST)
        #define TFT_SPI_HOST SPI2_HOST
    #endif
    #if !defined(TFT_SPI_MODE)
        #define TFT_SPI_MODE 0
    #endif
    #if !defined(TFT_WRITE_FREQ)
        #define TFT_WRITE_FREQ 40000000
    #endif
    #if !defined(TFT_READ_FREQ)
        #define TFT_READ_FREQ 16000000
    #endif
    #if !defined(TFT_SPI_3WIRE)
        #define TFT_SPI_3WIRE true
    #endif
    #if !defined(TFT_USE_LOCK)
        #define TFT_USE_LOCK true
    #endif

#elif defined(LOVYAN_I2C_BUS)
    #if !defined(TFT_SDA) || !defined(TFT_SCL) || !defined(TFT_ADDR)
        #error "To use an I2C bus, need to define: TFT_SDA, TFT_SCL, TFT_ADDR"
    #endif
    #if !defined(TFT_I2C_PORT)
        #define TFT_I2C_PORT 0
    #endif
    #if !defined(TFT_I2C_WRITE)
        #define TFT_I2C_WRITE 400000
    #endif
    #if !defined(TFT_I2C_READ)
        #define TFT_I2C_READ 400000
    #endif

#elif defined(LOVYAN_8PARALLEL_BUS)
    #if !defined(TFT_WR) || !defined(TFT_DC) || !defined(TFT_D0) || !defined(TFT_D1) ||                      \
        !defined(TFT_D2) || !defined(TFT_D3) || !defined(TFT_D4) || !defined(TFT_D5) ||                      \
        !defined(TFT_D6) || !defined(TFT_D7)
        #error "To use a Parallel 8 bit bus, need to define:\n \
        - TFT_WR\n \
        - TFT_DC\n \
        - TFT_D0 .. TFT_D7"
    #endif
    #if !defined(TFT_RD)
        #define TFT_RD -1
    #endif
    #if !defined(TFT_WRITE_FREQ)
        #define TFT_WRITE_FREQ 16000000
    #endif
#endif
// clang-format on

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
    // C strings get their own overloads: a plain char buffer (a VLA in
    // particular) cannot bind to the template's const T& parameter.
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

    // Present on every backend so callers do not have to know whether they are
    // driving a framebuffered/e-paper panel. Nothing to flush here.
    inline void display(bool fullRefresh = false) { (void)fullRefresh; }

private:
    template <typename Ptr> void pushImageFallback(int32_t x, int32_t y, int32_t w, int32_t h, Ptr data) {
        if (!data) return;
        for (int32_t row = 0; row < h; ++row) {
            for (int32_t col = 0; col < w; ++col) {
                uint16_t color = data[row * w + col];
                if (_swapBytes) color = static_cast<uint16_t>((color >> 8) | (color << 8));
                lgfx::LGFX_Device::drawPixel(x + col, y + row, color);
            }
        }
    }

    int16_t drawAlignedString(const String &s, int32_t x, int32_t y, uint8_t datum);

    lgfx::LOVYAN_PANEL _panel_instance;
    lgfx::LOVYAN_BUS _bus_instance;
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
    using lgfx::LGFX_Sprite::drawString;
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

    int16_t width() const;
    int16_t height() const;
    // Present on every backend's sprite: the launcher lays rows out with it.
    int16_t fontHeight(int16_t font = 1) const;
};

#endif // LIB_HAL_LOVYAN_H
