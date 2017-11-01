#include "defines.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGAtypes.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const   Choice cMode;                    ///< СИНХР - Режим
       void  OnChanged_TrigMode(bool active);   
extern const   Choice cSource;                  ///< СИНХР - Источник
static void  OnChanged_Source(bool active);
extern const   Choice cPolarity;                ///< СИНХР - Полярность
static void  OnChanged_Polarity(bool active);   
extern const   Choice cInput;                   ///< СИНХР - Вход
static void  OnChanged_Input(bool active);
extern const    Page ppSearch;                  ///< СИНХР - ПОИСК
extern const   Choice cSearch_Mode;             ///< СИНХР - ПОИСК - Режим
extern const   Button bSearch_Search;           ///< СИНХР - ПОИСК - Найти
static bool   IsActive_Search_Search(void);
static void    OnPress_Search_Search(void);
//static const Governor gTimeDelay;               ///< СИНХР - Удержание

extern const Page pTrig;
//extern const Page mainPage;


// СИНХР /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pTrig =
{
    Item_Page, 0, 0,
    {
        "СИНХР", "TRIG",
        "Содержит настройки синхронизации.",
        "Contains synchronization settings."
    },
    Page_Trig,
    {
        (void *)&cMode,         // СИНХР - Режим
        (void *)&cSource,       // СИНХР - Источник
        (void *)&cPolarity,     // СИНХР - Полярность
        (void *)&cInput,        // СИНХР - Вход
        (void *)&ppSearch       // СИНХР - ПОИСК
//        (void *)&gTimeDelay      // СИНХР - Удержание
    }
};

