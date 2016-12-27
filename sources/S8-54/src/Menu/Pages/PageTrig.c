#include "defines.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGA_Types.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mainPage;

static const Choice mcMode;
       void OnChange_Mode(bool active);
static const Choice mcSource;
static void OnChange_Source(bool active);
static const Choice mcPolarity;
static void OnChange_Polarity(bool active);
static const Choice mcInput;
static void OnChange_Input(bool active);
static const Governor mgTimeDelay;

static const Page mspSearch;
static const Choice mcSearch_Mode;
static const Button mbSearch_Search;
static bool IsActive_Search_Search(void);
static void OnPress_Search_Search(void);


// СИНХР ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpTrig =
{
    Item_Page, &mainPage,
    {
        "СИНХР", "TRIG"
    },
    {
        "Содержит настройки синхронизации.",
        "Contains synchronization settings."
    },
    EmptyFuncBV, Page_Trig,
    {
        (void*)&mcMode,         // СИНХР -> Режим
        (void*)&mcSource,       // СИНХР -> Источник
        (void*)&mcPolarity,     // СИНХР -> Полярность
        (void*)&mcInput,        // СИНХР -> Вход
        (void*)&mspSearch,      // СИНХР -> ПОИСК
        (void*)&mgTimeDelay     // СИНХР -> Удержание
    }
};

// СИНХР -> Режим --------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcMode =
{
    Item_Choice, &mpTrig, {"Режим", "Mode"},
    {
        "Задаёт режим запуска:\n"
        "1. \"Авто\" - запуск происходит автоматически.\n"
        "2. \"Ждущий\" - запуск происходит по уровню синхронизации.\n"
        "3. \"Однократный\" - запуск происходит по достижении уровня синхронизации один раз. Для следующего измерения нужно нажать кнопку ПУСК/СТОП."
        ,
        "Sets the trigger mode:\n"
        "1. \"Auto\" - start automatically.\n"
        "2. \"Standby\" - the launch takes place at the level of synchronization.\n"
        "3. \"Single\" - the launch takes place on reaching the trigger levelonce. For the next measurement is necessary to press the START/STOP."
    },
    EmptyFuncBV,
    {
        {"Авто ",       "Auto"},
        {"Ждущий",      "Wait"},
        {"Однократный", "Single"}
    },
    (int8*)&START_MODE, OnChange_Mode, EmptyFuncVII
};

void OnChange_Mode(bool active)
{
    FPGA_Stop(false);
    if(!START_MODE_SINGLE)
    {
        FPGA_OnPressStartStop();
    }
    
    // Елси находимся в режиме рандомизатора
    if(IN_RANDOM_MODE)
    //if (TBASE < TBase_50ns)    // WARN Это вместо функции sTime_RandomizeModeEnabled() было сделано потому, что с функцией экран периодически отваливался
    {
        // и переключаемся на одиночный режим запуска, то надо сохранить имеющийся тип выборки, чтобы восстановить при возвращении в режим рандомизатора автоматический или ждущий
        if (START_MODE_SINGLE)
        {
            set.time.sampleTypeOld = SAMPLE;
            SAMPLE = SampleType_Real;
        }
        else if(START_MODE_AUTO)    // Иначе восстановим ранее сохранённый
        {
            SAMPLE = set.time.sampleTypeOld;
        }
    }
}

// СИНХР -> Источник ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcSource =
{
    Item_Choice, &mpTrig, {"Источник", "Source"},
    {
        "Выбор источника сигнала синхронизации.",
        "Synchronization signal source choice."
    },
    EmptyFuncBV,
    {
        {"Канал 1", "Channel 1"},
        {"Канал 2", "Channel 2"},
        {"Внешний", "External"}
    },
    (int8*)&TRIGSOURCE, OnChange_Source, EmptyFuncVII
};

static void OnChange_Source(bool active)
{
    FPGA_SetTrigSource(TRIGSOURCE);
}

