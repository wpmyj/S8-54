// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGAtypes.h"
#include "Hardware/FSMC.h"
#include "Hardware/Sound.h"
#include "Menu/MenuDrawing.h"
#include "Menu/MenuFunctions.h"
#include "Log.h"
#include "DebugSerialNumber.h"
#include "DebugConsole.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;
extern void LoadTShift(void);

static const Choice mcStats;

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
static const Page mspShowSettingsInfo;
static void OnPress_ShowInfo(void);
static const SmallButton sbExitShowSetInfo;

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


// ������� ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpDebug =
{
    Item_Page, &mainPage, 0,
    {
        "�������", "DEBUG",
        "", ""
    },
    Page_Debug,
    {
        (void*)&mspConsole,             // ������� - �������
        (void*)&mspADC,                 // ������� - ���
        (void*)&mspRand,                // ������� - ����-���
        (void*)&mspChannels,            // ������� - ������
        (void*)&mcStats,                // ������� - ����������
        (void*)&mcDisplayOrientation,   // ������� - ����������
        (void*)&mcEMS,                  // ������� - ���
        (void*)&mgPred,                 // ������� - ����������
        (void*)&mgPost,                 // ������� - �����������
        (void*)&mspShowSettingsInfo,    // ������� - ���������
        (void*)&mspSerialNumber         // ������� - �/�
    }
};

// ������� - ���������� ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcStats =
{
    Item_Choice, &mpDebug, 0,
    {
        "����������", "Statistics",
        "����������/�� ���������� �����/����, ������ � �������, ���������� �������� � ���������� ����������� � ������/���������� ����������� � ������ ��������",
        "To show/not to show a time/shot, frames per second, quantity of signals with the last settings in memory/quantity of the signals kept in memory"
    },
    {
        {"�� ����������",   "Hide"},
        {"����������",      "Show"}
    },
    (int8*)&SHOW_STAT
};

// ������� - ��� /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspADC =
{
    Item_Page, &mpDebug, 0,
    {
        "���", "ADC",
        "",
        ""
    },
    Page_DebugADC,
    {
        (void*)&mspADC_Balance,   // ������� - ��� - ������
        (void*)&mspADC_Stretch,   // ������� - ��� - ��������
        (void*)&mspADC_Shift      // ������� - ��� - ��� ����
    }
};

// ������� - ��� - ������ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspADC_Balance =
{
    Item_Page, &mspADC, 0,
    {
        "������", "BALANCE",
        "",
        ""
    },
    Page_DebugADCbalance,
    {
        (void*)&mcADC_Balance_Mode,   // ������� - ��� - ������ - �����
        (void*)&mgADC_Balance_ShiftA, // ������� - ��� - ������ - �������� 1
        (void*)&mgADC_Balance_ShiftB  // ������� - ��� - ������ - �������� 2
    }
};

// ������� - ��� - ������ - ����� --------------------------------------------------------------------------------------------------------------------
static const Choice mcADC_Balance_Mode =
{
    Item_Choice, &mspADC_Balance, 0,
    {
        "�����", "Mode",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {"��������", "Real"},
        {"������", "Manual"}
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
        {0, SET_BALANCE_ADC_A, (int8)setNR.balanceADC[0]},
        {0, SET_BALANCE_ADC_B, (int8)setNR.balanceADC[1]}
    };

    shiftADCA = shift[A][setNR.balanceADCtype];
    shiftADCB = shift[B][setNR.balanceADCtype];
}

