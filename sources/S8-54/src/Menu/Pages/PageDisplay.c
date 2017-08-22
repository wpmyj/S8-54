// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Menu/Menu.h"
#include "Menu/MenuItems.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsDisplay.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "PageDisplay.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern ColorType colorTypeGrid;

       const Page pDisplay;
static const         Choice cViewMode;                          ///< ДИСПЛЕЙ - Отображение
static const          Page ppAccum;                             ///< ДИСПЛЕЙ - НАКОПЛЕНИЕ
static bool         IsActive_Accum(void);
static const         Choice cAccum_Num;                         ///< ДИСПЛЕЙ - НАКОПЛЕНИЕ - Количество
static const         Choice cAccum_Mode;                        ///< ДИСПЛЕЙ - НАКОПЛЕНИЕ - Режим
static const         Button bAccum_Clear;                       ///< ДИСПЛЕЙ - НАКОПЛЕНИЕ - Очистить
static bool         IsActive_Accum_Clear(void);
       void          OnPress_Accum_Clear(void);
static const          Page ppAverage;                           ///< ДИСПЛЕЙ - УСРЕДНЕНИЕ
static bool         IsActive_Average(void);
static const         Choice cAverage_Num;                       ///< ДИСПЛЕЙ - УСРЕДНЕНИЕ - Количество
static const         Choice cAverage_Mode;                      ///< ДИСПЛЕЙ - УСРЕДНЕНИЕ - Режим
static const         Choice cMinMax;                            ///< ДИСПЛЕЙ - Мин Макс
static bool         IsActive_MinMax(void);
static void        OnChanged_MinMax(bool active);
static const         Choice cSmoothing;                         ///< ДИСПЛЕЙ - Сглаживание
static const         Choice cRefreshFPS;                        ///< ДИСПЛЕЙ - Частота обновл
static void        OnChanged_RefreshFPS(bool active);
static const          Page ppGrid;                              ///< ДИСПЛЕЙ - СЕТКА
static const         Choice cGrid_Type;                         ///< ДИСПЛЕЙ - СЕТКА - Тип
static const       Governor gGrid_Brightness;                   ///< ДИСПЛЕЙ - СЕТКА - Яркость
       void        OnChanged_Grid_Brightness(void);
static void       BeforeDraw_Grid_Brightness(void);
static const         Choice cScaleYtype;                        ///< ДИСПЛЕЙ - Смещение
static const          Page ppSettings;                          ///< ДИСПЛЕЙ - НАСТРОЙКИ
static const         Page pppSettings_Colors;                   ///< ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА
static const         Choice cSettings_Colors_Scheme;            ///< ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Цветовая схема
static const GovernorColor gcSettings_Colors_ChannelA;          ///< ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 1
static const GovernorColor gcSettings_Colors_ChannelB;          ///< ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 2
static const GovernorColor gcSettings_Colors_Grid;              ///< ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Сетка
static const         Choice cSettings_Colors_Background;        ///< ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Фон
static void        OnChanged_Settings_Colors_Background(bool active);
       void PageService_InitGlobalColors(void);
static const       Governor gSettings_Brightness;               ///< ДИСПЛЕЙ - НАСТРОЙКИ - Яркость
static void        OnChanged_Settings_Brightness(void);
static const       Governor gSettings_Levels;                   ///< ДИСПЛЕЙ - НАСТРОЙКИ - Уровни
static const       Governor gSettings_Time;                     ///< ДИСПЛЕЙ - НАСТРОЙКИ - Время
static const         Choice cSettings_StringNavigation;         ///< ДИСПЛЕЙ - НАСТРОЙКИ - Строка меню
static const         Choice cSettings_AltMarkers;               ///< ДИСПЛЕЙ - НАСТРОЙКИ - Доп. маркеры
static const         Choice cSettings_AutoHide;                 ///< ДИСПЛЕЙ - НАСТРОЙКИ - Скрывать
static void        OnChanged_Settings_AutoHide(bool active);


extern Page mainPage;

// ДИСПЛЕЙ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PageDisplay_Init(void)
{
    OnChanged_Settings_Colors_Background(true);   // Заносим значения в гувернёры цветов
}

