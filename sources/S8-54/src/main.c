// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Log.h"
#include "Ethernet/Ethernet.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "Hardware/Timer.h"
#include "Menu/Menu.h"
#include "Panel/Panel.h"
#include "VCP/VCP.h"


/// \todo По идее c этим должно работать TODO("message")
#define STRINGIZE_2( _ ) #_
#define STRINGIZE( _ ) STRINGIZE_2( _ )
#define TODO(msg) __pragma(message(__FILE__ "(" STRINGIZE(__LINE__) ") : TODO: " msg))


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void main3(void);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    
    while(1)
    {
        Timer_StartMultiMeasurement();  // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.
        Ethernet_Update(0);
        FDrive_Update();
        FPGA_Update();                  // Обновляем аппаратную часть.
        Panel_Update();
        Menu_UpdateInput();             // Обновляем состояние меню
        Display_Update();               // Рисуем экран.
        Panel_DisableIfNessessary();
    }
}
