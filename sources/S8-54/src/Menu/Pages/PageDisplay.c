#include "defines.h"
#include "Menu/Menu.h"
#include "Menu/MenuItems.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsDisplay.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "PageDisplay.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern ColorType colorTypeGrid;

extern Page mainPage;

       const Page mpDisplay;
static const Choice mcViewMode;
static const Choice mcMinMax;
static const Choice mcSmoothing;
static const Choice mcRefreshFPS;
static const Choice mcScaleYtype;

static const Page mspAccum;
static const Choice mcAccum_Num;
static const Choice mcAccum_Mode;
static const Button mbAccum_Crear;

static const Page mspAverage;
static const Choice mcAverage_Num;
static const Choice mcAverage_Mode;

static const Page mspGrid;
static const Choice mcGrid_Type;
static const Governor mgGrid_Brightness;

static const Page mspSettings;
static const Choice mcSettings_Colors_Background;
static void OnChange_Settings_Colors_Background(bool active);
static const Choice mcSettings_Colors_Scheme;
static const GovernorColor mgcSettings_Colors_ChannelA;
static const GovernorColor mgcSettings_Colors_ChannelB;
static const GovernorColor mgcSettings_Colors_Grid;
static const Page mspSettings_Colors;
static const Governor mgSettings_Brightness;
static const Governor mgSettings_Levels;
static const Governor mgSettings_Time;
static const Choice mcSettings_StringNavigation;
static const Choice mcSettings_AltMarkers;
static const Choice mcSettings_AutoHide;
static void OnChange_Settings_Brightness(void);

static bool IsActive_MinMax(void);
static void OnChange_MinMax(bool active);
static void OnChange_RefreshFPS(bool active);
static bool IsActive_Accum(void);
static bool IsActive_Accum_Clear(void);
       void OnPress_Accum_Clear(void);
static bool IsActive_Average(void);
       void OnChange_Grid_Brightness(void);
static void BeforeDraw_Grid_Brightness(void);


// ДИСПЛЕЙ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PageDisplay_Init(void)
{
    OnChange_Settings_Colors_Background(true);   // Заносим значения в гувернёры цветов
}

const Page mpDisplay =
{
    Item_Page, &mainPage,
    {
        "ДИСПЛЕЙ", "DISPLAY"
    },
    {
        "Содержит настройки отображения дисплея.",
        "Contains settings of display of the display."
    },
    EmptyFuncBV, Page_Display,
    {
        (void*)&mcViewMode,     // ДИСПЛЕЙ -> Отображение
        (void*)&mspAccum,       // ДИСПЛЕЙ -> НАКОПЛЕНИЕ
        (void*)&mspAverage,     // ДИСПЛЕЙ -> УСРЕДНЕНИЕ
        (void*)&mcMinMax,       // ДИСПЛЕЙ -> Мин Макс
        (void*)&mcSmoothing,    // ДИСПЛЕЙ -> Сглаживание
        (void*)&mcRefreshFPS,   // ДИСПЛЕЙ -> ОБНОВЛЕНИЕ -> Частота
        (void*)&mspGrid,        // ДИСПЛЕЙ -> СЕТКА
        (void*)&mcScaleYtype,   // ДИСПЛЕЙ -> Смещение
        (void*)&mspSettings     // ДИСПЛЕЙ -> НАСТРОЙКИ
    }
};

// ДИСПЛЕЙ -> Отображение --------------------------------------------------------------------------------------------------------------------------------
static const Choice mcViewMode =
{
    Item_Choice, &mpDisplay, {"Отображение", "View"},
    {
        "Задаёт режим отображения сигнала.",
        "Sets the display mode signal."
    },
    EmptyFuncBV,
    {
        {"Вектор",  "Vector"},
        {"Точки",   "Points"}
    },
    (int8*)&MODE_DRAW_SIGNAL, EmptyFuncVB, EmptyFuncVII
};

