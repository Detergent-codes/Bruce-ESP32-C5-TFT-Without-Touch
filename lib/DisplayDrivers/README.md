# DisplayDrivers

One display API — `tft_display` and `tft_sprite` — over six graphics libraries, plus a headless
backend for boards with no panel. Pick one with a `USE_*` build flag and describe the panel with
plain `TFT_*` macros; swapping graphics libraries becomes a build-flag change instead of a rewrite.

| Flag                | Backend        | Panels                                    |
| ------------------- | -------------- | ----------------------------------------- |
| `USE_TFT_ESPI`      | TFT_eSPI       | LCD (default when nothing else is set)    |
| `USE_ARDUINO_GFX`   | Arduino_GFX    | LCD / OLED / AMOLED / RGB / DSI           |
| `USE_LOVYANGFX`     | LovyanGFX      | LCD / OLED                                |
| `USE_M5GFX`         | M5Unified      | any M5Stack unit, auto-detected           |
| `USE_EPD_PAINTER`   | EPD Painter    | parallel e-paper (LilyGo T5, M5Paper S3)  |
| `USE_GXEPD2`        | GxEPD2         | SPI e-paper (LilyGo T-Deck Pro, …)        |
| `USE_DUMMY_TFT`     | none           | headless — every draw call is a no-op     |

Only one backend compiles per build, and only the graphics library it selects has to be installed.
Targets ESP32 (Arduino core or ESP-IDF with Arduino as a component).

```cpp
#include <DisplayDrivers.h>

tft_display tft;

void setup() {
    tft.begin();
    tft.fillScreen(TFT_BLACK);
    tft.drawString("hello", 10, 10);
    tft.display();   // no-op on LCDs, required on e-paper
}
```

## Installing

**PlatformIO** — add the library plus the backend you picked:

```ini
lib_deps =
	https://github.com/bmorcelli/DisplayDrivers.git
	; and one of:
	;moononournation/GFX Library for Arduino @ ^1.5.5
	;lovyan03/LovyanGFX @ ^1.2.7
	;m5stack/M5Unified @ ^0.2.11        + m5stack/M5GFX @ ^0.2.18
	;bodmer/TFT_eSPI @ ^2.5.43
	;https://github.com/tonywestonuk/EPD_Painter  + adafruit/Adafruit GFX Library @ ^1.12.1
	;zinggjm/GxEPD2 @ ^1.6.4                      + adafruit/Adafruit GFX Library @ ^1.12.1

build_flags =
	-DUSE_ARDUINO_GFX=1
	; … the TFT_* macros for your panel, see the backend section below

; For ESP32-S3, LovyanGFX 1.2.7 predates Arduino 3.3.4 — use the develop branch:
; https://github.com/lovyan03/LovyanGFX#develop
```

**Arduino IDE** — copy this folder into `libraries/` (or use *Sketch → Include Library → Add .ZIP*),
then install the backend library through the Library Manager. The IDE has no build-flag field, so
the `USE_*` and `TFT_*` macros have to go into a `boards.local.txt`; for anything beyond a quick try
PlatformIO or ESP-IDF is a lot less painful.

**ESP-IDF (Arduino as component)** — copy this folder into your project's `components/`, next to the
`arduino-esp32` component and the backend library. `CMakeLists.txt` picks up whichever of them are in
the build. Set the flags from your project's `CMakeLists.txt`:

```cmake
idf_build_set_property(COMPILE_OPTIONS "-DUSE_ARDUINO_GFX=1" APPEND)
```

## Configuration

The `USE_*` and `TFT_*` macros have to reach **every translation unit**, not just yours. This
library compiles each backend as its own `.cpp`, so macros defined in your sketch before the
`#include` are invisible to them:

```cpp
#define USE_LOVYANGFX 1        // ✗ does not work
#define LOVYAN_PANEL Panel_ILI9341
#include <DisplayDrivers.h>
```

The backends would fall back to the default backend, and you get either a missing-header error for
a library you never asked for, or a link error against the `tft_display` your sketch declared.

There are three ways that do work.

**A device profile** — the shortest, if your board is one of the ~57 already described in
`src/devices/`. Name it and everything else follows:

```ini
build_flags = -DDISPLAY_DEVICE_M5STACK_CARDPUTER
```

The profile carries the geometry, the pins and the offsets, and states them in whichever library's
vocabulary the backend you picked speaks — so the same flag drives that panel under Arduino_GFX,
LovyanGFX or TFT_eSPI. See [src/devices/README.md](src/devices/README.md) for the list and for how
to add your own board.

**Build flags** — for a panel with no profile yet:

```ini
; PlatformIO
build_flags = -DUSE_LOVYANGFX=1 -DLOVYAN_PANEL=Panel_ILI9341 -DTFT_CS=5 …
```

```cmake
# ESP-IDF
idf_build_set_property(COMPILE_OPTIONS "-DUSE_LOVYANGFX=1" APPEND)
```

**A setup header** — nicer once the list grows past a few macros, and the only option in the
Arduino IDE, which has nowhere to put build flags.

The library ships `src/DisplayDrivers_Setup.h`: open it, uncomment your graphics library and your
device, done. `DisplayDrivers.h` reads it before deciding anything. The catch is that it lives
inside the library and is lost every time you update it.

