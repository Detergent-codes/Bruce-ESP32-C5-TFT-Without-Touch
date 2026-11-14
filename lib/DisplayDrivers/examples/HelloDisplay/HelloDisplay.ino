// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

// HelloDisplay — the smallest DisplayDrivers sketch.
//
// The backend is not chosen here: the same sketch runs on an LCD through
// Arduino_GFX, on an M5Stack through M5GFX or on an e-paper through GxEPD2.
//
// Configure it with build flags, or by copying DisplayDrivers_Setup.h.example
// from the library root into your project's include path. Defining the macros
// in this file would NOT work — the backends compile as separate translation
// units and never see them. See README.md, "Configuration".

#include <DisplayDrivers.h>

tft_display tft;

void setup() {
    tft.begin();
    tft.setRotation(1);

    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("DisplayDrivers", tft.width() / 2, tft.height() / 2);

    tft.drawRect(4, 4, tft.width() - 8, tft.height() - 8, TFT_DARKGREY);

    // No-op on the LCD backends. On e-paper (and with USE_CANVAS) this is what
    // actually puts the frame on the panel, so always call it.
    tft.display();
}

void loop() {
    static uint16_t hue = 0;
    const int16_t r = 12;

    // A dot walking across the screen, to show partial updates.
    const int16_t x = r + (millis() / 20) % (tft.width() - 2 * r);
    const int16_t y = tft.height() - 3 * r;

    tft.fillRect(0, y - r, tft.width(), 2 * r, TFT_BLACK);
    tft.fillCircle(x, y, r, tft.color565(hue, 255 - hue, 128));
    tft.display();

    hue += 4;
    delay(30);
}
