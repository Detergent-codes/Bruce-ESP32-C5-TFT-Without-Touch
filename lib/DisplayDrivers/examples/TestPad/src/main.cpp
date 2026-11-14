// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// TestPad — every call DisplayDrivers publishes, on every backend.
//
// This sketch is not meant to be looked at on a panel: it is a compile fence.
// One env per backend builds it, so anything that stops being callable, or
// stops meaning the same thing on one backend, breaks the build instead of
// breaking a board months later.
//
// Layout mirrors the promise the library makes:
//   commonDisplayApi()  — every method tft_display has on ALL backends
//   commonSpriteApi()   — same for tft_sprite
//   backendApi()        — the handful of calls that exist on one backend only
//
// Anything added to the first two has to hold for all seven backends. Anything
// that cannot goes in the third, behind its USE_* flag.

#include <Arduino.h>
#include <DisplayDrivers.h>

tft_display tft;
tft_sprite spr(&tft);

// Small fixtures for the bitmap/image calls. Contents are irrelevant; the
// point is that every overload still binds.
static const uint8_t kXBitmap[8] = {0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x7E, 0x3C, 0x18};
static uint16_t kRgb565[4] = {0xF800, 0x07E0, 0x001F, 0xFFFF};
static uint8_t kIndexed[4] = {0, 1, 2, 3};
static uint16_t kPalette[4] = {0x0000, 0x7BEF, 0xC618, 0xFFFF};

