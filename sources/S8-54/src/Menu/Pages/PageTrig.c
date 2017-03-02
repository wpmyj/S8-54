#include "defines.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGAtypes.h"
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


// ����� ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpTrig =
{
    Item_Page, &mainPage, 0,
    {
        "�����", "TRIG",
        "�������� ��������� �������������.",
        "Contains synchronization settings."
    },
    Page_Trig,
    {
        (void*)&mcMode,         // ����� -> �����
        (void*)&mcSource,       // ����� -> ��������
        (void*)&mcPolarity,     // ����� -> ����������
        (void*)&mcInput,        // ����� -> ����
        (void*)&mspSearch,      // ����� -> �����
        (void*)&mgTimeDelay     // ����� -> ���������
    }
};

// ����� -> ����� --------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcMode =
{
    Item_Choice, &mpTrig, 0,
    {
        "�����",        "Mode"
        ,
        "����� ����� �������:\n"
        "1. \"����\" - ������ ���������� �������������.\n"
        "2. \"������\" - ������ ���������� �� ������ �������������.\n"
        "3. \"�����������\" - ������ ���������� �� ���������� ������ ������������� ���� ���. ��� ���������� ��������� ����� ������ ������ ����/����."
        ,
        "Sets the trigger mode:\n"
        "1. \"Auto\" - start automatically.\n"
        "2. \"Standby\" - the launch takes place at the level of synchronization.\n"
        "3. \"Single\" - the launch takes place on reaching the trigger levelonce. For the next measurement is necessary to press the START/STOP."
    },
    {
        {"���� ",       "Auto"},
        {"������",      "Wait"},
        {"�����������", "Single"}
    },
    (int8*)&START_MODE, OnChange_Mode
};

void OnChange_Mode(bool active)
{
    FPGA_Stop(false);
    if(!START_MODE_SINGLE)
    {
        FPGA_OnPressStartStop();
    }
    
    // ���� ��������� � ������ �������������
    if(IN_RANDOM_MODE)
    //if (TBASE < TBase_50ns)    // WARN ��� ������ ������� sTime_RandomizeModeEnabled() ���� ������� ������, ��� � �������� ����� ������������ �����������
    {
        // � ������������� �� ��������� ����� �������, �� ���� ��������� ��������� ��� �������, ����� ������������ ��� ����������� � ����� ������������� �������������� ��� ������
        if (START_MODE_SINGLE)
        {
            SAMPLE_OLD = SAMPLE;
            SAMPLE = SampleType_Real;
        }
        else if(START_MODE_AUTO)    // ����� ����������� ����� �����������
        {
            SAMPLE = SAMPLE_OLD;
        }
    }
}

// ����� -> �������� ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcSource =
{
    Item_Choice, &mpTrig, 0,
    {
        "��������", "Source",
        "����� ��������� ������� �������������.",
        "Synchronization signal source choice."
    },
    {
        {"����� 1", "Channel 1"},
        {"����� 2", "Channel 2"},
        {"�������", "External"}
    },
    (int8*)&TRIGSOURCE, OnChange_Source
};

static void OnChange_Source(bool active)
{
    FPGA_SetTrigSource(TRIGSOURCE);
}

// ����� -> ���������� ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcPolarity =
{
    Item_Choice, &mpTrig, 0,
    {
        "����������",   "Polarity"
        ,
        "1. \"�����\" - ������ ���������� �� ������ ��������������.\n"
        "2. \"����\" - ������ ���������� �� ����� ��������������."
        ,
        "1. \"Front\" - start happens on the front of clock pulse.\n"
        "2. \"Back\" - start happens on a clock pulse cut."
    },
    {
        {"�����",       "Front"},
        {"����",        "Back"}
    },
    (int8*)&TRIG_POLARITY, OnChange_Polarity
};

static void OnChange_Polarity(bool active)
{
    FPGA_SetTrigPolarity(TRIG_POLARITY);
}

// ����� -> ���� -----------------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcInput =
{
    Item_Choice, &mpTrig, 0,
    {
        "����", "Input"
        ,
        "����� ����� � ���������� �������������:\n"
        "1. \"��\" - ������ ������.\n"
        "2. \"��\" - �������� ����.\n"
        "3. \"���\" - ������ ������ ������.\n"
        "4. \"���\" - ������ ������� ������."
        ,
        "The choice of communication with the source of synchronization:\n"
        "1. \"SS\" - a full signal.\n"
        "2. \"AS\" - a gated entrance.\n"
        "3. \"LPF\" - low-pass filter.\n"
        "4. \"HPF\" - high-pass filter frequency."
    },
    {
        {"��", "Full"},
        {"��", "AC"},
        {"���", "LPF"},
        {"���", "HPF"}
    },
    (int8*)&TRIG_INPUT, OnChange_Input
};

static void OnChange_Input(bool active)
{
    FPGA_SetTrigInput(TRIG_INPUT);
}

// ����� -> ��������� ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Governor mgTimeDelay =
{
    Item_Governor, &mpTrig, 0,
    {
        "���������, ��", "Holdoff, ms",
        "������������� ����������� ����� ����� ���������.",
        "Sets the minimum time between starts."
    },
    (int16*)&set.trig.timeDelay, 45, 10000
};

// ����� -> ����� //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspSearch =
{
    Item_Page, &mpTrig, 0,
    {
        "�����", "SEARCH",
        "���������� �������������� ������� ������ �������������.",
        "Office of the automatic search the trigger level."
    },
    Page_TrigAuto,
    {
        (void*)&mcSearch_Mode,
        (void*)&mbSearch_Search
    }
};

// ����� -> ����� -> ����� ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcSearch_Mode =
{
    Item_Choice, &mspSearch, 0,
    {
        "�����", "Mode"
        ,
        "����� ������ ��������������� ������ �������������:\n"
        "1. \"������\" - ����� ������������ �� ������� ������ \"�����\" ��� �� ��������� � ������� 0.5� ������ �����, ���� ����������� \"������\x99��� ���� �����\x99�����������\".\n"
        "2. \"��������������\" - ����� ������������ �������������."
        ,
        "Selecting the automatic search of synchronization:\n"
#pragma push
#pragma diag_suppress 192
        "1. \"Hand\" - search is run on pressing of the button \"Find\" or on deduction during 0.5s the ����� button if it is established \"SERVICE\x99Mode long �����\x99\Autolevel\".\n"
#pragma pop
        "2. \"Auto\" - the search is automatically."
    },
    {
        {"������",          "Hand"},
        {"��������������",  "Auto"}
    },
    (int8*)&TRIG_MODE_FIND
};

// ����� -> ����� -> ����� ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Button mbSearch_Search =
{
    Item_Button, &mspSearch, IsActive_Search_Search,
    {
        "�����", "Search",
        "���������� ����� ������ �������������.",
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
