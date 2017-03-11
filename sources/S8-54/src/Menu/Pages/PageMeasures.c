// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Menu/MenuItems.h"
#include "Utils/Measures.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Menu/Pages/Definition.h"
#include "Hardware/Sound.h"
#include "FPGA/fpgaExtensions.h  "


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mainPage;

static const Choice mcIsShow;
static const Choice mcNumber;
static const Choice mcChannels;
static const Choice mcMode;

static const Page mspFreqMeter;
static const Choice mcServiceFreqMeterEnable;
static const Choice mcServiceFreqMeterTimeF;
static const Choice mcServiceFreqFreqClc;
static const Choice mcServiceFreqMeterNumberPeriods;
static void ChangeParameterFreqMeter(bool param);

static const Page mspTune;
static const SmallButton sbTune_Exit;
static const SmallButton sbTune_Markers;
static const SmallButton sbTune_Settings;

static bool IsActive_Number(void);
static bool IsActive_Channels(void);
static bool IsActive_Mode(void);
static bool IsActive_Tune(void);
static void OnRot_Tune(int angle);
static void OnPress_Tune_Exit(void);
static void OnPress_Tune_Markers(void);
static void DrawSB_Tune_Markers(int x, int y);
static void OnPress_Tune_Settings(void);
static void DrawSB_Tune_Settings(int x, int y);

       int8 posActive = 0;                  // Позиция активного измерения (на котором курсор)
       bool pageChoiceIsActive = false;     // Если true - раскрыта страница выбора измерения
       int8 posOnPageChoice = 0;            // Позиция курсора на странице выбора измерения


// ИЗМЕРЕНИЯ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpMeasures =
{
    Item_Page, &mainPage, 0,
    {
        "ИЗМЕРЕНИЯ", "MEASURES",
        "Автоматические измерения",
        "Automatic measurements"
    },
    Page_Measures,
    {
        (void*)&mspFreqMeter,   // ИЗМЕРЕНИЯ - ЧАСТОТОМЕР
        (void*)&mcIsShow,       // ИЗМЕРЕНИЯ - Показывать
        (void*)&mcNumber,       // ИЗМЕРЕНИЯ - Количество
        (void*)&mcChannels,     // ИЗМЕРЕНИЯ - Каналы
        (void*)&mspTune,        // ИЗМЕРЕНИЯ - НАСТРОИТЬ
        (void*)&mcMode          // ИЗМЕРЕНИЯ - Вид
    }
};

// ИЗМЕРЕНИЯ - ЧАСТОТОМЕР ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspFreqMeter =
{
    Item_Page, &mpMeasures, 0,
    {
        "ЧАСТОТОМЕР", "FREQ METER",
        "",
        ""
    },
    Page_ServiceFreqMeter,
    {
        (void*)&mcServiceFreqMeterEnable,
        (void*)&mcServiceFreqMeterTimeF,
        (void*)&mcServiceFreqFreqClc,
        (void*)&mcServiceFreqMeterNumberPeriods
    }
};