// ������� - ��� - ������ - �������� 1 ---------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Balance_ShiftA =
{
    Item_Governor, &mspADC_Balance, IsActive_ADC_Balance,
    {
        "�������� 1", "Offset 1",
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

// ������� - ��� - ������ - �������� 2 ---------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Balance_ShiftB =
{
    Item_Governor, &mspADC_Balance, IsActive_ADC_Balance,
    {
        "�������� 2", "Offset 2",
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

// ������� - ��� - �������� //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspADC_Stretch =
{
    Item_Page, &mspADC, 0,
    {
        "��������", "STRETCH",
        "������������� ����� � �������� �������� (��� ������� ������)",
        "Sets mode and the value of stretching (manual mode)"
    },
    Page_DebugADCstretch,
    {
        (void*)&mcADC_Stretch_Mode,       // ������� - ��� - �������� - �����
        (void*)&mgADC_Stretch_A,          // ������� - ��� - �������� - �������� 1�
        (void*)&mgADC_Stretch_B,          // ������� - ��� - �������� - �������� 2�
        (void*)&emptyChoice,
        (void*)&emptyChoice,
        (void*)&mgADC_Stretch_Ak20mV,     // ������� - ��� - �������� - 20��/1� 1�
        (void*)&mgADC_Stretch_Ak50mV,     // ������� - ��� - �������� - 50�� 1� 
        (void*)&mgADC_Stretch_Ak100mV,    // ������� - ��� - �������� - 100��/5� 1�
        (void*)&mgADC_Stretch_Ak2V,       // ������� - A�� - �������� - 2� 1�
        (void*)&emptyChoice,
        (void*)&mgADC_Stretch_Bk20mV,     // ������� - ��� - �������� - 20��/1� 2�
        (void*)&mgADC_Stretch_Bk50mV,     // ������� - ��� - �������� - 50�� 2� 
        (void*)&mgADC_Stretch_Bk100mV,    // ������� - ��� - �������� - 100��/5� 2�
        (void*)&mgADC_Stretch_Bk2V,       // ������� - ��� - �������� - 2� 2�
        (void*)&emptyChoice
    }
};

// ������� - ��� - �������� - ����� ------------------------------------------------------------------------------------------------------------------
static const Choice mcADC_Stretch_Mode =
{
    Item_Choice, &mspADC_Stretch, 0,
    {
        "�����", "Mode",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {"��������", "Real"},
        {"������", "Manual"}
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

// ������� - ��� - �������� - �������� 1� ------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_A =
{
    Item_Governor, &mspADC_Stretch, IsActive_ADC_StretchAB,
    {
        "�������� 1�", "Stretch 1ch"
        ,
        "����� ������ �������� ������� ������.\n"
        "1 ������� = 0.0001"
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

// ������� - ��� - �������� - �������� 2� ------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_B =
{
    Item_Governor, &mspADC_Stretch, IsActive_ADC_StretchAB,
    {
        "�������� 2�", "Stretch 2ch"
        ,
        "����� ������ �������� ������� ������.\n"
        "1 ������� = 0.0001"
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

// ������� - ��� - �������� - 20��/1� 1� -------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Ak20mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "20��/1� 1�", "20mV/1V 1k",
        "",
        ""
    },
    &setNR.addStretch20mV[A], -10000, 10000
};

// ������� - ��� - �������� - 50�� 1� ----------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Ak50mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "50�� 1�", "50mV 1k",
        "",
        ""
    },
    &setNR.addStretch50mV[A], -10000, 10000
};

// ������� - ��� - �������� - 100��/5� 1� ------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Ak100mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "100��/5� 1�", "100mV/5V 1ch",
        "",
        ""
    },
    &setNR.addStretch100mV[A], -10000, 10000
};

// ������� - ��� - �������� - 2� 1� ------------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Ak2V =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "2� 1�", "2V 1ch",
        "",
        ""
    },
    &setNR.addStretch2V[A], -10000, 10000
};

// ������� - ��� - �������� - 20��/1� 2� -------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Bk20mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "20��/1� 2�", "20mV/1V 2k",
        "",
        ""
    },
    &setNR.addStretch20mV[B], -10000, 10000
};

// ������� - ��� - �������� - ��� ���� 50�� 2� -------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Bk50mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "50�� 2�", "50mV 2k",
        "",
        ""
    },
    &setNR.addStretch50mV[B], -10000, 10000
};

// ������� - ��� - �������� - 100��/5� 2� ------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Bk100mV =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "100��/5� 2�", "100mV/5V 2k",
        "",
        ""
    },
    &setNR.addStretch100mV[B], -10000, 10000
};

// ������� - ��� - �������� - 2� 2� ------------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_Bk2V =
{
    Item_Governor, &mspADC_Stretch, 0,
    {
        "2� 2�", "2V 2ch",
        "",
        ""
    },
    &setNR.addStretch2V[B], -10000, 10000
};

// ������� - ��� - ��� ���� //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspADC_Shift =
{
    Item_Page, &mspADC, 0,
    {
        "��� ����", "ADD RSHFIT",
        "",
        ""
    },
    Page_DebugADCrShift,
    {
        (void*)&mbADC_Shift_Reset,    // ������� - ��� - ��� ���� - �����
        (void*)&mgADC_Shift_A2mV,     // ������� - ��� - ��� ���� - �� 1� 2�� ����
        (void*)&mgADC_Shift_B2mV,     // ������� - ��� - ��� ���� - �� 2� 2�� ����
        (void*)&mgADC_Shift_A5mV,     // ������� - ��� - ��� ���� - �� 1� 5�� ����
        (void*)&mgADC_Shift_B5mV,     // ������� - ��� - ��� ���� - �� 2� 5�� ����
        (void*)&mgADC_Shift_A10mV,    // ������� - ��� - ��� ���� - �� 1� 10�� ����
        (void*)&mgADC_Shift_B10mV     // ������� - ��� - ��� ���� - �� 2� 10�� ����
    }
};

