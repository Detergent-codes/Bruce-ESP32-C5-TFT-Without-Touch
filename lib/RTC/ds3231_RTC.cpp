#include "ds3231_RTC.h"
#include "esp32-hal-i2c.h"
#include <esp_log.h>
#include <sys/time.h>

static const char *TAG = "DS3231";

#ifndef RTC_SDA
#ifdef SYS_I2C_SDA
#define RTC_SDA SYS_I2C_SDA
#else
#define RTC_SDA 8
#endif
#endif

#ifndef RTC_SCL
#ifdef SYS_I2C_SCL
#define RTC_SCL SYS_I2C_SCL
#else
#define RTC_SCL 9
#endif
#endif

static int8_t rtc_sda_pin = RTC_SDA;
static int8_t rtc_scl_pin = RTC_SCL;

void ds3231_RTC::setPins(int8_t sda, int8_t scl) {
    rtc_sda_pin = sda;
    rtc_scl_pin = scl;
}

static void ensureI2CBus(TwoWire *wr, bool forceReinit = false) {
    if (forceReinit) {
        wr->end();
    }
    if (!i2cIsInit(0)) {
        if (rtc_sda_pin >= 0 && rtc_scl_pin >= 0) {
            pinMode(rtc_sda_pin, INPUT_PULLUP);
            pinMode(rtc_scl_pin, INPUT_PULLUP);
            wr->begin(rtc_sda_pin, rtc_scl_pin);
        } else {
            wr->begin();
        }
    }
}

void ds3231_RTC::begin(void) {
    ensureI2CBus(wr, false);

    // Initialize Control (0x0E) and Status (0x0F) in one transaction:
    // 0x0E: !EOSC=0 (Oscillator running), BBSQW=0, CONV=0, RS2=0, RS1=0, INTCN=1, A2IE=0, A1IE=0 -> 0x04
    // 0x0F: Clear OSF (bit 7), clear alarms -> 0x00
    uint8_t err = 1;
    for (int retry = 0; retry < 3; retry++) {
        if (retry > 0) ensureI2CBus(wr, true);
        wr->beginTransmission(DS3231_ADDR);
        wr->write(DS3231_REG_CTRL);
        wr->write(0x04);
        wr->write(0x00);
        err = wr->endTransmission();
        if (err == 0) break;
        delay(10);
    }
    if (err != 0) {
        ESP_LOGE(TAG, "begin failed to write CTRL/STAT, err=%d", err);
    } else {
        ESP_LOGI(TAG, "Initialized, oscillator enabled");
    }
}

bool ds3231_RTC::setTimeDate(uint8_t sec, uint8_t min, uint8_t hr, uint8_t wday, uint8_t mday, uint8_t mon, uint16_t yr) {
    ensureI2CBus(wr, false);

    uint8_t err = 1;
    for (int retry = 0; retry < 3; retry++) {
        if (retry > 0) ensureI2CBus(wr, true);
        wr->beginTransmission(DS3231_ADDR);
        wr->write(DS3231_REG_SEC); // 0x00
        wr->write(ByteToBcd2(sec % 60));
        wr->write(ByteToBcd2(min % 60));
        wr->write(ByteToBcd2(hr % 24)); // 24-hr format (bit 6 is 0)
        wr->write(ByteToBcd2((wday == 0 ? 7 : wday) & 0x07));
        wr->write(ByteToBcd2((mday == 0 ? 1 : mday) & 0x3F));
        uint8_t m = ByteToBcd2((mon == 0 ? 1 : mon) & 0x1F);
        if (yr >= 2100) m |= 0x80;
        wr->write(m);
        wr->write(ByteToBcd2((uint8_t)(yr % 100)));
        err = wr->endTransmission();
        if (err == 0) break;
        delay(10);
    }

    if (err != 0) {
        ESP_LOGE(TAG, "setTimeDate I2C transmission failed, err=%d", err);
        return false;
    }

    // Ensure oscillator is running and clear stop flag
    for (int retry = 0; retry < 3; retry++) {
        wr->beginTransmission(DS3231_ADDR);
        wr->write(DS3231_REG_CTRL);
        wr->write(0x04);
        wr->write(0x00);
        if (wr->endTransmission() == 0) break;
        delay(10);
    }

    struct tm ti = {0};
    ti.tm_sec = sec % 60;
    ti.tm_min = min % 60;
    ti.tm_hour = hr % 24;
    ti.tm_wday = (wday == 7) ? 0 : wday;
    ti.tm_mday = mday;
    ti.tm_mon = (mon > 0) ? mon - 1 : 0;
    ti.tm_year = (yr >= 1900) ? yr - 1900 : 124;
    time_t epoch = mktime(&ti);
    struct timeval tv = {.tv_sec = epoch};
    settimeofday(&tv, nullptr);

    ESP_LOGI(TAG, "Time set: %02d:%02d:%02d Date: %04d-%02d-%02d (wday %d)", hr, min, sec, yr, mon, mday, wday);
    return true;
}

