#ifdef HAS_TOUCH
#include "CYD28_TouchscreenR.h"
#endif
#include "core/bus_HAL.h"
#include "core/powerSave.h"
#include "core/utils.h"
#include "hal/bright/bright.h"
#include "hal/device.h"
#include "hal/inputs/buttons.h"
#include "hal/inputs/touch.h"
#include <Wire.h>
#include <globals.h>
#include <interface.h>

#ifdef HAS_3_BUTTONS
#ifndef UP_BTN
#define UP_BTN 0
#endif
#ifndef SEL_BTN
#define SEL_BTN 28
#endif
#ifndef DW_BTN
#define DW_BTN 1
#endif
#endif

#ifdef HAS_3_BUTTONS
static DeviceButtons buttonsCfg() { return DeviceButtons{UP_BTN, DW_BTN, SEL_BTN}; }
#endif

#ifdef HAS_TOUCH
extern CYD28_TouchR touch; // defined by hal/inputs/touch.cpp
static DeviceTouch touchCfg() {
    DeviceTouch cfg;
    // The XPT2046 reports landscape (rotation 1) coordinates
    // rotation:        0      1      2      3
    const bool swapXY[4] = {true, false, true, false};
    const bool mirrorX[4] = {true, false, false, true};
    const bool mirrorY[4] = {false, false, true, true};
    for (int i = 0; i < 4; i++) {
        cfg.SwapXY[i] = swapXY[i];
        cfg.MirrorX[i] = mirrorX[i];
        cfg.MirrorY[i] = mirrorY[i];
    }
    return cfg;
}
#endif

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
#ifdef SYS_I2C_SDA
    bruceConfigPins.i2c_bus = {(gpio_num_t)SYS_I2C_SDA, (gpio_num_t)SYS_I2C_SCL}; // sda, scl (System/Grove/RTC)
    bruceConfigPins.sys_i2c = {(gpio_num_t)SYS_I2C_SDA, (gpio_num_t)SYS_I2C_SCL};
    pinMode(SYS_I2C_SDA, INPUT_PULLUP);
    pinMode(SYS_I2C_SCL, INPUT_PULLUP);
    Wire.begin(SYS_I2C_SDA, SYS_I2C_SCL);
    setSysI2CBus(&Wire);
#if defined(HAS_RTC)
    _rtc.setWire(&Wire);
#endif
#else
    bruceConfigPins.i2c_bus = {(gpio_num_t)8, (gpio_num_t)9};
#endif
    bruceConfigPins.rfTx = 4;
    bruceConfigPins.rfRx = 5;
    bruceConfigPins.irTx = 3;
    bruceConfigPins.irRx = 26;
    bruceConfigPins.rotation = 1;
    bruceConfigPins.uart_bus = {(gpio_num_t)12, (gpio_num_t)11}; // rx, tx
    bruceConfigPins.gps_bus = {(gpio_num_t)4, (gpio_num_t)5};    // rx, tx
    bruceConfigPins.badusb_bus = {(gpio_num_t)4, (gpio_num_t)5}; // rx, tx (CH9329)
    // Board's default/generic SPI bus (keep CS unassigned by default so GPIO 8/9 remain free for I2C/RTC)
    bruceConfigPins.outer_bus = {(gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, GPIO_NUM_NC};
    bruceConfigPins.PN532_bus = {(gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, GPIO_NUM_NC};
    // CC1101/NRF24/SDCARD/W5500 share the same SPI bus (sck=6, miso=2, mosi=7)
    bruceConfigPins.CC1101_bus = {
        (gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,gdo0,gdo2
    bruceConfigPins.NRF24_bus = {
        (gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, GPIO_NUM_NC, GPIO_NUM_NC
    }; // sck,miso,mosi,cs(ss),ce
    bruceConfigPins.SDCARD_bus = {
        (gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, (gpio_num_t)10
    }; // sck,miso,mosi,cs
#if !defined(LITE_VERSION)
    bruceConfigPins.W5500_bus = {
        (gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,int,rst
#endif

    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    pinMode(TFT_MOSI, OUTPUT);
    digitalWrite(TFT_MOSI, HIGH);
    pinMode(TFT_SCLK, OUTPUT);

#if defined(TFT_RST) && (TFT_RST >= 0)
    pinMode(TFT_RST, OUTPUT);
#endif
    pinMode(TFT_DC, OUTPUT);
    digitalWrite(TFT_DC, HIGH);

#ifdef HAS_3_BUTTONS
    hal_buttons_init(buttonsCfg(), 3);
#endif
    const gpio_num_t csPins[] = {
        bruceConfigPins.NRF24_bus.cs,
        bruceConfigPins.CC1101_bus.cs,
        bruceConfigPins.SDCARD_bus.cs,
#if !defined(LITE_VERSION)
        bruceConfigPins.W5500_bus.cs,
#endif
        (gpio_num_t)TFT_CS,
    };
    for (gpio_num_t pin : csPins) {
        if (pin >= 0) {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, HIGH);
        }
    }
#ifdef ILI9341_DRIVER
    bruceConfig.colorInverted = 0;
#endif
}
/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio() {
    hal_bright_attach(TFT_BL);
    hal_bright_set(TFT_BL, 100);
#ifdef HAS_TOUCH
    hal_touch_init(touchCfg(), 0, true); // shares the display SPI bus
    // calibration: loadTouchCalibration()/calibrateTouch() in main.cpp (NVS "touch_cal")
#endif
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) { hal_bright_set(TFT_BL, brightval); }

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = 0;
    if (millis() - tm < 200 && !LongPress) return;
    checkPowerSaveTime();
#ifdef HAS_TOUCH
    {
        BruceTouchPoint t;
        if (hal_touch_read(touchCfg(), t)) {
            tm = millis();
            if (!hal_touch_apply(t)) return;
        }
    }
#endif
#ifdef HAS_3_BUTTONS
    hal_buttons_poll_3(buttonsCfg());
#endif
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turnoff the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
