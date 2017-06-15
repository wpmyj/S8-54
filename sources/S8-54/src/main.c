// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "FPGA/Data.h"
#include "Settings/Settings.h"
#include "Hardware/FSMC.h"
#include "Hardware/Timer.h"
#include "Hardware/FLASH.h"
#include "Log.h"
#include "Panel/Panel.h"
#include "VCP/VCP.h"
#include "FPGA/FPGA.h"
#include "Ethernet/Ethernet.h"
#include "Menu/Menu.h"
#include "Utils/ProcessingSignal.h"
#include "Menu/Pages/PageCursors.h"
#include "FlashDrive/FlashDrive.h"


/// \todo По идее c этим должно работать TODO("message")
#define STRINGIZE_2( _ ) #_
#define STRINGIZE( _ ) STRINGIZE_2( _ )
#define TODO(msg) __pragma(message(__FILE__ "(" STRINGIZE(__LINE__) ") : TODO: " msg))


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void main3(void);

#define TICS ((gTimerTics - time) / 120.0f)

//static void ProcessingSignal(void);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Disable_IfNessessary(void)
{
    if (gBF.disablePower == 1)
    {
        Settings_Save();
        Log_DisconnectLoggerUSB();
        Panel_TransmitData(0x04);
        volatile bool run = true;
        while (run)
        {
        };
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int main(void)
{
    Log_EnableLoggerUSB(true);
    main3();
    VCP_Init();
    Settings_Load(false);
    FPGA_Init();
    FPGA_OnPressStartStop();
    Display_Init();
    Ethernet_Init();
    Menu_Init();

    volatile bool run = true;
    while(run)
    {
        LOG_WRITE("                                                     start cicle");

        Timer_StartMultiMeasurement();  // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.
        Ethernet_Update(0);
        FDrive_Update();
        FPGA_Update();                  // Обновляем аппаратную часть.
        Data_Load();
        Panel_Update();
        Menu_UpdateInput();             // Обновляем состояние меню
        Display_Update();               // Рисуем экран.
        Disable_IfNessessary();
    }
}
