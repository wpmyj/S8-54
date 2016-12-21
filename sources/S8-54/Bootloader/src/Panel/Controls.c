#include "Controls.h"


//---------------------------------------------------------------------------------------------------------------------------------------------------
static const char* namesButtons[] = 
{
    "B_Empty",
    "B_ChannelA",
    "B_Service",
    "B_ChannelB",
    "B_Display",
    "B_Time",
    "B_Memory",
    "B_Sinchro",
    "B_Start",
    "B_Cursors",
    "B_Measures",
    "B_Power",
    "B_Info",
    "B_Menu",
    "B_F1",
    "B_F2",
    "B_F3",
    "B_F4",
    "B_F5"
};


//---------------------------------------------------------------------------------------------------------------------------------------------------
const char* NameButton(PanelButton button) 
{
    return namesButtons[button];
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static const char* nameReg[] =
{
    "None", "None", "None", "None", "None", "None", "None", "None", "None", "None",
    "None", "None", "None", "None", "None", "None", "None", "None", "None", "None",
    "R_RangeA",         // 0x14
    "R_RShiftA",
    "R_RangeB",
    "R_RShiftB",
    "R_TBase",
    "R_TShift",
    "R_TrigLev",
    "R_Set",
    "R_RangeA_BTN",
    "R_RShiftA_BTN",
    "R_RangeB_BTN",
    "R_RShiftB_BTN",
    "R_TBase_BTN",
    "R_TShift_BTN",
    "R_TrigLev_BTN",
    "R_Set_BTN"
};


//---------------------------------------------------------------------------------------------------------------------------------------------------
const char* NameRegulator(Regulator regulator)
{
    return nameReg[regulator];
}
