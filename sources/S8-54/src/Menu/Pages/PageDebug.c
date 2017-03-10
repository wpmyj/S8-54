#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGAtypes.h"
#include "Hardware/FSMC.h"
#include "Hardware/Sound.h"
#include "Log.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;
extern void LoadTShift(void);

static const Choice mcStats;

static const Page mspConsole;
static const Governor mgConsole_NumStrings;
static const Choice mcConsole_SizeFont;
static const Choice mcConsole_ModeStop;

static const Page mspConsole_Registers;
static const Choice mcConsole_Registers_ShowAll;
static const Choice mcConsole_Registers_RD_FL;
static const Choice mcConsole_Registers_RShiftA;
static const Choice mcConsole_Registers_RShiftB;
static const Choice mcConsole_Registers_TrigLev;
static const Choice mcConsole_Registers_RangeA;
static const Choice mcConsole_Registers_RangeB;
static const Choice mcConsole_Registers_TrigParam;
static const Choice mcConsole_Registers_ChanParamA;
static const Choice mcConsole_Registers_ChanParamB;
static const Choice mcConsole_Registers_TBase;
static const Choice mcConsole_Registers_TShift;

static const Button mbConsole_SizeSettings;
static void FuncDrawSizeSettings(int, int);

static const Page mspADC;

static const Page mspADC_Balance;
static const Choice mcADC_Balance_Mode;
static const Governor mgADC_Balance_ShiftA;
static const Governor mgADC_Balance_ShiftB;

static const Page mspADC_Stretch;
static const Choice mcADC_Stretch_Mode;
static const Governor mgADC_Stretch_A;
static const Governor mgADC_Stretch_B;
static const Governor mgADC_Stretch_Ak20mV;
static const Governor mgADC_Stretch_Ak50mV;
static const Governor mgADC_Stretch_Ak100mV;
static const Governor mgADC_Stretch_Ak2V;
static const Governor mgADC_Stretch_Bk20mV;
static const Governor mgADC_Stretch_Bk50mV;
static const Governor mgADC_Stretch_Bk100mV;
static const Governor mgADC_Stretch_Bk2V;

static const Page mspADC_Shift;
static const Button mbADC_Shift_Reset;
static const Governor mgADC_Shift_A2mV;
static const Governor mgADC_Shift_B2mV;
static const Governor mgADC_Shift_A5mV;
static const Governor mgADC_Shift_B5mV;
static const Governor mgADC_Shift_A10mV;
static const Governor mgADC_Shift_B10mV;

static const Page mspRand;
static const Governor mgRand_NumMeasures;
static const Governor mgRand_NumAverage;
static const Governor mgRand_NumSmooth;
static const Choice mcRand_ShowInfo;
static const Choice mgRand_ShowStat;
static const Governor mgRand_TimeCompensation;
static void OnChange_Rand_TimeCompensation(void);
static const Governor mgRand_AddTimeShift;
static void OnChange_Rand_AddTimeShift(void);
static const Governor mgRand_Pretriggered;
static void OnChange_Rand_Pretriggered(void);

static const Page mspChannels;
static const Choice mcChannels_BandwidthA;
static const Choice mcChannels_BandwidthB;
static void OnChange_Channels_BandwidthA(bool active);
static void OnChange_Channels_BandwidthB(bool active);

static const Choice mcEMS;
static void OnChange_EMS(bool);

static const Choice mcDisplayOrientation;
       void OnChange_DisplayOrientation(bool);

static const Governor mgPred;
static void OnChange_Pred(void);
static const Governor mgPost;
static void OnChange_Post(void);

static bool IsActive_Registers(void);

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
        (void*)&mspConsole,             // ОТЛАДКА - КОНСОЛЬ
        (void*)&mspADC,                 // ОТЛАДКА - АЦП
        (void*)&mspRand,                // ОТЛАДКА - РАНД-ТОР
        (void*)&mspChannels,            // ОТЛАДКА - КАНАЛЫ
        (void*)&mcStats,                // ОТЛАДКА - Статистика
        (void*)&mcDisplayOrientation,   // ОТЛАДКА - Ориентация
        (void*)&mcEMS,                  // ОТЛАДКА - ЭМС
        (void*)&mgPred,                 // ОТЛАДКА - Предзапуск
        (void*)&mgPost                  // ОТЛАДКА - Послезапуск
    }
};

// ОТЛАДКА - Статистика ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcStats =
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