// ������� - ��� - ��� ���� - ����� ------------------------------------------------------------------------------------------------------------------
static const Button mbADC_Shift_Reset =
{
    Item_Button, &mspADC_Shift, 0,
    {
        "�����", "Reset",
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
    FPGA_SetRShift(A, SET_RSHIFT_A);
    FPGA_SetRShift(B, SET_RSHIFT_B);
}

// ������� - ��� - ��� ���� - �� 1� 2�� ���� ---------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_A2mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "�� 1� 2�� ����", "Shift 1ch 2mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[A][Range_2mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_A
};

static void OnChange_ADC_Shift_A(void)
{
    FPGA_SetRShift(A, SET_RSHIFT_A);
}

// ������� - ��� - ��� ���� - �� 2� 2�� ���� ---------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_B2mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "�� 2� 2�� ����", "Shift 2ch 2mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[B][Range_2mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_B
};

static void OnChange_ADC_Shift_B(void)
{
    FPGA_SetRShift(B, SET_RSHIFT_B);
}

// ������� - ��� - ��� ���� - �� 1� 5�� ���� ---------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_A5mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "�� 1� 5�� ����", "Shift 1ch 5mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[A][Range_5mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_A
};

// ������� - ��� - ��� ���� - �� 2� 5�� ���� ---------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_B5mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "�� 2� 5�� ����", "Shift 2ch 5mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[B][Range_5mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_B
};

// ������� - ��� - ��� ���� - �� 1� 10�� ���� --------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_A10mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "�� 1� 10�� ����", "Shift 1ch 10mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[A][Range_10mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_A
};

// ������� - ��� - ��� ���� - �� 2� 10�� ���� --------------------------------------------------------------------------------------------------------
static const Governor mgADC_Shift_B10mV =
{
    Item_Governor, &mspADC_Shift, 0,
    {
        "�� 2� 10�� ����", "Shift 2ch 10mV DC",
        "",
        ""
    },
    (int16*)(&setNR.rShiftAdd[B][Range_10mV][ModeCouple_DC]), -100, 100, OnChange_ADC_Shift_B
};

// ������� - ����-��� ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspRand =
{
    Item_Page, &mpDebug, 0,
    {
        "����-���", "RANDOMIZER",
        "",
        ""
    },
    Page_DebugRandomizer,
    {
        (void*)&mgRand_NumAverage,          // ������� - ����-��� - ������.
        (void*)&mgRand_NumSmooth,           // ������� - ����-��� - �����������
        (void*)&mgRand_NumMeasures,         // ������� - ����-��� - ���������
        (void*)&mcRand_ShowInfo,            // ������� - ����-��� - ����������
        (void*)&mgRand_ShowStat,            // ������� - ����-��� - ����������
        (void*)&mgRand_TimeCompensation,    // ������� - ����-��� - ����������� ��������
        (void*)&mgRand_AddTimeShift,        // ������� - ����-��� - ��������
        (void*)&mgRand_Pretriggered         // ������� - �A��-��� - ����������
    }
};

// ������� - ������ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspChannels =
{
    Item_Page, &mpDebug, 0,
    {
        "������", "CHANNELS",
        "",
        ""
    },
    Page_DebugChannels,
    {
        (void*)&mcChannels_BandwidthA,  // ������� - ������ - ������ 1
        (void*)&mcChannels_BandwidthB   // ������� - ������ - ������ 2
    }
};

// ������� - ������ - ������ 1 -----------------------------------------------------------------------------------------------------------------------
static const Choice mcChannels_BandwidthA =
{
    Item_Choice, &mspChannels, 0,
    {
        "������ 1", "Bandwidth 1",
        "����� ����� ������� ������, ������� ����� ����������� � �����1-������ ��� ������ �������� ������",
        "Here you can select the bandwidth, which will operate in CHANNEL1-Bandwidth when set to Full"
    },
    {
        {"������", "Full"},
        {"20���", "20MHz"},
        {"100���", "100MHz"},
        {"200���", "200MHz"},
        {"350���", "350MHz"},
        {"650���", "650MHz"},
        {"750���", "750MHz"}
    },
    (int8*)&BANDWIDTH_DEBUG(A), OnChange_Channels_BandwidthA
};

static void OnChange_Channels_BandwidthA(bool active)
{
    FPGA_SetBandwidth(A);
}