void ds3231_RTC::SetTime(RTC_TimeTypeDef *RTC_TimeStruct) {
    if (RTC_TimeStruct == nullptr) return;

    // Fetch current date so we preserve it when writing
    RTC_DateTypeDef dateStruct = {0};
    GetDate(&dateStruct);
    if (dateStruct.Year < 2024 || dateStruct.Date == 0) {
        dateStruct.WeekDay = 1;
        dateStruct.Date = 1;
        dateStruct.Month = 1;
        dateStruct.Year = 2026;
    }

    setTimeDate(
        RTC_TimeStruct->Seconds,
        RTC_TimeStruct->Minutes,
        RTC_TimeStruct->Hours,
        dateStruct.WeekDay,
        dateStruct.Date,
        dateStruct.Month,
        dateStruct.Year
    );
}

void ds3231_RTC::SetDate(RTC_DateTypeDef *RTC_DateStruct) {
    if (RTC_DateStruct == nullptr) return;

    // Fetch current time so we preserve it when writing
    RTC_TimeTypeDef timeStruct = {0};
    GetTime(&timeStruct);

    setTimeDate(
        timeStruct.Seconds,
        timeStruct.Minutes,
        timeStruct.Hours,
        RTC_DateStruct->WeekDay,
        RTC_DateStruct->Date,
        RTC_DateStruct->Month,
        RTC_DateStruct->Year
    );
}

void ds3231_RTC::GetTime(RTC_TimeTypeDef *RTC_TimeStruct) {
    if (RTC_TimeStruct == nullptr) return;
    ensureI2CBus(wr, false);

    for (int retry = 0; retry < 3; retry++) {
        if (retry > 0) ensureI2CBus(wr, true);
        wr->beginTransmission(DS3231_ADDR);
        wr->write(DS3231_REG_SEC);
        if (wr->endTransmission() == 0) {
            uint8_t count = wr->requestFrom((uint8_t)DS3231_ADDR, (uint8_t)3);
            if (count >= 3 || wr->available() >= 3) {
                uint8_t sec = wr->read();
                uint8_t min = wr->read();
                uint8_t hr  = wr->read();

                RTC_TimeStruct->Seconds = Bcd2ToByte(sec & 0x7F);
                RTC_TimeStruct->Minutes = Bcd2ToByte(min & 0x7F);
                if (hr & 0x40) {
                    bool pm = (hr & 0x20) != 0;
                    uint8_t h = Bcd2ToByte(hr & 0x1F);
                    if (pm && h < 12) h += 12;
                    if (!pm && h == 12) h = 0;
                    RTC_TimeStruct->Hours = h;
                } else {
                    RTC_TimeStruct->Hours = Bcd2ToByte(hr & 0x3F);
                }
                return;
            }
        }
        delay(2);
    }
}

