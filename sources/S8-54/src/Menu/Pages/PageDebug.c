// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Log.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "Hardware/FLASH.h"
#include "Hardware/Sound.h"
#include "Menu/MenuDrawing.h"
#include "Menu/MenuFunctions.h"
#include "Menu/Pages/Definition.h"
#include "Utils/GlobalFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;
extern void LoadTShift(void);

static const     Page ppConsole;                            ///< ОТЛАДКА - КОНСОЛЬ
static const  Governor gConsole_NumStrings;                 ///< ОТЛАДКА - КОНСОЛЬ - Размер шрифта
static const    Choice cConsole_SizeFont;                   ///< ОТЛАДКА - КОНСОЛЬ - Размер шрифта
static const    Choice cConsole_ModeStop;                   ///< ОТЛАДКА - КОНСОЛЬ - Реж. останова
static const    Page pppConsole_Registers;                  ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ
static const    Choice cConsole_Registers_ShowAll;          ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все
static const    Choice cConsole_Registers_RD_FL;            ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL
static bool    IsActive_Console_Registers(void);
static const    Choice cConsole_Registers_RShiftA;          ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к
static const    Choice cConsole_Registers_RShiftB;          ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к
static const    Choice cConsole_Registers_TrigLev;          ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр.
static const    Choice cConsole_Registers_RangeA;           ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1
static const    Choice cConsole_Registers_RangeB;           ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2
static const    Choice cConsole_Registers_TrigParam;        ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр.
static const    Choice cConsole_Registers_ChanParamA;       ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2
static const    Choice cConsole_Registers_ChanParamB;       ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2
static const    Choice cConsole_Registers_TBase;            ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ
static const    Choice cConsole_Registers_TShift;           ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см.
static const    Button bConsole_SizeSettings;               ///< ОТЛАДКА - КОНСОЛЬ - Размер настроек
static void        Draw_Console_SizeSettings(int x, int y); 
static const     Page ppADC;                                ///< ОТЛАДКА - АЦП
static const    Page pppADC_Balance;                        ///< ОТЛАДКА - АЦП - БАЛАНС
static const    Choice cADC_Balance_Mode;                   ///< ОТЛАДКА - АЦП - БАЛАНС - Режим
static void   OnChanged_ADC_Balance_Mode(bool active);
static void        Draw_ADC_Balance_Mode(int x, int y);
static const  Governor gADC_Balance_ShiftA;                 ///< ОТЛАДКА - АЦП - БАЛАНС - Смещение 1
static bool    IsActive_ADC_Balance_ShiftAB(void);
static void   OnChanged_ADC_Balance_ShiftA(void);
static const  Governor gADC_Balance_ShiftB;                 ///< ОТЛАДКА - АЦП - БАЛАНС - Смещение 2 
static void   OnChanged_ADC_Balance_ShiftB(void);
static const    Page pppADC_Stretch;                        ///< ОТЛАДКА - АЦП - РАСТЯЖКА
static const    Choice cADC_Stretch_Mode;                   ///< ОТЛАДКА - АЦП - РАСТЯЖКА - Режим
       void   OnChanged_ADC_Stretch_Mode(bool active);
