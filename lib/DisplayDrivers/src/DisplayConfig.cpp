// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

// Seeds displayConfig from the build macros.
//
// Including DisplayDrivers.h first pulls in the active backend's header, which
// is where each backend establishes its own defaults (LovyanGFX fills in
// TFT_OFFSET_*, GxEPD2 fills in GXEPD2_*, and so on). Whatever is still missing
// after that is a field the active backend does not use, and gets a neutral
// value from the block below — these fallbacks are local to this translation
// unit, so they cannot leak into a backend that does care.
#include "DisplayDrivers.h"

// clang-format off
// A backend that resolves a field itself gets the last word: gxepd2_hal.h, for
// instance, accepts a GXEPD2_CS override and otherwise falls back to TFT_CS,
// and it would be wrong to seed the struct from the raw macro behind its back.
// Aliases rather than a redefinition of TFT_*: gxepd2_hal.h defines GXEPD2_CS
// *as* TFT_CS when the board did not override it, so pointing TFT_CS back at
// GXEPD2_CS would be a cycle — the preprocessor would stop expanding and leave
// a bare identifier behind.
#if defined(USE_GXEPD2)
    #define DD_CS GXEPD2_CS
    #define DD_DC GXEPD2_DC
    #define DD_RST GXEPD2_RST
    #define DD_BUSY GXEPD2_BUSY
    // GXEPD2_PAGE_HEIGHT is a C++ constant expression (usually the panel
    // class's HEIGHT), not a preprocessor number, so it can only be used here
    // in the initializer itself.
    #define GXEPD2_PAGE_HEIGHT_VALUE GXEPD2_PAGE_HEIGHT
#endif

#ifndef TFT_DATABUS_N
    #define TFT_DATABUS_N 0
#endif
#ifndef TFT_DISPLAY_DRIVER_N
    #define TFT_DISPLAY_DRIVER_N 0
#endif
#ifndef TFT_WIDTH
    #define TFT_WIDTH 240
#endif
#ifndef TFT_HEIGHT
    #define TFT_HEIGHT 320
#endif
#ifndef TFT_MEM_WIDTH
    #define TFT_MEM_WIDTH TFT_WIDTH
#endif
#ifndef TFT_MEM_HEIGHT
    #define TFT_MEM_HEIGHT TFT_HEIGHT
#endif
#ifndef TFT_OFFSET_X
    #define TFT_OFFSET_X 0
#endif
#ifndef TFT_OFFSET_Y
    #define TFT_OFFSET_Y 0
#endif
#ifndef TFT_COL_OFS1
    #define TFT_COL_OFS1 0
#endif
#ifndef TFT_ROW_OFS1
    #define TFT_ROW_OFS1 0
#endif
#ifndef TFT_COL_OFS2
    #define TFT_COL_OFS2 0
#endif
#ifndef TFT_ROW_OFS2
    #define TFT_ROW_OFS2 0
#endif
#ifndef TFT_ROTATION
    #define TFT_ROTATION 0
#endif
#ifndef TFT_IPS
    #define TFT_IPS 0
#endif
#ifndef TFT_INVERTION
    #define TFT_INVERTION false
#endif
// Boards spell the colour order the way TFT_eSPI does -- -DTFT_RGB_ORDER=TFT_BGR
// -- whichever backend they end up on, but only TFT_eSPI defines those names.
// On any other backend the seed below would be a bare identifier, so give them
// TFT_eSPI's values, which are the ones lovyan.h documents too.
#ifndef TFT_BGR
    #define TFT_BGR 0
#endif
#ifndef TFT_RGB
    #define TFT_RGB 1
#endif
#ifndef TFT_RGB_ORDER
    #define TFT_RGB_ORDER 0
#endif
#ifndef TFT_CS
    #define TFT_CS -1
#endif
#ifndef TFT_DC
    #define TFT_DC -1
#endif
#ifndef TFT_RST
    #define TFT_RST -1
#endif
#if !defined(TFT_BUSY) && defined(TFT_BUSY_PIN)
    #define TFT_BUSY TFT_BUSY_PIN
#endif
#ifndef TFT_BUSY
    #define TFT_BUSY -1
