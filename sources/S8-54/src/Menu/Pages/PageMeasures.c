#include "defines.h"
#include "Menu/MenuItems.h"
#include "Utils/Measures.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Menu/Pages/Definition.h"
#include "Hardware/Sound.h"
#include "FPGA/fpgaExtensions.h  "


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const     Page pMeasures;
extern const    Page ppFreqMeter;                       ///< ИЗМЕРЕНИЯ - ЧАСТОТОМЕР
extern const   Choice cFreqMeter_Enable;                ///< ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Частотомер
static void  OnChanged_FreqMeter_Enable(bool param);    
extern const   Choice cFreqMeter_TimeF;                 ///< ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Время счёта F
extern const   Choice cFreqMeter_FreqClc;               ///< ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Метки времени
extern const   Choice cFreqMeter_NumPeriods;            ///< ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Кол-во периодов
extern const   Choice cIsShow;                          ///< ИЗМЕРЕНИЯ - Показывать
extern const   Choice cNumber;                          ///< ИЗМЕРЕНИЯ - Количество
static bool   IsActive_Number(void);
extern const   Choice cChannels;                        ///< ИЗМЕРЕНИЯ - Каналы
static bool   IsActive_Channels(void);
extern const    Page ppTune;                            ///< ИЗМЕРЕНИЯ - НАСТРОИТЬ
static bool   IsActive_Tune(void);
static void   OnRegSet_Tune(int angle);
extern const  SButton bTune_Exit;                       ///< ИЗМЕРЕНИЯ - НАСТРОИТЬ - Выход
extern const  SButton bTune_Markers;                    ///< ИЗМЕРЕНИЯ - НАСТРОИТЬ - Маркеры
static void    OnPress_Tune_Markers(void);
static void       Draw_Tune_Markers(int x, int y);
extern const  SButton bTune_Settings;                   ///< ИЗМЕРЕНИЯ - НАСТРОИТЬ - Настройка
static void    OnPress_Tune_Settings(void);
static void  Draw_Tune_Settings(int x, int y);
extern const   Choice cMode;                            ///< ИЗМЕРЕНИЯ - Вид
static bool   IsActive_Mode(void);

int8 posActive = 0;                 ///< Позиция активного измерения (на котором курсор)
bool pageChoiceIsActive = false;    ///< Если true - раскрыта страница выбора измерения
int8 posOnPageChoice = 0;           ///< Позиция курсора на странице выбора измерения

extern const Page mainPage;

// ИЗМЕРЕНИЯ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pMeasures =
{
    Item_Page, &mainPage, 0,
    {
        "ИЗМЕРЕНИЯ", "MEASURES",
        "Автоматические измерения",
        "Automatic measurements"
    },
    Page_Measures,
    {
        (void *)&ppFreqMeter,   // ИЗМЕРЕНИЯ - ЧАСТОТОМЕР
        (void *)&cIsShow,       // ИЗМЕРЕНИЯ - Показывать
        (void *)&cNumber,       // ИЗМЕРЕНИЯ - Количество
        (void *)&cChannels,     // ИЗМЕРЕНИЯ - Каналы
        (void *)&ppTune,        // ИЗМЕРЕНИЯ - НАСТРОИТЬ
        (void *)&cMode          // ИЗМЕРЕНИЯ - Вид
    }
};

// ИЗМЕРЕНИЯ - ЧАСТОТОМЕР ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page ppFreqMeter =
{
    Item_Page, &pMeasures, 0,
    {
        "ЧАСТОТОМЕР", "FREQ METER",
        "",
        ""
    },
    Page_Service_FreqMeter,
    {
        (void *)&cFreqMeter_Enable,     // ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Частотомер
        (void *)&cFreqMeter_TimeF,      // ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Время счёта F
        (void *)&cFreqMeter_FreqClc,    // ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Метки времени
        (void *)&cFreqMeter_NumPeriods  // ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Кол-во периодов
    }
};

// ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Частотомер ---------------------------------------------------------------------------------------------------------------
static const Choice cFreqMeter_Enable =
{
    Item_Choice, &ppFreqMeter, 0,
    {
        "Частотомер", "Freq meter",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8 *)&FREQ_METER_ENABLED, OnChanged_FreqMeter_Enable
};

static void OnChanged_FreqMeter_Enable(bool param)
{
    FreqMeter_Init();
}

// ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Время счёта F ------------------------------------------------------------------------------------------------------------
static const Choice cFreqMeter_TimeF =
{
    Item_Choice, &ppFreqMeter, 0,
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
    (int8 *)&FREQ_METER_TIMECOUNTING, OnChanged_FreqMeter_Enable
};

// ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Метки времени ------------------------------------------------------------------------------------------------------------
static const Choice cFreqMeter_FreqClc =
{
    Item_Choice, &ppFreqMeter, 0,
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
    (int8 *)&FREQ_METER_FREQ_CLC, OnChanged_FreqMeter_Enable
};