// ОТЛАДКА - КОНСОЛЬ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspConsole =
{
    Item_Page, &mpDebug, 0,
    {
        "КОНСОЛЬ", "CONSOLE",
        "",
        ""
    },
    Page_DebugConsole,
    {
        (void*)&mgConsole_NumStrings,   // ОТЛАДКА - КОНСОЛЬ - Число строк
        (void*)&mcConsole_SizeFont,     // ОТЛАДКА - КОНСОЛЬ - Размер шрифта
        (void*)&mcConsole_ModeStop,     // ОТЛАДКА - КОНСОЛЬ - Реж. останова
        (void*)&mspConsole_Registers,   // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ
        (void*)&mbConsole_SizeSettings  // ОТЛАДКА - КОНСОЛЬ - Разм. настроек

    }
};

// ОТЛАДКА - КОНСОЛЬ - Число строк -------------------------------------------------------------------------------------------------------------------
static const Governor mgConsole_NumStrings =
{
    Item_Governor, &mspConsole, 0,
    {
        "Число строк", "Number strings",
        "",
        ""
    },
    &CONSOLE_NUM_STRINGS, 0, 33
};

// ОТЛАДКА - КОНСОЛЬ - Размер шрифта -----------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_SizeFont =
{
    Item_Choice, &mspConsole, 0,
    {
        "Размер шрифта", "Size font",
        "",
        ""
    },
    {
        {"5", "5"},
        {"8", "8"}
    },
    &set.debug.sizeFont
};

// ОТЛАДКА - КОНСОЛЬ - Реж. останова -----------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_ModeStop =
{
    Item_Choice, &mspConsole, 0,
    {
        "Реж. останова", "Mode stop",
        "Предоставляет возможность приостановки вывода в консоль путём нажатия на кнопку ПУСК/СТОП",
        "It provides the ability to pause the output to the console by pressing the ПУСК/СТОП button"
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.modePauseConsole
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspConsole_Registers =
{
    Item_Page, &mspConsole, 0,
    {
        "РЕГИСТРЫ", "REGISTERS",
        "",
        ""
    },
    Page_DebugShowRegisters,
    {
        (void*)&mcConsole_Registers_ShowAll,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все
        (void*)&mcConsole_Registers_RD_FL,        // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL
        (void*)&mcConsole_Registers_RShiftA,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к
        (void*)&mcConsole_Registers_RShiftB,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к
        (void*)&mcConsole_Registers_TrigLev,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр
        (void*)&mcConsole_Registers_RangeA,       // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1
        (void*)&mcConsole_Registers_RangeB,       // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2
        (void*)&mcConsole_Registers_TrigParam,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр.
        (void*)&mcConsole_Registers_ChanParamA,   // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 1
        (void*)&mcConsole_Registers_ChanParamB,   // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2
        (void*)&mcConsole_Registers_TBase,        // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ
        (void*)&mcConsole_Registers_TShift        // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см.
    }
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все -----------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ShowAll =
{
    Item_Choice, &mspConsole_Registers, 0,
    {
        "Показывать все", "Show all",
        "Показывать все значения, засылаемые в регистры",
        "To show all values transferred in registers"
    },
    {
        {"Нет", "No"},
        {"Да", "Yes"}
    },
    (int8*)&set.debug.show.all
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL --------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RD_FL =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "RD_FL", "RD_FL",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.flag
};

static bool IsActive_Registers(void)
{
    return set.debug.show.all;
}

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к -----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RShiftA =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "U см. 1к", "U shift 1ch",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.rShift[A]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к -----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RShiftB =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "U см. 2к", "U shift 2ch",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.range[B]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр. -----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TrigLev =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "U синхр.", "U trig.",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.trigLev
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1 --------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RangeA =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "ВОЛЬТ/ДЕЛ 1", "Range 1",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.range[A]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2 --------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RangeB =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "ВОЛЬТ/ДЕЛ 2", "Range 2",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.range[B]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр. ------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TrigParam =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "Парам. синхр.", "Trig param",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.trigParam
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 1 ------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ChanParamA =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "Парам. кан. 1", "Chan 1 param",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.chanParam[A]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2 ------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ChanParamB =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "Парам. кан. 2", "Chan 2 param",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.chanParam[B]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ ----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TBase =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "ВРЕМЯ/ДЕЛ", "TBase",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.tBase
};


// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см. --------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TShift =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "Т см.", "tShift",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.tShift
};


// ОТЛАДКА - КОНСОЛЬ - Разм.настроек -----------------------------------------------------------------------------------------------------------------
static const Button mbConsole_SizeSettings =
{
    Item_Button, &mspConsole, 0,
    {
        "", "",
        "Показывает текущий размер структуры для сохранения настроек",
        "Displays the current size of the structure to save settings"
    },
    0, FuncDrawSizeSettings
};

static void FuncDrawSizeSettings(int x, int y)
{
    char buffer[30];
    sprintf(buffer, "Разм.настр. %d", sizeof(Settings));
    Painter_DrawTextC(x + 6, y + 13, buffer, gColorBack);
    LOG_WRITE(buffer);
}

// ОТЛАДКА - АЦП /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspADC =
{
    Item_Page, &mpDebug, 0,
    {
        "АЦП", "ADC",
        "",
        ""
    },
    Page_DebugADC,
    {
        (void*)&mspADC_Balance,   // ОТЛАДКА - АЦП - БАЛАНС
        (void*)&mspADC_Stretch,   // ОТЛАДКА - АЦП - РАСТЯЖКА
        (void*)&mspADC_Shift      // ОТЛАДКА - АЦП - ДОП СМЕЩ
    }
};

// ОТЛАДКА - АЦП - БАЛАНС ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspADC_Balance =
{
    Item_Page, &mspADC, 0,
    {
        "БАЛАНС", "BALANCE",
        "",
        ""
    },
    Page_DebugADCbalance,
    {
        (void*)&mcADC_Balance_Mode,   // ОТЛАДКА - АЦП - БАЛАНС - Режим
        (void*)&mgADC_Balance_ShiftA, // ОТЛАДКА - АЦП - БАЛАНС - Смещение 1
        (void*)&mgADC_Balance_ShiftB  // ОТЛАДКА - АЦП - БАЛАНС - Смещение 2
    }
};

// ОТЛАДКА - АЦП - БАЛАНС - Режим --------------------------------------------------------------------------------------------------------------------
static const Choice mcADC_Balance_Mode =
{
    Item_Choice, &mspADC_Balance, 0,
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
    (int8*)&setNR.balanceADCtype, OnChange_ADC_Balance_Mode, OnDraw_ADC_Balance_Mode
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
        {0, BALANCE_ADC_A, (int8)setNR.balanceADC[0]},
        {0, BALANCE_ADC_B, (int8)setNR.balanceADC[1]}
    };

    shiftADCA = shift[A][setNR.balanceADCtype];
    shiftADCB = shift[B][setNR.balanceADCtype];
}

