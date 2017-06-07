// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGAtypes.h"
#include "Hardware/FSMC.h"
#include "Hardware/Sound.h"
#include "Menu/MenuDrawing.h"
#include "Menu/MenuFunctions.h"
#include "Log.h"
#include "DebugSerialNumber.h"
#include "DebugConsole.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;
extern void LoadTShift(void);

static const Choice cStats;

static const Page ppADC;

static const Page pppADC_Balance;
static const Choice cADC_Balance_Mode;
static const Governor gADC_Balance_ShiftA;
static const Governor gADC_Balance_ShiftB;

static const Page pppADC_Stretch;
static const Choice cADC_Stretch_Mode;
static const Governor gADC_Stretch_A;
static const Governor gADC_Stretch_B;
static const Governor gADC_Stretch_Ak20mV;
static const Governor gADC_Stretch_Ak50mV;
static const Governor gADC_Stretch_Ak100mV;
static const Governor gADC_Stretch_Ak2V;
static const Governor gADC_Stretch_Bk20mV;
static const Governor gADC_Stretch_Bk50mV;
static const Governor gADC_Stretch_Bk100mV;
static const Governor gADC_Stretch_Bk2V;

static const Page pppADC_Shift;
static const Button bADC_Shift_Reset;
static const Governor gADC_Shift_A2mV;
static const Governor gADC_Shift_B2mV;
static const Governor gADC_Shift_A5mV;
static const Governor gADC_Shift_B5mV;
static const Governor gADC_Shift_A10mV;
static const Governor gADC_Shift_B10mV;

static const Page ppRand;
static const Governor gRand_NumMeasures;
static const Governor gRand_NumAverage;
static const Governor gRand_NumSmooth;
static const Choice cRand_ShowInfo;
static const Choice gRand_ShowStat;
static const Governor gRand_TimeCompensation;
static void OnChange_Rand_TimeCompensation(void);
static const Governor gRand_AddTimeShift;
static void OnChange_Rand_AddTimeShift(void);
static const Governor gRand_Pretriggered;
static void OnChange_Rand_Pretriggered(void);

static const Page ppChannels;
static const Choice cChannels_BandwidthA;
static const Choice cChannels_BandwidthB;
static void OnChange_Channels_BandwidthA(bool active);
static void OnChange_Channels_BandwidthB(bool active);

static const Choice cEMS;
static void OnChange_EMS(bool);

static const Choice cDisplayOrientation;
       void OnChange_DisplayOrientation(bool);

static const Governor mgPred;
static void OnChange_Pred(void);
static const Governor mgPost;
static void OnChange_Post(void);
static const Page ppShowSettingsInfo;
static void OnPress_ShowInfo(void);
static const SmallButton bExitShowSetInfo;

static void OnChange_ADC_Balance_Mode(bool active);
static void OnDraw_ADC_Balance_Mode(int x, int y);
static bool IsActive_ADC_Balance(void);
static void OnChange_ADC_BalanceA(void);
static void OnChange_ADC_BalanceB(void);

       void OnChange_ADC_Stretch_Mode(bool active);
static bool IsActive_ADC_StretchAB(void);
static void OnChange_ADC_Stretch_A(void);
static void OnChange_ADC_Stretch_B(void);

static void OnPress_ADC_Shift_Reset(void);
static void OnChange_ADC_Shift_A(void);
static void OnChange_ADC_Shift_B(void);

static void OnChange_Rand_NumMeasures(void);


// ОТЛАДКА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpDebug =
{
    Item_Page, &mainPage, 0,
    {
        "ОТЛАДКА", "DEBUG",
        "", ""
    },
    Page_Debug,
    {
        (void*)&ppConsole,             // ОТЛАДКА - КОНСОЛЬ
        (void*)&ppADC,                 // ОТЛАДКА - АЦП
        (void*)&ppRand,                // ОТЛАДКА - РАНД-ТОР
        (void*)&ppChannels,            // ОТЛАДКА - КАНАЛЫ
        (void*)&cStats,                // ОТЛАДКА - Статистика
        (void*)&cDisplayOrientation,   // ОТЛАДКА - Ориентация
        (void*)&cEMS,                  // ОТЛАДКА - ЭМС
        (void*)&mgPred,                // ОТЛАДКА - Предзапуск
        (void*)&mgPost,                // ОТЛАДКА - Послезапуск
        (void*)&ppShowSettingsInfo,    // ОТЛАДКА - Настройки
        (void*)&ppSerialNumber         // ОТЛАДКА - С/Н
    }
};

