// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Settings.h"
#include "SettingsChannel.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Структура для описания диапазона масштаба по напряжению.
typedef struct
{
    const char* name[2][2];     // Название диапазона в текстовом виде, пригодном для вывода на экран.
} RangeStruct;


// Массив структур описаний масштабов по напряжению.
static const RangeStruct ranges[RangeSize] =
{
    {"2\x10мВ", "20\x10мВ", "2\x10мV",  "20\x10мV"},
    {"5\x10мВ", "50\x10мВ", "5\x10mV",  "50\x10mV"},
    {"10\x10мВ","0.1\x10В", "10\x10mV", "0.1\x10V"},
    {"20\x10мВ","0.2\x10В", "20\x10mV", "0.2\x10V"},
    {"50\x10мВ","0.5\x10В", "50\x10mV", "0.5\x10V"},
    {"0.1\x10В","1\x10В",   "0.1\x10V", "1\x10V"},
    {"0.2\x10В","2\x10В",   "0.2\x10V", "2\x10V"},
    {"0.5\x10В","5\x10В",   "0.5\x10V", "5\x10V"},
    {"1\x10В",  "10\x10В",  "1\x10V",   "10\x10V"},
    {"2\x10В",  "20\x10В",  "2\x10V",   "20\x10V"},
    {"5\x10В",  "50\x10В",  "5\x10V",   "50\x10V"}
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sChannel_SetRange(Channel ch, Range range)
{
    SET_RANGE(ch) = range;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int sChannel_MultiplierRel2Abs(Divider divider)
{
    switch (divider)
    {
        case Divider_10:
            return 10;
    }
    return 1;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sChannel_Enabled(Channel ch)
{
    if (ch == Math && FUNC_ENABLED)
    {
        return true;
    }
    return SET_ENABLED(ch);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const char* sChannel_Range2String(Range range, Divider divider)
{
    return ranges[range].name[LANG][divider];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const char* sChannel_RShift2String(uint16 rShiftRel, Range range, Divider divider, char buffer[20])
{
    float rShiftVal = RSHIFT_2_ABS(rShiftRel, range) * sChannel_MultiplierRel2Abs(divider);
    return Voltage2String(rShiftVal, true, buffer);
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sChannel_NeedForDraw(const uint8 *data, Channel ch, DataSettings *ds)
{
    if (!data)
    {
        return false;
    }

    if (WORK_DIRECT)
    {
        if (!sChannel_Enabled(ch))
        {
            return false;
        }
    }
    else if (ds != 0)
    {
        if ((ch == A && !ENABLED_A(ds)) || (ch == B && !ENABLED_B(ds)))
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}