const Page pDisplay =
{
    Item_Page, &mainPage, 0,
    {
        "ДИСПЛЕЙ", "DISPLAY",
        "Содержит настройки отображения дисплея.",
        "Contains settings of display of the display."
    },
    Page_Display,
    {
        (void *)&cViewMode,     // ДИСПЛЕЙ - Отображение
        (void *)&ppAccum,       // ДИСПЛЕЙ - НАКОПЛЕНИЕ
        (void *)&ppAverage,     // ДИСПЛЕЙ - УСРЕДНЕНИЕ
        (void *)&cMinMax,       // ДИСПЛЕЙ - Мин Макс
        (void *)&cSmoothing,    // ДИСПЛЕЙ - Сглаживание
        (void *)&cRefreshFPS,   // ДИСПЛЕЙ - Частота обновл
        (void *)&ppGrid,        // ДИСПЛЕЙ - СЕТКА
        (void *)&cScaleYtype,   // ДИСПЛЕЙ - Смещение
        (void *)&ppSettings     // ДИСПЛЕЙ - НАСТРОЙКИ
    }
};

// ДИСПЛЕЙ - Отображение -----------------------------------------------------------------------------------------------------------------------------
static const Choice cViewMode =
{
    Item_Choice, &pDisplay, 0,
    {
        "Отображение", "View",
        "Задаёт режим отображения сигнала.",
        "Sets the display mode signal."
    },
    {
        {"Вектор",  "Vector"},
        {"Точки",   "Points"}
    },
    (int8 *)&MODE_DRAW_SIGNAL
};

// ДИСПЛЕЙ - НАКОПЛЕНИЕ ------------------------------------------------------------------------------------------------------------------------------
static const Page ppAccum =
{
    Item_Page, &pDisplay, IsActive_Accum,
    {
        "НАКОПЛЕНИЕ", "ACCUMULATION",
        "Настройки режима отображения последних сигналов на экране.",
        "Mode setting signals to display the last screen."
    },
    Page_Display_Accum,
    {
        (void *)&cAccum_Num,    // ДИСПЛЕЙ - НАКОПЛЕНИЕ - Количество
        (void *)&cAccum_Mode,   // ДИСПЛЕЙ - НАКОПЛЕНИЕ - Режим
        (void *)&bAccum_Clear   // ДИСПЛЕЙ - НАКОПЛЕНИЕ - Очистить
    }
};

static bool IsActive_Accum(void)
{
    return SET_TBASE > TBase_20ns;
}

// ДИСПЛЕЙ - НАКОПЛЕНИЕ - Количество -----------------------------------------------------------------------------------------------------------------
static const Choice cAccum_Num =
{
    Item_ChoiceReg, &ppAccum, 0,
    {
        "Количество", "Number"
        ,
        "Задаёт максимальное количество последних сигналов на экране. Если в настройке \"Режим\" выбрано \"Бесконечность\", экран очищается только "
        "нажатием кнопки \"Очистить\"."
        "\"Бесконечность\" - каждое измерение остаётся на дисплее до тех пор, пока не будет нажата кнопка \"Очистить\"."
        ,
        "Sets the maximum quantity of the last signals on the screen. If in control \"Mode\" it is chosen \"Infinity\", the screen is cleared only "
        "by pressing of the button \"Clear\"."
        "\"Infinity\" - each measurement remains on the display until the button \"Clear\" is pressed."
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {"2",   "2"},
        {"4",   "4"},
        {"8",   "8"},
        {"16",  "16"},
        {"32",  "32"},
        {"64",  "64"},
        {"128", "128"},
        {"Бесконечность", "Infinity"}
    },
    (int8 *)&ENUM_ACCUM
};