static const Choice mcServiceFreqMeterEnable =
{
    Item_Choice, &mspFreqMeter, 0,
    {
        "Частотомер", "Freq meter",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.service.freqMeter.enable, ChangeParameterFreqMeter
};

static void ChangeParameterFreqMeter(bool param)
{
    FreqMeter_Init();
}

static const Choice mcServiceFreqMeterTimeF =
{
    Item_Choice, &mspFreqMeter, 0,
    {
        "Время счёта F", "Time calc F",
        "Позволяет выбрать точность измерения частоты - чем больше время, тем больше точность и больше время измерения",
        "Allows to choose the accuracy of measurement of frequency - the more time, the accuracy more time of measurement is more"
    },
    {
        {"100мс", "100ms"},
        {"1с", "1s"},
        {"10с", "10ms"}
    },
    (int8*)&set.service.freqMeter.timeCounting, ChangeParameterFreqMeter
};

static const Choice mcServiceFreqFreqClc =
{
    Item_Choice, &mspFreqMeter, 0,
    {
        "Метки времени", "Timestamps",
        "Выбор частоты следования счётных импульсов",
        "Choice of frequency of following of calculating impulses"
    },
    {
        {"100кГц", "10MHz"},
        {"1МГц", "200MHz"},
        {"10МГц", "10MHz"},
        {"100МГц", "100MHz"}
    },
    (int8*)&set.service.freqMeter.freqClc, ChangeParameterFreqMeter
};

static const Choice mcServiceFreqMeterNumberPeriods =
{
    Item_Choice, &mspFreqMeter, 0,
    {
        "Кол-во периодов", "Num periods",
        "Позволяет выбрать точность измерения периода - чем больше время, тем больше точность и больше время измерения",
        "Allows to choose the accuracy of measurement of period - the more time, the accuracy more time of measurement is more"
    },
    {
        {"1", "1"},
        {"10", "10"},
        {"100", "100"}
    },
    (int8*)&set.service.freqMeter.numberPeriods, ChangeParameterFreqMeter
};

// ИЗМЕРЕНИЯ - Показывать ----------------------------------------------------------------------------------------------------------------------------
static const Choice mcIsShow =
{
    Item_Choice, &mpMeasures, 0,
    {
        "Показывать", "Show",
        "Выводить или не выводить измерения на экран",
        "Output or output measurements on screen"
    },
    {
        {"Нет", "No"},
        {"Да", "Yes"}
    },
    (int8*)&SHOW_MEASURES
};

// ИЗМЕРЕНИЯ - Количество ----------------------------------------------------------------------------------------------------------------------------
static const Choice mcNumber =
{
    Item_Choice, &mpMeasures, IsActive_Number,
    {
        "Количество", "Number"
        ,
        "Устанавливает максимальное количество выводимых измерений:\n"
        "\"1\" - одно измерение\n"
        "\"2\" - два измерения\n"
        "\"1х5\" - 1 строка с пятью измерениями\n"
        "\"2х5\" - 2 строки с пятью измерениями в каждой\n"
        "\"3х5\" - 3 строки с пятью измерениями в каждой\n"
        "\"6x1\" - 6 строк по одному измерению в каждой\n"
        "\"6х2\" - 6 строк по два измерения в каждой"
        ,
        "Sets the maximum number of output measurements:\n"
        "\"1\" - one measurement\n"
        "\"2\" - two measurements\n"
        "\"1x5\" - 1 line with the five dimensions\n"
        "\"2x5\" - two rows with five measurements in each\n"
        "\"3x5\" - 3 lines with five measurements in each"
        "\"6x1\" - 6 lines, one in each dimension\n"
        "\"6x2\" - 6 lines of two dimensions in each\n"
    },
    {
        {"1", "1"},
        {"2", "2"},
        {"1x5", "1x5"},
        {"2x5", "2x5"},
        {"3x5", "3x5"},
        {"6x1", "6x1"},
        {"6x2", "6x2"}
    },
    (int8*)&NUM_MEASURES
};

static bool IsActive_Number(void)
{
    return SHOW_MEASURES;
}

// ИЗМЕРЕНИЯ - Каналы --------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChannels =
{
    Item_Choice, &mpMeasures, IsActive_Channels,
    {
        "Каналы", "Channels",
        "По каким каналам выводить измерения",
        "Which channels to output measurement"
    },
    {
        {"1", "1"},
        {"2", "2"},
        {"1 и 2", "1 and 2"}
    },
    (int8*)&SOURCE_MEASURE
};

static bool IsActive_Channels(void)
{
    return SHOW_MEASURES;
}

// ИЗМЕРЕНИЯ - Вид -----------------------------------------------------------------------------------------------------------------------------------
static const Choice mcMode =
{
    Item_Choice, &mpMeasures, IsActive_Mode,
    {
        "Вид", "View",
        "Уменьшать или нет зону вывода сигнала для исключения перекрытия его результами измерений",
        "Decrease or no zone output signal to avoid overlapping of its measurement results"
    },
    {
        {"Как есть",    "As is"},
        {"Уменьшать",   "Reduce"}
    },
    (int8*)&set.measures.modeViewSignals
};

static bool IsActive_Mode(void)
{
    return SHOW_MEASURES;
}


// ИЗМЕРЕНИЯ - НАСТРОИТЬ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspTune =
{
    Item_Page, &mpMeasures, IsActive_Tune,
    {
        "НАСТРОИТЬ", "CONFIGURE",
        "Переход в режми точной настройки количества и видов измерений",
        "Transition to rezhm of exact control of quantity and types of measurements"
    },
    Page_SB_MeasTuneMeas,
    {
        (void*)&sbTune_Exit,     // ИЗМЕРЕНИЯ - НАСТРОИТЬ - Выход
        (void*)0,
        (void*)0,
        (void*)0,
        (void*)&sbTune_Markers,  // ИЗМЕРЕНИЯ - НАСТРОИТЬ - Маркеры
        (void*)&sbTune_Settings  // ИЗМЕРЕНИЯ - НАСТРОИТЬ - Настройка
    },
    EmptyFuncVV,
    EmptyFuncVV,
    OnRot_Tune
};

static bool IsActive_Tune(void)
{
    return SHOW_MEASURES;
}

static void OnRot_Tune(int angle)
{
    static const int8 step = 3;
    static int8 currentAngle = 0;
    currentAngle += (int8)angle;
    if (currentAngle < step && currentAngle > -step)
    {
        return;
    }
    if (pageChoiceIsActive)
    {
        posOnPageChoice += (int8)Math_Sign(currentAngle);
        Sound_RegulatorSwitchRotate();
        if (posOnPageChoice < 0)
        {
            posOnPageChoice = Measure_NumMeasures - 1;
        }
        else if (posOnPageChoice == Measure_NumMeasures)
        {
            posOnPageChoice = 0;
        }
        MEASURE(posActive) = (Measure)posOnPageChoice;
        Painter_ResetFlash();
    }
    else
    {
        int row = 0;
        int col = 0;
        Measure_GetActive(&row, &col);
        col += Math_Sign(currentAngle);
        if (col < 0)
        {
            col = Measure_NumCols() - 1;
            row--;
            if (row < 0)
            {
                row = Measure_NumRows() - 1;
            }
        }
        else if (col == Measure_NumCols())
        {
            col = 0;
            row++;
            if (row >= Measure_NumRows())
            {
                row = 0;
            }
        }
        Measure_SetActive(row, col);
        Sound_RegulatorSwitchRotate();
    }
    currentAngle = 0;
}

// ИЗМЕРЕНИЯ - НАСТРОИТЬ - Выход ---------------------------------------------------------------------------------------------------------------------
static const SmallButton sbTune_Exit =
{
    Item_SmallButton, &mspTune,
    COMMON_BEGIN_SB_EXIT,
    OnPress_Tune_Exit,
    DrawSB_Exit
};

static void OnPress_Tune_Exit(void)
{
    Display_RemoveAddDrawFunction();
}

// ИЗМЕРЕНИЯ - НАСТРОИТЬ - Маркеры -------------------------------------------------------------------------------------------------------------------
static const SmallButton sbTune_Markers =
{
    Item_SmallButton, &mspTune, 0,
    {
        "Маркер", "Marker",
        "Позволяет установить маркеры для визуального контроля измерений",
        "Allows to establish markers for visual control of measurements"
    },
    OnPress_Tune_Markers,
    DrawSB_Tune_Markers
};

static void OnPress_Tune_Markers(void)
{
    Measure_ShortPressOnSmallButonMarker();
}

static void DrawSB_Tune_Markers(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 2, '\x60');
    Painter_SetFont(TypeFont_8);
}

// ИЗМЕРЕНИЯ - НАСТРОИТЬ - Настройка -----------------------------------------------------------------------------------------------------------------
static const SmallButton sbTune_Settings =
{
    Item_SmallButton, &mspTune, 0,
    {
        "Настройка", "Setup",
        "Позволяет выбрать необходимые измерения",
        "Allows to choose necessary measurements"
    },
    OnPress_Tune_Settings,
    DrawSB_Tune_Settings
};

static void OnPress_Tune_Settings(void)
{
    pageChoiceIsActive = !pageChoiceIsActive;
    if (pageChoiceIsActive)
    {
        posOnPageChoice = MEASURE(posActive); //-V2006
    }
}

static void DrawSB_Tune_Settings(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x62');
    Painter_SetFont(TypeFont_8);
}
