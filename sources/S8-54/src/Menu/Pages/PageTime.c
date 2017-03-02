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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;

static const Choice mcTimeSample;
static bool FuncTimeSampleActive(void);
static const Choice mcTimePicDet;
static bool IsActiveChoiceTimePicDeat(void);
       void OnPeacDetChanged(bool active);
static const Choice mcTimeTPos;
       void OnTPosChanged(bool active);
static const Choice mcTimeDivRole;

// РАЗВЕРТКА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpTime =
{
    Item_Page, &mainPage,
    {
        "РАЗВЕРТКА", "SCAN"
    },
    {
        "Содержит настройки развёртки.",
        "Contains scan settings."
    },
    0, Page_Time,
    {
        (void*)&mcTimeSample,
        (void*)&mcTimePicDet,
        (void*)&mcTimeTPos,
        // (void*)&mcTimeSelfRecorder,
        (void*)&mcTimeDivRole
    }
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcTimeSample =
{
    Item_Choice, &mpTime, {"Выборка", "Sampling"},
    {
        "\"Реальная\" - \n"
        "\"Эквивалентная\" -"
        ,
        "\"Real\" - \n"
        "\"Equals\" - "
    },
    FuncTimeSampleActive,
    {
        {"Реальное время", "Real"},
        {"Эквивалентная", "Equals"}
    },
    (int8*)&SAMPLE
};

static bool FuncTimeSampleActive(void)
{
    return IN_RANDOM_MODE && !START_MODE_SINGLE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcTimePicDet =
{
    Item_Choice, &mpTime, {"Пик дет", "Pic deat"},
    {
        "Включает/выключает пиковый детектор.",
        "Turns on/off peak detector."
    },
    IsActiveChoiceTimePicDeat,
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    /* , {"Среднее",   "Average"} */
    },
    (int8*)&PEACKDET, OnPeacDetChanged
};

static bool IsActiveChoiceTimePicDeat(void)
{
    if (FPGA_NUM_POINTS_32k)                       // При 32к точек на канал мы не можем включать пиковый детектор
    {
        return false;
    }

    if (FPGA_NUM_POINTS_16k && ENABLE_B) // При 16к точках на канал мы можем работать только с одним каналом
    {
        return false;
    }

    return (TBASE >= MIN_TBASE_PEC_DEAT);
}

void OnPeacDetChanged(bool active)
{
    if(active)
    {
        FPGA_SetPeackDetMode(PEACKDET);
        FPGA_SetTBase(TBASE);
        OnChange_MemoryLength(true);
    }
    else
    {
        if (FPGA_NUM_POINTS_32k)
        {
            Display_ShowWarning(NoPeakDet32k);
        }
        else if (FPGA_NUM_POINTS_16k && ENABLE_B)
        {
            Display_ShowWarning(NoPeakDet16k);
        }
        else
        {
            Display_ShowWarning(TooSmallSweepForPeakDet);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcTimeTPos =
{
    Item_Choice, &mpTime,
    {
        "\x7b", "\x7b"
    },
    {
        "Задаёт точку привязки нулевого смещения по времени к экрану - левый край, центр, правый край.", 
        "Sets the anchor point nuleovgo time offset to the screen - the left edge, center, right edge."
    },
    0,
    {
        {"Лево", "Left"},
        {"Центр", "Center"},
        {"Право", "Right"}
    },
    (int8*)&TPOS, OnTPosChanged
};

void OnTPosChanged(bool active)
{
    OnChange_MemoryLength(active);
    FPGA_SetTShift(TSHIFT);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcTimeDivRole =
{
    Item_Choice, &mpTime, {"Ф-ция ВР/ДЕЛ", "Funct Time/DIV"},
    {
        "Задаёт функцию для ручки ВРЕМЯ/ДЕЛ: в режиме сбора информации (ПУСК/СТОП в положении ПУСК):\n"
        "1. \"Время\" - изменение смещения по времени.\n"
        "2. \"Память\" - перемещение по памяти.",

        "Sets the function to handle TIME/DIV: mode of collecting information (START/STOP to start position):\n"
        "1. \"Time\" - change the time shift.\n"
        "2. \"Memory\" - moving from memory."
    },
    0,
    {
        {"Время", "Time"},
        {"Память", "Memory"}
    },
    (int8*)&TIME_DIVXPOS
};
