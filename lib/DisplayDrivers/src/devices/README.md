# Device profiles

A device profile is one header that already knows a board's panel: its size, its pins, its window
offsets, and what to call all of that in each graphics library. Instead of copying thirty `TFT_*`
macros into your build, you name the board.

```ini
build_flags = -DDISPLAY_DEVICE_M5STACK_CARDPUTER
```

That is the whole configuration. Arduino_GFX is used unless you ask for another library:

```ini
build_flags =
	-DDISPLAY_DEVICE_M5STACK_CARDPUTER
	-DUSE_LOVYANGFX=1
```

The same profile answers in whichever vocabulary the selected backend speaks — `TFT_DATABUS_N` /
`TFT_DISPLAY_DRIVER_N` for Arduino_GFX, `LOVYAN_PANEL` / `LOVYAN_BUS` for LovyanGFX, `ST7789_DRIVER`
and friends for TFT_eSPI. Only the branch for the backend you picked is compiled.

## Without build flags

The Arduino IDE has nowhere to put build flags, so `src/DisplayDrivers_Setup.h` can be edited in
place instead. Open it and uncomment two lines — the graphics library, and the device:

```c
// #define USE_LOVYANGFX 1
#define DISPLAY_DEVICE_M5STACK_CARDPUTER 1
```

If you would rather keep the library pristine, copy that file somewhere on your include path (a
PlatformIO project's `include/`, an ESP-IDF component's public include dir) under the same name —
`DisplayDrivers_Setup.h` — and edit the copy. `DisplayDrivers.h` pulls in the first one it finds.

Either way it has to reach **every translation unit**: the backends compile as their own `.cpp`
files and never see macros defined next to your sketch. That is the whole reason this file exists;
the README in the library root goes into it.

## Changing a profile

The macros are plain `#define`s, so a profile is the single source of truth for its board: passing
the same macro as a build flag on top of one is a redefinition, and the compiler will say so.

To move a pin, edit the profile — or, if the change is yours alone, copy it under a new name and
select that instead. Only the backend branches are conditional; the geometry, offsets and pins
above them always apply.

**Rotation is not one of them.** Every profile sets `TFT_ROTATION 0`, because which way round you
hold a board is not a property of its panel. Rotate at runtime instead:

```cpp
tft.setRotation(1);   // landscape
```

## Writing a new one

Copy the closest profile, rename the guard and the file, and fix the numbers. The parts to get
right, in order:

1. **Geometry** — `TFT_WIDTH`, `TFT_HEIGHT` are the panel's *native*, unrotated size, and
   `TFT_ROTATION` stays 0. `TFT_COL_OFS1` / `TFT_ROW_OFS1` are the window offset into panel RAM, which small ST7789s
   almost always need; get these wrong and the image is shifted by a few pixels.
2. **Pins** — the set depends on the bus. SPI wants CS/DC/RST/SCLK/MOSI/MISO, QSPI wants D0-D3,
   parallel wants D0-D7 plus WR/RD, RGB wants all twenty data lines and the sync timings.
3. **The backend branches** — the bus and driver numbers are tabulated in the library's README.
   Say `#error` in a branch whose library cannot drive the panel rather than leaving it to fail
   later with something unreadable.

Then add it to `src/DisplayDrivers_Setup.h`, in both lists: the commented `#define` and the
`#elif defined(...)` chain below it.

## What is here