static const  Governor gADC_Stretch_A;                      ///< ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 1к
static bool    IsActive_ADC_StretchAB(void);
static void   OnChanged_ADC_Stretch_A(void);
static const  Governor gADC_Stretch_B;                      ///< ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 2к
static void   OnChanged_ADC_Stretch_B(void);
static const  Governor gADC_Stretch_Ak20mV;                 ///< ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 1к
static const  Governor gADC_Stretch_Ak50mV;                 ///< ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 1к 
static const  Governor gADC_Stretch_Ak100mV;                ///< ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 1к
static const  Governor gADC_Stretch_Ak2V;                   ///< ОТЛАДКА - AЦП - РАСТЯЖКА - 2В 1к
static const  Governor gADC_Stretch_Bk20mV;                 ///< ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 2к
static const  Governor gADC_Stretch_Bk50mV;                 ///< ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 2к 
static const  Governor gADC_Stretch_Bk100mV;                ///< ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 2к
static const  Governor gADC_Stretch_Bk2V;                   ///< ОТЛАДКА - АЦП - РАСТЯЖКА - 2В 2к
static const    Page pppADC_Shift;                          ///< ОТЛАДКА - АЦП - ДОП СМЕЩ
static const    Button bADC_Shift_Reset;                    ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс
static void     OnPress_ADC_Shift_Reset(void);
static const  Governor gADC_Shift_A2mV;                     ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост
static void   OnChanged_ADC_Shift_A(void);
static const  Governor gADC_Shift_B2mV;                     ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост
static void   OnChanged_ADC_Shift_B(void);
static const  Governor gADC_Shift_A5mV;                     ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост
static const  Governor gADC_Shift_B5mV;                     ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост
static const  Governor gADC_Shift_A10mV;                    ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост
static const  Governor gADC_Shift_B10mV;                    ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост
static const     Page ppRand;                               ///< ОТЛАДКА - РАНД-ТОР
static const  Governor gRand_NumMeasures;                   ///< ОТЛАДКА - РАНД-ТОР - Выб-к/ворота
static void   OnChanged_Rand_NumMeasures(void);
static const  Governor gRand_NumAverage;                    ///< ОТЛАДКА - РАНД-ТОР - Усредн.
static const  Governor gRand_NumSmooth;                     ///< ОТЛАДКА - РАНД-ТОР - Сглаживание
static const    Choice cRand_ShowInfo;                      ///< ОТЛАДКА - РАНД-ТОР - Информация
static const    Choice gRand_ShowStat;                      ///< ОТЛАДКА - РАНД-ТОР - Статистика
static const  Governor gRand_TimeCompensation;              ///< ОТЛАДКА - РАНД-ТОР - Компенсация задержки
static void   OnChanged_Rand_TimeCompensation(void);
static const  Governor gRand_AddTimeShift;                  ///< ОТЛАДКА - РАНД-ТОР - Смещение
static void   OnChanged_Rand_AddTimeShift(void);
static const  Governor gRand_Pretriggered;                  ///< ОТЛАДКА - РАНД-ТОР - Предзапуск
static void   OnChanged_Rand_Pretriggered(void);
static const     Page ppChannels;                           ///< ОТЛАДКА - КАНАЛЫ
static const    Choice cChannels_BandwidthA;                ///< ОТЛАДКА - КАНАЛЫ - Полоса 1
static void   OnChanged_Channels_BandwidthA(bool active);
static const    Choice cChannels_BandwidthB;                ///< ОТЛАДКА - КАНАЛЫ - Полоса 2
static void   OnChanged_Channels_BandwidthB(bool active);
static const    Choice cStats;                              ///< ОТЛАДКА - Статистика
static const    Choice cDisplayOrientation;                 ///< ОТЛАДКА - Ориентация
       void   OnChanged_DisplayOrientation(bool);
static const    Choice cEMS;                                ///< ОТЛАДКА - Режим ЭМС
static void   OnChanged_EMS(bool);
static const Governor mgPred;                               ///< ОТЛАДКА - Предзапуск
static void   OnChanged_Pred(void);
static const Governor mgPost;                               ///< ОТЛАДКА - Послезапуск
static void   OnChanged_Post(void);
static const     Page ppSettings;                           ///< ОТЛАДКА - НАСТРОЙКИ
static void     OnPress_Settings(void);
static const   SButton bSettings_Exit;                      ///< ОТЛАДКА - НАСТРОЙКИ - Выход
static void     OnPress_Settings_Exit(void);
static const    Button bSaveFirmware;                       ///< ОТЛАДКА - Сохр. прошивку
static bool    IsActive_SaveFirmware(void);
static void     OnPress_SaveFirmware(void);
static const     Page ppSerialNumber;                       ///< ОТЛАДКА - С/Н
static void     OnPress_SerialNumber(void);
static void        Draw_EnterSerialNumber(void);
static void    OnRegSet_SerialNumber(int);
static const   SButton bSerialNumber_Exit;                  ///< ОТЛАДКА - С/Н - Выход
static void     OnPress_SerialNumber_Exit(void);
static const   SButton bSerialNumber_Change;                ///< ОТЛАДКА - С/Н - Перейти
static void     OnPress_SerialNumber_Change(void);
static void        Draw_SerialNumber_Change(int, int);
static const   SButton bSerialNumber_Save;                  ///< ОТЛАДКА - С/Н - Сохранить
static void     OnPress_SerialNumber_Save(void);
static void        Draw_SerialNumber_Save(int, int);
static const    Choice cShowAutoFind;                       ///< ОТЛАДКА - Показывать поиск

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// В этой структуре будут храниться данные серийного номера при открытой странице ppSerialNumer
typedef struct
{
    int number;     ///< Соответственно, порядковый номер.
    int year;       ///< Соответственно, год.
    int curDigt;    ///< Соответственно, номером (0) или годом (1) управляет ручка УСТАНОВКА.
} StructForSN;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int8 showAutoFind = false;   ///< Если true, то процесс поиска сигнала будет визуализироваться.