void ds3231_RTC::GetDate(RTC_DateTypeDef *RTC_DateStruct) {
    if (RTC_DateStruct == nullptr) return;
    ensureI2CBus(wr, false);

    for (int retry = 0; retry < 3; retry++) {
        if (retry > 0) ensureI2CBus(wr, true);
        wr->beginTransmission(DS3231_ADDR);
        wr->write(DS3231_REG_DAY);
        if (wr->endTransmission() == 0) {
            uint8_t count = wr->requestFrom((uint8_t)DS3231_ADDR, (uint8_t)4);
            if (count >= 4 || wr->available() >= 4) {
                uint8_t wday  = wr->read();
                uint8_t date  = wr->read();
                uint8_t month = wr->read();
                uint8_t year  = wr->read();

                RTC_DateStruct->WeekDay = Bcd2ToByte(wday & 0x07);
                RTC_DateStruct->Date    = Bcd2ToByte(date & 0x3F);
                RTC_DateStruct->Month   = Bcd2ToByte(month & 0x1F);
                if (month & 0x80) {
                    RTC_DateStruct->Year = 2100 + Bcd2ToByte(year);
                } else {
                    RTC_DateStruct->Year = 2000 + Bcd2ToByte(year);
                }
                return;
            }
        }
        delay(2);
    }
}

void ds3231_RTC::GetDs3231Time(void) {
    ensureI2CBus(wr, false);

    for (int retry = 0; retry < 3; retry++) {
        if (retry > 0) ensureI2CBus(wr, true);
        wr->beginTransmission(DS3231_ADDR);
        wr->write(DS3231_REG_SEC);
        if (wr->endTransmission() == 0) {
            uint8_t count = wr->requestFrom((uint8_t)DS3231_ADDR, (uint8_t)7);
            if (count >= 7 || wr->available() >= 7) {
                for (int i = 0; i < 7; i++) {
                    trdata[i] = wr->read();
                }
                DataMask();
                Bcd2asc();
                Str2Time();
                return;
            }
        }
        delay(2);
    }
}

void ds3231_RTC::Str2Time(void) {
    Second = (asc[0] - 0x30) * 10 + (asc[1] - 0x30);
    Minute = (asc[2] - 0x30) * 10 + (asc[3] - 0x30);
    Hour   = (asc[4] - 0x30) * 10 + (asc[5] - 0x30);
}

void ds3231_RTC::DataMask(void) {
    trdata[0] = trdata[0] & 0x7F; // Seconds
    trdata[1] = trdata[1] & 0x7F; // Minutes
    if (trdata[2] & 0x40) {
        // 12-hour mode
        bool pm = (trdata[2] & 0x20) != 0;
        uint8_t hr = Bcd2ToByte(trdata[2] & 0x1F);
        if (pm && hr < 12) hr += 12;
        if (!pm && hr == 12) hr = 0;
        trdata[2] = ByteToBcd2(hr);
    } else {
        trdata[2] = trdata[2] & 0x3F; // 24-hour mode
    }
    trdata[3] = trdata[3] & 0x07; // Weekday (1-7)
    trdata[4] = trdata[4] & 0x3F; // Date (1-31)
    trdata[5] = trdata[5] & 0x1F; // Month (1-12)
    trdata[6] = trdata[6] & 0xFF; // Year (0-99)
}

void ds3231_RTC::Bcd2asc(void) {
    uint8_t i, j;
    for (j = 0, i = 0; i < 7; i++) {
        asc[j++] = ((trdata[i] & 0xF0) >> 4) | 0x30;
        asc[j++] = (trdata[i] & 0x0F) | 0x30;
    }
}

uint8_t ds3231_RTC::Bcd2ToByte(uint8_t Value) {
    return ((Value >> 4) * 10) + (Value & 0x0F);
}

uint8_t ds3231_RTC::ByteToBcd2(uint8_t Value) {
    return (((Value / 10) << 4) | (Value % 10));
}

void ds3231_RTC::WriteReg(uint8_t reg, uint8_t data) {
    ensureI2CBus(wr, false);
    wr->beginTransmission(DS3231_ADDR);
    wr->write(reg);
    wr->write(data);
    wr->endTransmission();
}

uint8_t ds3231_RTC::ReadReg(uint8_t reg) {
    ensureI2CBus(wr, false);
    wr->beginTransmission(DS3231_ADDR);
    wr->write(reg);
    wr->endTransmission(false);
    wr->requestFrom((uint8_t)DS3231_ADDR, (uint8_t)1);
    return wr->read();
}