To keep your configuration in your own tree instead, write a **`DisplayDrivers_User_Setup.h`** and
put it anywhere on the include path. `DisplayDrivers_Setup.h` reads that first, so it is the place
for anything a device profile does not already set:

```ini
; PlatformIO — the header goes in include/, and this line is required:
; PlatformIO does not hand include/ to library builds on its own
build_flags = -I include
```

For ESP-IDF, put it in any component's public include dir. The name matters: a file of yours called
`DisplayDrivers_Setup.h` would be *shadowed* by the library's copy, which sits earlier on the
include path.

## Layout

```
DisplayDrivers/
├── CMakeLists.txt                 ESP-IDF component
├── library.json                   PlatformIO
├── library.properties             Arduino IDE
├── examples/HelloDisplay
└── src/
    ├── DisplayDrivers.h           the only public header
    ├── DisplayDrivers_Setup.h     pick a library and a device, see Configuration
    ├── devices/                   one profile per board, plus its own README
    └── backends/                  one file pair per graphics library
                                   (dummy.h is header-only: nothing to compile)
```

`src/backends/` is deliberately off the include path. Windows resolves `#include` case-insensitively,
so a header named `gxepd2.h` or `m5gfx.h` sitting next to the public one would shadow the backend
library's own `GxEPD2.h` / `M5GFX.h` and break it in confusing ways. Keep new backends in there, and
keep their names distinct from the library they wrap.

## Credits and licences

DisplayDrivers is a thin wrapper: it contains no code from the projects below and does not
redistribute them — each one has to be installed separately, and only the one your `USE_*` flag
selects ends up in your binary. All the display work is theirs.