// ОТЛАДКА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pDebug =
{
    Item_Page, &mainPage, 0,
    {
        "ОТЛАДКА", "DEBUG",
        "", ""
    },
    Page_Debug,
    {
        (void*)&ppConsole,              // ОТЛАДКА - КОНСОЛЬ
        (void*)&ppADC,                  // ОТЛАДКА - АЦП
        (void*)&ppRand,                 // ОТЛАДКА - РАНД-ТОР
        (void*)&ppChannels,             // ОТЛАДКА - КАНАЛЫ
        (void*)&cStats,                 // ОТЛАДКА - Статистика
        (void*)&cDisplayOrientation,    // ОТЛАДКА - Ориентация
        (void*)&cEMS,                   // ОТЛАДКА - Режим ЭМС
        (void*)&mgPred,                 // ОТЛАДКА - Предзапуск
        (void*)&mgPost,                 // ОТЛАДКА - Послезапуск
        (void*)&ppSettings,             // ОТЛАДКА - НАСТРОЙКИ
        (void*)&bSaveFirmware,          // ОТЛАДКА - Сохр. прошивку
        (void*)&ppSerialNumber,         // ОТЛАДКА - С/Н
        (void*)&cShowAutoFind           // ОТЛАДКА - Показывать поиск
    }
};

// ОТЛАДКА - КОНСОЛЬ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppConsole =
{
    Item_Page, &pDebug, 0,
    {
        "КОНСОЛЬ", "CONSOLE",
        "",
        ""
    },
    Page_DebugConsole,
    {
        (void*)&gConsole_NumStrings,    // ОТЛАДКА - КОНСОЛЬ - Число строк
        (void*)&cConsole_SizeFont,      // ОТЛАДКА - КОНСОЛЬ - Размер шрифта
        (void*)&cConsole_ModeStop,      // ОТЛАДКА - КОНСОЛЬ - Реж. останова
        (void*)&pppConsole_Registers,   // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ
        (void*)&bConsole_SizeSettings   // ОТЛАДКА - КОНСОЛЬ - Размер настроек

    }
};

// ОТЛАДКА - КОНСОЛЬ - Число строк -------------------------------------------------------------------------------------------------------------------
static const Governor gConsole_NumStrings =
{
    Item_Governor, &ppConsole, 0,
    {
        "Число строк", "Number strings",
        "",
        ""
    },
    &CONSOLE_NUM_STRINGS, 0, 33
};

// ОТЛАДКА - КОНСОЛЬ - Размер шрифта -----------------------------------------------------------------------------------------------------------------
static const Choice cConsole_SizeFont =
{
    Item_Choice, &ppConsole, 0,
    {
        "Размер шрифта", "Size font",
        "",
        ""
    },
    {
        {"5", "5"},
        {"8", "8"}
    },
    &set.dbg_SizeFont
};

