#include "i2c_finder.h"
#include "bus_HAL.h"
#include "display.h"
#include "mykeyboard.h"
#include <globals.h>

#define FIRST_I2C_ADDRESS 0x01
#define LAST_I2C_ADDRESS 0x7F

void find_i2c_addresses() {
    drawMainBorderWithTitle("I2C Finder");
    padprintln("");

    int8_t sda = (int8_t)bruceConfigPins.i2c_bus.sda;
    int8_t scl = (int8_t)bruceConfigPins.i2c_bus.scl;
    padprintf("Pins: SDA=%d  SCL=%d\n\n", (int)sda, (int)scl);

    TwoWire *wire = acquireI2CBus();
    if (!wire) wire = &Wire;

    if (sda >= 0 && scl >= 0) {
        pinMode(sda, INPUT_PULLUP);
        pinMode(scl, INPUT_PULLUP);
        wire->begin(sda, scl);
    }
    wire->setTimeOut(50);

    padprintln("Checking I2C addresses ...\n");
    delay(200);

    padprint("Found: ");

    bool first_found = true;
    int found_count = 0;

    for (uint8_t i = FIRST_I2C_ADDRESS; i <= LAST_I2C_ADDRESS; i++) {
        wire->beginTransmission(i);
        if (wire->endTransmission() == 0) {
            if (!first_found) tft.print(", ");
            else first_found = false;
            tft.printf("0x%02X", i);
            found_count++;
        }
    }

    if (found_count == 0) {
        tft.print("None");
    }

    padprintln("\n\nPress SEL or ESC to exit");

    while (1) {
        if (check(EscPress) || check(SelPress)) {
            returnToMenu = true;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    releaseI2CBus();
}

uint8_t find_first_i2c_address() {
    TwoWire *wire = acquireI2CBus();
    if (!wire) wire = &Wire;
    wire->setTimeOut(50);
    uint8_t found = 0;
    for (uint8_t i = FIRST_I2C_ADDRESS; i <= LAST_I2C_ADDRESS; i++) {
        wire->beginTransmission(i);
        if (wire->endTransmission() == 0) {
            found = i;
            break;
        }
    }
    releaseI2CBus();
    return found;
}

bool check_i2c_address(uint8_t i2c_address) {
    TwoWire *wire = acquireI2CBus();
    if (!wire) wire = &Wire;
    wire->setTimeOut(50);
    wire->beginTransmission(i2c_address);
    int error = wire->endTransmission();
    releaseI2CBus();
    return (error == 0);
}