// ДИСПЛЕЙ - НАКОПЛЕНИЕ - Режим ----------------------------------------------------------------------------------------------------------------------
static const Choice cAccum_Mode =
{
    Item_Choice, &ppAccum, 0,
    {
        "Режим", "Mode"
        ,
        "1. \"Сбрасывать\" - после накопления заданного количества измерения происходит очистка дисплея. Этот режим удобен, когда памяти не хватает "
        "для сохранения нужного количества измерений.\n"
        "2. \"Не сбрасывать\" - на дисплей всегда выводится заданное или меньшее (в случае нехватки памяти) количество измерений. Недостатком является "
        "меньшее быстродействие и невозможность накопления заданного количества измерений при недостатке памяти."
        ,
        "1. \"Dump\" - after accumulation of the set number of measurement there is a cleaning of the display. This mode is convenient when memory "
        "isn't enough for preservation of the necessary number of measurements.\n"
        "2. \"Not to dump\" - the number of measurements is always output to the display set or smaller (in case of shortage of memory). Shortcoming "
        "is smaller speed and impossibility of accumulation of the set number of measurements at a lack of memory."
    },
    {
        {"Не сбрасывать",   "Not to dump"},
        {"Сбрасывать",      "Dump"}
    },
    (int8 *)&MODE_ACCUM
};

// ДИСПЛЕЙ - НАКОПЛЕНИЕ - Очистить -------------------------------------------------------------------------------------------------------------------
static const Button bAccum_Clear =
{
    Item_Button, &ppAccum, IsActive_Accum_Clear,
    {
        "Очистить", "Clear",
        "Очищает экран от накопленных сигналов.",
        "Clears the screen of the saved-up signals."
    },
    OnPress_Accum_Clear, EmptyFuncVII
};

static bool IsActive_Accum_Clear(void)
{
    return ENUM_ACCUM != ENumAccum_1 && !MODE_ACCUM_NO_RESET;
}

void OnPress_Accum_Clear(void)
{
    NEED_FINISH_DRAW = 1;
}

// ДИСПЛЕЙ - УСРЕДНЕНИЕ ------------------------------------------------------------------------------------------------------------------------------
static const Page ppAverage =
{
    Item_Page, &pDisplay, IsActive_Average,
    {
        "УСРЕДНЕНИЕ", "AVERAGE",
        "Настройки режима усреднения по последним измерениям.",
        "Settings of the mode of averaging on the last measurements."
    },
    Page_Display_Average,
    {
        (void *)&cAverage_Num,  // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Количество
        (void *)&cAverage_Mode  // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Режим
    }
};

static bool IsActive_Average(void)
{
    return true;
}

// ДИСПЛЕЙ - УСРЕДНЕНИЕ - Количество -----------------------------------------------------------------------------------------------------------------
static const Choice cAverage_Num =
{
    Item_ChoiceReg, &ppAverage, 0,
    {
        "Количество", "Number",
        "Задаёт количество последних измерений, по которым производится усреднение.",
        "Sets number of the last measurements on which averaging is made."
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {"2",   "2"},
        {"4",   "4"},
        {"8",   "8"},
        {"16",  "16"},
        {"32",  "32"},
        {"64",  "64"},
        {"128", "128"},
        {"256", "256"},
        {"512", "512"}
    },
    (int8 *)&ENUM_AVE
};

// ДИСПЛЕЙ - УСРЕДНЕНИЕ - Режим ----------------------------------------------------------------------------------------------------------------------
static const Choice cAverage_Mode =
{
    Item_Choice, &ppAverage, 0,
    {
        "Режим", "Mode"
        ,
        "1. \"Точно\" - точный режим усреднения, когда в расчёте участвуют только последние сигналы.\n"
        "2. \"Приблизительно\" - приблизительный режим усреднения. Имеет смысл использовать, когда задано количество измерений большее, чем может "
        "поместиться в памяти."
        ,
        "1. \"Accurately\" - the exact mode of averaging when only the last signals participate in calculation.\n"
        "2. \"Around\" - approximate mode of averaging. It makes sense to use when the number of measurements bigger is set, than can be located in "
        "memory."
    },
    {
        {"Точно",           "Accurately"},
        {"Приблизительно",  "Around"}
    },
    (int8 *)&MODE_AVERAGING
};

// ДИСПЛЕЙ - Мин Макс --------------------------------------------------------------------------------------------------------------------------------
static const Choice cMinMax =
{
    Item_ChoiceReg, &pDisplay, IsActive_MinMax,
    {
        "Мин Макс", "Min Max",
        "Задаёт количество последних измерений, по которым строятся ограничительные линии, огибающие минимумы и максимумы измерений.",
        "Sets number of the last measurements on which the limiting lines which are bending around minima and maxima of measurements are under "
        "construction."
    },
    {
        {DISABLE_RU,DISABLE_EN},
        {"2",   "2"},
        {"4",   "4"},
        {"8",   "8"},
        {"16",  "16"},
        {"32",  "32"},
        {"64",  "64"},
        {"128", "128"}
    },
    (int8 *)&set.disp_ENumMinMax, OnChanged_MinMax
};

