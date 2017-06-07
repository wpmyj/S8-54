// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#pragma once
#include "Menu/MenuFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mpDebug;


// ОТЛАДКА - С/Н /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Button sbExitSerialNumber;

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Button sbSerialNumberLeft;

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Button sbSerialNumberRight;

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Button sbSerialNumberWrite;

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSerialNumber(void)
{
    Painter_BeginScene(gColorGrid);
    Painter_EndScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPressSerialNumber(void)
{
    OpenPageAndSetItCurrent(Page_SB_SerialNumber);
    Display_SetAddDrawFunction(DrawSerialNumber);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnRegSetSerialNumber(int delta)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const Page ppSerialNumber =
{
    Item_Page, &mpDebug, 0,
    {
        "С/Н", "S/N",
        "Запись серийного номера в OTP-память. ВНИМАНИЕ!!! ОТP-память - память с однократной записью.",
        "Serial number recording in OTP-memory. ATTENTION!!! OTP memory is a one-time programming memory."
    },
    Page_SB_SerialNumber,
    {
        (void*)&sbExitSerialNumber,
        (void*)&sbSerialNumberLeft,
        (void*)&sbSerialNumberRight,
        (void*)0,
        (void*)0,
        (void*)&sbSerialNumberWrite
    },
    OnPressSerialNumber, 0, OnRegSetSerialNumber
};