// ОТЛАДКА - АЦП - БАЛАНС - Смещение 1 ---------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Balance_ShiftA =
{
    Item_Governor, &mspADC_Balance, IsActive_ADC_Balance,
    {
        "Смещение 1", "Offset 1",
        "",
        ""
    },
    &shiftADCA, -125, 125, OnChange_ADC_BalanceA
};

static bool IsActive_ADC_Balance(void)
{
    return setNR.balanceADCtype == BalanceADC_Hand;
}

static void OnChange_ADC_BalanceA(void)
{
    setNR.balanceADC[A] = shiftADCA;
}

// ОТЛАДКА - АЦП - БАЛАНС - Смещение 2 ---------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Balance_ShiftB =
{
    Item_Governor, &mspADC_Balance, IsActive_ADC_Balance,
    {
        "Смещение 2", "Offset 2",
        "",
        ""
    },
    &shiftADCB, -125, 125, OnChange_ADC_BalanceB
};

static void OnChange_ADC_BalanceB(void)
{
    setNR.balanceADC[B] = shiftADCB;
}

static const Choice emptyChoice;

// ОТЛАДКА - АЦП - РАСТЯЖКА //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspADC_Stretch =
{
    Item_Page, &mspADC, 0,
    {
        "РАСТЯЖКА", "STRETCH",
        "Устанавливает режим и величину растяжки (для ручного режима)",
        "Sets mode and the value of stretching (manual mode)"
    },
    Page_DebugADCstretch,
    {
        (void*)&mcADC_Stretch_Mode,       // ОТЛАДКА - АЦП - РАСТЯЖКА - Режим
        (void*)&mgADC_Stretch_A,          // ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 1к
        (void*)&mgADC_Stretch_B,          // ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 2к
        (void*)&emptyChoice,
        (void*)&emptyChoice,
        (void*)&mgADC_Stretch_Ak20mV,     // ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 1к
        (void*)&mgADC_Stretch_Ak50mV,     // ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 1к 
        (void*)&mgADC_Stretch_Ak100mV,    // ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 1к
        (void*)&mgADC_Stretch_Ak2V,       // ОТЛАДКА - AЦП - РАСТЯЖКА - 2В 1к
        (void*)&emptyChoice,
        (void*)&mgADC_Stretch_Bk20mV,     // ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 2к
        (void*)&mgADC_Stretch_Bk50mV,     // ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 2к 
        (void*)&mgADC_Stretch_Bk100mV,    // ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 2к
        (void*)&mgADC_Stretch_Bk2V,       // ОТЛАДКА - АЦП - РАСТЯЖКА - 2В 2к
        (void*)&emptyChoice
    }
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - Режим ------------------------------------------------------------------------------------------------------------------
static const Choice mcADC_Stretch_Mode =
{
    Item_Choice, &mspADC_Stretch, 0,
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
    (int8*)&setNR.stretchADCtype, OnChange_ADC_Stretch_Mode
};

static int16 stretchA;
static int16 stretchB;  

void OnChange_ADC_Stretch_Mode(bool active)
{
    if (setNR.stretchADCtype == StretchADC_Disable)
    {
        stretchA = setNR.stretchADC[A][StretchADC_Disable] = 0;
        stretchB = setNR.stretchADC[B][StretchADC_Disable] = 0;
    }
    else
    {
        stretchA = setNR.stretchADC[A][setNR.stretchADCtype];
        stretchB = setNR.stretchADC[B][setNR.stretchADCtype];
    }
}

// ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 1к ------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_A =
{
    Item_Governor, &mspADC_Stretch, IsActive_ADC_StretchAB,
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
    return setNR.stretchADCtype == StretchADC_Hand;
}

static void OnChange_ADC_Stretch_A(void)
{
    setNR.stretchADC[A][setNR.stretchADCtype] = stretchA;
}

// ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 2к ------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_B =
{
    Item_Governor, &mspADC_Stretch, IsActive_ADC_StretchAB,
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
    setNR.stretchADC[B][setNR.stretchADCtype] = stretchB;
}

// ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 1к -------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Ak20mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "20мВ/1В 1к", "20mV/1V 1k",
        "",
        ""
    },
    &setNR.addStretch20mV[A], -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 1к ----------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Ak50mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "50мВ 1к", "50mV 1k",
        "",
        ""
    },
    &setNR.addStretch50mV[A], -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 1к ------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Ak100mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "100мВ/5В 1к", "100mV/5V 1ch",
        "",
        ""
    },
    &setNR.addStretch100mV[A], -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 2В 1к ------------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Ak2V =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "2В 1к", "2V 1ch",
        "",
        ""
    },
    &setNR.addStretch2V[A], -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 2к -------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Bk20mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "20мВ/1В 2к", "20mV/1V 2k",
        "",
        ""
    },
    &setNR.addStretch20mV[B], -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - Доп смещ 50мВ 2к -------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Bk50mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "50мВ 2к", "50mV 2k",
        "",
        ""
    },
    &setNR.addStretch50mV[B], -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 2к ------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Bk100mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "100мВ/5В 2к", "100mV/5V 2k",
        "",
        ""
    },
    &setNR.addStretch100mV[B], -10000, 10000
};