Every board the [Launcher](https://github.com/bmorcelli/Launcher) builds for. Boards that share a
panel share a profile, so the last column is where you look for your own device name.

| Flag | Driver | Bus | Native size | Launcher envs |
| ---- | ------ | --- | ----------- | ------------- |
| `DISPLAY_DEVICE_ARDUINO_NESSO_N1` | M5GFX (auto) | — | 135x240 | arduino-nesso-n1 |
| `DISPLAY_DEVICE_MARAUDER_MINI` | ST7735 | SPI | 128x128 | Awok-Mini, Marauder-Mini |
| `DISPLAY_DEVICE_MARAUDER_V4_V6` | ILI9341 | SPI | 240x320 | Awok-Touch, Marauder-v4-OG, Marauder-v61, WaveSentry-R1 |
| `DISPLAY_DEVICE_CYD_2_USB` | ILI9341 | SPI | 240x320 | CYD-2-USB |
| `DISPLAY_DEVICE_CYD_2432S022C` | ST7789 | PAR8 | 240x320 | CYD-2432S022C, CYD-2432S022C-lovyan |
| `DISPLAY_DEVICE_CYD_2432S024R` | ILI9341 | SPI | 240x320 | CYD-2432S024R |
| `DISPLAY_DEVICE_CYD_2432S028` | ILI9341 | SPI | 240x320 | CYD-2432S028 |
| `DISPLAY_DEVICE_CYD_2432S032C` | ST7789 | SPI | 240x320 | CYD-2432S032C, CYD-2432S032R |
| `DISPLAY_DEVICE_CYD_2432W328C` | ILI9341 | SPI | 240x320 | CYD-2432W328C, CYD-2432W328C_2, CYD-2432W328R |
| `DISPLAY_DEVICE_CYD_3248S035C` | ST7796 | SPI | 320x480 | CYD-3248S035C, CYD-3248S035R |
| `DISPLAY_DEVICE_CYD_3248W535C` | AXS15231B | QSPI | 320x480 | CYD-3248W535C |
| `DISPLAY_DEVICE_CYD_4827S043R` | RGB | RGB16 | 480x272 | CYD-4827S043R |
| `DISPLAY_DEVICE_CYD_8048S043C` | RGB | RGB16 | 800x480 | CYD-8048S043C, CYD-8048W550C |
| `DISPLAY_DEVICE_ELECROW_24B` | ILI9341 | SPI | 240x320 | elecrow-24B, elecrow-28B |
| `DISPLAY_DEVICE_ELECROW_35B` | ILI9488 | SPI | 320x480 | elecrow-35B |
| `DISPLAY_DEVICE_ELECROW_35BV2_2` | ILI9488 | SPI | 320x480 | elecrow-35Bv2_2 |
| `DISPLAY_DEVICE_HEADLESS_ESP32_4MB` | none | — | — | headless-esp32-4mb, headless-esp32-8mb, headless-esp32s3-16mb, headless-esp32s3-4mb, headless-esp32s3-8mb |
| `DISPLAY_DEVICE_LILYGO_T_DECK` | ST7789 | SPI | 240x320 | lilygo-t-deck, lilygo-t-deck-plus |
| `DISPLAY_DEVICE_LILYGO_T_DECK_PRO` | GxEPD2 | — | 240x320 | lilygo-t-deck-pro |
| `DISPLAY_DEVICE_LILYGO_T_DISPLAY_C5` | ST7789 | SPI | 170x320 | lilygo-t-display-c5 |
| `DISPLAY_DEVICE_LILYGO_T_DISPLAY_P4` | DSI | DSI | 540x1168 | lilygo-t-display-p4 |
| `DISPLAY_DEVICE_LILYGO_T_DISPLAY_P4_AMOLED` | DSI | DSI | 568x1232 | lilygo-t-display-p4 (probed) |
| `DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_AMOLED` | RM67162 | QSPI | 240x536 | lilygo-t-display-S3-amoled |
| `DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_AMOLED_PLUS` | RM67162 | SPI | 240x536 | lilygo-t-display-S3-amoled-plus |
| `DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_PRO` | ST7796 | SPI | 222x480 | lilygo-t-display-S3-pro |
| `DISPLAY_DEVICE_LILYGO_T_DISPLAY_S3_TOUCH` | ST7789 | PAR8 | 170x320 | lilygo-t-display-S3-touch |
| `DISPLAY_DEVICE_LILYGO_T_DONGLE_C5_TFT` | ST7735 | SPI | 80x160 | lilygo-t-dongle-c5-tft |
| `DISPLAY_DEVICE_LILYGO_T_DONGLE_S3_TFT` | ST7735 | SPI | 80x160 | lilygo-t-dongle-s3-tft |
| `DISPLAY_DEVICE_LILYGO_T_EMBED` | ST7789 | SPI | 170x320 | lilygo-t-embed |
| `DISPLAY_DEVICE_LILYGO_T_EMBED_CC1101` | ST7789 | SPI | 170x320 | lilygo-t-embed-cc1101 |
| `DISPLAY_DEVICE_LILYGO_T_HMI` | ST7789 | PAR8 | 240x320 | lilygo-t-hmi |
| `DISPLAY_DEVICE_LILYGO_T_LORA_PAGER` | ST7796 | SPI | 222x480 | lilygo-t-lora-pager |
| `DISPLAY_DEVICE_LILYGO_T_WATCH_S3` | ST7789 | SPI | 240x240 | lilygo-t-watch-s3 |
| `DISPLAY_DEVICE_LILYGO_T_WATCH_ULTRA` | CO5300 | QSPI | 502x410 | lilygo-t-watch-ultra |
| `DISPLAY_DEVICE_LILYGO_T5_EPAPER_S3_PRO` | EPD Painter | — | 540x960 | lilygo-t5-epaper-s3-pro |
| `DISPLAY_DEVICE_M5STACK_C` | in fork | — | 80x160 | m5stack-c |
| `DISPLAY_DEVICE_M5STACK_CARDPUTER` | ST7789 | SPI | 135x240 | m5stack-cardputer |
| `DISPLAY_DEVICE_M5STACK_CORE` | ILI9342 | SPI | 320x240 | m5stack-core, m5stack-core-4Mb |
| `DISPLAY_DEVICE_M5STACK_CORE2` | ILI9342 | SPI | 320x240 | m5stack-core2 |
| `DISPLAY_DEVICE_M5STACK_CORES3` | M5GFX (auto) | — | 240x320 | m5stack-cores3 |
| `DISPLAY_DEVICE_M5STACK_CPLUS1_1` | ST7789 | SPI | 135x240 | m5stack-cplus1_1 |
| `DISPLAY_DEVICE_M5STACK_CPLUS2` | ST7789 | SPI | 135x240 | m5stack-cplus2 |
| `DISPLAY_DEVICE_M5STACK_DINMETER` | M5GFX (auto) | — | 135x240 | m5stack-dinmeter |
| `DISPLAY_DEVICE_M5STACK_PAPER` | M5GFX (auto) | — | 540x960 | m5stack-paper |
| `DISPLAY_DEVICE_M5STACK_PAPER_COLOR` | M5GFX (auto) | — | 400x600 | m5stack-paper-color |
| `DISPLAY_DEVICE_M5STACK_PAPER_S3` | M5GFX (auto) | — | 540x960 | m5stack-paper-s3 |
| `DISPLAY_DEVICE_M5STACK_STICKS3` | M5GFX (auto) | — | 135x240 | m5stack-sticks3 |
| `DISPLAY_DEVICE_M5STACK_TAB5` | M5GFX (auto) | — | 720x1280 | m5stack-tab5 |
| `DISPLAY_DEVICE_MARAUDER_V7` | ILI9341 | SPI | 240x320 | Marauder-v7 |
| `DISPLAY_DEVICE_MARAUDER_V8` | ILI9341 | SPI | 240x320 | Marauder-v8 |
| `DISPLAY_DEVICE_NM_CYD_C5` | ST7789 | SPI | 240x320 | NM-CYD-C5 |
| `DISPLAY_DEVICE_PANCAKE` | ST7796 | SPI | 320x480 | pancake |
| `DISPLAY_DEVICE_PHANTOM_S024R` | ILI9341 | SPI | 240x320 | Phantom_S024R |
| `DISPLAY_DEVICE_REAPER` | ST7789 | SPI | 170x320 | reaper |
| `DISPLAY_DEVICE_SMOOCHIEE_BOARD` | ST7789 | SPI | 170x320 | smoochiee-board |
| `DISPLAY_DEVICE_WAVESHARE_ESP32_S3_LCD_147` | ST7789 | SPI | 172x320 | waveshare-esp32-s3-lcd-147 |
| `DISPLAY_DEVICE_XUEERSI_XIAOMIAO` | ST7735 | SPI | 128x160 | xueersi-xiaomiao |

### A caveat on the translations

The Arduino_GFX column is the one every profile was built from, and it is the one the Launcher
ships. The LovyanGFX and TFT_eSPI branches are mechanical translations of the same panel, and two
things do not survive them:

- **Window offsets.** TFT_eSPI bakes them into its drivers instead of taking macros, so a panel
  needing `TFT_COL_OFS1` may come out shifted. The profiles set `CGRAM_OFFSET` and say so in a
  comment where it applies.
- **Buses nobody else has.** QSPI, RGB and DSI exist only in the Arduino_GFX backend here. Those
  profiles put an `#error` in the other branches rather than pretending.

Treat a branch as verified once it has run on the hardware, not because it compiles.