#endif
#ifndef TFT_SCLK
    #define TFT_SCLK -1
#endif
#ifndef TFT_MOSI
    #define TFT_MOSI -1
#endif
#ifndef TFT_MISO
    #define TFT_MISO -1
#endif
#ifndef TFT_BL
    #define TFT_BL -1
#endif
#ifndef TFT_SPI_HOST
    #define TFT_SPI_HOST 0
#endif
#ifndef TFT_SPI_MODE
    #define TFT_SPI_MODE 0
#endif
#ifndef TFT_WRITE_FREQ
    #define TFT_WRITE_FREQ 40000000
#endif
#ifndef TFT_READ_FREQ
    #define TFT_READ_FREQ 16000000
#endif
#ifndef TFT_SPI_3WIRE
    #define TFT_SPI_3WIRE false
#endif
#ifndef TFT_USE_LOCK
    #define TFT_USE_LOCK true
#endif
#ifndef TFT_BUS_SHARED
    #define TFT_BUS_SHARED false
#endif
#ifndef TFT_D0
    #define TFT_D0 -1
#endif
#ifndef TFT_D1
    #define TFT_D1 -1
#endif
#ifndef TFT_D2
    #define TFT_D2 -1
#endif
#ifndef TFT_D3
    #define TFT_D3 -1
#endif
#ifndef TFT_D4
    #define TFT_D4 -1
#endif
#ifndef TFT_D5
    #define TFT_D5 -1
#endif
#ifndef TFT_D6
    #define TFT_D6 -1
#endif
#ifndef TFT_D7
    #define TFT_D7 -1
#endif
#ifndef TFT_WR
    #define TFT_WR -1
#endif
#ifndef TFT_RD
    #define TFT_RD -1
#endif
#ifndef TFT_I2C_PORT
    #define TFT_I2C_PORT 0
#endif
#ifndef TFT_SDA
    #define TFT_SDA -1
#endif
#ifndef TFT_SCL
    #define TFT_SCL -1
#endif
#ifndef TFT_ADDR
    #define TFT_ADDR 0x3C
#endif
#ifndef TFT_DE
    #define TFT_DE -1
#endif
#ifndef TFT_VSYNC
    #define TFT_VSYNC -1
#endif
#ifndef TFT_HSYNC
    #define TFT_HSYNC -1
#endif
#ifndef TFT_PCLK
    #define TFT_PCLK -1
#endif
#ifndef TFT_R0
    #define TFT_R0 -1
    #define TFT_R1 -1
    #define TFT_R2 -1
    #define TFT_R3 -1
    #define TFT_R4 -1
    #define TFT_G0 -1
    #define TFT_G1 -1
    #define TFT_G2 -1
    #define TFT_G3 -1
    #define TFT_G4 -1
    #define TFT_G5 -1
    #define TFT_B0 -1
    #define TFT_B1 -1
    #define TFT_B2 -1
    #define TFT_B3 -1
    #define TFT_B4 -1
#endif
#ifndef TFT_HSYNC_POL
    #define TFT_HSYNC_POL 0
#endif
#ifndef TFT_VSYNC_POL
    #define TFT_VSYNC_POL 0
#endif
#ifndef TFT_PCLK_ACTIVE_NEG
    #define TFT_PCLK_ACTIVE_NEG 0
#endif
#ifndef TFT_HSYNC_FRONT_PORCH
    #define TFT_HSYNC_FRONT_PORCH 0
#endif
#ifndef TFT_HSYNC_PULSE_WIDTH
    #define TFT_HSYNC_PULSE_WIDTH 0
#endif
#ifndef TFT_HSYNC_BACK_PORCH
    #define TFT_HSYNC_BACK_PORCH 0
#endif
#ifndef TFT_VSYNC_FRONT_PORCH
    #define TFT_VSYNC_FRONT_PORCH 0
#endif
#ifndef TFT_VSYNC_PULSE_WIDTH
    #define TFT_VSYNC_PULSE_WIDTH 0
#endif
#ifndef TFT_VSYNC_BACK_PORCH
    #define TFT_VSYNC_BACK_PORCH 0