// ОТЛАДКА - АЦП - РАСТЯЖКА - 2В 2к ------------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Bk2V =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "2В 2к", "2V 2ch",
        "",
        ""
    },
    &setNR.addStretch2V[B], -10000, 10000
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspADC_Shift =
{
    Item_Page, &mspADC, 0,
    {
        "ДОП СМЕЩ", "ADD RSHFIT",
        "",
        ""
    },
    Page_DebugADCrShift,
    {
        (void*)&mbADC_Shift_Reset,    // ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс
        (void*)&mgADC_Shift_A2mV,     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост
        (void*)&mgADC_Shift_B2mV,     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост
        (void*)&mgADC_Shift_A5mV,     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост
        (void*)&mgADC_Shift_B5mV,     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост
        (void*)&mgADC_Shift_A10mV,    // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост
        (void*)&mgADC_Shift_B10mV     // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост
    }
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс ------------------------------------------------------------------------------------------------------------------
static const Button mbADC_Shift_Reset =
{
    Item_Button, &mspADC_Shift, 0,
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
                setNR.rShiftAdd[ch][range][mode] = 0;
            }
        }
    }
    FPGA_SetRShift(A, RSHIFT_A);
    FPGA_SetRShift(B, RSHIFT_B);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_A2mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "См 1к 2мВ пост", "Shift 1ch 2mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[A][Range_2mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_A
};

