// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT

// One case of the Arduino_GFX driver factory.
//
// Included once per driver id the board declared -- TFT_DISPLAY_DRIVER_N, plus
// any TFT_DISPLAY_DRIVER_N_ALT1..ALT4. The includer sets DD_DRV_ID to the id;
// this file maps that number to the Arduino_GFX class and to the constructor
// shape that class takes, then emits the `case`. Deliberately not #pragma once:
// the whole point is to be expanded several times in one translation unit.
//
// Only the classes named by an id the board declared get linked in, so a board
// with a single panel pays one integer comparison and nothing else.
//
// The arguments come from displayConfig, never from the TFT_* macros: an
// alternate panel differs from the primary in geometry and offsets as often as
// it does in controller, and the macros only describe the primary. `cfg` and
// `drvRot` are the includer's local names for the struct and the rotation the
// driver itself should take (see USE_CANVAS in ardgfx.h).

#if !defined(DD_DRV_ID)
    #error "ardgfx_driver.inc requires DD_DRV_ID to be defined by the includer"
#endif

// clang-format off
#if DD_DRV_ID == 49
    #error "Arduino_RGB_Display (49) takes its RGB panel as the bus and is built next to it,\n \
    so it is not part of the runtime driver factory; selecting TFT_DATABUS_N=3 already\n \
    pins it. Do not name 49 in TFT_DISPLAY_DRIVER_N_ALT*."
#elif DD_DRV_ID == 50
    #error "Arduino_DSI_Display (50) needs the compile-time TFT_DSI_INIT table and is built\n \
    next to its bus, so it is not part of the runtime driver factory; selecting\n \
    TFT_DATABUS_N=4 already pins it. Do not name 50 in TFT_DISPLAY_DRIVER_N_ALT*."
#elif DD_DRV_ID == 52
    #error "Arduino_ST77922_1L4 (52) owns its ESP-IDF SPI bus and panel driver together, so it\n \
    is not part of the runtime driver factory; selecting TFT_DATABUS_N=6 already pins it.\n \
    Do not name 52 in TFT_DISPLAY_DRIVER_N_ALT*."
#endif

#include "ardgfx_driver_class.inc"

case DD_DRV_ID:
#if DD_DRV_ID == 35 && TFT_DATABUS_N == 0
    // The RM67162 boots with a different register set over plain SPI than over
    // QSPI; Arduino_GFX ships both tables and defaults to the QSPI one.
    _gfx = new DD_DRV_CLASS(
        bus, cfg.rst, drvRot, cfg.ips, rm67162_spi_init_operations, sizeof(rm67162_spi_init_operations)
    );
#elif DD_DRV_ID >= 23 && DD_DRV_ID <= 35
    _gfx = new DD_DRV_CLASS(bus, cfg.rst, drvRot, cfg.ips);
#elif DD_DRV_ID >= 36 && DD_DRV_ID <= 43
    _gfx = new DD_DRV_CLASS(
        bus, cfg.rst, drvRot, cfg.width, cfg.height, cfg.colOfs1, cfg.rowOfs1, cfg.colOfs2, cfg.rowOfs2
    );
#elif DD_DRV_ID == 44
    _gfx = new DD_DRV_CLASS(bus, cfg.rst, drvRot);
#elif DD_DRV_ID >= 45 && DD_DRV_ID <= 46
    _gfx = new DD_DRV_CLASS(bus, cfg.rst);
#else
    _gfx = new DD_DRV_CLASS(
        bus, cfg.rst, drvRot, cfg.ips, cfg.width, cfg.height, cfg.colOfs1, cfg.rowOfs1, cfg.colOfs2,
        cfg.rowOfs2
    );
#endif
    break;
// clang-format on

#undef DD_DRV_ID