// ДИСПЛЕЙ -> Мин Макс -----------------------------------------------------------------------------------------------------------------------------------
static const Choice mcMinMax =
{
    Item_ChoiceReg, &mpDisplay, {"Мин Макс", "Min Max"},
    {
        "Задаёт количество последних измерений, по которым строятся ограничительные линии, огибающие минимумы и максимумы измерений.",
        "Sets number of the last measurements on which the limiting lines which are bending around minima and maxima of measurements are under construction."
    },
    IsActive_MinMax,
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
    (int8*)&set.display.numMinMax, OnChange_MinMax, EmptyFuncVII
};

static bool IsActive_MinMax(void)
{
    return TBASE > TBase_20ns;
}

static void OnChange_MinMax(bool active)
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

// ДИСПЛЕЙ -> Сглаживание ---------------------------------------------------------------------------------------------------------------------------------
static const Choice mcSmoothing =
{
    Item_ChoiceReg, &mpDisplay, {"Сглаживание", "Smoothing"},
    {
        "Устанавливает количество точек для расчёта сглаженного по соседним точкам сигнала.",
        "Establishes quantity of points for calculation of the signal smoothed on the next points."
    },
    EmptyFuncBV,
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
    (int8*)&SMOOTHING, EmptyFuncVB, EmptyFuncVII
};

// ДИСПЛЕЙ -> ОБНОВЛЕНИЕ -> Частота ------------------------------------------------------------------------------------------------------------------------
static const Choice mcRefreshFPS =
{
    Item_Choice, &mpDisplay, {"Частота обновл", "Refresh rate"},
    {
        "Задаёт максимальное число выводимых в секунду кадров.",
        "Sets the maximum number of the shots removed in a second."
    },
    EmptyFuncBV,
    {
        {"25",  "25"},
        {"10",  "10"},
        {"5",   "5"},
        {"2",   "2"},
        {"1",   "1"}
    },
    (int8*)&NUM_SIGNALS_IN_SEC, OnChange_RefreshFPS, EmptyFuncVII
};

static void OnChange_RefreshFPS(bool active)
{
    FPGA_SetNumSignalsInSec(sDisplay_NumSignalsInS());
}


// ДИСПЛЕЙ -> Смещение -----------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcScaleYtype =
{
    Item_Choice, &mpDisplay, {"Смещение", "Оffset"},
    {
        "Задаёт режим удержания смещения по вертикали\n1. \"Напряжение\" - сохраняется наряжение смещения.\n2. \"Деления\" - сохраняется положение смещения на экране.",
        "Sets the mode of retaining the vertical displacement\n1. \"Voltage\" - saved dressing bias.\n2. \"Divisions\" - retained the position of the offset on the screen."
    },
    EmptyFuncBV,
    {
        {"Напряжение", "Voltage"},
        {"Деления", "Divisions"}
    },
    (int8*)&set.display.linkingRShift, EmptyFuncVB, EmptyFuncVII
};


// ДИСПЛЕЙ -> НАКОПЛЕНИЕ -------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspAccum =
{
    Item_Page, &mpDisplay,
    {
        "НАКОПЛЕНИЕ", "ACCUMULATION"
    },
    {
        "Настройки режима отображения последних сигналов на экране.",
        "Mode setting signals to display the last screen."
    },
    IsActive_Accum, Page_DisplayAccumulation,
    {
        (void*)&mcAccum_Num,    // ДИСПЛЕЙ -> НАКОПЛЕНИЕ -> Количество
        (void*)&mcAccum_Mode,   // ДИСПЛЕЙ -> НАКОПЛЕНИЕ -> Тип
        (void*)&mbAccum_Crear   // ДИСПЛЕЙ -> НАКОПЛЕНИЕ -> Очистить
    }
};

static bool IsActive_Accum(void)
{
    return TBASE > TBase_20ns;
}