static void OnChange_ADC_Shift_A(void)
{
    FPGA_SetRShift(A, RSHIFT_A);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_B2mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "См 2к 2мВ пост", "Shift 2ch 2mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[B][Range_2mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_B
};

static void OnChange_ADC_Shift_B(void)
{
    FPGA_SetRShift(B, RSHIFT_B);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_A5mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "См 1к 5мВ пост", "Shift 1ch 5mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[A][Range_5mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_A
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_B5mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "См 2к 5мВ пост", "Shift 2ch 5mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[B][Range_5mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_B
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост --------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_A10mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "См 1к 10мВ пост", "Shift 1ch 10mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[A][Range_10mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_A
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост --------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_B10mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "См 2к 10мВ пост", "Shift 2ch 10mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[B][Range_10mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_B
};

// ОТЛАДКА - РАНД-ТОР ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspRand =
{
    Item_Page, &mpDebug, 0,
    {
        "РАНД-ТОР", "RANDOMIZER",
        "",
        ""
    },
    Page_DebugRandomizer,
    {
        (void*)&mgRand_NumAverage,          // ОТЛАДКА - РАНД-ТОР - Усредн.
        (void*)&mgRand_NumSmooth,           // ОТЛАДКА - РАНД-ТОР - Сглаживание
        (void*)&mgRand_NumMeasures,         // ОТЛАДКА - РАНД-ТОР - Измерений
        (void*)&mcRand_ShowInfo,            // ОТЛАДКА - РАНД-ТОР - Информация
        (void*)&mgRand_ShowStat,            // ОТЛАДКА - РАНД-ТОР - Статистика
        (void*)&mgRand_TimeCompensation,    // ОТЛАДКА - РАНД-ТОР - Компенсация задержки
        (void*)&mgRand_AddTimeShift,        // ОТЛАДКА - РАНД-ТОР - Смещение
        (void*)&mgRand_Pretriggered         // ОТЛАДКА - РAНД-ТОР - Предзапуск
    }
};

// ОТЛАДКА - КАНАЛЫ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspChannels =
{
    Item_Page, &mpDebug, 0,
    {
        "КАНЛАЫ", "CHANNELS",
        "",
        ""
    },
    Page_DebugChannels,
    {
        (void*)&mcChannels_BandwidthA,  // ОТЛАДКА - КАНАЛЫ - Полоса 1
        (void*)&mcChannels_BandwidthB   // ОТЛАДКА - КАНАЛЫ - Полоса 2
    }
};

// ОТЛАДКА - КАНЛАЫ - Полоса 1 -----------------------------------------------------------------------------------------------------------------------
static const Choice mcChannels_BandwidthA =
{
    Item_Choice, &mspChannels, 0,
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
static const Choice mcChannels_BandwidthB =
{
    Item_Choice, &mspChannels, 0,
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
static const Governor mgRand_NumMeasures =
{
    Item_Governor, &mspRand, 0,
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
static const Governor mgRand_TimeCompensation =
{
    Item_Governor, &mspRand, 0,
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

static const Governor mgRand_AddTimeShift =
{
    Item_Governor, &mspRand, 0,
    {
        "Доп смещение", "Add shift",
        "Добавочное смщение при вращении tShift",
        ""
    },
    &addShift, -100, 100, OnChange_Rand_AddTimeShift
};

static void OnChange_Rand_AddTimeShift(void)
{
    FPGA_SetTShift(TSHIFT);
}

// ОТЛАДКА - РАНД-ТОР - Предзапуск -------------------------------------------------------------------------------------------------------------------
static const Governor mgRand_Pretriggered =
{
    Item_Governor, &mspRand, 0,
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
static const Governor mgRand_NumAverage =
{
    Item_Governor, &mspRand, 0,
    {
        "Усредн.", "Average",
        "",
        ""
    },
    &setNR.numAveForRand, 1, 32
};

// ОТЛАДКА - РАНД-ТОР - Сглаживание ------------------------------------------------------------------------------------------------------------------
static const Governor mgRand_NumSmooth =
{
    Item_Governor, &mspRand, 0,
    {
        "Сглаживание", "Smoothing",
        "",
        ""
    },
    &setNR.numSmoothForRand, 1, 10
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
static const Choice mcRand_ShowInfo =
{
    Item_Choice, &mspRand, 0,
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
static const Choice mgRand_ShowStat =
{
    Item_Choice, &mspRand, 0,
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
static const Choice mcEMS =
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
static const Choice mcDisplayOrientation =
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
    Item_Page, &mspADC,
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