// СИНХР - Режим -------------------------------------------------------------------------------------------------------------------------------------
static const Choice cMode =
{
    Item_Choice, &pTrig, 0,
    {
        "Режим",        "Mode"
        ,
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
    {
        {"Авто ",       "Auto"},
        {"Ждущий",      "Wait"},
        {"Однократный", "Single"}
    },
    (int8 *)&START_MODE, OnChanged_TrigMode
};

void OnChanged_TrigMode(bool active)
{
    FPGA_Stop(false);
    if(!START_MODE_SINGLE)
    {
        FPGA_OnPressStartStop();
    }
    
    // Елси находимся в режиме рандомизатора
    if(IN_RANDOM_MODE)
    /// \todo Это вместо функции sTime_RandomizeModeEnabled() было сделано потому, что с функцией экран периодически отваливался
    //if (SET_TBASE < TBase_50ns)
    {
        // и переключаемся на одиночный режим запуска, то надо сохранить имеющийся тип выборки, чтобы восстановить при возвращении в режим 
        // рандомизатора автоматический или ждущий
        if (START_MODE_SINGLE)
        {
            SAMPLE_TYPE_IS_OLD = SAMPLE_TYPE;
            SAMPLE_TYPE = SampleType_Real;
        }
        else if(START_MODE_AUTO)    // Иначе восстановим ранее сохранённый
        {
            SAMPLE_TYPE = SAMPLE_TYPE_IS_OLD;
        }
    }
}

// СИНХР - Источник ----------------------------------------------------------------------------------------------------------------------------------
static const Choice cSource =
{
    Item_Choice, &pTrig, 0,
    {
        "Источник", "Source",
        "Выбор источника сигнала синхронизации.",
        "Synchronization signal source choice."
    },
    {
        {"Канал 1", "Channel 1"},
        {"Канал 2", "Channel 2"},
        {"Внешний", "External"}
    },
    (int8 *)&TRIGSOURCE, OnChanged_Source
};

static void OnChanged_Source(bool active)
{
    FPGA_SetTrigSource(TRIGSOURCE);
}

// СИНХР - Полярность --------------------------------------------------------------------------------------------------------------------------------
static const Choice cPolarity =
{
    Item_Choice, &pTrig, 0,
    {
        "Полярность",   "Polarity"
        ,
        "1. \"Фронт\" - запуск происходит по фронту синхроимпульса.\n"
        "2. \"Срез\" - запуск происходит по срезу синхроимпульса."
        ,
        "1. \"Front\" - start happens on the front of clock pulse.\n"
        "2. \"Back\" - start happens on a clock pulse cut."
    },
    {
        {"Фронт",       "Front"},
        {"Срез",        "Back"}
    },
    (int8 *)&TRIG_POLARITY, OnChanged_Polarity
};

static void OnChanged_Polarity(bool active)
{
    FPGA_SetTrigPolarity(TRIG_POLARITY);
}

// СИНХР - Вход --------------------------------------------------------------------------------------------------------------------------------------
static const Choice cInput =
{
    Item_Choice, &pTrig, 0,
    {
        "Вход", "Input"
        ,
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
    {
        {"ПС", "Full"},
        {"АС", "AC"},
        {"ФНЧ", "LPF"},
        {"ФВЧ", "HPF"}
    },
    (int8 *)&TRIG_INPUT, OnChanged_Input
};

static void OnChanged_Input(bool active)
{
    FPGA_SetTrigInput(TRIG_INPUT);
}

// СИНХР - Удержание ---------------------------------------------------------------------------------------------------------------------------------
/*
static const Governor gTimeDelay =
{
    Item_Governor, &pTrig, 0,
    {
        "Удержание, мс", "Holdoff, ms",
        "Устанавливает минимальное время между запусками.",
        "Sets the minimum time between starts."
    },
    (int16 *)&TRIG_TIME_DELAY, 45, 10000
};
*/

// СИНХР - ПОИСК /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppSearch =
{
    Item_Page, &pTrig, 0,
    {
        "ПОИСК", "SEARCH",
        "Управление автоматическим поиском уровня синхронизации.",
        "Office of the automatic search the trigger level."
    },
    Page_Trig_Search,
    {
        (void *)&cSearch_Mode,      // СИНХР - ПОИСК - Режим
        (void *)&bSearch_Search     // СИНХР - ПОИСК - Найти
    }
};

// СИНХР - ПОИСК - Режим -----------------------------------------------------------------------------------------------------------------------------
static const Choice cSearch_Mode =
{
    Item_Choice, &ppSearch, 0,
    {
        "Режим", "Mode"
        ,
        "Выбор режима автоматического поиска синхронизации:\n"
        "1. \"Ручной\" - поиск производится по нажатию кнопки \"Найти\" или по удержанию в течение 0.5с кнопки СИНХР, если установлено "
        "\"СЕРВИС\x99Реж длит СИНХР\x99Автоуровень\".\n"
        "2. \"Автоматический\" - поиск производится автоматически."
        ,
        "Selecting the automatic search of synchronization:\n"
#pragma push
#pragma diag_suppress 192
        "1. \"Hand\" - search is run on pressing of the button \"Find\" or on deduction during 0.5s the СИНХР button if it is established "
        "\"SERVICE\x99Mode long СИНХР\x99\Autolevel\".\n"
#pragma pop
        "2. \"Auto\" - the search is automatically."
    },
    {
        {"Ручной",          "Hand"},
        {"Автоматический",  "Auto"}
    },
    (int8 *)&TRIG_MODE_FIND
};

// СИНХР - ПОИСК - Найти -----------------------------------------------------------------------------------------------------------------------------
static const Button bSearch_Search =
{
    Item_Button, &ppSearch, IsActive_Search_Search,
    {
        "Найти", "Search",
        "Производит поиск уровня синхронизации.",
        "Runs for search synchronization level."
    },
    OnPress_Search_Search, EmptyFuncVII
};

static bool IsActive_Search_Search(void)
{
    return TRIG_MODE_FIND_HAND;
}

static void OnPress_Search_Search(void)
{
    FPGA_FindAndSetTrigLevel();
}