// ---------------------------------------------------------------------------
// Common to every backend
// ---------------------------------------------------------------------------
static void commonDisplayApi() {
    // --- lifecycle ---------------------------------------------------------
    tft.begin();
    tft.begin(40000000);
    tft.init();
    tft.init(1);
    tft.setRotation(1);

    // --- geometry ----------------------------------------------------------
    tft.drawPixel(1, 1, TFT_RED);
    tft.drawLine(0, 0, 20, 20, TFT_GREEN);
    tft.drawFastHLine(0, 4, 20, TFT_BLUE);
    tft.drawFastVLine(4, 0, 20, TFT_BLUE);
    tft.drawRect(0, 0, 20, 20, TFT_WHITE);
    tft.fillRect(0, 0, 20, 20, TFT_BLACK);
    tft.fillRectHGradient(0, 0, 20, 20, TFT_RED, TFT_BLUE);
    tft.fillRectVGradient(0, 0, 20, 20, TFT_RED, TFT_BLUE);
    tft.fillScreen(TFT_BLACK);
    tft.drawRoundRect(0, 0, 20, 20, 4, TFT_WHITE);
    tft.fillRoundRect(0, 0, 20, 20, 4, TFT_WHITE);
    tft.drawCircle(10, 10, 8, TFT_WHITE);
    tft.fillCircle(10, 10, 8, TFT_WHITE);
    tft.drawTriangle(0, 0, 10, 10, 0, 10, TFT_WHITE);
    tft.fillTriangle(0, 0, 10, 10, 0, 10, TFT_WHITE);
    tft.drawEllipse(10, 10, 8, 4, TFT_WHITE);
    tft.fillEllipse(10, 10, 8, 4, TFT_WHITE);
    tft.drawArc(10, 10, 9, 5, 0, 90, TFT_WHITE, TFT_BLACK);
    tft.drawArc(10, 10, 9, 5, 0, 90, TFT_WHITE, TFT_BLACK, false);
    tft.drawWideLine(0, 0, 20, 20, 2.0f, TFT_WHITE);
    tft.drawWideLine(0, 0, 20, 20, 2.0f, TFT_WHITE, TFT_BLACK);

    // --- bitmaps and images ------------------------------------------------
    tft.drawXBitmap(0, 0, kXBitmap, 8, 8, TFT_WHITE);
    tft.drawXBitmap(0, 0, kXBitmap, 8, 8, TFT_WHITE, TFT_BLACK);
    tft.pushImage(0, 0, 2, 2, kRgb565);
    tft.pushImage(0, 0, 2, 2, static_cast<const uint16_t *>(kRgb565));
    tft.pushImage(0, 0, 2, 2, kIndexed, true, kPalette);
    tft.pushImage(0, 0, 2, 2, static_cast<const uint8_t *>(kIndexed), true, kPalette);

    // --- panel state -------------------------------------------------------
    tft.invertDisplay(true);
    tft.invertDisplay(false);
    tft.sleep(false);
    tft.setSwapBytes(true);
    tft.writecommand(0x00);

    // --- text --------------------------------------------------------------
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(1);
    tft.setTextWrap(true);
    tft.setTextWrap(true, false);
    tft.drawString("TestPad", 4, 4);
    tft.drawString("TestPad", 4, 4, 2);
    tft.drawCentreString("TestPad", 40, 4);
    tft.drawCentreString("TestPad", 40, 4, 2);
    tft.drawRightString("TestPad", 80, 4);
    tft.drawRightString("TestPad", 80, 4, 2);

    // --- Print-compatible output -------------------------------------------
    tft.write('x');
    tft.write(reinterpret_cast<const uint8_t *>("abc"), 3);
    tft.print("a C string");
    tft.print(String("an Arduino String"));
    tft.print(42);
    tft.println("a C string");
    tft.println(String("an Arduino String"));
    tft.println();
    tft.printf("%d %s\n", 1, "two");

    // --- read-back ---------------------------------------------------------
    const int16_t w = tft.width();
    const int16_t h = tft.height();
    const uint8_t rot = tft.getRotation();
    const uint32_t fg = tft.getTextColor();
    const uint32_t bg = tft.getTextBgColor();
    const uint8_t size = tft.getTextSize();
    const uint8_t datum = tft.getTextDatum();
    const bool swap = tft.getSwapBytes();
    const int16_t fh = tft.fontHeight();
    const int16_t fh2 = tft.fontHeight(2);
    const int16_t tw = tft.textWidth("TestPad");
    const int16_t tws = tft.textWidth(String("TestPad"));
    const int16_t twf = tft.textWidth("TestPad", 2);
    const int16_t cx = tft.getCursorX();
    const int16_t cy = tft.getCursorY();
    const uint16_t packed = tft.color565(255, 128, 0);
    SPIClass &spi = tft.getSPIinstance();

    Serial.printf(
        "display %dx%d rot=%u fg=%lu bg=%lu size=%u datum=%u swap=%d font=%d/%d "
        "text=%d/%d/%d cursor=%d,%d c565=%u spi=%p\n",
        w, h, rot, (unsigned long)fg, (unsigned long)bg, size, datum, (int)swap, fh, fh2, tw, tws,
        twf, cx, cy, packed, (void *)&spi
    );

    // --- flush -------------------------------------------------------------
    // A no-op on the LCD backends, the actual panel update on e-paper.
    tft.display();
    tft.display(true);
}

static void commonSpriteApi() {
    spr.setColorDepth(16);
    void *buffer = spr.createSprite(48, 24);
    Serial.printf("sprite buffer=%p %dx%d fh=%d\n", buffer, spr.width(), spr.height(),
                  spr.fontHeight());

    spr.fillScreen(TFT_BLACK);
    spr.setCursor(0, 0);
    spr.setTextColor(TFT_WHITE);
    spr.setTextColor(TFT_WHITE, TFT_BLACK);
    spr.setTextSize(1);
    spr.setTextDatum(TL_DATUM);

    spr.drawPixel(0, 0, TFT_RED);
    spr.drawLine(0, 0, 8, 8, TFT_RED);
    spr.drawFastVLine(2, 0, 8, TFT_RED);
    spr.drawRect(0, 0, 8, 8, TFT_RED);
    spr.fillRect(0, 0, 8, 8, TFT_RED);
    spr.drawRoundRect(0, 0, 8, 8, 2, TFT_RED);
    spr.fillRoundRect(0, 0, 8, 8, 2, TFT_RED);
    spr.drawCircle(4, 4, 3, TFT_RED);
    spr.fillCircle(4, 4, 3, TFT_RED);
    spr.fillEllipse(4, 4, 3, 2, TFT_RED);
    spr.fillTriangle(0, 0, 4, 4, 0, 4, TFT_RED);
    spr.fillRectHGradient(0, 0, 8, 8, TFT_RED, TFT_BLUE);
    spr.fillRectVGradient(0, 0, 8, 8, TFT_RED, TFT_BLUE);

    spr.drawXBitmap(0, 0, kXBitmap, 8, 8, TFT_WHITE);
    spr.drawXBitmap(0, 0, kXBitmap, 8, 8, TFT_WHITE, TFT_BLACK);
    spr.pushImage(0, 0, 2, 2, static_cast<const uint16_t *>(kRgb565));
    spr.pushImage(0, 0, 2, 2, kIndexed, true, kPalette);
    spr.pushImage(0, 0, 2, 2, static_cast<const uint8_t *>(kIndexed), true, kPalette);

    spr.drawString("spr", 0, 0);
    spr.drawString("spr", 0, 0, 2);

    spr.pushSprite(0, 0);
    spr.pushSprite(0, 0, TFT_BLACK);

    tft_sprite other(&tft);
    other.createSprite(48, 24);
    spr.pushToSprite(&other, 0, 0);
    spr.pushToSprite(&other, 0, 0, TFT_BLACK);
    other.deleteSprite();

    spr.deleteSprite();
}

