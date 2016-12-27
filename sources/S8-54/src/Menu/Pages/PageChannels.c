#include "defines.h"
#include "Log.h"
#include "Settings/SettingsTypes.h"
#include "Menu/MenuItems.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGA_Calibration.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mainPage;

#ifdef _MS_VS
#pragma warning(push)
#pragma warning(disable:4132)
#endif

static const Choice mcChanA_Input;
static const Choice mcChanA_Couple;
static const Choice mcChanA_Bandwidth;
static const Choice mcChanA_Resistance;
static const Choice mcChanA_Inverse;
static const Choice mcChanA_Divider;
static const Button mbChanA_Balance;

static const Choice mcChanB_Input;
static const Choice mcChanB_Couple;
static const Choice mcChanB_Bandwidth;
static const Choice mcChanB_Resistance;
static const Choice mcChanB_Inverse;
static const Choice mcChanB_Divider;
static const Button mbChanB_Balance;

       void OnChange_ChanA_Input(bool active);
static void OnChange_ChanA_Couple(bool active);
static void OnChange_ChanA_Bandwidth(bool active);
static void OnChange_ChanA_Resistance(bool active);
static void OnChange_ChanA_Inverse(bool active);
static void OnPress_ChanA_Balance(void);

static bool IsActive_ChanB_Input(void);
       void OnChange_ChanB_Input(bool active);
static void OnChange_ChanB_Couple(bool active);
static void OnChange_ChanB_Bandwidth(bool active);
static void OnChange_ChanB_Resistance(bool active);
static void OnChange_ChanB_Inverse(bool active);
static void OnPress_ChanB_Balance(void);

#ifdef _MS_VS
#pragma warning(pop)
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------------
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


// КАНАЛ 1 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpChanA =
{
    Item_Page, &mainPage, {"КАНАЛ 1", "CHANNEL 1"},
    {
        "Содержит настройки канала 1.",
        "Contains settings of the channel 1."
    },
    EmptyFuncBV, Page_ChannelA,
    {
        (void*)&mcChanA_Input,      // КАНАЛ 1 -> Вход
        (void*)&mcChanA_Couple,     // КАНАЛ 1 -> Связь
        (void*)&mcChanA_Bandwidth,  // КАНАЛ 1 -> Полоса
        (void*)&mcChanA_Resistance, // КАНАЛ 1 -> Вх сопр
        (void*)&mcChanA_Inverse,    // КАНАЛ 1 -> Инверсия
        (void*)&mcChanA_Divider,    // КАНАЛ 1 -> Делитель
        (void*)&mbChanA_Balance     // КАНАЛ 1 -> Балансировать
    }
};


// КАНАЛ 1 -> Вход ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanA_Input =
{
    Item_Choice, &mpChanA, {"Вход", "Input"},
    {
        chanInputRu,
        chanInputEn
    },
    EmptyFuncBV,
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&ENABLE_A, OnChange_ChanA_Input, EmptyFuncVII
};

void OnChange_ChanA_Input(bool active)
{
    Panel_EnableLEDChannelA(sChannel_Enabled(A));
}


// КАНАЛ 1 -> Связь ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanA_Couple =
{
    Item_Choice, &mpChanA, {"Связь", "Couple"},
    {
        chanCoupleRu,
        chanCoupleEn
    },
    EmptyFuncBV,
    {
        {"Пост", "AC"},
        {"Перем", "DC"},
        {"Земля", "Ground"}
    },
    (int8*)&COUPLE_A, OnChange_ChanA_Couple, EmptyFuncVII
};

static void OnChange_ChanA_Couple(bool active)
{
    FPGA_SetModeCouple(A, COUPLE_A);
}


// КАНАЛ 1 -> Полоса ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanA_Bandwidth =
{
    Item_Choice, &mpChanA, {"Полоса", "Bandwidth"},
    {
        "Задаёт полосу пропускания канала", "Sets the channel bandwidth"
    },
    EmptyFuncBV,
    {
        {"Полная", "Full"},
        {"20МГц", "20MHz"}
    },
    (int8*)&BANDWIDTH_A, OnChange_ChanA_Bandwidth, EmptyFuncVII
};

static void OnChange_ChanA_Bandwidth(bool active)
{
    FPGA_SetBandwidth(A);
}


// КАНАЛ 1 -> Сопротивление ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanA_Resistance =
{
    Item_Choice, &mpChanA, {"Вх сопр", "Resistance"},
    {
        "", ""
    },
    EmptyFuncBV,
    {
        {"1 МОм", "1 Mohm"},
        {"50 Ом", "50 Ohm"}
    },
    (int8*)&RESISTANCE_A, OnChange_ChanA_Resistance, EmptyFuncVII
};

static void OnChange_ChanA_Resistance(bool active)
{
    FPGA_SetResistance(A, RESISTANCE_A);
    if (RESISTANCE_A == Resistance_50Om)
    {
        Display_ShowWarning(Warn50Ohms);
    }
}


// КАНАЛ 1 -> Инверсия ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanA_Inverse =
{
    Item_Choice, &mpChanA, {"Инверсия", "Inverse"},
    {
        chanInverseRu,
        chanInverseEn
    },
    EmptyFuncBV,
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&INVERSE(A), OnChange_ChanA_Inverse, EmptyFuncVII
};

static void OnChange_ChanA_Inverse(bool active)
{
    FPGA_SetRShift(A, RSHIFT_A);
}