// ОТЛАДКА - КОНСОЛЬ - Реж. останова -----------------------------------------------------------------------------------------------------------------
static const Choice cConsole_ModeStop =
{
    Item_Choice, &ppConsole, 0,
    {
        "Реж. останова", "Mode stop",
        "Предоставляет возможность приостановки вывода в консоль путём нажатия на кнопку ПУСК/СТОП",
        "It provides the ability to pause the output to the console by pressing the ПУСК/СТОП button"
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ModePauseConsole
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pppConsole_Registers =
{
    Item_Page, &ppConsole, 0,
    {
        "РЕГИСТРЫ", "REGISTERS",
        "",
        ""
    },
    Page_DebugShowRegisters,
    {
        (void*)&cConsole_Registers_ShowAll,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все
        (void*)&cConsole_Registers_RD_FL,        // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL
        (void*)&cConsole_Registers_RShiftA,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к
        (void*)&cConsole_Registers_RShiftB,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к
        (void*)&cConsole_Registers_TrigLev,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр
        (void*)&cConsole_Registers_RangeA,       // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1
        (void*)&cConsole_Registers_RangeB,       // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2
        (void*)&cConsole_Registers_TrigParam,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр.
        (void*)&cConsole_Registers_ChanParamA,   // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 1
        (void*)&cConsole_Registers_ChanParamB,   // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2
        (void*)&cConsole_Registers_TBase,        // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ
        (void*)&cConsole_Registers_TShift        // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см.
    }
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все -----------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_ShowAll =
{
    Item_Choice, &pppConsole_Registers, 0,
    {
        "Показывать все", "Show all",
        "Показывать все значения, засылаемые в регистры",
        "To show all values transferred in registers"
    },
    {
        {"Нет", "No"},
        {"Да", "Yes"}
    },
    (int8*)&DBG_SHOW_ALL
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL --------------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_RD_FL =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "RD_FL", "RD_FL",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&DBG_SHOW_FLAG
};

static bool IsActive_Console_Registers(void)
{
    return DBG_SHOW_ALL;
}

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к -----------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_RShiftA =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "U см. 1к", "U shift 1ch",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowRShift[A]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к -----------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_RShiftB =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "U см. 2к", "U shift 2ch",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowRShift[B]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр. -----------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_TrigLev =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "U синхр.", "U trig.",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowTrigLev
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1 --------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_RangeA =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "ВОЛЬТ/ДЕЛ 1", "Range 1",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowRange[A]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2 --------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_RangeB =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "ВОЛЬТ/ДЕЛ 2", "Range 2",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowRange[B]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр. ------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_TrigParam =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "Парам. синхр.", "Trig param",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowTrigParam
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2 ------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_ChanParamA =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "Парам. кан. 1", "Chan 1 param",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowChanParam[A]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2 ------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_ChanParamB =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "Парам. кан. 2", "Chan 2 param",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowChanParam[B]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ ----------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_TBase =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "ВРЕМЯ/ДЕЛ", "TBase",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowTBase
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см. --------------------------------------------------------------------------------------------------------------
static const Choice cConsole_Registers_TShift =
{
    Item_Choice, &pppConsole_Registers, IsActive_Console_Registers,
    {
        "Т см.", "tShift",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.dbg_ShowTShift
};

// ОТЛАДКА - КОНСОЛЬ - Размер настроек ---------------------------------------------------------------------------------------------------------------
static const Button bConsole_SizeSettings =
{
    Item_Button, &ppConsole, 0,
    {
        "", "",
        "Показывает текущий размер структуры для сохранения настроек",
        "Displays the current size of the structure to save settings"
    },
    0, Draw_Console_SizeSettings
};

static void Draw_Console_SizeSettings(int x, int y)
{
    char buffer[30];
    sprintf(buffer, "Разм.настр. %d", sizeof(Settings));
    Painter_DrawTextC(x + 6, y + 13, buffer, gColorBack);
}

// ОТЛАДКА - АЦП /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppADC =
{
    Item_Page, &pDebug, 0,
    {
        "АЦП", "ADC",
        "",
        ""
    },
    Page_DebugADC,
    {
        (void*)&pppADC_Balance,     // ОТЛАДКА - АЦП - БАЛАНС
        (void*)&pppADC_Stretch,     // ОТЛАДКА - АЦП - РАСТЯЖКА
        (void*)&pppADC_Shift        // ОТЛАДКА - АЦП - ДОП СМЕЩ
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
    (int8*)&NRST_BALANCE_ADC_TYPE, OnChanged_ADC_Balance_Mode, Draw_ADC_Balance_Mode
};

static void OnChanged_ADC_Balance_Mode(bool active)
{
    Draw_ADC_Balance_Mode(0, 0);
}

static int16 shiftADCA;
static int16 shiftADCB;

static void Draw_ADC_Balance_Mode(int x, int y)
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
    Item_Governor, &pppADC_Balance, IsActive_ADC_Balance_ShiftAB,
    {
        "Смещение 1", "Offset 1",
        "",
        ""
    },
    &shiftADCA, -125, 125, OnChanged_ADC_Balance_ShiftA
};

static bool IsActive_ADC_Balance_ShiftAB(void)
{
    return NRST_BALANCE_ADC_TYPE_IS_HAND;
}

static void OnChanged_ADC_Balance_ShiftA(void)
{
    NRST_BALANCE_ADC_A = shiftADCA;
}

// ОТЛАДКА - АЦП - БАЛАНС - Смещение 2 ---------------------------------------------------------------------------------------------------------------
static const Governor gADC_Balance_ShiftB =
{
    Item_Governor, &pppADC_Balance, IsActive_ADC_Balance_ShiftAB,
    {
        "Смещение 2", "Offset 2",
        "",
        ""
    },
    &shiftADCB, -125, 125, OnChanged_ADC_Balance_ShiftB
};

static void OnChanged_ADC_Balance_ShiftB(void)
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
        (void*)&cADC_Stretch_Mode,      // ОТЛАДКА - АЦП - РАСТЯЖКА - Режим
        (void*)&gADC_Stretch_A,         // ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 1к
        (void*)&gADC_Stretch_B,         // ОТЛАДКА - АЦП - РАСТЯЖКА - Растяжка 2к
        (void*)&emptyChoice,
        (void*)&emptyChoice,
        (void*)&gADC_Stretch_Ak20mV,    // ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 1к
        (void*)&gADC_Stretch_Ak50mV,    // ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 1к 
        (void*)&gADC_Stretch_Ak100mV,   // ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 1к
        (void*)&gADC_Stretch_Ak2V,      // ОТЛАДКА - AЦП - РАСТЯЖКА - 2В 1к
        (void*)&emptyChoice,
        (void*)&gADC_Stretch_Bk20mV,    // ОТЛАДКА - АЦП - РАСТЯЖКА - 20мВ/1В 2к
        (void*)&gADC_Stretch_Bk50mV,    // ОТЛАДКА - АЦП - РАСТЯЖКА - 50мВ 2к 
        (void*)&gADC_Stretch_Bk100mV,   // ОТЛАДКА - АЦП - РАСТЯЖКА - 100мВ/5В 2к
        (void*)&gADC_Stretch_Bk2V,      // ОТЛАДКА - АЦП - РАСТЯЖКА - 2В 2к
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
    (int8*)&NRST_STRETCH_ADC_TYPE, OnChanged_ADC_Stretch_Mode
};

static int16 stretchA;
static int16 stretchB;  

void OnChanged_ADC_Stretch_Mode(bool active)
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
    &stretchA, -10000, 10000, OnChanged_ADC_Stretch_A
};

static bool IsActive_ADC_StretchAB(void)
{
    return NRST_STRETCH_ADC_TYPE_IS_HAND;
}

static void OnChanged_ADC_Stretch_A(void)
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
    &stretchB, -10000, 10000, OnChanged_ADC_Stretch_B
};

static void OnChanged_ADC_Stretch_B(void)
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
    (int16*)(&NRST_RSHIFT_ADD_A(Range_2mV, ModeCouple_DC)), -100, 100, OnChanged_ADC_Shift_A
};

