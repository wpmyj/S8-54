// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "RTC.h"
#include "Log.h"
#include "Display/Display.h"
#include "Hardware/Hardware.h"
#include "Hardware/Timer.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_rtc.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define VALUE_FOR_RTC 0x644


#define RTC_CLOCK_SOURCE_LSE
// #define RTC_CLOCK_SOURCE_LSI


#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV 0x7f
#define RTC_SYNCH_PREDIV 0x0130
#endif


#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV 0x7f
#define RTC_SYNCH_PREDIV 0x00ff
#endif


static RTC_HandleTypeDef rtcHandle =
{
    RTC,
    {
        RTC_HOURFORMAT_24,          // HourFormat
        RTC_ASYNCH_PREDIV,          // AsynchPrediv
        RTC_SYNCH_PREDIV,           // SynchPrediv
        RTC_OUTPUT_DISABLE,         // OutPut
        RTC_OUTPUT_POLARITY_HIGH,   // OutPutPolarity
        RTC_OUTPUT_TYPE_OPENDRAIN   // OutPutType
    }
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
void RTC_Init(void)
{
    if (HAL_RTC_Init((RTC_HandleTypeDef*)(&rtcHandle)) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    if (HAL_RTCEx_BKUPRead((RTC_HandleTypeDef*)&rtcHandle, RTC_BKP_DR0) != VALUE_FOR_RTC)
    {
        if(RTC_SetTimeAndData(11, 11, 11, 11, 11, 11))
        {
            HAL_RTCEx_BKUPWrite((RTC_HandleTypeDef*)&rtcHandle, RTC_BKP_DR0, VALUE_FOR_RTC);
        }
        gBF.tuneTime = 1;
    }

    RTC_SetCorrection((int8)setNR.correctionTime);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
PackedTime RTC_GetPackedTime(void)
{
    PackedTime time;

    RTC_TimeTypeDef isTime;
    HAL_RTC_GetTime((RTC_HandleTypeDef*)&rtcHandle, &isTime, FORMAT_BIN);

    time.hours = isTime.Hours;
    time.minutes = isTime.Minutes;
    time.seconds = isTime.Seconds;

    RTC_DateTypeDef isDate;
    HAL_RTC_GetDate((RTC_HandleTypeDef*)&rtcHandle, &isDate, FORMAT_BIN);

    time.year = isDate.Year;
    time.month = isDate.Month;
    time.day = isDate.Date;

    time.timeMS = gTimerMS;

    return time;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool RTC_SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds)
{
    RTC_DateTypeDef dateStruct;
    dateStruct.WeekDay = RTC_WEEKDAY_MONDAY;
    dateStruct.Month = month;
    dateStruct.Date = day;
    dateStruct.Year = year;

    if(HAL_RTC_SetDate((RTC_HandleTypeDef*)&rtcHandle, &dateStruct, FORMAT_BIN) != HAL_OK)
    {
        return false;
    };

    RTC_TimeTypeDef timeStruct;
    timeStruct.Hours = hours;
    timeStruct.Minutes = minutes;
    timeStruct.Seconds = seconds;
    timeStruct.TimeFormat = RTC_HOURFORMAT_24;
    timeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    timeStruct.StoreOperation = RTC_STOREOPERATION_SET;

    if(HAL_RTC_SetTime((RTC_HandleTypeDef*)&rtcHandle, &timeStruct, FORMAT_BIN) != HAL_OK)
    {
        return false;
    };
    
    return true;
}

void RTC_SetCorrection(int8 correction)
{
    setNR.correctionTime = correction;
    
    HAL_RTCEx_SetCoarseCalib(&rtcHandle, correction < 0 ? RTC_CALIBSIGN_NEGATIVE : RTC_CALIBSIGN_POSITIVE, correction & 0x7f);
}