// ДИСПЛЕЙ -> НАКОПЛЕНИЕ -> Количество ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcAccum_Num =
{
    Item_ChoiceReg, &mspAccum, {"Количество", "Number"},
    {
        "Задаёт максимальное количество последних сигналов на экране. Если в настройке \"Режим\" выбрано \"Бесконечность\", экран очищается только нажатием кнопки \"Очистить\"."
        "\"Бесконечность\" - каждое измерение остаётся на дисплее до тех пор, пока не будет нажата кнопка \"Очистить\"."
        ,
        "Sets the maximum quantity of the last signals on the screen. If in control \"Mode\" it is chosen \"Infinity\", the screen is cleared only by pressing of the button \"Clear\"."
        "\"Infinity\" - each measurement remains on the display until the button \"Clear\" is pressed."
    },
    EmptyFuncBV,
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
    (int8*)&NUM_ACCUM, EmptyFuncVB, EmptyFuncVII
};

// ДИСПЛЕЙ -> НАКОПЛЕНИЕ -> Тип ---------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcAccum_Mode =
{
    Item_Choice, &mspAccum, {"Режим", "Mode"},
    {
        "1. \"Сбрасывать\" - после накопления заданного количества измерения происходит очистка дисплея. Этот режим удобен, когда памяти не хватает для сохранения нужного количества измерений.\n"
        "2. \"Не сбрасывать\" - на дисплей всегда выводится заданное или меньшее (в случае нехватки памяти) количество измерений. Недостатком является меньшее быстродействие и невозможность накопления заданного количества измерений при недостатке памяти."
        ,
        "1. \"Dump\" - after accumulation of the set number of measurement there is a cleaning of the display. This mode is convenient when memory isn't enough for preservation of the necessary number of measurements.\n"
        "2. \"Not to dump\" - the number of measurements is always output to the display set or smaller (in case of shortage of memory). Shortcoming is smaller speed and impossibility of accumulation of the set number of measurements at a lack of memory."
    },
    EmptyFuncBV,
    {
        {"Не сбрасывать",   "Not to dump"},
        {"Сбрасывать",      "Dump"}
    },
    (int8*)&MODE_ACCUM, EmptyFuncVB, EmptyFuncVII
};

// ДИСПЛЕЙ -> НАКОПЛЕНИЕ -> Очистить -------------------------------------------------------------------------------------------------------------------------------------
static const Button mbAccum_Crear =
{
    Item_Button, &mspAccum,
    {
        "Очистить", "Clear"
    },
    {
        "Очищает экран от накопленных сигналов.",
        "Clears the screen of the saved-up signals."
    },
    IsActive_Accum_Clear, OnPress_Accum_Clear, EmptyFuncVII
};

static bool IsActive_Accum_Clear(void)
{
    return NUM_ACCUM_INF;
}

void OnPress_Accum_Clear(void)
{
    Display_Redraw();
}


// ДИСПЛЕЙ -> УСРЕДНЕНИЕ -------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspAverage =
{
    Item_Page, &mpDisplay,
    {
        "УСРЕДНЕНИЕ", "AVERAGE"
    },
    {
        "Настройки режима усреднения по последним измерениям.",
        "Settings of the mode of averaging on the last measurements."
    },
    IsActive_Average, Page_DisplayAverage,
    {
        (void*)&mcAverage_Num,  // ДИСПЛЕЙ -> УСРЕДНЕНИЕ -> Количество
        (void*)&mcAverage_Mode  // ДИСПЛЕЙ -> УСРЕДНЕНИЕ -> Режим
    }
};

static bool IsActive_Average(void)
{
    return true;
}

// ДИСПЛЕЙ -> УСРЕДНЕНИЕ -> Количество ------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcAverage_Num =
{
    Item_ChoiceReg, &mspAverage, {"Количество", "Number"},
    {
        "Задаёт количество последних измерений, по которым производится усреднение.",
        "Sets number of the last measurements on which averaging is made."
    },
    EmptyFuncBV,
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
    (int8*)&NUM_AVE, EmptyFuncVB, EmptyFuncVII
};