static void OnChanged_ADC_Shift_A(void)
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
    (int16*)(&NRST_RSHIFT_ADD_B(Range_2mV, ModeCouple_DC)), -100, 100, OnChanged_ADC_Shift_B
};

static void OnChanged_ADC_Shift_B(void)
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
    (int16*)(&NRST_RSHIFT_ADD_A(Range_5mV, ModeCouple_DC)), -100, 100, OnChanged_ADC_Shift_A
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
    (int16*)(&NRST_RSHIFT_ADD_B(Range_5mV, ModeCouple_DC)), -100, 100, OnChanged_ADC_Shift_B
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
    (int16*)(&NRST_RSHIFT_ADD_A(Range_10mV, ModeCouple_DC)), -100, 100, OnChanged_ADC_Shift_A
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
    (int16*)(&NRST_RSHIFT_ADD_B(Range_10mV, ModeCouple_DC)), -100, 100, OnChanged_ADC_Shift_B
};

// ОТЛАДКА - РАНД-ТОР ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppRand =
{
    Item_Page, &pDebug, 0,
    {
        "РАНД-ТОР", "RANDOMIZER",
        "",
        ""
    },
    Page_DebugRandomizer,
    {
        (void*)&gRand_NumAverage,          // ОТЛАДКА - РАНД-ТОР - Усредн.
        (void*)&gRand_NumSmooth,           // ОТЛАДКА - РАНД-ТОР - Сглаживание
        (void*)&gRand_NumMeasures,         // ОТЛАДКА - РАНД-ТОР - Выб-к/ворота
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
    Item_Page, &pDebug, 0,
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
    (int8*)&BANDWIDTH_DEBUG(A), OnChanged_Channels_BandwidthA
};

static void OnChanged_Channels_BandwidthA(bool active)
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
    (int8*)&BANDWIDTH_DEBUG(B), OnChanged_Channels_BandwidthB
};