#endif
#ifndef TFT_DSI_LANE_BIT_RATE
    #define TFT_DSI_LANE_BIT_RATE 0
#endif

#ifndef TFT_RGB_BOUNCE_BUF_PX
    #define TFT_RGB_BOUNCE_BUF_PX 0
#endif

#ifndef TFT_PREF_SPEED
    #define TFT_PREF_SPEED 0
#endif

// The panel's init table, named by the board as TFT_DSI_INIT or TFT_RGB_INIT.
// Both name an array Arduino_GFX ships in its display/ headers (or one the
// board defines itself), and the two buses count its length differently -- see
// DisplayConfig.h. Only the branch the board actually took is compiled, which
// is what keeps lcd_init_cmd_t out of the way on every non-P4 build.
#if defined(TFT_DSI_INIT)
    #define DD_INIT_OPS TFT_DSI_INIT
    #define DD_INIT_OPS_LEN (sizeof(TFT_DSI_INIT) / sizeof(lcd_init_cmd_t))
#elif defined(TFT_RGB_INIT)
    #define DD_INIT_OPS TFT_RGB_INIT
    #define DD_INIT_OPS_LEN sizeof(TFT_RGB_INIT)
#else
    #define DD_INIT_OPS nullptr
    #define DD_INIT_OPS_LEN 0
#endif
#ifndef GXEPD2_PAGE_HEIGHT_VALUE
    #define GXEPD2_PAGE_HEIGHT_VALUE 0
#endif
#ifndef GXEPD2_RESET_DURATION
    #define GXEPD2_RESET_DURATION 2
#endif
#ifndef GXEPD2_PULLDOWN_RST
    #define GXEPD2_PULLDOWN_RST false
#endif
#if defined(GXEPD2_BEGIN_SPI)
    #define DD_BEGIN_SPI true
#else
    #define DD_BEGIN_SPI false
#endif

// An I2C panel states its clock in its own macros, and they are three orders of
// magnitude away from the SPI defaults — seeding freqWrite from TFT_WRITE_FREQ
// there would ask a 400 kHz bus to run at 40 MHz.
#if defined(LOVYAN_I2C_BUS)
    #ifndef TFT_I2C_WRITE
        #define TFT_I2C_WRITE 400000
    #endif
    #ifndef TFT_I2C_READ
        #define TFT_I2C_READ 400000
    #endif
    #define DD_FREQ_WRITE TFT_I2C_WRITE
    #define DD_FREQ_READ TFT_I2C_READ
#endif
#ifndef DD_FREQ_WRITE
    #define DD_FREQ_WRITE TFT_WRITE_FREQ
#endif
#ifndef DD_FREQ_READ
    #define DD_FREQ_READ TFT_READ_FREQ
#endif

// Backends that did not claim these fall back to the generic pin macros.
#ifndef DD_CS
    #define DD_CS TFT_CS
#endif
#ifndef DD_DC
    #define DD_DC TFT_DC
#endif
#ifndef DD_RST
    #define DD_RST TFT_RST
#endif
#ifndef DD_BUSY
    #define DD_BUSY TFT_BUSY
#endif

// LovyanGFX states its bus as a type name, which the preprocessor cannot
// compare; lovyan.h has already turned that into one of these three flags.
#if defined(LOVYAN_I2C_BUS)
    #define DD_BUS_ID DISPLAY_BUS_LGFX_I2C
#elif defined(LOVYAN_8PARALLEL_BUS)
    #define DD_BUS_ID DISPLAY_BUS_LGFX_PAR8
#elif defined(LOVYAN_SPI_BUS)
    #define DD_BUS_ID DISPLAY_BUS_LGFX_SPI
#else
    #define DD_BUS_ID TFT_DATABUS_N
#endif
// clang-format on