static bool IsActive_MinMax(void)
{
    return SET_TBASE > TBase_20ns;
}

static void OnChanged_MinMax(bool active)
{
    /*
    int maxMeasures = DS_NumberAvailableEntries();  
    int numMinMax = sDisplay_NumberMinMax();

    if (maxMeasures < numMinMax)
    {
        Display_ShowWarningWithNumber(ExcessValues, maxMeasures);
    }
    */
}

// ДИСПЛЕЙ - Сглаживание -----------------------------------------------------------------------------------------------------------------------------
static const Choice cSmoothing =
{
    Item_ChoiceReg, &pDisplay, 0,
    {
        "Сглаживание", "Smoothing",
        "Устанавливает количество точек для расчёта сглаженного по соседним точкам сигнала.",
        "Establishes quantity of points for calculation of the signal smoothed on the next points."
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {"2 точки", "2 points"},
        {"3 точки", "3 points"},
        {"4 точки", "4 points"},
        {"5 точек", "5 points"},
        {"6 точки", "6 points"},
        {"7 точек", "7 points"},
        {"8 точки", "8 points"},
        {"9 точки", "9 points"},
        {"10 точек", "10 points"}
    },
    (int8 *)&ENUM_SMOOTHING
};

// ДИСПЛЕЙ - Частота обновл --------------------------------------------------------------------------------------------------------------------------
static const Choice cRefreshFPS =
{
    Item_Choice, &pDisplay, 0,
    {
        "Частота обновл", "Refresh rate",
        "Задаёт максимальное число выводимых в секунду кадров.",
        "Sets the maximum number of the shots removed in a second."
    },
    {
        {"25",  "25"},
        {"10",  "10"},
        {"5",   "5"},
        {"2",   "2"},
        {"1",   "1"}
    },
    (int8 *)&ENUM_SIGNALS_IN_SEC, OnChanged_RefreshFPS
};

static void OnChanged_RefreshFPS(bool active)
{
    FPGA_SetENumSignalsInSec(NUM_SIGNALS_IN_SEC);
}

// ДИСПЛЕЙ - СЕТКА -----------------------------------------------------------------------------------------------------------------------------------
static const Page ppGrid =
{
    Item_Page, &pDisplay, 0,
    {
        "СЕТКА", "GRID",
        "Содержит настройки отображения координатной сетки.",
        "Contains settings of display of a coordinate grid."
    },
    Page_Display_Grid,
    {
        (void *)&cGrid_Type,        // ДИСПЛЕЙ - СЕТКА - Тип
        (void *)&gGrid_Brightness   // ДИСПЛЕЙ - СЕТКА - Яркость
    }
};

// ДИСПЛЕЙ - СЕТКА - Тип -----------------------------------------------------------------------------------------------------------------------------
static const Choice cGrid_Type =
{
    Item_Choice, &ppGrid, 0,
    {
        "Тип", "Type",
        "Выбор типа сетки.",
        "Choice like grid."
    },
    {
        {"Тип 1", "Type 1"},
        {"Тип 2", "Type 2"},
        {"Тип 3", "Type 3"},
        {"Тип 4", "Type 4"}
    },
    (int8 *)&TYPE_GRID
};

// ДИСПЛЕЙ - СЕТКА - Яркость -------------------------------------------------------------------------------------------------------------------------
static const Governor gGrid_Brightness =
{
    Item_Governor, &ppGrid, 0,
    {
        "Яркость", "Brightness",
        "Устанавливает яркость сетки.",
        "Adjust the brightness of the grid."
    },
    &set.disp_BrightnessGrid, 0, 100, OnChanged_Grid_Brightness, BeforeDraw_Grid_Brightness
};

void OnChanged_Grid_Brightness(void)
{
    Color_SetBrightness(&colorTypeGrid, set.disp_BrightnessGrid / 1e2f);
}


static void BeforeDraw_Grid_Brightness(void)
{
    Color_Init(&colorTypeGrid, false);
    set.disp_BrightnessGrid = (int16)(colorTypeGrid.brightness * 100.0f);
}