static void OnChanged_Channels_BandwidthB(bool active)
{
    FPGA_SetBandwidth(B);
}

// ОТЛАДКА - РАНД-ТОР - Выб-к/ворота -----------------------------------------------------------------------------------------------------------------
static const Governor gRand_NumMeasures =
{
    Item_Governor, &ppRand, 0,
    {
        "Выб-к/ворота", "Samples/gates",
        "",
        ""
    },
    &NUM_MEASURES_FOR_GATES, 1, 2500, OnChanged_Rand_NumMeasures
};

static void OnChanged_Rand_NumMeasures(void)
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
    &TIME_COMPENSATION, 0, 510, OnChanged_Rand_TimeCompensation
};

static void OnChanged_Rand_TimeCompensation(void)
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
    &addShift, -100, 100, OnChanged_Rand_AddTimeShift
};

static void OnChanged_Rand_AddTimeShift(void)
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
    &PRETRIGGERED, 0, 30000, OnChanged_Rand_Pretriggered
};

static void OnChanged_Rand_Pretriggered(void)
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

// ОТЛАДКА - Предзапуск ------------------------------------------------------------------------------------------------------------------------------
static const Governor mgPred =
{
    Item_Governor, &pDebug, 0,
    {
        "Предзапуск", "",
        "", ""
    },
    &pred, 0, 15000, OnChanged_Pred
};

// ОТЛАДКА - Послезапуск -----------------------------------------------------------------------------------------------------------------------------
static const Governor mgPost =
{
    Item_Governor, &pDebug, 0,
    {
        "Послезапуск", "",
        "", ""
    },
    &post, 0, 15000, OnChanged_Post
};


// ОТЛАДКА - НАСТРОЙКИ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppSettings =
{
    Item_Page, &pDebug, 0,
    {
        "НАСТРОЙКИ", "SETTINGS",
        "Показать информацию о настройках",
        "Show settings information"
    },
    Page_SB_DebugShowSetInfo,
    {
        (void*)&bSettings_Exit        // ОТЛАДКА - НАСТРОЙКИ - Выход
    },
    OnPress_Settings
};

// ОТЛАДКА - НАСТРОЙКИ - Выход -----------------------------------------------------------------------------------------------------------------------
static const SButton bSettings_Exit =
{
    Item_SmallButton, &ppSettings,
    COMMON_BEGIN_SB_EXIT,
    OnPress_Settings_Exit,
    DrawSB_Exit
};

static void OnPress_Settings_Exit(void)
{
    Display_SetDrawMode(DrawMode_Auto, 0);
}

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

static void OnPress_Settings(void)
{
    OpenPageAndSetItCurrent(Page_SB_DebugShowSetInfo);
    Display_SetDrawMode(DrawMode_Auto, DebugShowSetInfo_Draw);
}

static void OnChanged_Pred(void)
{
    gPred = ~pred;
    static char buffer[30];
    LOG_WRITE("pred %d %s", pred, Hex16toString(gPred, buffer, true));
}

static void OnChanged_Post(void)
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

