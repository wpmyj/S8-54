// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Display/Display.h"
#include "FPGA/fpga.h"
#include "FPGA/fpgaExtensions.h"
#include "Log.h"
#include "Menu/MenuItems.h"
#include "Panel/Panel.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"
#include "Utils/GlobalFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const  Choice cChanA_Input;                      ///< КАНАЛ 1 - Вход
       void OnChanged_ChanA_Input(bool active);
static const  Choice cChanA_Couple;                     ///< КАНАЛ 1 - Связь
static void OnChanged_ChanA_Couple(bool active);
static const  Choice cChanA_Bandwidth;                  ///< КАНАЛ 1 - Полоса
static void OnChanged_ChanA_Bandwidth(bool active);
static const  Choice cChanA_Resistance;                 ///< КАНАЛ 1 - Вх сопр
static void OnChanged_ChanA_Resistance(bool active);
static const  Choice cChanA_Inverse;                    ///< КАНАЛ 1 - Инверсия
static void OnChanged_ChanA_Inverse(bool active);
static const Choice cChanA_Divider;                     ///< КАНАЛ 1 - Делитель
static const Button bChanA_Balance;                     ///< КАНАЛ 1 - Балансировать
static void OnPress_ChanA_Balance(void);
static const Choice cChanB_Input;
static bool IsActive_ChanB_Input(void);
      void OnChanged_ChanB_Input(bool active);
static const Choice cChanB_Couple;
static void OnChanged_ChanB_Couple(bool active);
static const Choice cChanB_Bandwidth;
static void OnChanged_ChanB_Bandwidth(bool active);
static const Choice cChanB_Resistance;
static void OnChanged_ChanB_Resistance(bool active);
static const Choice cChanB_Inverse;
static void OnChanged_ChanB_Inverse(bool active);
static const Choice cChanB_Divider;
static const Button bChanB_Balance;
static void OnPress_ChanB_Balance(void);


//----------------------------------------------------------------------------------------------------------------------------------------------------
static const char chanInputRu[] =   "1. \"Вкл\" - выводить сигнал на экран.\n"
                                    "2. \"Откл\" - не выводить сигнал на экран.";
static const char chanInputEn[] =   "1. \"Enable\" - signal output to the screen.\n"
                                    "2. \"Disable\" - no output to the screen.";

static const char chanCoupleRu[] =  "Задаёт вид связи с источником сигнала.\n"
                                    "1. \"Пост\" - открытый вход.\n"
                                    "2. \"Перем\" - закрытый вход.\n"
                                    "3. \"Земля\" - вход соединён с землёй.";
static const char chanCoupleEn[] =  "Sets a type of communication with a signal source.\n"
                                    "1. \"AC\" - open input.\n"
                                    "2. \"DC\" - closed input.\n"
                                    "3. \"Ground\" - input is connected to the ground.";

static const char chanInverseRu[] = "При \"Вкл\" сигнал на экране будет симметрично отражён относительно U = 0В.";
static const char chanInverseEn[] = "When \"Enable\" signal on the screen will be reflected symmetrically with respect to U = 0V.";

static const char chanDividerRu[] = "Ослабление сигнала:\n\"Выкл\" - сигнал не ослабляется.\n\"x10\" - сигнал ослабляется в 10 раз";
static const char chanDividerEn[] = "Attenuation: \n\"Off\" - the signal is not attenuated.\n\"x10\" - the signal is attenuated by 10 times";

extern const Page mainPage;

// КАНАЛ 1 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pChanA =
{
    Item_Page, &mainPage, 0,
    {
        "КАНАЛ 1", "CHANNEL 1",
        "Содержит настройки канала 1.",
        "Contains settings of the channel 1."
    },
    Page_ChannelA,
    {
        (void*)&cChanA_Input,       // КАНАЛ 1 - Вход
        (void*)&cChanA_Couple,      // КАНАЛ 1 - Связь
        (void*)&cChanA_Bandwidth,   // КАНАЛ 1 - Полоса
        (void*)&cChanA_Resistance,  // КАНАЛ 1 - Вх сопр
        (void*)&cChanA_Inverse,     // КАНАЛ 1 - Инверсия
        (void*)&cChanA_Divider,     // КАНАЛ 1 - Делитель
        (void*)&bChanA_Balance      // КАНАЛ 1 - Балансировать
    }
};