// ДИСПЛЕЙ - Смещение --------------------------------------------------------------------------------------------------------------------------------
static const Choice cScaleYtype =
{
    Item_Choice, &pDisplay, 0,
    {
        "Смещение", "Оffset",
        "Задаёт режим удержания смещения по вертикали\n1. \"Напряжение\" - сохраняется наряжение смещения.\n2. \"Деления\" - сохраняется положение "
        "смещения на экране.",
        "Sets the mode of retaining the vertical displacement\n1. \"Voltage\" - saved dressing bias.\n2. \"Divisions\" - retained the position of "
        "the offset on the screen."
    },
    {
        {"Напряжение", "Voltage"},
        {"Деления", "Divisions"}
    },
    (int8 *)&LINKING_RSHIFT
};

// ДИСПЛЕЙ - НАСТРОЙКИ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page ppSettings =
{
    Item_Page, &pDisplay, 0,
    {
        "НАСТРОЙКИ", "SETTINGS",
        "Дополнительные настройки дисплея",
        "Additional display settings"
    },
    Page_Display_Settings,
    {
        (void *)&pppSettings_Colors,            // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА
        (void *)&gSettings_Brightness,          // ДИСПЛЕЙ - НАСТРОЙКИ - Яркость
        (void *)&gSettings_Levels,              // ДИСПЛЕЙ - НАСТРОЙКИ - Уровни
        (void *)&gSettings_Time,                // ДИСПЛЕЙ - НАСТРОЙКИ - Время
        (void *)&cSettings_StringNavigation,    // ДИСПЛЕЙ - НАСТРОЙКИ - Строка меню
        (void *)&cSettings_AltMarkers,          // ДИСПЛЕЙ - НАСТРОЙКИ - Доп. маркеры
        (void *)&cSettings_AutoHide             // ДИСПЛЕЙ - НАСТРОЙКИ - Скрывать
    }
};

// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА -----------------------------------------------------------------------------------------------------------------------
/// \todo Добавить дополнительные цвета 1-го и 2-го каналов
static const Page pppSettings_Colors =
{
    Item_Page, &ppSettings, 0,
    {
        "ЦВЕТА", "COLORS",
        "Выбор цветов дисплея",
        "The choice of colors display"
    },
    Page_Display_Settings_Colors,
    {
        (void *)&cSettings_Colors_Scheme,       // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Цветовая схема
        (void *)&gcSettings_Colors_ChannelA,    // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 1
        (void *)&gcSettings_Colors_ChannelB,    // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 2
        (void *)&gcSettings_Colors_Grid,        // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Сетка
        (void *)&cSettings_Colors_Background    // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Фон
        //(void *)&mgcColorChannel0alt,
        //(void *)&mgcColorChannel1alt,
        //(void *)&mgcColorMenu1,
        //(void *)&mgcColorMenu2,
        //(void *)&mgcColorMenu3
        //,(void *)&mgcColorMathem,
        //(void *)&mgcColorFFT,
        //(void *)&mcServDisplInverse
    }
};

// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Цветовая схема ------------------------------------------------------------------------------------------------------
static const Choice cSettings_Colors_Scheme =
{
    Item_Choice, &pppSettings_Colors, 0,
    {
        "Цветовая схема", "Color scheme",
        "Режим работы калибратора",
        "Mode of operation of the calibrator"
    },
    {
        { "Схема 1", "Scheme 1" },
        { "Схема 2", "Scheme 2" }
    },
    (int8 *)&set.serv_ColorScheme
};

// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 1 -------------------------------------------------------------------------------------------------------------
static ColorType colorT1 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_DATA_1};
static const GovernorColor gcSettings_Colors_ChannelA =
{
    Item_GovernorColor, &pppSettings_Colors, 0,
    {
        "Канал 1", "Channel 1",
        "",
        ""
    },
    &colorT1
};

// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 2 -------------------------------------------------------------------------------------------------------------
static ColorType colorT2 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_DATA_2};
static const GovernorColor gcSettings_Colors_ChannelB =
{
    Item_GovernorColor, &pppSettings_Colors, 0,
    {
        "Канал 2", "Channel 2",
        "",
        ""
    },
    &colorT2
};

// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Сетка ---------------------------------------------------------------------------------------------------------------
ColorType colorTypeGrid = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_GRID};
static const GovernorColor gcSettings_Colors_Grid =
{
    Item_GovernorColor, &pppSettings_Colors, 0,
    {
        "Сетка", "Grid",
        "Устанавливает цвет сетки",
        "Sets the grid color"
    },
    &colorTypeGrid
};

// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Фон -----------------------------------------------------------------------------------------------------------------
static const Choice cSettings_Colors_Background =
{
    Item_Choice, &pppSettings_Colors, 0,
    {
        "Фон", "Background",
        "Выбор цвета фона",
        "Choice of color of a background"
    },
    {
        { "Чёрный", "Black" },
        { "Белый", "White" }
    },
    (int8 *)&BACKGROUND, OnChanged_Settings_Colors_Background
};

static void OnChanged_Settings_Colors_Background(bool active)
{
    PageService_InitGlobalColors();

    gcSettings_Colors_ChannelA.colorType->color = gColorChan[A];
    gcSettings_Colors_ChannelB.colorType->color = gColorChan[B];
    gcSettings_Colors_Grid.colorType->color = gColorGrid;

    Color_Init((ColorType*)&gcSettings_Colors_ChannelA, true);
    Color_Init((ColorType*)&gcSettings_Colors_ChannelB, true);
    Color_Init((ColorType*)&gcSettings_Colors_Grid, true);
}

void PageService_InitGlobalColors(void)
{
    gColorBack = BACKGROUND_BLACK ? COLOR_BLACK : COLOR_WHITE;
    gColorFill = BACKGROUND_BLACK ? COLOR_WHITE : COLOR_BLACK;
    gColorGrid = BACKGROUND_BLACK ? COLOR_GRID : COLOR_GRID_WHITE;
    gColorChan[A] = BACKGROUND_BLACK ? COLOR_DATA_1 : COLOR_DATA_A_WHITE_ACCUM;
    gColorChan[B] = BACKGROUND_BLACK ? COLOR_DATA_2 : COLOR_DATA_B_WHITE_ACCUM;
    gColorChan[A_B] = gColorChan[Math] = BACKGROUND_BLACK ? COLOR_WHITE : COLOR_BLACK;
}

// ДИСПЛЕЙ - НАСТРОЙКИ - Яркость ---------------------------------------------------------------------------------------------------------------------
static const Governor gSettings_Brightness =
{
    Item_Governor, &ppSettings, 0,
    {
        "Яркость", "Brightness",
        "Установка яркости свечения дисплея",
        "Setting the brightness of the display"
    },
    &BRIGHTNESS_DISPLAY, 0, 100, OnChanged_Settings_Brightness
};

static void OnChanged_Settings_Brightness(void)
{
    Painter_SetBrightnessDisplay(BRIGHTNESS_DISPLAY);
}

// ДИСПЛЕЙ - НАСТРОЙКИ - Уровни ----------------------------------------------------------------------------------------------------------------------
static const Governor gSettings_Levels =
{
    Item_Governor, &ppSettings, 0,
    {
        "Уровни", "Levels",
        "Задаёт время, в течение которого после поворота ручки сещения напряжения на экране остаётся вспомогательная метка уровня смещения",
        "Defines the time during which, after turning the handle visits to the voltage on the screen remains auxiliary label offset level"
    },
    &TIME_SHOW_LEVELS, 0, 125
};

// ДИСПЛЕЙ - НАСТРОЙКИ - Время -----------------------------------------------------------------------------------------------------------------------
static const Governor gSettings_Time =
{
    Item_Governor, &ppSettings, 0,
    {
        "Время", "Time",
        "Установка времени, в течение которого сообщения будут находиться на экране",
        "Set the time during which the message will be on the screen"
    },
    &TIME_MESSAGES, 1, 99
};

// ДИСПЛЕЙ - НАСТРОЙКИ - Строка меню -----------------------------------------------------------------------------------------------------------------
static const Choice cSettings_StringNavigation =
{
    Item_Choice, &ppSettings, 0,
    {
        "Строка меню", "Path menu",
        "При выборе \nПоказывать\n слева вверху экрана выводится полный путь до текущей страницы меню", /// \todo Исправить перевод
        "When choosing \nDisplay\n at the top left of the screen displays the full path to the current page menu"
    },
    {
        { "Временно", "Temporary" },    /// \todo Исправить перевод
        { "Всегда", "All" },
        { "Никогда", "None" }
    },
    (int8 *)&SHOW_STRING_NAVI
};