// ОТЛАДКА - Режим ЭМС ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Choice cEMS =
{
    Item_Choice, &pDebug, 0,
    {
        "Режим ЭМС", "EMS mode",
        "Принудительно включает фильтр 20МГц, сглаживание по 4-м точкам, усреднение по 8-ми точкам",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&MODE_EMS, OnChanged_EMS
};

static void OnChanged_EMS(bool active)
{
    FPGA_SetBandwidth(A);
    FPGA_SetBandwidth(B);
}

// ОТЛАДКА - Ориентация //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Choice cDisplayOrientation =
{
    Item_Choice, &pDebug, 0,
    {
        "Ориентация", "DisplayOrientation",
        "Устанавливает ориентацию дисплея",
        "Sets display orientation"
    },
    {
        { "Прямая", "Direct" },
        { "Обратная", "Back" }
    },
    (int8*)&DISPLAY_ORIENTATION, OnChanged_DisplayOrientation
};

void OnChanged_DisplayOrientation(bool active)
{
    Display_SetOrientation(DISPLAY_ORIENTATION);
}

// ОТЛАДКА - Статистика ------------------------------------------------------------------------------------------------------------------------------
static const Choice cStats =
{
    Item_Choice, &pDebug, 0,
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

// ОТЛАДКА - Сохр. прошивку --------------------------------------------------------------------------------------------------------------------------
static const Button bSaveFirmware =
{
    Item_Button, &pDebug, IsActive_SaveFirmware,
    {
        "Сохр. прошивку", "Save firmware",
        "Сохранение прошивки - секторов 5, 6, 7 общим объёмом 3 х 128 кБ, где хранится программа",
        "Saving firmware - sectors 5, 6, 7 with a total size of 3 x 128 kB, where the program is stored"
    },
    OnPress_SaveFirmware
};

static bool IsActive_SaveFirmware(void)
{
    return gFlashDriveIsConnected;
}

static void OnPress_SaveFirmware(void)
{
    Display_FuncOnWaitStart("Сохраняю прошивку", "Save the firmware", false);

    StructForWrite structForWrite;

    FDrive_OpenNewFileForWrite("S8-54.bin", &structForWrite);

    uint8 *address = (uint8*)0x08020000;
    uint8 *endAddress = address + 128 * 1024 * 3;

    int sizeBlock = 512;

    while (address < endAddress)
    {
        FDrive_WriteToFile(address, sizeBlock, &structForWrite);
        address += sizeBlock;
    }

    FDrive_CloseFile(&structForWrite);

    Display_FuncOnWaitStop();

    Display_ShowWarning(FirmwareSaved);
}

// ОТЛАДКА - С/Н /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppSerialNumber =
{
    Item_Page, &pDebug, 0,
    {
        "С/Н", "S/N",
        "Запись серийного номера в OTP-память. ВНИМАНИЕ!!! ОТP-память - память с однократной записью.",
        "Serial number recording in OTP-memory. ATTENTION!!! OTP memory is a one-time programming memory."
    },
    Page_SB_SerialNumber,
    {
        (void*)&bSerialNumber_Exit,     // ОТЛАДКА - С/Н - Выход
        (void*)&bSerialNumber_Change,   // ОТЛАДКА - С/Н - Перейти
        (void*)0,
        (void*)0,
        (void*)0,
        (void*)&bSerialNumber_Save      // ОТЛАДКА - С/Н - Сохранить
    },
    OnPress_SerialNumber, 0, OnRegSet_SerialNumber
};

static void OnPress_SerialNumber(void)
{
    OpenPageAndSetItCurrent(Page_SB_SerialNumber);
    Display_SetAddDrawFunction(Draw_EnterSerialNumber);
    MALLOC_EXTRAMEM(StructForSN, s);
    s->number = 01;
    s->year = 2017;
    s->curDigt = 0;
}

static void Draw_EnterSerialNumber(void)
{
    int x0 = GridLeft() + 40;
    int y0 = GRID_TOP + 20;
    int width = GridWidth() - 80;
    int height = 160;

    Painter_DrawRectangleC(x0, y0, width, height, gColorFill);
    Painter_FillRegionC(x0 + 1, y0 + 1, width - 2, height - 2, gColorBack);

    int deltaX = 10;

    ACCESS_EXTRAMEM(StructForSN, s);
    
    bool selNumber = s->curDigt == 0;

    char buffer[20];
    snprintf(buffer, 19, "%02d", s->number);

    Color colorText = gColorFill;
    Color colorBackground = gColorBack;

    if (selNumber)
    {
        colorText = COLOR_FLASH_01;
        colorBackground = COLOR_FLASH_10;
    }

    int y = y0 + 50;

    Painter_SetColor(colorText);
    int x = Painter_DrawTextOnBackground(x0 + deltaX, y, buffer, colorBackground);

    colorText = COLOR_FLASH_01;
    colorBackground = COLOR_FLASH_10;

    if (selNumber)
    {
        colorText = gColorFill;
        colorBackground = gColorBack;
    }

    snprintf(buffer, 19, "%04d", s->year);

    Painter_SetColor(colorText);
    Painter_DrawTextOnBackground(x + 5, y, buffer, colorBackground);

    // Теперь выведем информацию об оставшемся месте в OTP-памяти для записи

    int allShots = OTP_GetSerialNumber(buffer);

    Painter_DrawFormText(x0 + deltaX, y0 + 130, gColorFill, "Текущий сохранённый номер %s", buffer[0] == 0 ? "-- ----" : buffer);

    Painter_DrawFormText(x0 + deltaX, y0 + 100, gColorFill, "Осталось места для %d попыток", allShots);
}

static void OnRegSet_SerialNumber(int angle)
{
    typedef int (*pFunc)(int*, int, int);

    pFunc p = angle > 0 ? CircleIncreaseInt : CircleDecreaseInt;

    ACCESS_EXTRAMEM(StructForSN, s);

    if (s->curDigt == 0)
    {
        p(&s->number, 1, 99);
    }
    else
    {
        p(&s->year, 2016, 2050);
    }
    Sound_GovernorChangedValue();
}

// ОТЛАДКА - С/Н - Выход -----------------------------------------------------------------------------------------------------------------------------
static const SButton bSerialNumber_Exit =
{
    Item_SmallButton, &ppSerialNumber,
    COMMON_BEGIN_SB_EXIT,
    OnPress_SerialNumber_Exit,
    DrawSB_Exit
};

static void OnPress_SerialNumber_Exit(void)
{
    Display_RemoveAddDrawFunction();
    FREE_EXTRAMEM();
}

// ОТЛАДКА - С/Н - Вставить --------------------------------------------------------------------------------------------------------------------------
static const SButton bSerialNumber_Change =
{
    Item_SmallButton, &ppSerialNumber, 0,
    {
        "Вставить", "Insert",
        "Вставляет выбраный символ",
        "Inserts the chosen symbol"
    },
    OnPress_SerialNumber_Change,
    Draw_SerialNumber_Change
};

static void OnPress_SerialNumber_Change(void)
{
    ACCESS_EXTRAMEM(StructForSN, s);
    ++s->curDigt;
    s->curDigt %= 2;
    Painter_ResetFlash();
}

static void Draw_SerialNumber_Change(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_TAB);
    Painter_SetFont(TypeFont_8);
}

// ОТЛАДКА - С/Н - Сохранить -------------------------------------------------------------------------------------------------------------------------
static const SButton bSerialNumber_Save =
{
    Item_SmallButton, &ppSerialNumber, 0,
    {
        "Сохранить", "Save",
        "Записывает серийный номер в OTP",
        "Records the serial number in OTP"
    },
    OnPress_SerialNumber_Save,
    Draw_SerialNumber_Save
};

static void OnPress_SerialNumber_Save(void)
{
    ACCESS_EXTRAMEM(StructForSN, s);

    char stringSN[20];

    snprintf(stringSN, 19, "%02d %04d", s->number, s->year);

    if (!OTP_SaveSerialNumber(stringSN))
    {
        Display_ShowWarning(FullyCompletedOTP);
    }
}

static void Draw_SerialNumber_Save(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Painter_SetFont(TypeFont_8);
}


// ОТЛАДКА - Показывать поиск ------------------------------------------------------------------------------------------------------------------------
static const Choice cShowAutoFind =
{
    Item_Choice, &pDebug, 0,
    {
        "Показывать поиск", "Show autofind",
        "Визуализировать процесс поиска сигнала",
        "Visualize the signal search process"
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    &showAutoFind
};


































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
