

#include "defines.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"
#include "PageMemory.h"
#include "FPGA/FPGAtypes.h"
#include "FPGA/FPGA.h"
#include "Log.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h" 
#include "Hardware/FSMC.h"
#include "Display/Grid.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;

static const  Choice cSample;                ///< РАЗВЕРТКА - Выборка
static bool  IsActive_Sample(void);
static const  Choice cPeakDet;               ///< РАЗВЕРТКА - Пик дет
static bool  IsActive_PeakDet(void);
       void OnChanged_PeakDet(bool active);
static const  Choice cTPos;                  ///< РАЗВЕРТКА - То
       void OnChanged_TPos(bool active);
static const  Choice cDivRole;               ///< РАЗВЕРТКА - Ф-ция ВР/ДЕЛ

// РАЗВЕРТКА /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pTime =
{
    Item_Page, &mainPage, 0,
    {
        "РАЗВЕРТКА", "SCAN",
        "Содержит настройки развёртки.",
        "Contains scan settings."
    },
    Page_Time,
    {
        (void *)&cSample,       // РАЗВЕРТКА - Выборка
        (void *)&cPeakDet,      // РАЗВЕРТКА - Пик дет
        (void *)&cTPos,         // РАЗВЕРТКА - То
        (void *)&cDivRole       // РАЗВЕРТКА - Ф-ция ВР/ДЕЛ
        // (void *)&mcTimeSelfRecorder,
    }
};

// РАЗВЕРТКА - Выборка -------------------------------------------------------------------------------------------------------------------------------
static const Choice cSample =
{
    Item_Choice, &pTime, IsActive_Sample,
    {
        "Выборка",          "Sampling"
        ,
        "\"Реальная\" - \n"
        "\"Эквивалентная\" -"
        ,
        "\"Real\" - \n"
        "\"Equals\" - "
    },
    {
        {"Реальное время",  "Real"},
        {"Эквивалентная",   "Equals"}
    },
    (int8 *)&SAMPLE_TYPE
};

static bool IsActive_Sample(void)
{
    return IN_RANDOM_MODE && !START_MODE_SINGLE;
}

// РАЗВЕРТКА - Пик дет -------------------------------------------------------------------------------------------------------------------------------
static const Choice cPeakDet =
{
    Item_Choice, &pTime, IsActive_PeakDet,
    {
        "Пик дет",      "Pic deat",
        "Включает/выключает пиковый детектор.",
        "Turns on/off peak detector."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    /* , {"Среднее",   "Average"} */
    },
    (int8 *)&SET_PEAKDET, OnChanged_PeakDet
};

static bool IsActive_PeakDet(void)
{
    if (FPGA_POINTS_32k)                       // При 32к точек на канал мы не можем включать пиковый детектор
    {
        return false;
    }

    if (FPGA_POINTS_16k && SET_ENABLED_B) // При 16к точках на канал мы можем работать только с одним каналом
    {
        return false;
    }

    return (SET_TBASE >= MIN_TBASE_PEC_DEAT);
}

void OnChanged_PeakDet(bool active)
{
    if(active)
    {
        FPGA_SetPeackDetMode(SET_PEAKDET);
        FPGA_SetTBase(SET_TBASE);
        OnChanged_Points(true);
    }
    else
    {
        if (FPGA_POINTS_32k)
        {
            Display_ShowWarning(NoPeakDet32k);
        }
        else if (FPGA_POINTS_16k && SET_ENABLED_B)
        {
            Display_ShowWarning(NoPeakDet16k);
        }
        else
        {
            Display_ShowWarning(TooSmallSweepForPeakDet);
        }
    }
}

// РАЗВЕРТКА - То ------------------------------------------------------------------------------------------------------------------------------------
static const Choice cTPos =
{
    Item_Choice, &pTime, 0,
    {
        "\x7b",     "\x7b",
        "Задаёт точку привязки нулевого смещения по времени к экрану - левый край, центр, правый край.", 
        "Sets the anchor point nuleovgo time offset to the screen - the left edge, center, right edge."
    },
    {
        {"Лево",    "Left"},
        {"Центр",   "Center"},
        {"Право",   "Right"}
    },
    (int8 *)&TPOS, OnChanged_TPos
};

void OnChanged_TPos(bool active)
{
    OnChanged_Points(active);
    FPGA_SetTShift(SET_TSHIFT);
}

// РАЗВЕРТКА - Ф-ция ВР/ДЕЛ --------------------------------------------------------------------------------------------------------------------------
static const Choice cDivRole =
{
    Item_Choice, &pTime, 0,
    {
        "Ф-ция ВР/ДЕЛ", "Funct Time/DIV"
        ,
        "Задаёт функцию для ручки ВРЕМЯ/ДЕЛ: в режиме сбора информации (ПУСК/СТОП в положении ПУСК):\n"
        "1. \"Время\" - изменение смещения по времени.\n"
        "2. \"Память\" - перемещение по памяти."
        ,
        "Sets the function to handle TIME/DIV: mode of collecting information (START/STOP to start position):\n"
        "1. \"Time\" - change the time shift.\n"
        "2. \"Memory\" - moving from memory."
    },
    {
        {"Время",       "Time"},
        {"Память",      "Memory"}
    },
    (int8 *)&TIME_DIVXPOS
};
