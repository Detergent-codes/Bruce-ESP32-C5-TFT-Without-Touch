# NativeUI

DisplayDrivers, driven from an SDL2 window on the desktop instead of a panel.
Same `tft_display`/`tft_sprite` API every board uses, no ESP32 involved — good
for iterating on a screen's layout, or checking it at a size you don't own
hardware for, without a flash cycle each time.

This works through a new backend, `USE_NATIVE_SDL`, added alongside the
existing ones in `src/backends/`. It only builds under PlatformIO's
`platform = native` (a desktop g++/clang build): every other backend assumes
the Arduino core is present (`pins_arduino.h`, `WString.h`, `SPIClass`), which
`platform = native` has none of. This backend brings its own minimal,
source-compatible `String` and an empty `SPIClass` instead — enough for most
draw code to compile unmodified, but not a full Arduino-core replacement.

Verified end-to-end on WSL2 (Ubuntu): built, launched a window, rendered a
menu, and took real Up/Down/letter key input correctly.

## Prerequisites

PlatformIO's `native` platform needs a desktop toolchain and SDL2, neither of
which ships with PlatformIO itself.

**Linux / WSL** (verified):

```
sudo apt install -y build-essential libsdl2-dev
```

Nothing else — `#include <SDL2/SDL.h>` resolves from `/usr/include/SDL2`
without extra `-I`/`-L` flags once that package is in.

**Windows** — install [MSYS2](https://www.msys2.org/), then from the MSYS2
shell:

```
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gdb
```

Add `C:\msys64\mingw64\bin`, `C:\msys64\ucrt64\bin` and `C:\msys64\usr\bin` to
your `PATH`. Then grab an SDL2 "devel, mingw" release from
[libsdl-org/SDL/releases](https://github.com/libsdl-org/SDL/releases) and copy
its `include`, `lib`, `bin` and `share` folders into `C:\msys64\ucrt64`.
(Not verified by me — I don't have a Windows toolchain in my own environment.
If PlatformIO on native Windows still can't find SDL2 after this, WSL is the
more reliable path and this example builds there unchanged.)

**macOS** (not verified) — `xcode-select --install`, then `brew install sdl2`.
Use the `native_mac` / `native_mac_arm` envs in `platformio.ini`.

## Running it

```
pio run -e native -t upload
```

("Upload" is PlatformIO's generic run step on `platform = native` — there is
nothing to flash, it just launches the built program.)

## Controls

| Input                          | Effect                                                              |
|---------------------------------|----------------------------------------------------------------------|
| Arrow keys                      | `UpPress` / `DownPress` / `PrevPress` (left) / `NextPress` (right) |
| Enter                            | `SelPress`, and `KeyStroke.enter`                                  |
| Esc                              | `EscPress`, and quits the window                                   |
| Backspace                        | `KeyStroke.del`                                                    |
| a-z, 0-9, space                  | `KeyStroke.word`, for testing text entry                           |

These globals are named the same as a board's `main.cpp` (`PrevPress`,
`NextPress`, `KeyStroke`, ...), so navigation/keyboard-driven drawing code
written against those names should drop into `drawUI()` with no changes.
Mapping letters and digits as keys does take over SDL's own `r`/`l` (rotate
window) and `1`-`6` (resize window) shortcuts — expected, since this emulates
a keyboard rather than a handful of nav buttons.

## Testing your own screen

- **Screen size**: edit `tftWidth` / `tftHeight` in `src/main.cpp` and
  re-run. They are applied in `setup_gpio()`, the same point a board's
  `_setup_gpio()` would retarget `displayConfig` before `tft.begin()`.
  Verified working at both 320x240 and 480x320.
- **Drawing code**: replace the body of `drawUI()` with the screen you are
  actually testing. It only needs `tft` (a `tft_display`) and the input
  globals above — paste in menu/icon drawing code as-is and see what needs
  adjusting.
- **Sprites**: `tft_sprite spr(&tft);` works the same as on device.

## What this does not cover

This is a UI harness, not the Launcher firmware running on a PC: no WiFi, SD,
BLE, NVS, or the rest of `src/*.cpp`, which are all ESP-IDF/Arduino-only.
Draw and navigate here; test the real firmware behaviour on a board.