// ������� - ������ - ������ 1 -----------------------------------------------------------------------------------------------------------------------
static const Choice mcChannels_BandwidthB =
{
    Item_Choice, &mspChannels, 0,
    {
        "������ 2", "Bandwidth 2",
        "����� ����� ������� ������, ������� ����� ����������� � �����2-������ ��� ������ �������� ������",
        "Here you can select the bandwidth, which will operate in CHANNEL2-Bandwidth when set to Full"
    },
    {
        {"������", "Full"},
        {"20���", "20MHz"},
        {"100���", "100MHz"},
        {"200���", "200MHz"},
        {"350���", "350MHz"},
        {"650���", "650MHz"},
        {"750���", "750MHz"}
    },
    (int8*)&BANDWIDTH_DEBUG(B), OnChange_Channels_BandwidthB
};

static void OnChange_Channels_BandwidthB(bool active)
{
    FPGA_SetBandwidth(B);
}

// ������� - ����-��� - ��������� --------------------------------------------------------------------------------------------------------------------
static const Governor mgRand_NumMeasures =
{
    Item_Governor, &mspRand, 0,
    {
        "���-�/������", "Samples/gates",
        "",
        ""
    },
    &NUM_MEASURES_FOR_GATES, 1, 2500, OnChange_Rand_NumMeasures
};

static void OnChange_Rand_NumMeasures(void)
{
    FPGA_SetNumberMeasuresForGates(NUM_MEASURES_FOR_GATES);
}

// ������� - ����-��� - ����������� �������� ---------------------------------------------------------------------------------------------------------
static const Governor mgRand_TimeCompensation =
{
    Item_Governor, &mspRand, 0,
    {
        "����������� ��������", "Compenstaion time",
        "���������� ����������� �������� ��� 40 ��",
        ""
    },
    &TIME_COMPENSATION, 0, 510, OnChange_Rand_TimeCompensation
};

static void OnChange_Rand_TimeCompensation(void)
{
    FPGA_SetDeltaTShift(TIME_COMPENSATION);
}

// ������� - ����-��� - �������� ---------------------------------------------------------------------------------------------------------------------
int16 addShift = 0;

static const Governor mgRand_AddTimeShift =
{
    Item_Governor, &mspRand, 0,
    {
        "��� ��������", "Add shift",
        "���������� ������� ��� �������� tShift",
        ""
    },
    &addShift, -100, 100, OnChange_Rand_AddTimeShift
};

static void OnChange_Rand_AddTimeShift(void)
{
    FPGA_SetTShift(SET_TSHIFT);
}

// ������� - ����-��� - ���������� -------------------------------------------------------------------------------------------------------------------
static const Governor mgRand_Pretriggered =
{
    Item_Governor, &mspRand, 0,
    {
        "����������", "Pretiggered",
        "�������� �����������, ������� ������� � ������������",
        ""
    },
    &PRETRIGGERED, 0, 30000, OnChange_Rand_Pretriggered
};

static void OnChange_Rand_Pretriggered(void)
{
    LoadTShift();
}

// ������� - ����-��� - ������. ----------------------------------------------------------------------------------------------------------------------
static const Governor mgRand_NumAverage =
{
    Item_Governor, &mspRand, 0,
    {
        "������.", "Average",
        "",
        ""
    },
    &setNR.numAveForRand, 1, 32
};

// ������� - ����-��� - ����������� ------------------------------------------------------------------------------------------------------------------
static const Governor mgRand_NumSmooth =
{
    Item_Governor, &mspRand, 0,
    {
        "�����������", "Smoothing",
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
        "����������", "",
        "", ""
    },
    &pred, 0, 15000, OnChange_Pred
};

static const Governor mgPost =
{
    Item_Governor, &mpDebug, 0,
    {
        "�����������", "",
        "", ""
    },
    &post, 0, 15000, OnChange_Post
};


// ������� - ��������� ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspShowSettingsInfo =
{
    Item_Page, &mpDebug, 0,
    {
        "���������", "Settings",
        "�������� ���������� � ����������",
        "Show settings information"
    },
    Page_SB_DebugShowSetInfo,
    {
        (void*)&sbExitShowSetInfo
    },
    OnPress_ShowInfo
};

static void PressSB_ShowSetInfo_Exit(void)
{
    Display_SetDrawMode(DrawMode_Auto, 0);
}