// ДИСПЛЕЙ -> УСРЕДНЕНИЕ -> Режим -----------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcAverage_Mode =
{
    Item_Choice, &mspAverage, {"Режим", "Mode"},
    {
        "1. \"Точно\" - точный режим усреднения, когда в расчёте участвуют только последние сигналы.\n"
        "2. \"Приблизительно\" - приблизительный режим усреднения. Имеет смысл использовать, когда задано количество измерений большее, чем может поместиться в памяти.",
        "1. \"Accurately\" - the exact mode of averaging when only the last signals participate in calculation.\n"
        "2. \"Around\" - approximate mode of averaging. It makes sense to use when the number of measurements bigger is set, than can be located in memory."
    },
    EmptyFuncBV,
    {
        {"Точно",           "Accurately"},
        {"Приблизительно",  "Around"}
    },
    (int8*)&set.display.modeAveraging, EmptyFuncVB, EmptyFuncVII
};


// ДИСПЛЕЙ -> СЕТКА --------------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspGrid =
{
    Item_Page, &mpDisplay,
    {
        "СЕТКА", "GRID",
    },
    {
        "Содержит настройки отображения координатной сетки.",
        "Contains settings of display of a coordinate grid."
    },
    EmptyFuncBV, Page_DisplayGrid,
    {
        (void*)&mcGrid_Type,        // ДИСПЛЕЙ -> СЕТКА -> Тип
        (void*)&mgGrid_Brightness   // ДИСПЛЕЙ -> СЕТКА -> Яркость
    }
};

// ДИСПЛЕЙ -> СЕТКА -> Тип --------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcGrid_Type =
{
    Item_Choice, &mspGrid, {"Тип", "Type"},
    {
        "Выбор типа сетки.",
        "Choice like grid."
    },
    EmptyFuncBV,
    {
        {"Тип 1", "Type 1"},
        {"Тип 2", "Type 2"},
        {"Тип 3", "Type 3"},
        {"Тип 4", "Type 4"}
    },
    (int8*)&TYPE_GRID, EmptyFuncVB, EmptyFuncVII
};

// ДИСПЛЕЙ -> СЕТКА -> Яркость -------------------------------------------------------------------------------------------------------------------------------------------------
static const Governor mgGrid_Brightness =
{
    Item_Governor, &mspGrid,
    {
        "Яркость",
        "Brightness"
    },
    {
        "Устанавливает яркость сетки.",
        "Adjust the brightness of the grid."
    },
    EmptyFuncBV,
    &set.display.brightnessGrid, 0, 100, OnChange_Grid_Brightness, BeforeDraw_Grid_Brightness
};

void OnChange_Grid_Brightness(void)
{
    Color_SetBrightness(&colorTypeGrid, set.display.brightnessGrid / 1e2f);
}


static void BeforeDraw_Grid_Brightness(void)
{
    Color_Init(&colorTypeGrid, false);
    set.display.brightnessGrid = (int16)(colorTypeGrid.brightness * 100.0f);
}


// ДИСПЛЕЙ -> НАСТРОЙКИ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspSettings =
{
    Item_Page, &mpDisplay,
    {
        "НАСТРОЙКИ", "SETTINGS"
    },
    {
        "Дополнительные настройки дисплея",
        "Additional display settings"
    },
    EmptyFuncBV, Page_DisplaySettings,
    {
        (void*)&mspSettings_Colors,
        (void*)&mgSettings_Brightness,
        (void*)&mgSettings_Levels,
        (void*)&mgSettings_Time,
        (void*)&mcSettings_StringNavigation,
        (void*)&mcSettings_AltMarkers,
        (void*)&mcSettings_AutoHide
    }
};

// ДИСПЛЕЙ -> НАСТРОЙКИ -> Скрывать ------------------------------------------------------------------------------------------------------------------------------------------------------
static void Menu_AutoHide(bool autoHide)
{
    Menu_SetAutoHide(autoHide);
}

