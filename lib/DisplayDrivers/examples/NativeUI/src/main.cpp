// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// NativeUI — DisplayDrivers on the desktop.
//
// Runs on PlatformIO's `platform = native` (an SDL2 window, no ESP32 involved)
// so screen-drawing code can be iterated on without flashing hardware. Arrow
// keys emulate a 5-button nav pad (Up/Down/Left/Right/Sel/Esc); a-z, 0-9,
// space, enter, backspace and esc are scanned in as a HAS_KEYBOARD-style
// emulation. The globals below are named the way a board's main.cpp names
// them (PrevPress, NextPress, KeyStroke, ...) so drawing code written against
// those names drops in with no changes.
//
// Screen size is set in setup_gpio(), the same point a board's _setup_gpio()
// would retarget it — edit tftWidth/tftHeight there to try any panel size.
//
// Replace drawUI() with whatever screen you are actually testing.

#include <DisplayDrivers.h>
#include <lgfx/v1/platforms/sdl/Panel_sdl.hpp>

#include <cstdlib>

// ---------------------------------------------------------------------------
// Navigation state, named the way a board's main.cpp names them.
// ---------------------------------------------------------------------------
volatile bool PrevPress = false;
volatile bool NextPress = false;
volatile bool UpPress = false;
volatile bool DownPress = false;
volatile bool SelPress = false;
volatile bool EscPress = false;
volatile bool AnyKeyPress = false;

// A trimmed version of the on-device keyStroke: enough for text entry in a
// test screen (pressed/enter/del/word), not the HID/modifier bookkeeping a
// real keyboard driver needs.
struct keyStroke {
    bool pressed = false;
    bool enter = false;
    bool del = false;
    char word = 0;
    void Clear() {
        pressed = false;
        enter = false;
        del = false;
        word = 0;
    }
} KeyStroke;

tft_display tft;

// Screen size for this run. Change these — not TFT_WIDTH/TFT_HEIGHT, which
// only seed the initial displayConfig — and re-run to try another panel.
uint16_t tftWidth = 320;
uint16_t tftHeight = 240;

// ---------------------------------------------------------------------------
// GPIO map. Panel_sdl wires the arrow keys itself (36..39, active low);
// everything else here is mapped once at startup with addKeyCodeMapping().
// Mapping a letter/digit key steals it from Panel_sdl's own shortcuts (r/l
// rotate the window, 1..6 resize it) — expected, since this emulates a
// keyboard rather than a handful of nav buttons.
// ---------------------------------------------------------------------------
enum {
    GPIO_UP = 36,
    GPIO_RIGHT = 37,
    GPIO_DOWN = 38,
    GPIO_LEFT = 39,
    GPIO_SEL = 1,
    GPIO_ESC = 2,
    GPIO_DEL = 3,
};

struct KeyMapEntry {
    SDL_KeyCode key;
    uint8_t gpio;
    char ch;
};

// a-z -> 10..35, 0-9 -> 40..49, space -> 50.
static const KeyMapEntry kKeyMap[] = {
    {SDLK_a, 10, 'a'}, {SDLK_b, 11, 'b'}, {SDLK_c, 12, 'c'}, {SDLK_d, 13, 'd'}, {SDLK_e, 14, 'e'},
    {SDLK_f, 15, 'f'}, {SDLK_g, 16, 'g'}, {SDLK_h, 17, 'h'}, {SDLK_i, 18, 'i'}, {SDLK_j, 19, 'j'},
    {SDLK_k, 20, 'k'}, {SDLK_l, 21, 'l'}, {SDLK_m, 22, 'm'}, {SDLK_n, 23, 'n'}, {SDLK_o, 24, 'o'},
    {SDLK_p, 25, 'p'}, {SDLK_q, 26, 'q'}, {SDLK_r, 27, 'r'}, {SDLK_s, 28, 's'}, {SDLK_t, 29, 't'},
    {SDLK_u, 30, 'u'}, {SDLK_v, 31, 'v'}, {SDLK_w, 32, 'w'}, {SDLK_x, 33, 'x'}, {SDLK_y, 34, 'y'},
    {SDLK_z, 35, 'z'}, {SDLK_0, 40, '0'}, {SDLK_1, 41, '1'}, {SDLK_2, 42, '2'}, {SDLK_3, 43, '3'},
    {SDLK_4, 44, '4'}, {SDLK_5, 45, '5'}, {SDLK_6, 46, '6'}, {SDLK_7, 47, '7'}, {SDLK_8, 48, '8'},
    {SDLK_9, 49, '9'}, {SDLK_SPACE, 50, ' '},
};
static constexpr size_t kKeyMapCount = sizeof(kKeyMap) / sizeof(kKeyMap[0]);