static const SmallButton sbExitShowSetInfo =
{
    Item_SmallButton, &mspShowSettingsInfo,
    COMMON_BEGIN_SB_EXIT,
    PressSB_ShowSetInfo_Exit,
    DrawSB_Exit
};

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

    //Painter_DrawFormatText(x0, Y_AND_INCREASE, "������ �������� ��������� %d", sizeof(set));
    //Painter_DrawFormatText(x0, Y_AND_INCREASE, "������ �������������� ��������� %d", sizeof(setNR));
    DRAW_FORMAT("������ �������� ��������� : %d", sizeof(set));
    DRAW_FORMAT("������ �������������� ��������� : %d", sizeof(setNR));
    Painter_DrawText(x0, Y_AND_INCREASE, "�������������� ���������:");
    int x = Painter_DrawText(x0, Y_AND_INCREASE, "rShiftAdd :") + 5;

    int ddY = 0;

    for (int type = 0; type < 2; type++)
    {
        for (int ch = 0; ch < 2; ch++)
        {
            for (int range = 0; range < RangeSize; range++)
            {
                Painter_DrawFormatText(x + range * 20, y + dY * ddY, "%d", setNR.rShiftAdd[ch][range][type]);
            }
            ddY++;
        }
    }

    y += dY * 3;

    DRAW_FORMAT("correctionTime : %d", setNR.correctionTime);
    DRAW_FORMAT2("balanceADC : %d %d", setNR.balanceADC[0], setNR.balanceADC[1]);
    DRAW_FORMAT("numAveForRand : %d", setNR.numAveForRand);

    const char *s[3] = {"���������", "��������� �������������", "������ �������"};
    DRAW_FORMAT("balanceADCtype : %s", (setNR.balanceADCtype < 3 ? s[setNR.balanceADCtype] : "!!! ������������ �������� !!!"));
    DRAW_FORMAT("stretchADCtype : %s", (setNR.stretchADCtype < 3 ? s[setNR.stretchADCtype] : "!!! ������������ �������� !!!"));
  
    x = Painter_DrawText(x0, Y_AND_INCREASE, "stretchADC :") + 5;

    for (int ch = 0; ch < 2; ch++)
    {
        for (int num = 0; num < 3; num++)
        {
            Painter_DrawFormatText(x + num * 20, y + dY * ch, "%d", setNR.stretchADC[ch][num]);
        }
    }

    y += dY;

#define DRAW_STRETCH(name) DRAW_FORMAT2(#name " : %d %d", setNR.##name[0], setNR.##name[1])

    DRAW_STRETCH(addStretch20mV);
    DRAW_STRETCH(addStretch50mV);
    DRAW_STRETCH(addStretch100mV);
    DRAW_STRETCH(addStretch2V);

    DRAW_FORMAT("numSmoothForRand : %d", setNR.numSmoothForRand);

    Menu_Draw();
    Painter_EndScene();
}

static void OnPress_ShowInfo(void)
{
    OpenPageAndSetItCurrent(Page_SB_DebugShowSetInfo);
    Display_SetDrawMode(DrawMode_Auto, DebugShowSetInfo_Draw);
}

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

// ������� - ����-��� - ���������� -------------------------------------------------------------------------------------------------------------------
static const Choice mcRand_ShowInfo =
{
    Item_Choice, &mspRand, 0,
    {
        "����������", "Information",
        "���������� ���������� � ������� �������������",
        "To show information on randomizer gate"
    },
    {
        {"�� ����������", "Hide"},
        {"����������", "Show"}
    },
    (int8*)&SHOW_RAND_INFO
};

// ������� - ����-��� - ���������� -------------------------------------------------------------------------------------------------------------------
static const Choice mgRand_ShowStat =
{
    Item_Choice, &mspRand, 0,
    {
        "����������", "Statistics",
        "���������� ������ ����������",
        "Statistics show schedule"
    },
    {
        {"�� ����������",   "Hide"},
        {"����������",      "Show"}
    },
    (int8*)&SHOW_RAND_STAT
};

// ������� - ��� /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Choice mcEMS =
{
    Item_Choice, &mpDebug, 0,
    {
        "����� ���", "EMS mode",
        "������������� �������� ������ 20���, ����������� �� 4-� ������, ���������� �� 8-�� ������",
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

// ������� - ���������� //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Choice mcDisplayOrientation =
{
    Item_Choice, &mpDebug, 0,
    {
        "����������", "DisplayOrientation",
        "������������� ���������� �������",
        "Sets display orientation"
    },
    {
        { "������", "Direct" },
        { "��������", "Back" }
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

        /// ������� - ��� - ���. ����. ���. - ��������
        const Governor mgDebugADCaltShift =
        {
            Item_Governor, &mspDebugADCaltShift,
            {
                "��������", "Value"
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


const Page mspDebugADCaltShift =    // ������� - ��� - ��� ���� ���
{
    Item_Page, &mspADC,
    {
        "��� ���� ���", "ALT SHIFT MEM"
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