// СИНХР -> Полярность ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcPolarity =
{
    Item_Choice, &mpTrig, {"Полярность", "Polarity"},
    {
        "1. \"Фронт\" - запуск происходит по фронту синхроимпульса.\n"
        "2. \"Срез\" - запуск происходит по срезу синхроимпульса."
        ,
        "1. \"Front\" - start happens on the front of clock pulse.\n"
        "2. \"Back\" - start happens on a clock pulse cut."
    },
    EmptyFuncBV,
    {
        {"Фронт", "Front"},
        {"Срез", "Back"}
    },
    (int8*)&TRIG_POLARITY, OnChange_Polarity, EmptyFuncVII
};

static void OnChange_Polarity(bool active)
{
    FPGA_SetTrigPolarity(TRIG_POLARITY);
}

// СИНХР -> Вход -----------------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcInput =
{
    Item_Choice, &mpTrig, {"Вход", "Input"},
    {
        "Выбор связи с источником синхронизации:\n"
        "1. \"ПС\" - полный сигнал.\n"
        "2. \"АС\" - закрытый вход.\n"
        "3. \"ФНЧ\" - фильтр низких частот.\n"
        "4. \"ФВЧ\" - фильтр высоких частот."
        ,
        "The choice of communication with the source of synchronization:\n"
        "1. \"SS\" - a full signal.\n"
        "2. \"AS\" - a gated entrance.\n"
        "3. \"LPF\" - low-pass filter.\n"
        "4. \"HPF\" - high-pass filter frequency."
    },
    EmptyFuncBV,
    {
        {"ПС", "Full"},
        {"АС", "AC"},
        {"ФНЧ", "LPF"},
        {"ФВЧ", "HPF"}
    },
    (int8*)&TRIG_INPUT, OnChange_Input, EmptyFuncVII
};

static void OnChange_Input(bool active)
{
    FPGA_SetTrigInput(TRIG_INPUT);
}

// СИНХР -> Удержание ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Governor mgTimeDelay =
{
    Item_Governor, &mpTrig, {"Удержание, мс", "Holdoff, ms"},
    {
        "Устанавливает минимальное время между запусками.",
        "Sets the minimum time between starts."
    },
    EmptyFuncBV,
    (int16*)&set.trig.timeDelay, 45, 10000, EmptyFuncVV
};

// СИНХР -> ПОИСК //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspSearch =
{
    Item_Page, &mpTrig,
    {
        "ПОИСК", "SEARCH"
    },
    {
        "Управление автоматическим поиском уровня синхронизации.",
        "Office of the automatic search the trigger level."
    },
    EmptyFuncBV, Page_TrigAuto,
    {
        (void*)&mcSearch_Mode,
        (void*)&mbSearch_Search
    }
};

// СИНХР -> ПОИСК -> Режим ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcSearch_Mode =
{
    Item_Choice, &mspSearch, {"Режим", "Mode"},
    {
        "Выбор режима автоматического поиска синхронизации:\n"
        "1. \"Ручной\" - поиск производится по нажатию кнопки \"Найти\" или по удержанию в течение 0.5с кнопки СИНХР, если установлено \"СЕРВИС\x99Реж длит СИНХР\x99Автоуровень\".\n"
        "2. \"Автоматический\" - поиск производится автоматически."
        ,
        "Selecting the automatic search of synchronization:\n"
#pragma push
#pragma diag_suppress 192
        "1. \"Hand\" - search is run on pressing of the button \"Find\" or on deduction during 0.5s the СИНХР button if it is established \"SERVICE\x99Mode long СИНХР\x99\Autolevel\".\n"
#pragma pop
        "2. \"Auto\" - the search is automatically."
    },
    EmptyFuncBV,
    {
        {"Ручной",          "Hand"},
        {"Автоматический",  "Auto"}
    },
    (int8*)&TRIG_MODE_FIND, EmptyFuncVB, EmptyFuncVII
};

// СИНХР -> ПОИСК -> Найти ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Button mbSearch_Search =
{
    Item_Button, &mspSearch,
    {
        "Найти", "Search"
    },
    {
        "Производит поиск уровня синхронизации.",
        "Runs for search synchronization level."
    },
    IsActive_Search_Search, OnPress_Search_Search, EmptyFuncVII
};

static bool IsActive_Search_Search(void)
{
    return TRIG_MODE_FIND_HAND;
}

static void OnPress_Search_Search(void)
{
    FPGA_FindAndSetTrigLevel();
}