static const Choice mcSettings_AutoHide =
{
    Item_Choice, &mspSettings,
    {
        "Скрывать", "Hide"
    },
    {
        "Установка после последнего нажатия кнопки или поворота ручки, по истечении которого меню автоматически убирается с экрана",
        "Installation after the last keystroke or turning the handle, after which the menu will automatically disappear"
    },
    EmptyFuncBV,
    {
        { "Никогда", "Never" },
        { "Через 5 сек", "Through 5 s" },
        { "Через 10 сек", "Through 10 s" },
        { "Через 15 сек", "Through 15 s" },
        { "Через 30 сек", "Through 30 s" },
        { "Через 60 сек", "Through 60 s" }
    },
    (int8*)&set.display.menuAutoHide, Menu_AutoHide, EmptyFuncVII
};

// СЕРВИС -> ДИСПЛЕЙ -> Доп. маркеры ------------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcSettings_AltMarkers =
{
    Item_Choice, &mspSettings,
    {
        "Доп. маркеры", "Alt. markers"
    },
    {
        "Устанавливает режим отображения дополнительных маркеров уровней смещения и синхронизации:\n"
        "\"Скрывать\" - дополнительные маркеры не показываются,\n"
        "\"Показывать\" - дополнительные маркеры показываются всегда,\n"
        "\"Авто\" - дополнительные маркеры показываются в течение 5 сек после поворота ручки смещения канала по напряжению или уровня синхронизации",

        "Sets the display mode of additional markers levels of displacement and synchronization:\n"
        "\"Hide\" - additional markers are not shown,\n"
        "\"Show\" - additional markers are shown always,\n"
        "\"Auto\" - additional markers are displayed for 5 seconds after turning the handle channel offset voltage or trigger level"
    },
    EmptyFuncBV,
    {
        { "Скрывать", "Hide" },
        { "Показывать", "Show" },
        { "Авто", "Auto" }
    },
    (int8*)&ALT_MARKERS, Display_ChangedRShiftMarkers, EmptyFuncVII
};

// ДИСПЛЕЙ - НАСТРОЙКИ - Строка меню -----------------------------------------------------------------------------------------------------------------
static const Choice mcSettings_StringNavigation =
{
    Item_Choice, &mspSettings,
    {
        "Строка меню", "Path menu"
    },
    {
        "При выборе \nПоказывать\n слева вверху экрана выводится полный путь до текущей страницы меню",                         // WARN Исправить перевод
        "When choosing \nDisplay\n at the top left of the screen displays the full path to the current page menu"
    },
    EmptyFuncBV,
    {
        { "Временно", "Temporary" },                    // WARN Исправить перевод
        { "Всегда", "All" },
        { "Никогда", "None" }
    },
    (int8*)&SHOW_STRING_NAVI, EmptyFuncVB, EmptyFuncVII
};


// ДИСПЛЕЙ -> НАСТРОЙКИ -> ЦВЕТА ------------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspSettings_Colors =
{
    Item_Page, &mspSettings,
    {
        "ЦВЕТА", "COLORS"
    },
    {
        "Выбор цветов дисплея",
        "The choice of colors display"
    },
    EmptyFuncBV, Page_ServiceDisplayColors,
    {
        (void*)&mcSettings_Colors_Scheme,
        (void*)&mgcSettings_Colors_ChannelA,
        (void*)&mgcSettings_Colors_ChannelB,
        (void*)&mgcSettings_Colors_Grid,
        (void*)&mcSettings_Colors_Background
        //(void*)&mgcColorChannel0alt,  // TODO Это для дополнительного цвета первого канала - для накопления, например.
        //(void*)&mgcColorChannel1alt,  // TODO Это для доп. цвета второго канала - для накопления, например.
        //(void*)&mgcColorMenu1,
        //(void*)&mgcColorMenu2,
        //(void*)&mgcColorMenu3
        //,(void*)&mgcColorMathem,
        //(void*)&mgcColorFFT,
        //(void*)&mcServDisplInverse
    }
};