| Project | Author | Licence |
| ------- | ------ | ------- |
| [Arduino_GFX](https://github.com/moononournation/Arduino_GFX) | Moon On Our Nation | BSD 2-Clause, © 2012 Adafruit Industries |
| [LovyanGFX](https://github.com/lovyan03/LovyanGFX) | lovyan03 | FreeBSD (BSD 2-Clause), © 2020 lovyan03 |
| [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) | Bodmer | FreeBSD (BSD 2-Clause), © 2020 Bodmer |
| [M5GFX](https://github.com/m5stack/M5GFX) | M5Stack | MIT, © 2021 M5Stack |
| [M5Unified](https://github.com/m5stack/M5Unified) | M5Stack | MIT, © 2021 M5Stack |
| [EPD Painter](https://github.com/tonywestonuk/EPD_Painter) | Tony Weston | Apache-2.0 |
| [GxEPD2](https://github.com/ZinggJM/GxEPD2) | Jean-Marc Zingg | **GPL-3.0** |
| [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) | Adafruit | BSD 2-Clause, © 2012 Adafruit Industries |

Adafruit GFX is what EPD Painter and GxEPD2 draw through, so those two backends need it as well.
LovyanGFX and TFT_eSPI both trace back to Adafruit_ILI9341, and their licence files carry the
original Adafruit notices.

## Licensing

DisplayDrivers is **MIT**, with one exception. Every file carries an SPDX header, and the full texts
are in `LICENSES/`.

`src/backends/gxepd2_hal.{h,cpp}` are **GPL-3.0-or-later**. They subclass `GxEPD2_BW`, so that file
pair is a derivative work of GxEPD2 no matter what the rest of the package says. A firmware built
with `-DUSE_GXEPD2` links GxEPD2 and is a derivative work too: **what you distribute has to be
GPL-3.0**, including offering the corresponding source.

MIT is compatible with the GPL in that direction, so nothing here conflicts — the obligation simply
follows the GxEPD2 backend. Build with any of the other five and none of it applies. If GPL-3.0 does
not suit your distribution, use a different backend or delete that file pair. This is a description
of the licences as published, not legal advice.

```
scanning tools: MIT AND GPL-3.0-or-later
```

## Flushing the framebuffer (`display()`)

Every backend exposes `void display(bool fullRefresh = false)`. On the LCD backends it does nothing,
so callers never need to know what they are driving. On the e-paper backends — and on Arduino_GFX
with `USE_CANVAS` — it is what actually pushes the framebuffer to the panel; nothing shows up until
you call it:

```cpp
tft.fillScreen(TFT_WHITE);
tft.drawString("hello", 10, 10);
tft.display();       // partial/fast refresh of what changed
tft.display(true);   // full (flashing) refresh, clears ghosting
```

## Panel macros

Backends read the same `TFT_*` names wherever the concept is shared, so moving a board between
them mostly means changing one flag:

| Macro                       | Meaning                                  |
| --------------------------- | ---------------------------------------- |
| `TFT_WIDTH` / `TFT_HEIGHT`  | panel size in pixels                     |
| `TFT_CS` `TFT_DC` `TFT_RST` | chip select, data/command, reset         |
| `TFT_SCLK` `TFT_MOSI` `TFT_MISO` | SPI pins                            |
| `TFT_D0` … `TFT_D7`         | parallel / quad-SPI data pins            |
| `TFT_WR` `TFT_RD`           | parallel write / read strobes            |
| `TFT_ROTATION`              | panel mounting rotation (0-3)            |
| `TFT_IPS`                   | IPS panel (inverted colours)             |
| `TFT_COL_OFS1` `TFT_ROW_OFS1` | window offset into panel RAM           |

The backend sections below list what each one additionally needs.

## LovyanGFX (USE_LOVYANGFX)

LovyanGFX here supports SPI, I2C, and Parallel 8-bit. You must define:
- The panel type (`LOVYAN_PANEL`)
- The bus type (`LOVYAN_BUS`) — the bus branch is derived from it, no second selector macro
- The panel and bus pins, using the same `TFT_*` names the rest of the project already uses

### Required panel macros (all buses)

```c
#define TFT_CS     5
#define TFT_RST    12
#define TFT_WIDTH  135
#define TFT_HEIGHT 240
```

Everything else has a default, so a board only declares what differs. The project-wide names are
read first; the LovyanGFX-specific names on the right are per-board overrides:

| Setting             | Taken from     | Override         | Default        |
| ------------------- | -------------- | ---------------- | -------------- |
| Window offset X / Y | `TFT_COL_OFS1` / `TFT_ROW_OFS1` | `TFT_OFFSET_X` / `TFT_OFFSET_Y` | `0` |
| Panel RAM size      | `TFT_WIDTH` / `TFT_HEIGHT`      | `TFT_MEM_WIDTH` / `TFT_MEM_HEIGHT` | visible size |
| Inversion           | `TFT_INVERTED` | `TFT_INVERTION`  | `0`            |
| Colour order        | —              | `TFT_RGB_ORDER`  | `0` (BGR)      |
| Busy pin            | —              | `TFT_BUSY_PIN`   | `-1`           |
| Mounting rotation   | `TFT_ROTATION` | —                | `0`            |
| Bus shared with SD… | `TFT_BUS_SHARED` | —              | `true`         |

`TFT_ROTATION` is the panel's mounting offset; `setRotation()` is applied on top of it.

### SPI bus (`LOVYAN_BUS Bus_SPI`)

```c
#define USE_LOVYANGFX 1
#define LOVYAN_PANEL Panel_ST7789
#define LOVYAN_BUS   Bus_SPI

// Required
#define TFT_SCLK 13
#define TFT_MOSI 15
#define TFT_DC   14

// Optional, shown with their defaults
#define TFT_MISO       -1
#define TFT_SPI_HOST   SPI2_HOST
#define TFT_SPI_MODE   0
#define TFT_WRITE_FREQ 40000000
#define TFT_READ_FREQ  16000000
#define TFT_SPI_3WIRE  true
#define TFT_USE_LOCK   true

// Panel macros (from the previous section)
#define TFT_CS     5
#define TFT_RST    12
#define TFT_WIDTH  135
#define TFT_HEIGHT 240
```

For a QSPI panel (e.g. `Panel_NV3031B`), define `TFT_D0` .. `TFT_D3` instead of
`TFT_MOSI`/`TFT_MISO`; `TFT_DC` is not needed. `Bus_SPI` switches to quad mode when all
four are set, and the panel is marked non-readable.

### I2C bus (`LOVYAN_BUS Bus_I2C`)

```c
#define USE_LOVYANGFX 1
#define LOVYAN_PANEL Panel_SSD1306
#define LOVYAN_BUS   Bus_I2C

// Required
#define TFT_SDA  21
#define TFT_SCL  22
#define TFT_ADDR 0x3C

// Optional, shown with their defaults
#define TFT_I2C_PORT  0
#define TFT_I2C_WRITE 400000
#define TFT_I2C_READ  400000

// Panel macros (from the previous section)
#define TFT_CS     -1
#define TFT_RST    -1
#define TFT_WIDTH  128
#define TFT_HEIGHT 64
```

### Parallel 8-bit bus (`LOVYAN_BUS Bus_Parallel8`)

```c
#define USE_LOVYANGFX 1
#define LOVYAN_PANEL Panel_ILI9341
#define LOVYAN_BUS   Bus_Parallel8

// Required
#define TFT_WR 4
#define TFT_DC 2
#define TFT_D0 12
#define TFT_D1 13
#define TFT_D2 26
#define TFT_D3 25
#define TFT_D4 17
#define TFT_D5 16
#define TFT_D6 27
#define TFT_D7 14

// Optional, shown with their defaults
#define TFT_RD         -1
#define TFT_WRITE_FREQ 16000000

// Panel macros (from the previous section)
#define TFT_CS     5
#define TFT_RST    33
#define TFT_WIDTH  240
#define TFT_HEIGHT 320
```

### Panel selection (LOVYAN_PANEL)

Valid values in this project include:
`Panel_ST7789`, `Panel_GC9A01`, `Panel_GDEW0154M09`, `Panel_HX8357B`, `Panel_HX8357D`, `Panel_ILI9163`,
`Panel_ILI9341`, `Panel_ILI9342`, `Panel_ILI9481`, `Panel_ILI9486`, `Panel_ILI9488`, `Panel_IT8951`,
`Panel_RA8875`, `Panel_SH1106`, `Panel_SH1107`, `Panel_SSD1306`, `Panel_SSD1327`, `Panel_SSD1331`,
`Panel_SSD1351`, `Panel_SSD1357`, `Panel_SSD1963`, `Panel_ST7735`, `Panel_ST7735S`, `Panel_ST7796`.

## Arduino_GFX (USE_ARDUINO_GFX)

Arduino_GFX uses two selections:
- `TFT_DATABUS_N` to select the bus type.
- `TFT_DISPLAY_DRIVER_N` to select the panel driver.

### Reaching past the common API

Two escape hatches, for the parts of a panel that no shared API can cover — panel brightness on an
AMOLED, a bus-level command:

| Call | Returns |
| ---- | ------- |
| `native()` | the `Arduino_GFX` you draw on. With `USE_CANVAS` that is the canvas, not the panel |
| `outputDriver()` | the panel driver itself. Same object as `native()` without a canvas. Cast it to `TFT_DISPLAY_DRIVER` — the macro is in scope — to reach things like `setBrightness()` |
| `dataBus()` | the bus, typed `TFT_DATABUS_TYPE`, so it is already the concrete class for RGB and DSI (`Arduino_ESP32DSIPanel::writeCommand()`, for one) |

```cpp
auto *panel = static_cast<TFT_DISPLAY_DRIVER *>(tft.outputDriver());
if (panel) panel->setBrightness(200);
```

### Data Bus selection (`TFT_DATABUS_N`)

`TFT_DATABUS_N` values:
- `0` = `Arduino_HWSPI` (shared SPI)
- `1` = `Arduino_ESP32QSPI` (quad SPI)
- `2` = `Arduino_ESP32PAR8Q` (parallel 8-bit, D0-D7 on the same GPIO bank)
- `3` = `Arduino_ESP32RGBPanel` (RGB panel)
- `4` = `Arduino_ESP32DSIPanel` (DSI panel, ESP32-P4)
- `5` = `Arduino_ESP32PAR8` (parallel 8-bit, D0-D7 spread over both GPIO banks)

Bus 0: `Arduino_HWSPI` (SPI)
```c
// Arduino_GFX: SPI
#define USE_ARDUINO_GFX 1
#define TFT_DATABUS_N 0
#define TFT_DC   14
#define TFT_CS   5
#define TFT_SCLK 13
#define TFT_MOSI 15
#define TFT_MISO -1
```

Bus 1: `Arduino_ESP32QSPI` (Quad SPI)
```c
// Arduino_GFX: QSPI
#define USE_ARDUINO_GFX 1
#define TFT_DATABUS_N 1
#define TFT_CS   5
#define TFT_SCLK 13
#define TFT_D0   15
#define TFT_D1   2
#define TFT_D2   4
#define TFT_D3   12
```

Bus 2: `Arduino_ESP32PAR8Q` (Parallel 8-bit)
```c
// Arduino_GFX: PAR8
#define USE_ARDUINO_GFX 1
#define TFT_DATABUS_N 2
#define TFT_DC  21
#define TFT_CS  22
#define TFT_WR  23
#define TFT_RD  -1
#define TFT_D0  12
#define TFT_D1  13
#define TFT_D2  26
#define TFT_D3  25
#define TFT_D4  17
#define TFT_D5  16
#define TFT_D6  27
#define TFT_D7  14
```

Bus 5: `Arduino_ESP32PAR8` (Parallel 8-bit, mixed GPIO banks)
```c
// Arduino_GFX: PAR8 (generic writer)
// Same pins as bus 2. Only pick this one when D0-D7 do not all fit in the same
// GPIO bank, because PAR8Q is faster whenever it can be used.
#define USE_ARDUINO_GFX 1
#define TFT_DATABUS_N 5
#define TFT_DC  21
#define TFT_CS  22
#define TFT_WR  23
#define TFT_RD  -1
#define TFT_D0  12
#define TFT_D1  13
#define TFT_D2  26
#define TFT_D3  25
#define TFT_D4  17
#define TFT_D5  16
#define TFT_D6  27
#define TFT_D7  14
```

Bus 3: `Arduino_ESP32RGBPanel` (RGB panel)
```c
// Arduino_GFX: RGB panel
#define USE_ARDUINO_GFX 1
#define TFT_DATABUS_N 3
#define TFT_DE   40
#define TFT_VSYNC 41
#define TFT_HSYNC 39
#define TFT_PCLK  42
#define TFT_R0  45
#define TFT_R1  48
#define TFT_R2  47
#define TFT_R3  21
#define TFT_R4  14
#define TFT_G0  5
#define TFT_G1  6
#define TFT_G2  7
#define TFT_G3  15
#define TFT_G4  16
#define TFT_G5  4
#define TFT_B0  8
#define TFT_B1  3
#define TFT_B2  46
#define TFT_B3  9
#define TFT_B4  1
#define TFT_HSYNC_POL         0
#define TFT_HSYNC_FRONT_PORCH 10
#define TFT_HSYNC_PULSE_WIDTH 8
#define TFT_HSYNC_BACK_PORCH  50
#define TFT_VSYNC_POL         0
#define TFT_VSYNC_FRONT_PORCH 10
#define TFT_VSYNC_PULSE_WIDTH 8
#define TFT_VSYNC_BACK_PORCH  20
#define TFT_PCLK_ACTIVE_NEG   0
#define TFT_PREF_SPEED        16000000
```

Bus 4: `Arduino_ESP32DSIPanel` (DSI used in ESP32-P4)
```c
// Example of T-Display P4 TFT
#define TFT_DATABUS_N 4
#define TFT_HSYNC_PULSE_WIDTH 	28
#define TFT_HSYNC_BACK_PORCH	26
#define TFT_HSYNC_FRONT_PORCH	20
#define TFT_VSYNC_PULSE_WIDTH	2
#define TFT_VSYNC_BACK_PORCH	22
#define TFT_VSYNC_FRONT_PORCH	200
#define TFT_PREF_SPEED			60000000

#define TFT_DISPLAY_DRIVER_N 	50
#define TFT_WIDTH				540
#define TFT_HEIGHT				1168
#define TFT_RST					-1
#define TFT_DSI_INIT			hi8561_lcd_init
```
### Display driver selection (`TFT_DISPLAY_DRIVER_N`)

All drivers use `TFT_DISPLAY_DRIVER_N`. The grouping below matches the constructor signature used in
`lib/HAL/display/ardgfx.cpp`.

Common macros used by all non-RGB/DSI drivers:
```c
#define TFT_RST       12
#define TFT_ROTATION  3
#define TFT_IPS       0
#define TFT_WIDTH     135
#define TFT_HEIGHT    240
#define TFT_COL_OFS1  0
#define TFT_ROW_OFS1  0
#define TFT_COL_OFS2  0
#define TFT_ROW_OFS2  0
```

Group A: `[bus,rst,r,ips,w,h,ofs1,ofs2]` -> `TFT_DISPLAY_DRIVER_N` 0-22
Drivers: ST7735, ST7789, ST7796, ST77916, ILI9341, GC9A01, GC9C01, GC9D01, GC9106, GC9107,
HX8347C, HX8347D, HX8352C, HX8369A, NT35310, NT35510, NT39125, NV3007, NV3023, NV3041A, OTM8009A,
JBT6K71, AXS15231B.

Example (ST7789, SPI):
```c
#define TFT_DISPLAY_DRIVER_N 1
// plus the common macros above
```

Group B: `[bus,rst,r,ips]` -> `TFT_DISPLAY_DRIVER_N` 23-35
Drivers: ILI9331, ILI9342, ILI9481_18bit, ILI9486, ILI9486_18bit, ILI9488, ILI9488_18bit,
ILI9488_3bit, ILI9806, HX8357A, HX8357B, R61529, RM67162.

RM67162 (35) needs a different register set over plain SPI than over QSPI. The HAL handles it:
with `TFT_DATABUS_N 0` it passes `rm67162_spi_init_operations`, with `TFT_DATABUS_N 1` it keeps
the QSPI default. Nothing extra to define.

Example:
```c
#define TFT_DISPLAY_DRIVER_N 26
#define TFT_RST      12
#define TFT_ROTATION 1
#define TFT_IPS      0
```

Group C: `[bus,rst,r,w,h,ofs1,ofs2]` -> `TFT_DISPLAY_DRIVER_N` 36-43
Drivers: SSD1283A, SSD1331, SSD1351, SH8601, RM690B0, CO5300, JD9613, SEPS525.

Example:
```c
#define TFT_DISPLAY_DRIVER_N 38
#define TFT_RST      12
#define TFT_ROTATION 0
#define TFT_WIDTH    128
#define TFT_HEIGHT   128
#define TFT_COL_OFS1 0
#define TFT_ROW_OFS1 0
#define TFT_COL_OFS2 0
#define TFT_ROW_OFS2 0
```

Group D: `[bus,rst,r]` -> `TFT_DISPLAY_DRIVER_N` 44
Driver: ILI9225.

Example:
```c
#define TFT_DISPLAY_DRIVER_N 44
#define TFT_RST      12
#define TFT_ROTATION 0
```

Group E: `[bus,rst]` -> `TFT_DISPLAY_DRIVER_N` 45-46
Drivers: SPD2010, WEA2012.

Example:
```c
#define TFT_DISPLAY_DRIVER_N 46
#define TFT_RST 12
```

Group F: `TFT_DISPLAY_DRIVER_N` 47-48 — SSD1306, SH1106. **Not supported.**
In Arduino_GFX these derive from `Arduino_G`, not `Arduino_GFX`: they are output sinks with no
drawing API of their own and must be driven through an `Arduino_Canvas_Mono`, which takes no
rotation and uses `verticalByte` instead — a different shape from the `USE_CANVAS` path below.
Selecting one raises a `#error`. Use the LovyanGFX backend (`Panel_SSD1306` / `Panel_SH1106`)
for these panels.

Group G: `[RGB]` -> `TFT_DISPLAY_DRIVER_N` 49
Driver: Arduino_RGB_Display. Must be used with `TFT_DATABUS_N 3`; the panel doubles as the bus, so
both are built together and no `TFT_RST` / `TFT_ROTATION` / offsets are needed.

Group H: `[DSI]` -> `TFT_DISPLAY_DRIVER_N` 50
Driver: Arduino_DSI_Display. Needs `TFT_DATABUS_N 4` and `TFT_DSI_INIT`, naming one of the init
tables Arduino_GFX ships in `display/Arduino_DSI_Display.h` (`hi8561_init_operations`,
`rm69a10_amoled_init_operations`, `jd9365_init_operations`, …).

Group I: `[bus,rst,r,ips,w,h,ofs1,ofs2]` (same as group A) -> `TFT_DISPLAY_DRIVER_N` 51
Drivers shipped by this library in `src/panels/` rather than by Arduino_GFX: NV3031B (51, QSPI,
`TFT_DATABUS_N 1`, Seeed Wio Tracker L2).

### Init tables, and swapping them at runtime (groups G and H)

Groups G and H have no driver class of their own — the panel *is* the bus — so their register
sequence arrives as a table rather than being baked into a class. On those two buses the table,
the timings and the geometry are the *only* things separating one panel from another, which is why
`displayConfig` carries a pointer to it:

```c
const void *initOps;    // DSI: const lcd_init_cmd_t *   RGB: const uint8_t *
uint32_t    initOpsLen; // DSI: number of commands       RGB: number of bytes
```

`TFT_DSI_INIT` / `TFT_RGB_INIT` only seed it. Board code can aim it elsewhere before `begin()` —
from `_setup_gpio()`, after probing whichever panel this unit shipped with — using the macros that
get the length unit right:

```c
void _setup_gpio() {
    if (isAmoledVariant()) {
        TFT_SET_DSI_INIT(rm69a10_amoled_init_operations);
        displayConfig.width  = 568;
        displayConfig.height = 1232;
        displayConfig.hsyncPulseWidth = 50;   // ...and the rest of the timings
    }
}
```

The pointer is `const void *` because this struct is compiled for every board and
`lcd_init_cmd_t` only exists on the ESP32-P4.

RGB only: `Arduino_RGB_Display` sends its table over a separate SPI sideband, not over the RGB bus,
so a board with `TFT_RGB_INIT` must also give it that bus as an expression —
`-D TFT_RGB_INIT_BUS=new Arduino_SWSPI(-1,10,11,12,-1)`. Without it the table would be dropped
silently, so the build stops instead. DSI needs none of this: its table goes out over the DSI link.

### More than one panel in one binary

A board that shipped with different panels behind the same case can declare the alternates and
pick between them at boot instead of needing one build per variant:

```c
#define TFT_DISPLAY_DRIVER_N      1   // primary: ST7789
#define TFT_DISPLAY_DRIVER_N_ALT1 4   // also seen with an ILI9341
#define TFT_DISPLAY_DRIVER_N_ALT2 5   // ...and a GC9A01
```

`_ALT1` through `_ALT4`, numbered in order. Board code writes the id it detected into
`displayConfig.driver` before `begin()` — from `_setup_gpio()`, say, after reading the panel's ID
register — and `begin()` builds that one. Everything else the driver needs comes from
`displayConfig` too, so an alternate can differ in geometry and offsets as well as in controller.

The data bus is *not* selectable this way: `TFT_DATABUS_N` stays a build-time choice. Nor are
drivers 49 and 50, whose panel doubles as the bus.

Each id declared links its driver class in, roughly 1 kB of flash apiece. Declaring none — which
is every board that has only ever had one panel — costs 16 bytes.

### Canvas (`USE_CANVAS`)

Draw into a full-screen RGB565 framebuffer and push it to the panel in one go on `display()`.
Costs `width * height * 2` bytes of PSRAM and buys tear-free updates on panels that are slow or
noisy to draw on incrementally. Works with every bus and every driver group above except F.

```c
#define USE_CANVAS 1
```

Nothing else changes: the HAL builds the panel driver as usual, then puts an `Arduino_Canvas` in
front of it. `display()` flushes, `invertDisplay()` still goes to the panel register, and
`setRotation()` rotates the framebuffer.

`flush()` hands the driver a `TFT_WIDTH x TFT_HEIGHT` bitmap in panel-native orientation, so the
driver is built unrotated and the canvas owns the rotation. Panels whose controller expects the
rotated window instead — the T-Watch S3 Ultra is the known case — flip that around:

```c
#define TFT_CANVAS_ROTATE_OUTPUT 1  // driver keeps TFT_ROTATION, canvas is allocated pre-rotated
```

`TFT_CANVAS_ROTATION` overrides the rotation the canvas is created with; it defaults to
`TFT_ROTATION`, or to `0` on RGB and DSI panels, whose boards never define it.

## TFT_eSPI (USE_TFT_ESPI)

This backend uses the official TFT_eSPI configuration style. Use the macros from
`lib/TFT_eSPI/User_Setups` as your template. Copy one of those setup files and place the macros into
your `pins_arduino.h` or board `.ini` defines.

Example (based on a typical ST7789 User_Setup):
```c
// TFT_eSPI (example)
#define USE_TFT_ESPI 1
#define ST7789_2_DRIVER
#define TFT_WIDTH  135
#define TFT_HEIGHT 240
#define TFT_RGB_ORDER TFT_BGR
#define TFT_MOSI  15
#define TFT_SCLK  13
#define TFT_CS     5
#define TFT_DC    14
#define TFT_RST   12
#define TFT_BL    27
#define TFT_BACKLIGHT_ON 1
```

## M5Unified / M5GFX (USE_M5GFX)

No extra macros are required. The library auto-detects the hardware and configures the display
automatically, at the cost of additional flash usage.

Example:
```c
#define USE_M5GFX 1
```

`USE_CANVAS` works here too: everything is drawn into a full-screen `M5Canvas` and pushed to the
panel on `display()`. On the M5 e-paper units that is the difference between one panel refresh per
frame and one per primitive.

## EPD Painter (USE_EPD_PAINTER)

Parallel e-paper panels driven by [EPD_Painter](https://github.com/tonywestonuk/EPD_Painter). The
panel wiring does not come from `TFT_*` macros here: EPD Painter carries a *preset* per board
(pins, waveforms, I2C bus), selected in `EPD_Painter_presets.h`.

Add to your env:
```
lib_deps =
	adafruit/Adafruit GFX Library @ ^1.12.1
	https://github.com/tonywestonuk/EPD_Painter
```

Board selection — either pin the preset, or leave it out and let the library probe the I2C bus:
```c
#define USE_EPD_PAINTER 1

// Pick exactly one, or none at all to use the AUTO probe (the default):
// #define EPD_PAINTER_PRESET_LILYGO_T5_S3_H752
// #define EPD_PAINTER_PRESET_LILYGO_T5_S3_GPS
// #define EPD_PAINTER_PRESET_LILYGO_EPD47_H716
// #define EPD_PAINTER_PRESET_M5PAPER_S3
```

Optional tuning macros (all have defaults):
```c
#define EPD_PAINTER_PORTRAIT        1           // build the canvas rotated 90deg (default)
#define EPD_PAINTER_QUALITY         QUALITY_HIGH // or QUALITY_NORMAL / QUALITY_FAST
#define EPD_PAINTER_GREY_LEVELS     4           // 4 (default) or 16
#define EPD_PAINTER_DIRTY_TOLERANCE 10          // px tolerance when merging dirty rects
```

`TFT_WIDTH` / `TFT_HEIGHT` are still worth defining for the rest of the project, but the real
geometry comes from the preset and is read back from the panel after `begin()`.

### Colours

The Project draws in RGB565; the canvas is level-indexed (`0` = paper white, `LEVELS-1` = black).
The HAL converts by luminance, so `TFT_RED` lands on a mid grey instead of disappearing into white.

### Extra methods

```cpp
tft->setPaintGuard(&xHandle);   // task to suspend while the panel is bit-banged
tft->getConfig();               // EPD_Painter::Config — pins and the shared I2C bus
tft->driver();                  // EPD_Painter*, for anything the HAL does not wrap
tft->native();                  // EPD_PainterAdafruit*
```

`setPaintGuard()` takes the *address* of the handle, so clearing the handle at runtime is picked up.
Driving a parallel EPD is bit-banged and timing sensitive; without a guard a busy input task can
corrupt the frame.

## GxEPD2 (USE_GXEPD2)

SPI e-paper panels driven by [GxEPD2](https://github.com/ZinggJM/GxEPD2). Only the black/white
panels (`GxEPD2_BW`) are wired up.

Add to your env:
```
lib_deps =
	adafruit/Adafruit GFX Library @ ^1.12.1
	zinggjm/GxEPD2 @ ^1.6.4
```

Required macros:
```c
#define USE_GXEPD2 1
#define GXEPD2_PANEL GxEPD2_310_GDEQ031T10  // panel class, see the epd*/gde*/ folders of GxEPD2
#define TFT_CS   34
#define TFT_DC   35
#define TFT_BUSY 37
#define TFT_RST  -1     // optional, defaults to -1
#define TFT_WIDTH  240
#define TFT_HEIGHT 320
```

Optional macros:
```c
#define GXEPD2_PAGE_HEIGHT GXEPD2_PANEL::HEIGHT // rows kept in RAM; lower it to page a big panel
#define GXEPD2_DIAG_BAUD   115200               // serial diagnostics rate passed to init()
#define GXEPD2_INITIAL     true
#define GXEPD2_RESET_DURATION 2                 // ms; 2 for Waveshare "clever" reset circuits
#define GXEPD2_PULLDOWN_RST   false
#define GXEPD2_DITHER      1                    // 0 = plain 50% threshold instead of 4x4 Bayer

// Define this (plus TFT_SCLK / TFT_MOSI / TFT_MISO) to have the HAL call SPI.begin() itself.
// Leave it out when the bus is shared with the SD card and started elsewhere.
// #define GXEPD2_BEGIN_SPI 1
```

Each of `GXEPD2_CS`, `GXEPD2_DC`, `GXEPD2_BUSY` and `GXEPD2_RST` can be defined directly if the
`TFT_*` names are already taken by something else on your board.

### More than one panel in one binary

E-paper boards change controller mid-production more often than most — the Xteink X4 shipped as
SSD1677, then UC8179, then UC8279 behind the same glass and the same pinout. Declare the ones the
board can come with and pick at boot:

```c
#define GXEPD2_PANEL      GxEPD2_it103                 // index 0
#define GXEPD2_PANEL_ALT1 GxEPD2_426_GDEQ0426T82       // index 1
#define GXEPD2_PANEL_ALT2 GxEPD2_X3_792x528            // index 2
```

`_ALT1` through `_ALT4`, numbered in order. Board code writes the index into
`displayConfig.driver` before `begin()`; asking for one that was not declared leaves the display
unbuilt rather than driving the glass with the wrong controller. Pins come from `displayConfig`
either way, so an alternate can sit on a different pinout as well.

`GXEPD2_PAGE_HEIGHT` left alone gives each panel its own full height. Setting it pins every panel
to that many rows.

`native()` returns the shared `GxEPD2_GFX *` base rather than one panel's class; cast it with
`GXEPD2_PANEL_OF(GXEPD2_PANEL)` (or the `_ALTn` you selected) to reach anything panel-specific.

Each panel declared costs about 5.7 kB of flash — GxEPD2 puts its whole implementation in the
`GxEPD2_BW<>` template, so every panel gets its own copy. Declaring none costs nothing beyond the
2.7 kB the shared base class costs every GxEPD2 board. Panels that share one of this library's own
base classes (the Xteink group below) come in nearer 3.8 kB.

### Panels this library carries itself

Glass GxEPD2 has no class for, in `src/panels/`. They are ordinary GxEPD2 panel classes, so
`GXEPD2_PANEL` and the `_ALT`s name them exactly like a stock one.

| Class | Controller | Size | Where |
|---|---|---|---|
| `GxEPD2_X3_792x528` | UC8253 | 792x528 | Xteink X3, original batch |
| `GxEPD2_X3_792x528_UC8279` | UC8279d | 792x528 | Xteink X3, batches from July 2026 |
| `GxEPD2_X4_800x480_UC8179` | UC8179 | 800x480 | Xteink X4 / X4 Pro, newer batches |
| `GxEPD2_X4_800x480_UC8279` | UC8279 | 800x480 | Xteink X4 / X4 Pro, newer batches |

The X4's original batch is an SSD1677 and uses GxEPD2's stock `GxEPD2_426_GDEQ0426T82`.

All four are UltraChip parts driven in KW mode and the last three share `GxEPD2_Xteink_UC`, which
carries the GxEPD2 boilerplate and the deferred plane write; a subclass supplies only its register
bring-up and its refresh. Three of them differ enough that using the wrong one leaves the panel
dark rather than merely wrong: the UC8179 needs an explicit booster/VCOM bring-up the SSD1677 path
never sends, the UC8279 offsets its visible rows by 120 gates and writes no CDI, and the UC8279d
ships a blank MTP so the host has to supply the drive rails and upload the waveform banks itself.

Which controller a unit carries is not knowable at build time — it is read from the controller's
VER register (0x70) at boot. **None of the three new classes has been tested on hardware.** They
are transcriptions of the [FreeInk SDK](https://github.com/Free-Ink/freeink-sdk) drivers, which
are themselves marked pending validation.

### Colours

The panel has two levels. Fills and images are ordered-dithered (4x4 Bayer) by luminance, so the
projects's greys survive as patterns; text is snapped to the nearer level instead, because
dithering glyph pixels smears them.

### Refresh policy

`display()` does a fast partial update. It promotes itself to a full (flashing) refresh after
`fillScreen()` — repainting everything otherwise leaves the previous screen ghosted — and whenever
you ask for one with `display(true)`. `hibernate()` (or `sleep(true)`) powers the controller down;
the next draw wakes it.

## Headless (USE_DUMMY_TFT)

For firmware that also runs on boards with no panel at all — driven over serial or a web UI. Every
drawing call becomes nothing, so the display code compiles and runs unchanged instead of having to
be wrapped in `#ifdef`s.

```c
#define USE_DUMMY_TFT 1
```

That is the whole configuration. No graphics library is needed, none is pulled in, and the backend
is header-only and fully inline, so at `-Os` the calls disappear rather than turning into empty
function calls.

`USE_DUMMY_TFT` takes priority over the other backend flags — including `USE_CANVAS` — instead of
colliding with them: a board that declares it has no display has no display, whatever panel flags
the build inherited from a shared `.ini` section. `DisplayDrivers.h` clears them, and since every
backend `.cpp` includes it before testing its own flag, none of them compiles and no graphics
library has to be installed at all.

### What it still answers

Only the state a caller can read back is kept, so layout maths keeps working:

| Call | Answer |
| ---- | ------ |
| `width()` / `height()` | `TFT_WIDTH` / `TFT_HEIGHT`, swapped on odd rotations. Both default to 240x320 if the board declares no size |
| `getRotation()` `getCursorX/Y()` `getTextColor()` `getTextBgColor()` `getTextSize()` `getTextDatum()` `getSwapBytes()` | whatever was last set |
| `textWidth()` / `fontHeight()` | a 6x8 cell times the text size — override with `DUMMY_TFT_CHAR_WIDTH` / `DUMMY_TFT_CHAR_HEIGHT`. Never zero, so code that divides by the font height cannot trip over it |
| `color565()` | the real RGB565 packing |
| `drawString()` | the width the string would have taken |
| `write()` / `printf()` | the byte count, as `Print` would report it |
| `native()` | `nullptr` — there is no panel object |

`tft_sprite` remembers the geometry passed to `createSprite()`, so `width()` and `height()` answer
what was asked for, but allocates nothing and returns `nullptr` — the same thing a sprite whose
allocation failed returns. Callers that check it take their "no sprite" path, which draws nothing
either way here; callers that ignore it keep working, since every sprite call is a no-op too.
