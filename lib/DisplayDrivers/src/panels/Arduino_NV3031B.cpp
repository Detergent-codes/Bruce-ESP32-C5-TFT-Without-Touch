// SPDX-FileCopyrightText: 2026 bmorcelli
//
// SPDX-License-Identifier: MIT
//
// See Arduino_NV3031B.h for provenance.

#include "../DisplayDrivers.h"

#if defined(USE_ARDUINO_GFX)

#include "Arduino_NV3031B.h"

// clang-format off
static const uint8_t nv3031b_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_BYTES, 0xFD, 2, 0x06, 0x08,                         // unlock factory registers
    WRITE_C8_D8, 0x60, 0x0C,                                     // source timing
    WRITE_C8_BYTES, 0x61, 2, 0x07, 0x04,                         // gate timing
    WRITE_C8_D8, 0xB4, 0x01,                                     // 1-dot column inversion
    WRITE_C8_BYTES, 0xB1, 3, 0x0F, 0x02, 0x03,                   // frame rate
    WRITE_C8_BYTES, 0xB5, 4, 0x02, 0x02, 0x0A, 0x14,             // blanking porch
    WRITE_C8_BYTES, 0xB6, 5, 0x44, 0x01, 0x9F, 0x00, 0x02,       // display function
    WRITE_C8_D8, 0xDF, 0x11,                                     // bias/oscillator trim
    WRITE_C8_D8, 0x67, 0x21,                                     // bias current
    WRITE_C8_BYTES, 0x68, 4, 0x90, 0x4F, 0x27, 0x21,             // VCOM / power
    WRITE_C8_BYTES, 0xE1, 2, 0x20, 0x69,                         // gamma
    WRITE_C8_BYTES, 0xE4, 2, 0x69, 0x20,
    WRITE_C8_BYTES, 0xE2, 6, 0x10, 0x12, 0x12, 0x30, 0x39, 0x3F,
    WRITE_C8_BYTES, 0xE5, 6, 0x3F, 0x33, 0x2D, 0x12, 0x12, 0x10,
    WRITE_C8_BYTES, 0xE0, 8, 0x06, 0x06, 0x0B, 0x12, 0x11, 0x11, 0x0E, 0x19,
    WRITE_C8_BYTES, 0xE3, 8, 0x19, 0x13, 0x14, 0x14, 0x14, 0x12, 0x08, 0x05,
    WRITE_C8_BYTES, 0xE6, 2, 0x00, 0xFF,                         // AVDD/AVCL slope
    WRITE_C8_BYTES, 0xE7, 6, 0x01, 0x04, 0x03, 0x03, 0x00, 0x12, // source output / EQ
    WRITE_C8_BYTES, 0xE8, 3, 0x00, 0x70, 0x00,                   // source level
    WRITE_C8_D8, 0xEC, 0x54,                                     // gate EQ / charge pump
    WRITE_C8_BYTES, 0xFD, 2, 0xFA, 0xFC,                         // lock factory registers
    WRITE_C8_D8, 0x3A, 0x55,                                     // RGB565
    WRITE_COMMAND_8, NV3031B_SLPOUT,
    END_WRITE,
    DELAY, NV3031B_SLPOUT_DELAY,
    BEGIN_WRITE,
    WRITE_C8_D8, NV3031B_DISPON, 0x00, // NV303x wants a dummy parameter after DISPON
    END_WRITE,
};
// clang-format on

Arduino_NV3031B::Arduino_NV3031B(
    Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips, int16_t w, int16_t h, uint8_t col_offset1,
    uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2
)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2) {
    _override_datamode = SPI_MODE3; // what the vendor (LovyanGFX/Meshtastic) setup runs at
}

bool Arduino_NV3031B::begin(int32_t speed) {
    if (speed == GFX_NOT_DEFINED) speed = 40000000UL;
    return Arduino_TFT::begin(speed);
}

void Arduino_NV3031B::setRotation(uint8_t r) {
    Arduino_TFT::setRotation(r);
    switch (_rotation) {
        case 1: r = NV3031B_MADCTL_MY | NV3031B_MADCTL_MV | NV3031B_MADCTL_RGB; break;
        case 2: r = NV3031B_MADCTL_RGB; break;
        case 3: r = NV3031B_MADCTL_MX | NV3031B_MADCTL_MV | NV3031B_MADCTL_RGB; break;
        default: r = NV3031B_MADCTL_MX | NV3031B_MADCTL_MY | NV3031B_MADCTL_RGB; break;
    }
    _bus->beginWrite();
    _bus->writeC8D8(NV3031B_MADCTL, r);
    _bus->endWrite();
}

void Arduino_NV3031B::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) {
    if ((x != _currentX) || (w != _currentW)) {
        _currentX = x;
        _currentW = w;
        x += _xStart;
        _bus->writeC8D16D16Split(NV3031B_CASET, x, x + w - 1);
    }
    if ((y != _currentY) || (h != _currentH)) {
        _currentY = y;
        _currentH = h;
        y += _yStart;
        _bus->writeC8D16D16Split(NV3031B_RASET, y, y + h - 1);
    }
    _bus->writeCommand(NV3031B_RAMWR);
}

void Arduino_NV3031B::invertDisplay(bool i) {
    _inverted = i;
    _bus->sendCommand((_ips ^ i) ? NV3031B_INVON : NV3031B_INVOFF);
}

// SLPOUT alone leaves the panel one column shifted; a software reset plus the
// full init table is the only thing that brings it back clean (LovyanGFX's
// Panel_NV3031B::setSleep() does the same).
void Arduino_NV3031B::displayOn(void) {
    runInit();
    setRotation(_rotation);
    _currentX = _currentY = -1;
    _currentW = _currentH = 0;
}

void Arduino_NV3031B::displayOff(void) {
    _bus->sendCommand(NV3031B_SLPIN);
    delay(NV3031B_SLPIN_DELAY);
}

void Arduino_NV3031B::runInit() {
    if (_rst != GFX_NOT_DEFINED) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, HIGH);
        delay(10);
        digitalWrite(_rst, LOW);
        delay(10);
        digitalWrite(_rst, HIGH);
        delay(NV3031B_RST_DELAY);
    } else {
        _bus->sendCommand(NV3031B_SWRESET);
        delay(NV3031B_RST_DELAY);
    }
    _bus->batchOperation(nv3031b_init_operations, sizeof(nv3031b_init_operations));
    _bus->sendCommand((_ips ^ _inverted) ? NV3031B_INVON : NV3031B_INVOFF);
}

void Arduino_NV3031B::tftInit() { runInit(); }

#endif // USE_ARDUINO_GFX