// ДИСПЛЕЙ -> НАСТРОЙКИ -> Яркость ------------------------------------------------------------------------------------------------------------------------------------------------------
static const Governor mgSettings_Brightness =
{
    Item_Governor, &mspSettings,
    {
        "Яркость", "Brightness"
    },
    {
        "Установка яркости свечения дисплея",
        "Setting the brightness of the display"
    },
    EmptyFuncBV,
    &set.display.brightness, 0, 100, OnChange_Settings_Brightness
};

static void OnChange_Settings_Brightness(void)
{
    Painter_SetBrightnessDisplay(set.display.brightness);
}

// ДИСПЛЕЙ -> НАСТРОЙКИ -> Уровни ------------------------------------------------------------------------------------------------------------------------------------------------------
static const Governor mgSettings_Levels =
{
    Item_Governor, &mspSettings,
    {
        "Уровни", "Levels"
    },
    {
        "Задаёт время, в течение которого после поворота ручки сещения напряжения на экране остаётся вспомогательная метка уровня смещения",
        "Defines the time during which, after turning the handle visits to the voltage on the screen remains auxiliary label offset level"
    },
    EmptyFuncBV,
    &TIME_SHOW_LEVELS, 0, 125, EmptyFuncVV
};

// ДИСПЛЕЙ -> НАСТРОЙКИ -> Время ------------------------------------------------------------------------------------------------------------------------------------------------------
static const Governor mgSettings_Time =
{
    Item_Governor, &mspSettings,
    {
        "Время", "Time"
    },
    {
        "Установка времени, в течение которого сообщения будут находиться на экране",
        "Set the time during which the message will be on the screen"
    },
    EmptyFuncBV,
    &TIME_MESSAGES, 1, 99, EmptyFuncVV
};


static const Choice mcSettings_Colors_Background =
{
    Item_Choice, &mspSettings_Colors, { "Фон", "Background" },
    {
        "Выбор цвета фона",
        "Choice of color of a background"
    },
    EmptyFuncBV,
    {
        { "Чёрный", "Black" },
        { "Белый", "White" }
    },
    (int8*)&BACKGROUND, OnChange_Settings_Colors_Background, EmptyFuncVII
};

// ДИСПЛЕЙ -> НАСТРОЙКИ -> ЦВЕТА -> Цветовая схема ------------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcSettings_Colors_Scheme =
{
    Item_Choice, &mspSettings_Colors, { "Цветовая схема", "Color scheme" },
    {
        "Режим работы калибратора",
        "Mode of operation of the calibrator"
    },
    EmptyFuncBV,
    {
        { "Схема 1", "Scheme 1" },
        { "Схема 2", "Scheme 2" }
    },
    (int8*)&set.service.colorScheme, EmptyFuncVB, EmptyFuncVII
};

static void OnChange_Settings_Colors_Background(bool active)
{
    PageService_InitGlobalColors();

    mgcSettings_Colors_ChannelA.colorType->color = gColorChan[A];
    mgcSettings_Colors_ChannelB.colorType->color = gColorChan[B];
    mgcSettings_Colors_Grid.colorType->color = gColorGrid;

    Color_Init((ColorType*)&mgcSettings_Colors_ChannelA, true);
    Color_Init((ColorType*)&mgcSettings_Colors_ChannelB, true);
    Color_Init((ColorType*)&mgcSettings_Colors_Grid, true);
}


// ДИСПЛЕЙ -> НАСТРОЙКИ -> ЦВЕТА -> Канал 1 ------------------------------------------------------------------------------------------------------------------------------------------------------
static ColorType colorT1 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_DATA_A };
static const GovernorColor mgcSettings_Colors_ChannelA =
{
    Item_GovernorColor, &mspSettings_Colors,
    {
        "Канал 1", "Channel 1"
    },
    {
        "",
        ""
    },
    EmptyFuncBV, &colorT1, EmptyFuncVV
};