// ИЗМЕРЕНИЯ - ЧАСТОТОМЕР - Кол-во периодов ----------------------------------------------------------------------------------------------------------
static const Choice cFreqMeter_NumPeriods =
{
    Item_Choice, &ppFreqMeter, 0,
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
    (int8 *)&FREQ_METER_NUM_PERIODS, OnChanged_FreqMeter_Enable
};

// ИЗМЕРЕНИЯ - Показывать ----------------------------------------------------------------------------------------------------------------------------
static const Choice cIsShow =
{
    Item_Choice, &pMeasures, 0,
    {
        "Показывать", "Show",
        "Выводить или не выводить измерения на экран",
        "Output or output measurements on screen"
    },
    {
        {"Нет", "No"},
        {"Да", "Yes"}
    },
    (int8 *)&SHOW_MEASURES
};

// ИЗМЕРЕНИЯ - Количество ----------------------------------------------------------------------------------------------------------------------------
static const Choice cNumber =
{
    Item_Choice, &pMeasures, IsActive_Number,
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
    (int8 *)&NUM_MEASURES
};

static bool IsActive_Number(void)
{
    return SHOW_MEASURES;
}

// ИЗМЕРЕНИЯ - Каналы --------------------------------------------------------------------------------------------------------------------------------
static const Choice cChannels =
{
    Item_Choice, &pMeasures, IsActive_Channels,
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
    (int8 *)&SOURCE_MEASURE
};

static bool IsActive_Channels(void)
{
    return SHOW_MEASURES;
}

// ИЗМЕРЕНИЯ - Вид -----------------------------------------------------------------------------------------------------------------------------------
static const Choice cMode =
{
    Item_Choice, &pMeasures, IsActive_Mode,
    {
        "Вид", "View",
        "Уменьшать или нет зону вывода сигнала для исключения перекрытия его результами измерений",
        "Decrease or no zone output signal to avoid overlapping of its measurement results"
    },
    {
        {"Как есть",    "As is"},
        {"Уменьшать",   "Reduce"}
    },
    (int8 *)&set.meas_ModeViewSignals
};

static bool IsActive_Mode(void)
{
    return SHOW_MEASURES;
}


// ИЗМЕРЕНИЯ - НАСТРОИТЬ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppTune =
{
    Item_Page, &pMeasures, IsActive_Tune,
    {
        "НАСТРОИТЬ", "CONFIGURE",
        "Переход в режми точной настройки количества и видов измерений",
        "Transition to rezhm of exact control of quantity and types of measurements"
    },
    PageSB_Measures_Tune,
    {
        (void *)&bTune_Exit,    // ИЗМЕРЕНИЯ - НАСТРОИТЬ - Выход
        (void *)0,
        (void *)0,
        (void *)0,
        (void *)&bTune_Markers, // ИЗМЕРЕНИЯ - НАСТРОИТЬ - Маркеры
        (void *)&bTune_Settings // ИЗМЕРЕНИЯ - НАСТРОИТЬ - Настройка
    },
    true, 0, 0, OnRegSet_Tune
};

static bool IsActive_Tune(void)
{
    return SHOW_MEASURES;
}

static void OnRegSet_Tune(int angle)
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
        painter.ResetFlash();
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
static const SButton bTune_Exit =
{
    Item_SmallButton, &ppTune,
    COMMON_BEGIN_SB_EXIT,
    OnPressSB_Exit,
    DrawSB_Exit
};

// ИЗМЕРЕНИЯ - НАСТРОИТЬ - Маркеры -------------------------------------------------------------------------------------------------------------------
static const SButton bTune_Markers =
{
    Item_SmallButton, &ppTune, 0,
    {
        "Маркер", "Marker",
        "Позволяет установить маркеры для визуального контроля измерений",
        "Allows to establish markers for visual control of measurements"
    },
    OnPress_Tune_Markers,
    Draw_Tune_Markers
};

static void OnPress_Tune_Markers(void)
{
    Measure_ShortPressOnSmallButonMarker();
}

static void Draw_Tune_Markers(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 2, '\x60');
    painter.SetFont(TypeFont_8);
}

// ИЗМЕРЕНИЯ - НАСТРОИТЬ - Настройка -----------------------------------------------------------------------------------------------------------------
static const SButton bTune_Settings =
{
    Item_SmallButton, &ppTune, 0,
    {
        "Настройка", "Setup",
        "Позволяет выбрать необходимые измерения",
        "Allows to choose necessary measurements"
    },
    OnPress_Tune_Settings,
    Draw_Tune_Settings
};

static void OnPress_Tune_Settings(void)
{
    pageChoiceIsActive = !pageChoiceIsActive;
    if (pageChoiceIsActive)
    {
        posOnPageChoice = MEASURE(posActive);
    }
}

static void Draw_Tune_Settings(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, '\x62');
    painter.SetFont(TypeFont_8);
}
