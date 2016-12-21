#include "defines.h"

#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "Tables.h"
#include "Utils/GlobalFunctions.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Структура для описания диапазона масштаба по времени.
typedef struct
{
    const char* name[2];    // Название диапазона в текстовом виде, пригодном для вывода на экран.
} TBaseStruct;


// Массив структур описаний масштабов по времени.
static const TBaseStruct tBases[TBaseSize] =
{
    {"1\x10нс",     "1\x10ns"},
    {"2\x10нс",     "2\x10ns"},
    {"5\x10нс",     "5\x10ns"},
    {"10\x10нс",    "10\x10ns"},
    {"20\x10нс",    "20\x10ns"},
    {"50\x10нс",    "50\x10ns"},
    {"0.1\x10мкс",  "0.1\x10us"},
    {"0.2\x10мкс",  "0.2\x10us"},
    {"0.5\x10мкс",  "0.5\x10us"},
    {"1\x10мкс",    "1\x10us"},
    {"2\x10мкс",    "2\x10us"},
    {"5\x10мкс",    "5\x10us"},
    {"10\x10мкс",   "10\x10us"},
    {"20\x10мкс",   "20\x10us"},
    {"50\x10мкс",   "50\x10us"},
    {"0.1\x10мс",   "0.1\x10ms"},
    {"0.2\x10мс",   "0.2\x10ms"},
    {"0.5\x10мс",   "0.5\x10ms"},
    {"1\x10мс",     "1\x10ms"},
    {"2\x10мс",     "2\x10ms"},
    {"5\x10мс",     "5\x10ms"},
    {"10\x10мс",    "10\x10ms"},
    {"20\x10мс",    "20\x10ms"},
    {"50\x10мс",    "50\x10ms"},
    {"0.1\x10с",    "0.1\x10s"},
    {"0.2\x10с",    "0.2\x10s"},
    {"0.5\x10с",    "0.5\x10s"},
    {"1\x10с",      "1\x10s"},
    {"2\x10с",      "2\x10s"},
    {"5\x10с",      "5\x10s"},
    {"10\x10с",     "10\x10s"}
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NumSignalsInSec Tables_NumSignalsInSecToENUM(int numSignalsInSec)
{
    if(numSignalsInSec == 1)
    {
        return NumSignalsInSec_1;
    }
    else if(numSignalsInSec == 2)
    {
        return NumSignalsInSec_2;
    }
    else if(numSignalsInSec == 5)
    {
        return NumSignalsInSec_5;
    }
    else if(numSignalsInSec == 10)
    {
        return NumSignalsInSec_10;
    }
    else if(numSignalsInSec == 25)
    {
        return NumSignalsInSec_25;
    }
    LOG_ERROR("Число сигналов в секунду равно %d", numSignalsInSec);
    return NumSignalsInSec_1;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int Tables_ENUMtoNumSignalsInSec(NumSignalsInSec numSignalsInSec)
{
    static const int fps[] = {25, 10, 5, 2, 1};
    if(numSignalsInSec <= NumSignalsInSec_1)
    {
        return fps[numSignalsInSec];
    }
    LOG_ERROR("Неправильный параметр %d", (int)numSignalsInSec);
    return 1;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const char* Tables_GetTBaseString(TBase tBase)
{
    return tBases[tBase].name[LANG];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const char* Tables_GetTBaseStringEN(TBase tBase)
{
    return tBases[tBase].name[English];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const char* Tables_RangeNameFromValue(Range range)
{
    static const char* names[RangeSize] =
    {
        "Range_2mV",
        "Range_5mV",
        "Range_10mV",
        "Range_20mV",
        "Range_50mV",
        "Range_100mV",
        "Range_200mV",
        "Range_500mV",
        "Range_1V",
        "Range_2V",
        "Range_5V"
    };
    return names[range];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int Tables_GetNumChannel(Channel ch)
{
    return ch == A ? 1 : 2;
}