// ---------------------------------------------------------------------------
// One backend only
// ---------------------------------------------------------------------------
static void backendApi() {
#if defined(USE_ARDUINO_GFX)
    // native() is what you draw on — the canvas itself when USE_CANVAS is set.
    // outputDriver() is always the panel behind it, and dataBus() the bus, both
    // typed concretely enough to reach a driver's own extras.
    Arduino_GFX *canvas = tft.native();
    Arduino_GFX *panel = tft.outputDriver();
    TFT_DATABUS_TYPE *bus = tft.dataBus();
    Serial.printf("arduino_gfx canvas=%p panel=%p bus=%p\n", (void *)canvas, (void *)panel,
                  (void *)bus);
    if (panel) panel->setRotation(0);

#elif defined(USE_LOVYANGFX)
    lgfx::LGFX_Device *dev = tft.native();
    Serial.printf("lovyangfx dev=%p\n", (void *)dev);
    if (dev) dev->setBrightness(128);

#elif defined(USE_M5GFX)
    lgfx::LGFX_Device *dev = tft.native();
    Serial.printf("m5gfx dev=%p\n", (void *)dev);
    if (dev) dev->setBrightness(128);

#elif defined(USE_TFT_ESPI)
    TFT_eSPI *dev = tft.native();
    TFT_eSprite *sprite = spr.nativeSprite();
    Serial.printf("tft_espi dev=%p sprite=%p\n", (void *)dev, (void *)sprite);

#elif defined(USE_EPD_PAINTER)
    // The panel is bit-banged and timing sensitive, so a task can be registered
    // to be suspended around every paint. A pointer, because the handle is
    // cleared at runtime when the task goes away.
    static TaskHandle_t guard = nullptr;
    tft.setPaintGuard(&guard);

    const EPD_Painter::Config &cfg = tft.getConfig();
    EPD_Painter *drv = tft.driver();
    EPD_PainterAdafruit *gfx = tft.native();
    Serial.printf("epd_painter drv=%p gfx=%p i2c=%p\n", (void *)drv, (void *)gfx,
                  (void *)cfg.i2c.wire);

#elif defined(USE_GXEPD2)
    GxEPD2_GFX *panel = tft.native();
    Serial.printf("gxepd2 panel=%p\n", (void *)panel);
    // Powers the controller down; the next draw wakes it.
    tft.hibernate();

#elif defined(USE_DUMMY_TFT)
    // There is no panel object to hand out, so this is always null. Present so
    // portable code that guards on it still compiles.
    void *none = tft.native();
    Serial.printf("dummy native=%p (expected null)\n", none);

#else
    #error "No backend selected: TestPad has nothing to exercise."
#endif
}

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("TestPad: exercising the DisplayDrivers API");

    commonDisplayApi();
    commonSpriteApi();
    backendApi();

    Serial.println("TestPad: done");
}

void loop() { delay(1000); }