struct tm ds3231_RTC::getTimeStruct(void) {
    static unsigned long last_i2c_sync = 0;
    static struct tm last_valid_time = {0};

    unsigned long now_ms = millis();
    // Return time counted by internal system clock between 10-second hardware RTC sync intervals
    if (last_i2c_sync != 0 && (now_ms - last_i2c_sync < 10000)) {
        time_t now = time(nullptr);
        struct tm *t = localtime(&now);
        if (t && t->tm_year >= (2024 - 1900)) {
            return *t;
        }
    }

    struct tm timeinfo = {0};
    ensureI2CBus(wr, false);

    for (int retry = 0; retry < 2; retry++) {
        if (retry > 0) ensureI2CBus(wr, true);
        wr->beginTransmission(DS3231_ADDR);
        wr->write(DS3231_REG_SEC);
        if (wr->endTransmission() == 0) {
            uint8_t count = wr->requestFrom((uint8_t)DS3231_ADDR, (uint8_t)7);
            if (count >= 7 || wr->available() >= 7) {
                uint8_t sec   = wr->read();
                uint8_t min   = wr->read();
                uint8_t hr    = wr->read();
                uint8_t wday  = wr->read();
                uint8_t date  = wr->read();
                uint8_t month = wr->read();
                uint8_t year  = wr->read();

                timeinfo.tm_sec  = Bcd2ToByte(sec & 0x7F);
                timeinfo.tm_min  = Bcd2ToByte(min & 0x7F);
                if (hr & 0x40) {
                    bool pm = (hr & 0x20) != 0;
                    uint8_t h = Bcd2ToByte(hr & 0x1F);
                    if (pm && h < 12) h += 12;
                    if (!pm && h == 12) h = 0;
                    timeinfo.tm_hour = h;
                } else {
                    timeinfo.tm_hour = Bcd2ToByte(hr & 0x3F);
                }
                timeinfo.tm_wday = Bcd2ToByte(wday & 0x07);
                timeinfo.tm_mday = Bcd2ToByte(date & 0x3F);
                if (timeinfo.tm_mday == 0) timeinfo.tm_mday = 1;
                uint8_t m = Bcd2ToByte(month & 0x1F);
                timeinfo.tm_mon  = (m > 0) ? m - 1 : 0;
                uint16_t fullYear = ((month & 0x80) ? 2100 : 2000) + Bcd2ToByte(year);
                timeinfo.tm_year = fullYear >= 1900 ? fullYear - 1900 : 124;

                mktime(&timeinfo);

                if (timeinfo.tm_year >= (2024 - 1900)) {
                    last_valid_time = timeinfo;
                    last_i2c_sync = now_ms;
                    time_t epoch = mktime(&timeinfo);
                    struct timeval tv = {.tv_sec = epoch};
                    settimeofday(&tv, nullptr);
                    return timeinfo;
                }
            }
        }
        delay(2);
    }

    // Fallback to internal system clock (never returns 12 AM unless system clock is actually 12 AM)
    time_t now = time(nullptr);
    struct tm *t = localtime(&now);
    if (t && t->tm_year >= (2024 - 1900)) return *t;
    if (last_valid_time.tm_year >= (2024 - 1900)) return last_valid_time;
    return timeinfo;
}

float ds3231_RTC::getTemperature(void) {
    wr->beginTransmission(DS3231_ADDR);
    wr->write(DS3231_REG_TEMP_MSB);
    if (wr->endTransmission() != 0) return 0.0f;

    if (wr->requestFrom((uint8_t)DS3231_ADDR, (uint8_t)2) >= 2) {
        int8_t msb = wr->read();
        uint8_t lsb = wr->read();
        return (float)msb + ((lsb >> 6) * 0.25f);
    }
    return 0.0f;
}

int ds3231_RTC::SetAlarmIRQ(int afterSeconds) {
    return -1;
}

void ds3231_RTC::clearIRQ(void) {}

void ds3231_RTC::disableIRQ(void) {}