DisplayConfig displayConfig = {
    /* bus        */ (uint8_t)DD_BUS_ID,
    /* driver     */ (uint16_t)TFT_DISPLAY_DRIVER_N,

    /* width      */ TFT_WIDTH,
    /* height     */ TFT_HEIGHT,
    /* memWidth   */ TFT_MEM_WIDTH,
    /* memHeight  */ TFT_MEM_HEIGHT,
    /* offsetX    */ TFT_OFFSET_X,
    /* offsetY    */ TFT_OFFSET_Y,
    /* colOfs1    */ TFT_COL_OFS1,
    /* rowOfs1    */ TFT_ROW_OFS1,
    /* colOfs2    */ TFT_COL_OFS2,
    /* rowOfs2    */ TFT_ROW_OFS2,
    /* rotation   */ (uint8_t)TFT_ROTATION,
    /* ips        */ (bool)TFT_IPS,
    /* invert     */ (bool)TFT_INVERTION,
    /* rgbOrder   */ (uint8_t)TFT_RGB_ORDER,

    /* cs         */ DD_CS,
    /* dc         */ DD_DC,
    /* rst        */ DD_RST,
    /* busy       */ DD_BUSY,
    /* sclk       */ TFT_SCLK,
    /* mosi       */ TFT_MOSI,
    /* miso       */ TFT_MISO,
    /* bl         */ TFT_BL,

    /* spiHost    */ (int8_t)TFT_SPI_HOST,
    /* spiMode    */ (uint8_t)TFT_SPI_MODE,
    /* freqWrite  */ (uint32_t)DD_FREQ_WRITE,
    /* freqRead   */ (uint32_t)DD_FREQ_READ,
    /* spi3wire   */ (bool)TFT_SPI_3WIRE,
    /* useLock    */ (bool)TFT_USE_LOCK,
    /* busShared  */ (bool)TFT_BUS_SHARED,

    /* d[8]       */ {TFT_D0, TFT_D1, TFT_D2, TFT_D3, TFT_D4, TFT_D5, TFT_D6, TFT_D7},
    /* wr         */ TFT_WR,
    /* rd         */ TFT_RD,

    /* i2cPort    */ (int8_t)TFT_I2C_PORT,
    /* sda        */ TFT_SDA,
    /* scl        */ TFT_SCL,
    /* i2cAddr    */ (uint8_t)TFT_ADDR,

    /* de         */ TFT_DE,
    /* vsync      */ TFT_VSYNC,
    /* hsync      */ TFT_HSYNC,
    /* pclk       */ TFT_PCLK,
    /* r[5]       */ {TFT_R0, TFT_R1, TFT_R2, TFT_R3, TFT_R4},
    /* g[6]       */ {TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5},
    /* b[5]       */ {TFT_B0, TFT_B1, TFT_B2, TFT_B3, TFT_B4},
    /* hsyncPol   */ (bool)TFT_HSYNC_POL,
    /* vsyncPol   */ (bool)TFT_VSYNC_POL,
    /* pclkNeg    */ (bool)TFT_PCLK_ACTIVE_NEG,
    /* hFront     */ (uint16_t)TFT_HSYNC_FRONT_PORCH,
    /* hPulse     */ (uint16_t)TFT_HSYNC_PULSE_WIDTH,
    /* hBack      */ (uint16_t)TFT_HSYNC_BACK_PORCH,
    /* vFront     */ (uint16_t)TFT_VSYNC_FRONT_PORCH,
    /* vPulse     */ (uint16_t)TFT_VSYNC_PULSE_WIDTH,
    /* vBack      */ (uint16_t)TFT_VSYNC_BACK_PORCH,
    /* prefSpeed  */ (uint32_t)TFT_PREF_SPEED,
    /* dsiLaneBitRate */ (uint32_t)TFT_DSI_LANE_BIT_RATE,
    /* rgbBounceBufPx */ (uint32_t)TFT_RGB_BOUNCE_BUF_PX,
    /* initOps    */ DD_INIT_OPS,
    /* initOpsLen */ (uint32_t)DD_INIT_OPS_LEN,

    /* pageHeight    */ (uint16_t)GXEPD2_PAGE_HEIGHT_VALUE,
    /* resetDuration */ (uint16_t)GXEPD2_RESET_DURATION,
    /* pulldownRst   */ (bool)GXEPD2_PULLDOWN_RST,
    /* beginSpi      */ DD_BEGIN_SPI,
};