// КАНАЛ 1 -> Множитель ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanA_Divider =
{
    Item_Choice, &mpChanA, {"Делитель", "Divider"},
    {
        chanDividerRu,
        chanDividerEn
    },
    EmptyFuncBV,
    {
        {"Выкл", "Off"},
        {"1/10", "1/10"}
    },
    (int8*)&DIVIDER_A, EmptyFuncVB, EmptyFuncVII
};

// КАНАЛ 1 -> Балансировать ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Button mbChanA_Balance =
{
    Item_Button, &mpChanA,
    {
        "Балансировать", "Balance"
    },
    {
        "Балансировать канал",
        "Balancing channel"
    },
    EmptyFuncBV, OnPress_ChanA_Balance, EmptyFuncVII
};

static void OnPress_ChanA_Balance(void)
{
    FPGA_BalanceChannel(A);
}


// КАНАЛ 2 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpChanB =
{
    Item_Page, &mainPage, {"КАНАЛ 2", "CHANNEL 2"},
    {
        "Содержит настройки канала 2.",
        "Contains settings of the channel 2."
    },
    EmptyFuncBV, Page_ChannelB,
    {
        (void*)&mcChanB_Input,      // КАНАЛ 2 -> Вход
        (void*)&mcChanB_Couple,     // КАНАЛ 2 -> Связь
        (void*)&mcChanB_Bandwidth,  // КАНАЛ 2 -> Полоса
        (void*)&mcChanB_Resistance, // КАНАЛ 2 -> Сопротивление
        (void*)&mcChanB_Inverse,    // КАНАЛ 2 -> Инверсия
        (void*)&mcChanB_Divider,    // КАНАЛ 2 -> Делитель
        (void*)&mbChanB_Balance     // КАНАЛ 2 -> Балансировать
    }
};

// КАНАЛ 2 -> Вход ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanB_Input =
{
    Item_Choice, &mpChanB, {"Вход", "Input"},
    {
        chanInputRu,
        chanInputEn
    },
    IsActive_ChanB_Input,
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&ENABLE_B, OnChange_ChanB_Input, EmptyFuncVII
};

static bool IsActive_ChanB_Input(void)
{
    return !FPGA_NUM_POINTS_32k;
}


void OnChange_ChanB_Input(bool active)
{
    if (!active)
    {
        Display_ShowWarning(TooLongMemory);
        return;
    }

    if (FPGA_NUM_POINTS_32k && ENABLE_B)
    {
        ENABLE_B = false;
    }
    Panel_EnableLEDChannelB(sChannel_Enabled(B));
}

// КАНАЛ 2 -> Связь ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanB_Couple =
{
    Item_Choice, &mpChanB, {"Связь", "Couple"},
    {
        chanCoupleRu,
        chanCoupleEn
    },
    EmptyFuncBV,
    {
        {"Пост", "AC"},
        {"Перем", "DC"},
        {"Земля", "Ground"}
    },
    (int8*)&COUPLE_B, OnChange_ChanB_Couple, EmptyFuncVII
};

static void OnChange_ChanB_Couple(bool active)
{
    FPGA_SetModeCouple(B, COUPLE_B);
}

// КАНАЛ 2 -> Полоса ------------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanB_Bandwidth =
{
    Item_Choice, &mpChanB, {"Полоса", "Bandwidth"},
    {
        "", ""
    },
    EmptyFuncBV,
    {
        {"Полная", "Full"}, 
        {"20МГц", "20MHz"}
    },
    (int8*)&BANDWIDTH_B, OnChange_ChanB_Bandwidth, EmptyFuncVII
};

static void OnChange_ChanB_Bandwidth(bool active)
{
    FPGA_SetBandwidth(B);
}

// КАНАЛ 2 -> Сопротивление ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanB_Resistance =
{
    Item_Choice, &mpChanB, {"Вх сопр", "Resistance"},
    {
        "", ""
    },
    EmptyFuncBV,
    {
        {"1 МОм", "1 Mohm"},
        {"50 Ом", "50 Ohm"}
    },
    (int8*)&RESISTANCE_B, OnChange_ChanB_Resistance, EmptyFuncVII
};

static void OnChange_ChanB_Resistance(bool active)
{
    FPGA_SetResistance(B, RESISTANCE_B);
    if (RESISTANCE_B == Resistance_50Om)
    {
        Display_ShowWarning(Warn50Ohms);
    }
}

// КАНАЛ 2 -> Инверсия ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanB_Inverse =
{
    Item_Choice, &mpChanB, {"Инверсия", "Inverse"},
    {
        chanInverseRu,
        chanInverseEn
    },
    EmptyFuncBV,
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&INVERSE(B), OnChange_ChanB_Inverse, EmptyFuncVII
};

static void OnChange_ChanB_Inverse(bool active)
{
    FPGA_SetRShift(B, RSHIFT_B);
}

// КАНАЛ 2 -> Множитель ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcChanB_Divider =
{
    Item_Choice, &mpChanB, {"Делитель", "Divider"},
    {
        chanDividerRu,
        chanDividerEn
    },
    EmptyFuncBV,
    {
        {"Выкл", "Jff"},
        {"1/10", "1/10"}
    },
    (int8*)&DIVIDER_B, EmptyFuncVB, EmptyFuncVII
};

// КАНАЛ 2 -> Балансировать ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Button mbChanB_Balance =
{
    Item_Button, &mpChanB,
    {
        "Балансировать", "Balance"
    },
    {
        "Балансировать канал",
        "Balancing channel"
    },
    EmptyFuncBV, OnPress_ChanB_Balance, EmptyFuncVII
};

static void OnPress_ChanB_Balance(void)
{
    FPGA_BalanceChannel(B);
}