static void mapKeys() {
    lgfx::Panel_sdl::addKeyCodeMapping(SDLK_RETURN, GPIO_SEL);
    lgfx::Panel_sdl::addKeyCodeMapping(SDLK_ESCAPE, GPIO_ESC);
    lgfx::Panel_sdl::addKeyCodeMapping(SDLK_BACKSPACE, GPIO_DEL);
    for (const auto &k : kKeyMap) lgfx::Panel_sdl::addKeyCodeMapping(k.key, k.gpio);
}

// gpio_in() is active-low, like a real button on INPUT_PULLUP: true means
// released. wasDown carries the previous state so callers see one edge per
// physical press instead of one per polled frame.
static bool pressedEdge(uint8_t gpio, bool &wasDown) {
    bool down = !lgfx::gpio_in(gpio);
    bool edge = down && !wasDown;
    wasDown = down;
    return edge;
}

static void InputHandler() {
    static bool upDown = false, downDown = false, leftDown = false, rightDown = false, selDown = false,
                escDown = false, delDown = false;
    static bool letterDown[kKeyMapCount] = {};

    PrevPress = pressedEdge(GPIO_LEFT, leftDown);
    NextPress = pressedEdge(GPIO_RIGHT, rightDown);
    UpPress = pressedEdge(GPIO_UP, upDown);
    DownPress = pressedEdge(GPIO_DOWN, downDown);
    SelPress = pressedEdge(GPIO_SEL, selDown);
    EscPress = pressedEdge(GPIO_ESC, escDown);
    AnyKeyPress = PrevPress || NextPress || UpPress || DownPress || SelPress || EscPress;

    KeyStroke.Clear();
    if (pressedEdge(GPIO_DEL, delDown)) {
        KeyStroke.pressed = true;
        KeyStroke.del = true;
        AnyKeyPress = true;
    }
    if (SelPress) KeyStroke.enter = true;
    for (size_t i = 0; i < kKeyMapCount; ++i) {
        if (pressedEdge(kKeyMap[i].gpio, letterDown[i])) {
            KeyStroke.pressed = true;
            KeyStroke.word = kKeyMap[i].ch;
            AnyKeyPress = true;
        }
    }
}

// ---------------------------------------------------------------------------
// Sample screen — swap this out for the real thing you want to test.
// Up/Down move the cursor, Sel/Esc are logged, typed letters echo at the
// bottom so HAS_KEYBOARD wiring can be checked too.
// ---------------------------------------------------------------------------
static int cursor = 0;
static const char *kMenu[] = {"Wifi", "Bluetooth", "IR", "Sub-GHz", "Settings"};
static constexpr int kMenuCount = sizeof(kMenu) / sizeof(kMenu[0]);

static void drawUI() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.setCursor(4, 4);
    tft.printf("%dx%d", tft.width(), tft.height());

    const int rowH = 18;
    const int top = 20;
    for (int i = 0; i < kMenuCount; ++i) {
        int y = top + i * rowH;
        if (i == cursor) {
            tft.fillRect(0, y, tft.width(), rowH, TFT_BLUE);
            tft.setTextColor(TFT_WHITE, TFT_BLUE);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.drawString(kMenu[i], 8, y + rowH / 2 - 4);
    }

    if (KeyStroke.pressed) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        String label = KeyStroke.enter  ? String("key: enter")
                        : KeyStroke.del  ? String("key: del")
                        : KeyStroke.word ? String("key: ") + String(KeyStroke.word)
                                         : String("key: ?");
        tft.drawString(label, 8, tft.height() - 16);
    }
}

// ---------------------------------------------------------------------------
// setup()/loop(), the same shape as a board's main.cpp. setup_gpio() is
// where tftWidth/tftHeight take effect, mirroring _setup_gpio() on device.
// ---------------------------------------------------------------------------
static void setup_gpio() {
    displayConfig.width = tftWidth;
    displayConfig.height = tftHeight;
}

void setup() {
    mapKeys();
    setup_gpio();
    tft.begin();
    tft.setRotation(0);
    drawUI();
}

void loop() {
    InputHandler();
    if (EscPress) exit(0);
    if (UpPress && cursor > 0) --cursor;
    if (DownPress && cursor < kMenuCount - 1) ++cursor;
    if (AnyKeyPress) drawUI();
    lgfx::delay(10);
}

// ---------------------------------------------------------------------------
// Panel_sdl owns main(): it pumps the SDL event/render loop on this thread
// and runs setup()/loop() on a second one, closing both when the window does.
// ---------------------------------------------------------------------------
__attribute__((weak)) int user_func(bool *running) {
    setup();
    do {
        loop();
    } while (*running);
    return 0;
}

int main(int, char **) { return lgfx::Panel_sdl::main(user_func, 128); }
