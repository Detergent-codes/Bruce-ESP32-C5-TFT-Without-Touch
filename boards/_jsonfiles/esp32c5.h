#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

#define PIN_RGB_LED 27
// BUILTIN_LED can be used in new Arduino API digitalWrite() like in Blink.ino
static const uint8_t LED_BUILTIN = SOC_GPIO_PIN_COUNT + PIN_RGB_LED;
#define BUILTIN_LED LED_BUILTIN // backward compatibility
#define LED_BUILTIN LED_BUILTIN // allow testing #ifdef LED_BUILTIN
// RGB_BUILTIN and RGB_BRIGHTNESS can be used in new Arduino API rgbLedWrite()
#define RGB_BUILTIN (LED_BUILTIN + SOC_GPIO_PIN_COUNT)
#define RGB_BRIGHTNESS 64

// LP UART Pins are fixed on ESP32-C5
static const uint8_t LP_RX = 12;
static const uint8_t LP_TX = 11;

static const uint8_t USB_DM = 13;
static const uint8_t USB_DP = 14;

static const uint8_t A0 = 1;
static const uint8_t A1 = 2;
static const uint8_t A2 = 3;
static const uint8_t A3 = 4;
static const uint8_t A4 = 5;
static const uint8_t A5 = 6;

// UART
static const uint8_t TX = 11;
static const uint8_t RX = 12;

// I2C (System / DS3231 RTC / Grove default)
static const uint8_t SDA = 8;
static const uint8_t SCL = 9;
static const uint8_t LP_SDA = 4;
static const uint8_t LP_SCL = 5;
#define WIRE1_PIN_DEFINED
static const uint8_t SDA1 = 8;
static const uint8_t SCL1 = 9;
// SPI2 (FSPI) - shared by Display and SD card
static const uint8_t SS = 10; // SD card CS
static const uint8_t MOSI = 7;
static const uint8_t MISO = 2;
static const uint8_t SCK = 6;

#endif /* Pins_Arduino_h */