// КАНАЛ 1 - Вход ------------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanA_Input =
{
    Item_Choice, &pChanA, 0,
    {
        "Вход", "Input",
        chanInputRu,
        chanInputEn
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&SET_ENABLED_A, OnChanged_ChanA_Input
};

void OnChanged_ChanA_Input(bool active)
{
    Panel_EnableLEDChannelA(sChannel_Enabled(A));
}

// КАНАЛ 1 - Связь -----------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanA_Couple =
{
    Item_Choice, &pChanA, 0,
    {
        "Связь", "Couple",
        chanCoupleRu,
        chanCoupleEn
    },
    {
        {"Пост", "AC"},
        {"Перем", "DC"},
        {"Земля", "Ground"}
    },
    (int8*)&SET_COUPLE_A, OnChanged_ChanA_Couple
};

static void OnChanged_ChanA_Couple(bool active)
{
    FPGA_SetModeCouple(A, SET_COUPLE_A);
}

// КАНАЛ 1 - Полоса ----------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanA_Bandwidth =
{
    Item_Choice, &pChanA, 0,
    {
        "Полоса", "Bandwidth",
        "Задаёт полосу пропускания канала", "Sets the channel bandwidth"
    },
    {
        {"Полная", "Full"},
        {"20МГц", "20MHz"}
    },
    (int8*)&SET_BANDWIDTH_A, OnChanged_ChanA_Bandwidth
};

static void OnChanged_ChanA_Bandwidth(bool active)
{
    FPGA_SetBandwidth(A);
}

// КАНАЛ 1 - Сопротивление ---------------------------------------------------------------------------------------------------------------------------
static const Choice cChanA_Resistance =
{
    Item_Choice, &pChanA, 0,
    {
        "Вх сопр", "Resistance",
        "", ""
    },
    {
        {"1 МОм", "1 Mohm"},
        {"50 Ом", "50 Ohm"}
    },
    (int8*)&SET_RESISTANCE_A, OnChanged_ChanA_Resistance
};

static void OnChanged_ChanA_Resistance(bool active)
{
    FPGA_SetResistance(A, SET_RESISTANCE_A);
    if (SET_RESISTANCE_A == Resistance_50Om)
    {
        Display_ShowWarning(Warn50Ohms);
    }
}

// КАНАЛ 1 - Инверсия --------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanA_Inverse =
{
    Item_Choice, &pChanA, 0,
    {
        "Инверсия", "Inverse",
        chanInverseRu,
        chanInverseEn
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&SET_INVERSE_A, OnChanged_ChanA_Inverse
};

static void OnChanged_ChanA_Inverse(bool active)
{
    FPGA_SetRShift(A, SET_RSHIFT_A);
}

// КАНАЛ 1 - Делитель --------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanA_Divider =
{
    Item_Choice, &pChanA, 0,
    {
        "Делитель", "Divider",
        chanDividerRu,
        chanDividerEn
    },
    {
        {"Выкл", "Off"},
        {"1/10", "1/10"}
    },
    (int8*)&SET_DIVIDER_A
};

// КАНАЛ 1 - Балансировать ---------------------------------------------------------------------------------------------------------------------------
static const Button bChanA_Balance =
{
    Item_Button, &pChanA, 0,
    {
        "Балансировать", "Balance",
        "Балансировать канал",
        "Balancing channel"
    },
    OnPress_ChanA_Balance
};

static void OnPress_ChanA_Balance(void)
{
    FPGA_BalanceChannel(A);
}

// КАНАЛ 2 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pChanB =
{
    Item_Page, &mainPage, 0,
    {
        "КАНАЛ 2", "CHANNEL 2",
        "Содержит настройки канала 2.",
        "Contains settings of the channel 2."
    },
    Page_ChannelB,
    {
        (void*)&cChanB_Input,      // КАНАЛ 2 - Вход
        (void*)&cChanB_Couple,     // КАНАЛ 2 - Связь
        (void*)&cChanB_Bandwidth,  // КАНАЛ 2 - Полоса
        (void*)&cChanB_Resistance, // КАНАЛ 2 - Сопротивление
        (void*)&cChanB_Inverse,    // КАНАЛ 2 - Инверсия
        (void*)&cChanB_Divider,    // КАНАЛ 2 - Делитель
        (void*)&bChanB_Balance     // КАНАЛ 2 - Балансировать
    }
};

// КАНАЛ 2 - Вход ------------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanB_Input =
{
    Item_Choice, &pChanB, IsActive_ChanB_Input,
    {
        "Вход", "Input",
        chanInputRu,
        chanInputEn
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&SET_ENABLED_B, OnChanged_ChanB_Input
};

static bool IsActive_ChanB_Input(void)
{
    return !FPGA_NUM_POINTS_32k;
}

void OnChanged_ChanB_Input(bool active)
{
    if (!active)
    {
        Display_ShowWarning(TooLongMemory);
        return;
    }

    if (FPGA_NUM_POINTS_32k && SET_ENABLED_B)
    {
        SET_ENABLED_B = false;
    }
    Panel_EnableLEDChannelB(sChannel_Enabled(B));
}

// КАНАЛ 2 - Связь -----------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanB_Couple =
{
    Item_Choice, &pChanB, 0,
    {
        "Связь", "Couple",
        chanCoupleRu,
        chanCoupleEn
    },
    {
        {"Пост", "AC"},
        {"Перем", "DC"},
        {"Земля", "Ground"}
    },
    (int8*)&SET_COUPLE_B, OnChanged_ChanB_Couple
};

static void OnChanged_ChanB_Couple(bool active)
{
    FPGA_SetModeCouple(B, SET_COUPLE_B);
}

// КАНАЛ 2 - Полоса ----------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanB_Bandwidth =
{
    Item_Choice, &pChanB, 0,
    {
        "Полоса", "Bandwidth",
        "",
        ""
    },
    {
        {"Полная", "Full"}, 
        {"20МГц", "20MHz"}
    },
    (int8*)&SET_BANDWIDTH_B, OnChanged_ChanB_Bandwidth
};

static void OnChanged_ChanB_Bandwidth(bool active)
{
    FPGA_SetBandwidth(B);
}

// КАНАЛ 2 - Сопротивление ---------------------------------------------------------------------------------------------------------------------------
static const Choice cChanB_Resistance =
{
    Item_Choice, &pChanB, 0,
    {
        "Вх сопр", "Resistance",
        "", ""
    },
    {
        {"1 МОм", "1 Mohm"},
        {"50 Ом", "50 Ohm"}
    },
    (int8*)&SET_RESISTANCE_B, OnChanged_ChanB_Resistance
};

static void OnChanged_ChanB_Resistance(bool active)
{
    FPGA_SetResistance(B, SET_RESISTANCE_B);
    if (SET_RESISTANCE_B == Resistance_50Om)
    {
        Display_ShowWarning(Warn50Ohms);
    }
}

// КАНАЛ 2 - Инверсия --------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanB_Inverse =
{
    Item_Choice, &pChanB, 0,
    {
        "Инверсия", "Inverse",
        chanInverseRu,
        chanInverseEn
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&SET_INVERSE_B, OnChanged_ChanB_Inverse
};

static void OnChanged_ChanB_Inverse(bool active)
{
    FPGA_SetRShift(B, SET_RSHIFT_B);
}

// КАНАЛ 2 - Делитель --------------------------------------------------------------------------------------------------------------------------------
static const Choice cChanB_Divider =
{
    Item_Choice, &pChanB, 0,
    {
        "Делитель", "Divider",
        chanDividerRu,
        chanDividerEn
    },
    {
        {"Выкл", "Jff"},
        {"1/10", "1/10"}
    },
    (int8*)&SET_DIVIDER_B
};

// КАНАЛ 2 - Балансировать ---------------------------------------------------------------------------------------------------------------------------
static const Button bChanB_Balance =
{
    Item_Button, &pChanB, 0,
    {
        "Балансировать", "Balance",
        "Балансировать канал",
        "Balancing channel"
    },
    OnPress_ChanB_Balance
};

static void OnPress_ChanB_Balance(void)
{
    FPGA_BalanceChannel(B);
}