// ДИСПЛЕЙ -> НАСТРОЙКИ -> ЦВЕТА -> Канал 2 ------------------------------------------------------------------------------------------------------------------------------------------------------
static ColorType colorT2 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_DATA_B };
static const GovernorColor mgcSettings_Colors_ChannelB =
{
    Item_GovernorColor, &mspSettings_Colors,
    {
        "Канал 2", "Channel 2"
    },
    {
        "",
        ""
    },
    EmptyFuncBV, &colorT2, EmptyFuncVV
};


// ДИСПЛЕЙ -> НАСТРОЙКИ -> ЦВЕТА -> Сетка ------------------------------------------------------------------------------------------------------------------------------------------------------
ColorType colorTypeGrid = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_GRID };
static const GovernorColor mgcSettings_Colors_Grid = 
{ 
    Item_GovernorColor, &mspSettings_Colors, 
    { 
        "Сетка", "Grid" 
    },
    {
        "Устанавливает цвет сетки",
        "Sets the grid color"
    },
    EmptyFuncBV, &colorTypeGrid, EmptyFuncVV
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PageService_InitGlobalColors(void)
{
    gColorBack = BACKGROUND_BLACK ? COLOR_BLACK : COLOR_WHITE;
    gColorFill = BACKGROUND_BLACK ? COLOR_WHITE : COLOR_BLACK;
    gColorGrid = BACKGROUND_BLACK ? COLOR_GRID : COLOR_GRID_WHITE;
    gColorChan[A] = BACKGROUND_BLACK ? COLOR_DATA_A : COLOR_DATA_A_WHITE;
    gColorChan[B] = BACKGROUND_BLACK ? COLOR_DATA_B : COLOR_DATA_B_WHITE;
    gColorChan[A_B] = gColorChan[Math] = BACKGROUND_BLACK ? COLOR_WHITE : COLOR_BLACK;
}

/*
//------------------------------------------------------------------------------------------------------------------------------------------------------ъ
// СЕРВИС - ДИСПЛЕЙ - ЦВЕТА - МЕНЮ Область
static ColorType colorT6 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_MENU_FIELD };
const GovernorColor mgcColorMenu3 = { Item_GovernorColor, &mspSettings_Colors, { "Меню Область", "Menu Field" },
{
    "",
    "",
},
EmptyFuncBV, &colorT6, EmptyFuncVV
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
// СЕРВИС - ДИСПЛЕЙ - ЦВЕТА - МЕНЮ Заголовок
static ColorType colorT7 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_MENU_TITLE };
const GovernorColor mgcColorMenu1 = { Item_GovernorColor, &mspSettings_Colors, { "Mеню Заголовок", "Menu Title" },
{
    "",
    ""
},
EmptyFuncBV, &colorT7, OnChangedColorMenuTitle
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
// СЕРВИС - ДИСПЛЕЙ - ЦВЕТА - МЕНЮ Пункт
static ColorType colorT8 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_MENU_ITEM };
const GovernorColor mgcColorMenu2 = { Item_GovernorColor, &mspSettings_Colors, { "Меню Пункт", "Menu Item" },
{
    "",
    ""
},
EmptyFuncBV, &colorT8, OnChangedColorMenu2Item
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
// СЕРВИС - ДИСПЛЕЙ - ЦВЕТА - Инверсия
const Choice mcServDisplInverse =
{
    Item_Choice, &mspSettings_Colors, { "Инверсия", "Inverse" },
    {
        "",
        ""
    },
    EmptyFuncBV,
    { { DISABLE_RU, DISABLE_EN },
    { ENABLE_RU, ENABLE_EN },
    },
    0, EmptyFuncVB, EmptyFuncVII
};
*/
