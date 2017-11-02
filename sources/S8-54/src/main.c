#include "Log.h"
#include "Ethernet/Ethernet.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "Hardware/Hardware.h"
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
    //main3();
    Hardware_Init();
    VCP_Init();
    Settings_Load(false);
    FPGA_Init();
    FPGA_OnPressStartStop();
    display.Init();
    Ethernet_Init();
    Menu_Init();
    
    while(1)
    {
        Timer_StartMultiMeasurement();  // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.
        Ethernet_Update(0);             // Обрабатываем LAN
        FDrive_Update();                // Обрабатываем флешку
        FPGA_Update();                  // Обновляем аппаратную часть.
        panel.Update();                 // Обрабатываем панель
        Menu_UpdateInput();             // Обновляем состояние меню
        display.Update();               // Рисуем экран.
        panel.DisableIfNessessary();    // Выключаем, если необходимо
    }
}
