#pragma once

#include <Wire.h>
#include <time.h>
#include <sys/time.h>

#define DS3231_ADDR 0x68

#define DS3231_REG_SEC      0x00
#define DS3231_REG_MIN      0x01
#define DS3231_REG_HOUR     0x02
#define DS3231_REG_DAY      0x03
#define DS3231_REG_DATE     0x04
#define DS3231_REG_MONTH    0x05
#define DS3231_REG_YEAR     0x06
#define DS3231_REG_CTRL     0x0E
#define DS3231_REG_STAT     0x0F
#define DS3231_REG_TEMP_MSB 0x11
#define DS3231_REG_TEMP_LSB 0x12

typedef struct {
    uint8_t Hours;
    uint8_t Minutes;
    uint8_t Seconds;
} RTC_TimeTypeDef;

typedef struct {
    uint8_t WeekDay;
    uint8_t Month;
    uint8_t Date;
    uint16_t Year;
} RTC_DateTypeDef;

class ds3231_RTC {
public:
    void begin(void);
    void setWire(TwoWire *obj) { wr = obj; }
    void setPins(int8_t sda, int8_t scl);
    void GetDs3231Time(void);

    void SetTime(RTC_TimeTypeDef *RTC_TimeStruct);
    void SetDate(RTC_DateTypeDef *RTC_DateStruct);
    bool setTimeDate(uint8_t sec, uint8_t min, uint8_t hr, uint8_t wday, uint8_t mday, uint8_t mon, uint16_t yr);

    void GetTime(RTC_TimeTypeDef *RTC_TimeStruct);
    void GetDate(RTC_DateTypeDef *RTC_DateStruct);

    struct tm getTimeStruct();

    void WriteReg(uint8_t reg, uint8_t data);
    uint8_t ReadReg(uint8_t reg);

    int SetAlarmIRQ(int afterSeconds);
    void clearIRQ();
    void disableIRQ();

    float getTemperature();

public:
    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Week;
    uint8_t Day;
    uint8_t Month;
    uint8_t Year;
    uint8_t DateString[9];
    uint8_t TimeString[9];
    uint8_t asc[14];

private:
    TwoWire *wr = &Wire;
    void Bcd2asc(void);
    void DataMask();
    void Str2Time(void);

    uint8_t Bcd2ToByte(uint8_t Value);
    uint8_t ByteToBcd2(uint8_t Value);

private:
    uint8_t trdata[7];
};
