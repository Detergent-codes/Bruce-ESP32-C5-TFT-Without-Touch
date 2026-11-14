// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

// Runtime display configuration.
//
// Backends used to read their pins, geometry and driver selection straight out
// of build macros at construction time, which meant a binary could drive
// exactly one panel. Several boards ship with more than one: the Xteink X3/X4
// changed panel controller mid-production, the T-Display P4 has variants that
// differ only in their init sequence, and the T-Embed and T-Embed CC1101 differ
// only in pinout. One binary that picks the right panel at boot is cheaper than
// one build per variant, and these devices have the flash to spare.
//
// So the macros no longer feed the constructor. They seed this struct, once, at
// static-init time; the backend reads the struct in begin(). Anything that runs
// before begin() — a board's _setup_gpio(), a probe of the panel's ID register
// — can overwrite it, and the display comes up as whatever the struct says.
//
// A board that has only ever had one panel does not have to do anything: its
// macros still describe it, and the seeded values are what they always were.
#ifndef DISPLAY_DRIVERS_CONFIG_H
#define DISPLAY_DRIVERS_CONFIG_H

#include <stdint.h>

// Bus families. The numbering of ARDUINO_GFX buses matches the TFT_DATABUS_N
// vocabulary the boards already use, so a board's macro and this enum agree.
enum DisplayBus : uint8_t {
    DISPLAY_BUS_HWSPI = 0,    // Arduino_HWSPI
    DISPLAY_BUS_QSPI = 1,     // Arduino_ESP32QSPI
    DISPLAY_BUS_PAR8Q = 2,    // Arduino_ESP32PAR8Q
    DISPLAY_BUS_RGB = 3,      // Arduino_ESP32RGBPanel
    DISPLAY_BUS_DSI = 4,      // Arduino_ESP32DSIPanel
    DISPLAY_BUS_PAR8 = 5,     // Arduino_ESP32PAR8
    // Own-bus QSPI, like RGB/DSI above: the panel and its bus are one class
    // (Arduino_ST77922_1L4), not an Arduino_DataBus, because this panel's
    // command phase is single-line while Arduino_ESP32QSPI (bus 1) hardcodes
    // every phase to 4 lines -- a real protocol mismatch, not a config gap.
    // See Arduino_ST77922_1L4.h.
    DISPLAY_BUS_QSPI_1L4 = 6,
    // LovyanGFX bus families, kept clear of the ArduinoGFX range.
    DISPLAY_BUS_LGFX_SPI = 16,
    DISPLAY_BUS_LGFX_I2C = 17,
    DISPLAY_BUS_LGFX_PAR8 = 18,
};

// Panel/driver identity. The meaning is backend-specific — for Arduino_GFX it
// is the TFT_DISPLAY_DRIVER_N number, for GxEPD2 and LovyanGFX it indexes that
// backend's own table of compiled-in panel classes. A backend only knows how
// to build the drivers it was compiled with; asking for one that was not linked
// leaves the display unbuilt rather than crashing.
struct DisplayConfig {
    // --- what to build ------------------------------------------------------
    uint8_t bus;      // DisplayBus
    uint16_t driver;  // backend-specific panel/driver id

    // --- geometry -----------------------------------------------------------
    int16_t width, height;         // visible panel size, unrotated
    int16_t memWidth, memHeight;   // controller RAM, when it is larger
    int16_t offsetX, offsetY;      // window offset into that RAM
    int16_t colOfs1, rowOfs1;      // Arduino_GFX per-rotation offsets
    int16_t colOfs2, rowOfs2;
    uint8_t rotation;              // mounting offset, not the runtime rotation
    bool ips;
    bool invert;
    uint8_t rgbOrder;              // 0 = RGB, 1 = BGR

    // --- pins shared by every bus -------------------------------------------
    int16_t cs, dc, rst, busy;
    int16_t sclk, mosi, miso;
    int16_t bl;                    // backlight, -1 when the board drives it

    // --- SPI ----------------------------------------------------------------
    int8_t spiHost;
    uint8_t spiMode;
    uint32_t freqWrite, freqRead;
    bool spi3wire, useLock, busShared;

    // --- QSPI (d[0..3]) and 8-bit parallel (d[0..7]) -------------------------
    int16_t d[8];
    int16_t wr, rd;

    // --- I2C ----------------------------------------------------------------
    int8_t i2cPort;
    int16_t sda, scl;
    uint8_t i2cAddr;

    // --- RGB / DSI timings --------------------------------------------------
    // Only read when bus is DISPLAY_BUS_RGB or DISPLAY_BUS_DSI.
    int16_t de, vsync, hsync, pclk;
    int16_t r[5], g[6], b[5];
    bool hsyncPol, vsyncPol, pclkActiveNeg;
    uint16_t hsyncFrontPorch, hsyncPulseWidth, hsyncBackPorch;
    uint16_t vsyncFrontPorch, vsyncPulseWidth, vsyncBackPorch;
    uint32_t prefSpeed;
    uint32_t dsiLaneBitRate; // Mbps per lane. 0 lets the backend use its own default.
    uint32_t rgbBounceBufPx; // RGB bus only. 0 disables it (Arduino_ESP32RGBPanel's own default).

    // Register dump the panel wants before it will show anything. RGB and DSI
    // panels are the two that need one, and they disagree on both the element
    // type and what the length counts, so this is deliberately untyped:
    //
    //   DSI  const lcd_init_cmd_t *   initOpsLen = number of commands
    //   RGB  const uint8_t *          initOpsLen = number of bytes
    //
    // A void pointer rather than either of those because this header is
    // compiled for every board, and lcd_init_cmd_t only exists on the ESP32-P4.
    // Retarget it with TFT_SET_DSI_INIT() / TFT_SET_RGB_INIT(), which get the
    // length unit right for you. Null leaves the driver on its own default.
    const void *initOps;
    uint32_t initOpsLen;

    // --- e-paper ------------------------------------------------------------
    // GxEPD2 drives its panel through a class chosen by `driver`; these are the
    // knobs that are not baked into that class.
    uint16_t pageHeight;   // 0 = the panel's full height (no paging)
    uint16_t resetDuration;
    bool pulldownRst;
    bool beginSpi;         // let the backend bring the SPI bus up itself
};

// Seeded from the build macros at static-init time. Change it before begin();
// after that the panel objects exist and the struct is only a record of what
// they were built from.
extern DisplayConfig displayConfig;

#endif // DISPLAY_DRIVERS_CONFIG_H