// ОТЛАДКА - Статистика ------------------------------------------------------------------------------------------------------------------------------
static const Choice cStats =
{
    Item_Choice, &mpDebug, 0,
    {
        "Статистика", "Statistics",
        "Показывать/не показывать время/кадр, кадров в секунду, количество сигналов с последними настройками в памяти/количество сохраняемых в памяти сигналов",
        "To show/not to show a time/shot, frames per second, quantity of signals with the last settings in memory/quantity of the signals kept in memory"
    },
    {
        {"Не показывать",   "Hide"},
        {"Показывать",      "Show"}
    },
    (int8*)&SHOW_STAT
};

// ОТЛАДКА - АЦП /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppADC =
{
    Item_Page, &mpDebug, 0,
    {
        "АЦП", "ADC",
        "",
        ""
    },
    Page_DebugADC,
    {
        (void*)&pppADC_Balance,   // ОТЛАДКА - АЦП - БАЛАНС
        (void*)&pppADC_Stretch,   // ОТЛАДКА - АЦП - РАСТЯЖКА
        (void*)&pppADC_Shift      // ОТЛАДКА - АЦП - ДОП СМЕЩ
    }
};

// ОТЛАДКА - АЦП - БАЛАНС ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pppADC_Balance =
{
    Item_Page, &ppADC, 0,
    {
        "БАЛАНС", "BALANCE",
        "",
        ""
    },
    Page_DebugADCbalance,
    {
        (void*)&cADC_Balance_Mode,   // ОТЛАДКА - АЦП - БАЛАНС - Режим
        (void*)&gADC_Balance_ShiftA, // ОТЛАДКА - АЦП - БАЛАНС - Смещение 1
        (void*)&gADC_Balance_ShiftB  // ОТЛАДКА - АЦП - БАЛАНС - Смещение 2
    }
};

// ОТЛАДКА - АЦП - БАЛАНС - Режим --------------------------------------------------------------------------------------------------------------------
static const Choice cADC_Balance_Mode =
{
    Item_Choice, &pppADC_Balance, 0,
    {
        "Режим", "Mode",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {"Реальный", "Real"},
        {"Ручной", "Manual"}
    },
    (int8*)&NRST_BALANCE_ADC_TYPE, OnChange_ADC_Balance_Mode, OnDraw_ADC_Balance_Mode
};

static void OnChange_ADC_Balance_Mode(bool active)
{
    OnDraw_ADC_Balance_Mode(0, 0);
}

static int16 shiftADCA;
static int16 shiftADCB;

static void OnDraw_ADC_Balance_Mode(int x, int y)
{
    int8 shift[2][3] =
    {
        {0, SET_BALANCE_ADC_A, (int8)NRST_BALANCE_ADC_A},
        {0, SET_BALANCE_ADC_B, (int8)NRST_BALANCE_ADC_B}
    };

    shiftADCA = shift[A][NRST_BALANCE_ADC_TYPE];
    shiftADCB = shift[B][NRST_BALANCE_ADC_TYPE];
}

// ОТЛАДКА - АЦП - БАЛАНС - Смещение 1 ---------------------------------------------------------------------------------------------------------------
static const Governor gADC_Balance_ShiftA =
{
    Item_Governor, &pppADC_Balance, IsActive_ADC_Balance,
    {
        "Смещение 1", "Offset 1",
        "",
        ""
    },
    &shiftADCA, -125, 125, OnChange_ADC_BalanceA
};

static bool IsActive_ADC_Balance(void)
{
    return NRST_BALANCE_ADC_TYPE_IS_HAND;
}

static void OnChange_ADC_BalanceA(void)
{
    NRST_BALANCE_ADC_A = shiftADCA;
}

// ОТЛАДКА - АЦП - БАЛАНС - Смещение 2 ---------------------------------------------------------------------------------------------------------------
static const Governor gADC_Balance_ShiftB =
{
    Item_Governor, &pppADC_Balance, IsActive_ADC_Balance,
    {
        "Смещение 2", "Offset 2",
        "",
        ""
    },
    &shiftADCB, -125, 125, OnChange_ADC_BalanceB
};

static void OnChange_ADC_BalanceB(void)
{
    NRST_BALANCE_ADC_B = shiftADCB;
}