// ДИСПЛЕЙ - НАСТРОЙКИ - Доп. маркеры ----------------------------------------------------------------------------------------------------------------
static const Choice cSettings_AltMarkers =
{
    Item_Choice, &ppSettings, 0,
    {
        "Доп. маркеры", "Alt. markers"
        ,
        "Устанавливает режим отображения дополнительных маркеров уровней смещения и синхронизации:\n"
        "\"Скрывать\" - дополнительные маркеры не показываются,\n"
        "\"Показывать\" - дополнительные маркеры показываются всегда,\n"
        "\"Авто\" - дополнительные маркеры показываются в течение 5 сек после поворота ручки смещения канала по напряжению или уровня синхронизации"
        ,
        "Sets the display mode of additional markers levels of displacement and synchronization:\n"
        "\"Hide\" - additional markers are not shown,\n"
        "\"Show\" - additional markers are shown always,\n"
        "\"Auto\" - additional markers are displayed for 5 seconds after turning the handle channel offset voltage or trigger level"
    },
    {
        { "Скрывать", "Hide" },
        { "Показывать", "Show" },
        { "Авто", "Auto" }
    },
    (int8 *)&ALT_MARKERS, Display_ChangedRShiftMarkers
};

// ДИСПЛЕЙ - НАСТРОЙКИ - Скрывать --------------------------------------------------------------------------------------------------------------------
static const Choice cSettings_AutoHide =
{
    Item_Choice, &ppSettings, 0,
    {
        "Скрывать", "Hide",
        "Установка после последнего нажатия кнопки или поворота ручки, по истечении которого меню автоматически убирается с экрана",
        "Installation after the last keystroke or turning the handle, after which the menu will automatically disappear"
    },
    {
        { "Никогда", "Never" },
        { "Через 5 сек", "Through 5 s" },
        { "Через 10 сек", "Through 10 s" },
        { "Через 15 сек", "Through 15 s" },
        { "Через 30 сек", "Through 30 s" },
        { "Через 60 сек", "Through 60 s" }
    },
    (int8 *)&MENU_AUTO_HIDE, OnChanged_Settings_AutoHide
};

static void OnChanged_Settings_AutoHide(bool autoHide)
{
    Menu_SetAutoHide(autoHide);
}



/*
//----------------------------------------------------------------------------------------------------------------------------------------------------
// СЕРВИС - ДИСПЛЕЙ - ЦВЕТА - МЕНЮ Область
static ColorType colorT6 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_MENU_FIELD };
const GovernorColor mgcColorMenu3 = { Item_GovernorColor, &pppSettings_Colors, { "Меню Область", "Menu Field" },
{
    "",
    "",
},
0, &colorT6
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
// СЕРВИС - ДИСПЛЕЙ - ЦВЕТА - МЕНЮ Заголовок
static ColorType colorT7 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_MENU_TITLE };
const GovernorColor mgcColorMenu1 = { Item_GovernorColor, &pppSettings_Colors, { "Mеню Заголовок", "Menu Title" },
{
    "",
    ""
},
0, &colorT7, OnChangedColorMenuTitle
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
// СЕРВИС - ДИСПЛЕЙ - ЦВЕТА - МЕНЮ Пункт
static ColorType colorT8 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_MENU_ITEM };
const GovernorColor mgcColorMenu2 = { Item_GovernorColor, &pppSettings_Colors, { "Меню Пункт", "Menu Item" },
{
    "",
    ""
},
0, &colorT8, OnChangedColorMenu2Item
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
// СЕРВИС - ДИСПЛЕЙ - ЦВЕТА - Инверсия
const Choice mcServDisplInverse =
{
    Item_Choice, &pppSettings_Colors, { "Инверсия", "Inverse" },
    {
        "",
        ""
    },
    0,
    { { DISABLE_RU, DISABLE_EN },
    { ENABLE_RU, ENABLE_EN },
    },
    0
};
*/