static const Choice emptyChoice;

// ОТЛАДКА - АЦП - РАСТЯЖКА //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pppADC_Stretch =
{
    Item_Page, &ppADC, 0,
    {
        "РАСТЯЖКА", "STRETCH",
        "Устанавливает режим и величину растяжки (для ручного режима)",
        "Sets mode and the value of stretching (manual mode)"
    },
    Page_DebugADCstretch,
    {
        (void*)&cADC_Stretch_Mode,       // ОТЛАДКА - АЦП - РАСТЯЖКА - Режим
        (void*)&gADC_Stretch_A,          // ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 1к
        (void*)&gADC_Stretch_B,          // ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 2к
        (void*)&emptyChoice,
        (void*)&emptyChoice,
        (void*)&gADC_Stretch_Ak20mV,     // ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 1к
        (void*)&gADC_Stretch_Ak50mV,     // ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 1к 
        (void*)&gADC_Stretch_Ak100mV,    // ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 1к
        (void*)&gADC_Stretch_Ak2V,       // ОТЛАДКА - AЦП - РАСТЯЖКА - 2В 1к
        (void*)&emptyChoice,
        (void*)&gADC_Stretch_Bk20mV,     // ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 2к
        (void*)&gADC_Stretch_Bk50mV,     // ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 2к 
        (void*)&gADC_Stretch_Bk100mV,    // ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 2к
        (void*)&gADC_Stretch_Bk2V,       // ОТЛАДКА - АЦП - РАСТЯЖКА - 2В 2к
        (void*)&emptyChoice
    }
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - Режим ------------------------------------------------------------------------------------------------------------------
static const Choice cADC_Stretch_Mode =
{
    Item_Choice, &pppADC_Stretch, 0,
    {
        "Режим", "Mode",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {"Реальный", "Real"},
        {"Ручной", "Manual"}
    },
    (int8*)&NRST_STRETCH_ADC_TYPE, OnChange_ADC_Stretch_Mode
};

static int16 stretchA;
static int16 stretchB;  

void OnChange_ADC_Stretch_Mode(bool active)
{
    if (NRST_STRETCH_ADC_TYPE_IS_DISABLE)
    {
        stretchA = NRST_STRETCH_ADC_A(StretchADC_Disable) = 0;
        stretchB = NRST_STRETCH_ADC_B(StretchADC_Disable) = 0;
    }
    else
    {
        stretchA = NRST_STRETCH_ADC_A(NRST_STRETCH_ADC_TYPE);
        stretchB = NRST_STRETCH_ADC_B(NRST_STRETCH_ADC_TYPE);
    }
}

// ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 1к ------------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_A =
{
    Item_Governor, &pppADC_Stretch, IsActive_ADC_StretchAB,
    {
        "Растяжка 1к", "Stretch 1ch"
        ,
        "Задаёт ручную растяжку первого канала.\n"
        "1 единица = 0.0001"
        ,
        "Sets the manual stretching of the first channel.\n"
        "1 = 0.0001"
    },
    &stretchA, -10000, 10000, OnChange_ADC_Stretch_A
};

static bool IsActive_ADC_StretchAB(void)
{
    return NRST_STRETCH_ADC_TYPE_IS_HAND;
}

static void OnChange_ADC_Stretch_A(void)
{
    NRST_STRETCH_ADC_A(NRST_STRETCH_ADC_TYPE) = stretchA;
}

// ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 2к ------------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_B =
{
    Item_Governor, &pppADC_Stretch, IsActive_ADC_StretchAB,
    {
        "Растяжка 2к", "Stretch 2ch"
        ,
        "Задаёт ручную растяжку второго канала.\n"
        "1 единица = 0.0001"
        ,
        "Sets the manual stretching of the second channel.\n"
        "1 = 0.0001"
    },
    &stretchB, -10000, 10000, OnChange_ADC_Stretch_B
};

static void OnChange_ADC_Stretch_B(void)
{
    NRST_STRETCH_ADC_B(NRST_STRETCH_ADC_TYPE) = stretchB;
}

// ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 1к -------------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_Ak20mV =
{
    Item_Governor, &pppADC_Stretch, 0,
    {
        "20мВ/1В 1к", "20mV/1V 1k",
        "",
        ""
    },
    &NRST_ADD_STRETCH_20mV_A, -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 1к ----------------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_Ak50mV =
{
    Item_Governor, &pppADC_Stretch, 0,
    {
        "50мВ 1к", "50mV 1k",
        "",
        ""
    },
    &NRST_ADD_STRETCH_50mV_A, -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 1к ------------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_Ak100mV =
{
    Item_Governor, &pppADC_Stretch, 0,
    {
        "100мВ/5В 1к", "100mV/5V 1ch",
        "",
        ""
    },
    &NRST_ADD_STRETCH_100mV_A, -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 2В 1к ------------------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_Ak2V =
{
    Item_Governor, &pppADC_Stretch, 0,
    {
        "2В 1к", "2V 1ch",
        "",
        ""
    },
    &NRST_ADD_STRETCH_2V_A, -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 2к -------------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_Bk20mV =
{
    Item_Governor, &pppADC_Stretch, 0,
    {
        "20мВ/1В 2к", "20mV/1V 2k",
        "",
        ""
    },
    &NRST_ADD_STRETCH_20mV_B, -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - Доп смещ 50мВ 2к -------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_Bk50mV =
{
    Item_Governor, &pppADC_Stretch, 0,
    {
        "50мВ 2к", "50mV 2k",
        "",
        ""
    },
    &NRST_ADD_STRETCH_50mV_B, -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 2к ------------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_Bk100mV =
{
    Item_Governor, &pppADC_Stretch, 0,
    {
        "100мВ/5В 2к", "100mV/5V 2k",
        "",
        ""
    },
    &NRST_ADD_STRETCH_100mV_B, -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 2В 2к ------------------------------------------------------------------------------------------------------------------
static const Governor gADC_Stretch_Bk2V =
{
    Item_Governor, &pppADC_Stretch, 0,
    {
        "2В 2к", "2V 2ch",
        "",
        ""
    },
    &NRST_ADD_STRETCH_2V_B, -10000, 10000
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pppADC_Shift =
{
    Item_Page, &ppADC, 0,
    {
        "ДОП СМЕЩ", "ADD RSHFIT",
        "",
        ""
    },
    Page_DebugADCrShift,
    {
        (void*)&bADC_Shift_Reset,    // ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс
        (void*)&gADC_Shift_A2mV,     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост
        (void*)&gADC_Shift_B2mV,     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост
        (void*)&gADC_Shift_A5mV,     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост
        (void*)&gADC_Shift_B5mV,     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост
        (void*)&gADC_Shift_A10mV,    // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост
        (void*)&gADC_Shift_B10mV     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост
    }
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс ------------------------------------------------------------------------------------------------------------------
static const Button bADC_Shift_Reset =
{
    Item_Button, &pppADC_Shift, 0,
    {
        "Сброс", "Reset",
        "",
        ""
    },
    OnPress_ADC_Shift_Reset
};

static void OnPress_ADC_Shift_Reset(void)
{
    for (int ch = 0; ch < 2; ch++)
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < RangeSize; range++)
            {
                NRST_RSHIFT_ADD(ch, range, mode) = 0;
            }
        }
    }
    FPGA_SetRShift(A, SET_RSHIFT_A);
    FPGA_SetRShift(B, SET_RSHIFT_B);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor gADC_Shift_A2mV =
{
    Item_Governor, &pppADC_Shift, 0,
    {
        "См 1к 2мВ пост", "Shift 1ch 2mV DC",
        "",
        ""
    },
    (int16*)(&NRST_RSHIFT_ADD_A(Range_2mV, ModeCouple_DC)), -100, 100, OnChange_ADC_Shift_A
};

static void OnChange_ADC_Shift_A(void)
{
    FPGA_SetRShift(A, SET_RSHIFT_A);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor gADC_Shift_B2mV =
{
    Item_Governor, &pppADC_Shift, 0,
    {
        "См 2к 2мВ пост", "Shift 2ch 2mV DC",
        "",
        ""
    },
    (int16*)(&NRST_RSHIFT_ADD_B(Range_2mV, ModeCouple_DC)), -100, 100, OnChange_ADC_Shift_B
};

static void OnChange_ADC_Shift_B(void)
{
    FPGA_SetRShift(B, SET_RSHIFT_B);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor gADC_Shift_A5mV =
{
    Item_Governor, &pppADC_Shift, 0,
    {
        "См 1к 5мВ пост", "Shift 1ch 5mV DC",
        "",
        ""
    },
    (int16*)(&NRST_RSHIFT_ADD_A(Range_5mV, ModeCouple_DC)), -100, 100, OnChange_ADC_Shift_A
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor gADC_Shift_B5mV =
{
    Item_Governor, &pppADC_Shift, 0,
    {
        "См 2к 5мВ пост", "Shift 2ch 5mV DC",
        "",
        ""
    },
    (int16*)(&NRST_RSHIFT_ADD_B(Range_5mV, ModeCouple_DC)), -100, 100, OnChange_ADC_Shift_B
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост --------------------------------------------------------------------------------------------------------
static const Governor gADC_Shift_A10mV =
{
    Item_Governor, &pppADC_Shift, 0,
    {
        "См 1к 10мВ пост", "Shift 1ch 10mV DC",
        "",
        ""
    },
    (int16*)(&NRST_RSHIFT_ADD_A(Range_10mV, ModeCouple_DC)), -100, 100, OnChange_ADC_Shift_A
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост --------------------------------------------------------------------------------------------------------
static const Governor gADC_Shift_B10mV =
{
    Item_Governor, &pppADC_Shift, 0,
    {
        "См 2к 10мВ пост", "Shift 2ch 10mV DC",
        "",
        ""
    },
    (int16*)(&NRST_RSHIFT_ADD_B(Range_10mV, ModeCouple_DC)), -100, 100, OnChange_ADC_Shift_B
};

// ОТЛАДКА - РАНД-ТОР ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppRand =
{
    Item_Page, &mpDebug, 0,
    {
        "РАНД-ТОР", "RANDOMIZER",
        "",
        ""
    },
    Page_DebugRandomizer,
    {
        (void*)&gRand_NumAverage,          // ОТЛАДКА - РАНД-ТОР - Усредн.
        (void*)&gRand_NumSmooth,           // ОТЛАДКА - РАНД-ТОР - Сглаживание
        (void*)&gRand_NumMeasures,         // ОТЛАДКА - РАНД-ТОР - Измерений
        (void*)&cRand_ShowInfo,            // ОТЛАДКА - РАНД-ТОР - Информация
        (void*)&gRand_ShowStat,            // ОТЛАДКА - РАНД-ТОР - Статистика
        (void*)&gRand_TimeCompensation,    // ОТЛАДКА - РАНД-ТОР - Компенсация задержки
        (void*)&gRand_AddTimeShift,        // ОТЛАДКА - РАНД-ТОР - Смещение
        (void*)&gRand_Pretriggered         // ОТЛАДКА - РAНД-ТОР - Предзапуск
    }
};

// ОТЛАДКА - КАНАЛЫ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppChannels =
{
    Item_Page, &mpDebug, 0,
    {
        "КАНЛАЫ", "CHANNELS",
        "",
        ""
    },
    Page_DebugChannels,
    {
        (void*)&cChannels_BandwidthA,  // ОТЛАДКА - КАНАЛЫ - Полоса 1
        (void*)&cChannels_BandwidthB   // ОТЛАДКА - КАНАЛЫ - Полоса 2
    }
};

// ОТЛАДКА - КАНЛАЫ - Полоса 1 -----------------------------------------------------------------------------------------------------------------------
static const Choice cChannels_BandwidthA =
{
    Item_Choice, &ppChannels, 0,
    {
        "Полоса 1", "Bandwidth 1",
        "Здесь можно выбрать полосу, которая будет действовать в КАНАЛ1-Полоса при выборе значения Полная",
        "Here you can select the bandwidth, which will operate in CHANNEL1-Bandwidth when set to Full"
    },
    {
        {"Полная", "Full"},
        {"20МГц", "20MHz"},
        {"100МГц", "100MHz"},
        {"200МГц", "200MHz"},
        {"350МГц", "350MHz"},
        {"650МГц", "650MHz"},
        {"750МГц", "750MHz"}
    },
    (int8*)&BANDWIDTH_DEBUG(A), OnChange_Channels_BandwidthA
};

static void OnChange_Channels_BandwidthA(bool active)
{
    FPGA_SetBandwidth(A);
}

// ОТЛАДКА - КАНЛАЫ - Полоса 1 -----------------------------------------------------------------------------------------------------------------------
static const Choice cChannels_BandwidthB =
{
    Item_Choice, &ppChannels, 0,
    {
        "Полоса 2", "Bandwidth 2",
        "Здесь можно выбрать полосу, которая будет действовать в КАНАЛ2-Полоса при выборе значения Полная",
        "Here you can select the bandwidth, which will operate in CHANNEL2-Bandwidth when set to Full"
    },
    {
        {"Полная", "Full"},
        {"20МГц", "20MHz"},
        {"100МГц", "100MHz"},
        {"200МГц", "200MHz"},
        {"350МГц", "350MHz"},
        {"650МГц", "650MHz"},
        {"750МГц", "750MHz"}
    },
    (int8*)&BANDWIDTH_DEBUG(B), OnChange_Channels_BandwidthB
};

static void OnChange_Channels_BandwidthB(bool active)
{
    FPGA_SetBandwidth(B);
}

// ОТЛАДКА - РАНД-ТОР - Измерений --------------------------------------------------------------------------------------------------------------------
static const Governor gRand_NumMeasures =
{
    Item_Governor, &ppRand, 0,
    {
        "Выб-к/ворота", "Samples/gates",
        "",
        ""
    },
    &NUM_MEASURES_FOR_GATES, 1, 2500, OnChange_Rand_NumMeasures
};

static void OnChange_Rand_NumMeasures(void)
{
    FPGA_SetNumberMeasuresForGates(NUM_MEASURES_FOR_GATES);
}

// ОТЛАДКА - РАНД-ТОР - Компенсация задержки ---------------------------------------------------------------------------------------------------------
static const Governor gRand_TimeCompensation =
{
    Item_Governor, &ppRand, 0,
    {
        "Компенсация задержки", "Compenstaion time",
        "Подстройка компенсации задержки АЦП 40 нс",
        ""
    },
    &TIME_COMPENSATION, 0, 510, OnChange_Rand_TimeCompensation
};

static void OnChange_Rand_TimeCompensation(void)
{
    FPGA_SetDeltaTShift(TIME_COMPENSATION);
}

// ОТЛАДКА - РАНД-ТОР - Смещение ---------------------------------------------------------------------------------------------------------------------
int16 addShift = 0;

static const Governor gRand_AddTimeShift =
{
    Item_Governor, &ppRand, 0,
    {
        "Доп смещение", "Add shift",
        "Добавочное смщение при вращении tShift",
        ""
    },
    &addShift, -100, 100, OnChange_Rand_AddTimeShift
};

static void OnChange_Rand_AddTimeShift(void)
{
    FPGA_SetTShift(SET_TSHIFT);
}

// ОТЛАДКА - РАНД-ТОР - Предзапуск -------------------------------------------------------------------------------------------------------------------
static const Governor gRand_Pretriggered =
{
    Item_Governor, &ppRand, 0,
    {
        "Предзапуск", "Pretiggered",
        "Величина предзапуска, которая пишется в рандомизатор",
        ""
    },
    &PRETRIGGERED, 0, 30000, OnChange_Rand_Pretriggered
};

static void OnChange_Rand_Pretriggered(void)
{
    LoadTShift();
}

// ОТЛАДКА - РАНД-ТОР - Усредн. ----------------------------------------------------------------------------------------------------------------------
static const Governor gRand_NumAverage =
{
    Item_Governor, &ppRand, 0,
    {
        "Усредн.", "Average",
        "",
        ""
    },
    &NRST_NUM_AVE_FOR_RAND, 1, 32
};

// ОТЛАДКА - РАНД-ТОР - Сглаживание ------------------------------------------------------------------------------------------------------------------
static const Governor gRand_NumSmooth =
{
    Item_Governor, &ppRand, 0,
    {
        "Сглаживание", "Smoothing",
        "",
        ""
    },
    &NRST_NUM_SMOOTH_FOR_RAND, 1, 10
};

static int16 pred;
static int16 post;

static const Governor mgPred =
{
    Item_Governor, &mpDebug, 0,
    {
        "Предзапуск", "",
        "", ""
    },
    &pred, 0, 15000, OnChange_Pred
};

static const Governor mgPost =
{
    Item_Governor, &mpDebug, 0,
    {
        "Послезапуск", "",
        "", ""
    },
    &post, 0, 15000, OnChange_Post
};


// ОТЛАДКА - Настройки ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppShowSettingsInfo =
{
    Item_Page, &mpDebug, 0,
    {
        "Настройки", "Settings",
        "Показать информацию о настройках",
        "Show settings information"
    },
    Page_SB_DebugShowSetInfo,
    {
        (void*)&bExitShowSetInfo
    },
    OnPress_ShowInfo
};

static void PressSB_ShowSetInfo_Exit(void)
{
    Display_SetDrawMode(DrawMode_Auto, 0);
}

static const SmallButton bExitShowSetInfo =
{
    Item_SmallButton, &ppShowSettingsInfo,
    COMMON_BEGIN_SB_EXIT,
    PressSB_ShowSetInfo_Exit,
    DrawSB_Exit
};

static void DebugShowSetInfo_Draw(void)
{
    Painter_BeginScene(gColorBack);
    Painter_DrawRectangleC(0, 0, 319, 239, gColorFill);

    int x0 = 30;
    int y0 = 25;
    int dY = 10;
    int y = y0 - dY;

#define Y_AND_INCREASE (y += dY, y)
#define DRAW_TEXT(str)                  Painter_DrawText(x0, Y_AND_INCREASE, str);
#define DRAW_FORMAT(str, value)         Painter_DrawFormatText(x0, Y_AND_INCREASE, str, value)
#define DRAW_FORMAT2(str, val1, val2)   Painter_DrawFormatText(x0, Y_AND_INCREASE, str, val1, val2);

    //Painter_DrawFormatText(x0, Y_AND_INCREASE, "Размер основной структуры %d", sizeof(set));
    DRAW_FORMAT("Размер основной структуры : %d", sizeof(set));
    Painter_DrawText(x0, Y_AND_INCREASE, "Несбрасываемая структура:");
    int x = Painter_DrawText(x0, Y_AND_INCREASE, "rShiftAdd :") + 5;

    int ddY = 0;

    for (int type = 0; type < 2; type++)
    {
        for (int ch = 0; ch < 2; ch++)
        {
            for (int range = 0; range < RangeSize; range++)
            {
                Painter_DrawFormatText(x + range * 20, y + dY * ddY, "%d", NRST_RSHIFT_ADD(ch, range, type));
            }
            ddY++;
        }
    }

    y += dY * 3;

    DRAW_FORMAT("correctionTime : %d", NRST_CORRECTION_TIME);
    DRAW_FORMAT2("balanceADC : %d %d", NRST_BALANCE_ADC_A, NRST_BALANCE_ADC_B);
    DRAW_FORMAT("numAveForRand : %d", NRST_NUM_AVE_FOR_RAND);

    const char *s[3] = {"выключено", "настроено автоматически", "задано вручную"};
    DRAW_FORMAT("balanceADCtype : %s", (NRST_BALANCE_ADC_TYPE < 3 ? s[NRST_BALANCE_ADC_TYPE] : "!!! неправильное значение !!!"));
    DRAW_FORMAT("stretchADCtype : %s", (NRST_STRETCH_ADC_TYPE < 3 ? s[NRST_STRETCH_ADC_TYPE] : "!!! неправильное значение !!!"));
  
    x = Painter_DrawText(x0, Y_AND_INCREASE, "stretchADC :") + 5;

    for (int ch = 0; ch < 2; ch++)
    {
        for (int num = 0; num < 3; num++)
        {
            Painter_DrawFormatText(x + num * 20, y + dY * ch, "%d", NRST_STRETCH_ADC(ch, num));
        }
    }

    y += dY;

#define DRAW_STRETCH(name) DRAW_FORMAT2(#name " : %d %d", set.nrst_##name[0], set.nrst_##name[1])

    DRAW_STRETCH(AddStretch20mV);
    DRAW_STRETCH(AddStretch50mV);
    DRAW_STRETCH(AddStretch100mV);
    DRAW_STRETCH(AddStretch2V);

    DRAW_FORMAT("numSmoothForRand : %d", NRST_NUM_SMOOTH_FOR_RAND);

    Menu_Draw();
    Painter_EndScene();
}

static void OnPress_ShowInfo(void)
{
    OpenPageAndSetItCurrent(Page_SB_DebugShowSetInfo);
    Display_SetDrawMode(DrawMode_Auto, DebugShowSetInfo_Draw);
}

static void OnChange_Pred(void)
{
    gPred = ~pred;
    static char buffer[30];
    LOG_WRITE("pred %d %s", pred, Hex16toString(gPred, buffer, true));
}

static void OnChange_Post(void)
{
    gPost = ~post;
    static char buffer[30];
    LOG_WRITE("post %d %s", post, Hex16toString(gPost, buffer, true));
}

// ОТЛАДКА - РАНД-ТОР - Информация -------------------------------------------------------------------------------------------------------------------
static const Choice cRand_ShowInfo =
{
    Item_Choice, &ppRand, 0,
    {
        "Информация", "Information",
        "Показывать информацию о воротах рандомизатора",
        "To show information on randomizer gate"
    },
    {
        {"Не показывать", "Hide"},
        {"Показывать", "Show"}
    },
    (int8*)&SHOW_RAND_INFO
};

// ОТЛАДКА - РАНД-ТОР - Статистика -------------------------------------------------------------------------------------------------------------------
static const Choice gRand_ShowStat =
{
    Item_Choice, &ppRand, 0,
    {
        "Статистика", "Statistics",
        "Показывать график статистики",
        "Statistics show schedule"
    },
    {
        {"Не показывать",   "Hide"},
        {"Показывать",      "Show"}
    },
    (int8*)&SHOW_RAND_STAT
};

// ОТЛАДКА - ЭМС /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Choice cEMS =
{
    Item_Choice, &mpDebug, 0,
    {
        "Режим ЭМС", "EMS mode",
        "Принудительно включает фильтр 20МГц, сглаживание по 4-м точкам, усреднение по 8-ми точкам",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&MODE_EMS, OnChange_EMS
};

static void OnChange_EMS(bool active)
{
    FPGA_SetBandwidth(A);
    FPGA_SetBandwidth(B);
}

// ОТЛАДКА - Ориентация //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Choice cDisplayOrientation =
{
    Item_Choice, &mpDebug, 0,
    {
        "Ориентация", "DisplayOrientation",
        "Устанавливает ориентацию дисплея",
        "Sets display orientation"
    },
    {
        { "Прямая", "Direct" },
        { "Обратная", "Back" }
    },
    (int8*)&DISPLAY_ORIENTATION, OnChange_DisplayOrientation
};

void OnChange_DisplayOrientation(bool active)
{
    Display_SetOrientation(DISPLAY_ORIENTATION);
}





















/*
void OnPressDebugDisable(void)
{
    ShowMenu(false);
    SetMenuPageDebugActive(false);
    SetMenuPosActItem(Page_MainPage, 0);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void OnDegubConsoleViewChanged(bool active)
{
    Display_SetPauseForConsole(CONSOLE_IN_PAUSE);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void OnPressDebugConsoleUp(void)
{
    Display_OneStringUp();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void OnPressDebugConsoleDown(void)
{
    Display_OneStringDown();
}

        /// ОТЛАДКА - АЦП - ДОП. СМЕЩ. ПАМ. - Величина
        const Governor mgDebugADCaltShift =
        {
            Item_Governor, &mspDebugADCaltShift,
            {
                "Величина", "Value"
            },
            {
                "",
                ""
            },
            0,
            &set.debug.altShift, -2, 2, 0
        };

bool sIsShowReg_RShift1(void)
{
    return set.debug.showRegisters.rShiftB || set.debug.showRegisters.all;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sIsShowReg_TrigLev(void)
{
    return set.debug.showRegisters.trigLev || set.debug.showRegisters.all;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sIsShowReg_Range(Channel ch)
{
    return set.debug.showRegisters.range[ch] || set.debug.showRegisters.all;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sIsShowReg_TrigParam(void)
{
    return set.debug.showRegisters.trigParam || set.debug.showRegisters.all;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sIsShowReg_ChanParam(Channel ch)
{
    return set.debug.showRegisters.chanParam[ch] || set.debug.showRegisters.all;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sIsShowReg_TShift(void)
{
    return set.debug.showRegisters.tShift || set.debug.showRegisters.all;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sIsShowReg_TBase(void)
{
    return set.debug.showRegisters.tBase || set.debug.showRegisters.all;
}


const Page mspDebugADCaltShift =    // ОТЛАДКА - АЦП - ДОП СМЕЩ ПАМ
{
    Item_Page, &ppADC,
    {
        "ДОП СМЕЩ ПАМ", "ALT SHIFT MEM"
    },
    {
        "",
        ""
    },
    0, Page_DebugADCaltShift,
    {
        (void*)&mgDebugADCaltShift
    }
};
*/
