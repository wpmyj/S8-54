#include "defines.h"
#include "FPGA/DataStorage.h"
#include "Settings/Settings.h"
#include "Hardware/FSMC.h"
#include "Hardware/Timer.h"
#include "Hardware/Timer2.h"
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void main3(void);

#define TICS ((gTimerTics - time) / 120.0f)

#ifdef MODE_NORMAL

static void ProcessingSignal(void);
static void DrawWelcomeScreen(void);
static void StopDrawWelcomeScreen(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Disable_IfNessessary(void)
{
    if (gBF.disablePower == 1)
    {
        Settings_Save();
        Log_DisconnectLoggerUSB();
        Panel_TransmitData(0x04);
        bool run = true;
        while (run)
        {
        };
    }
}


#endif


//------------------------------------------------------------------------------------------------------------------------------------------------------
int main(void)
{
    /*
    Программа записывается по адресу 0x08020000
    Таблица векторов прерываний должна быть записана по адресу 0x08020000. Это делается путём строки в system_stm32xx.c строки #define VECT_TAB_OFFSET 0x08020000
    Адрес запуска находится по адресу 0x08020004 (там адрес обработчика прерывания сброса)
    Мы переходим на него из загрузчика, расположенного по адресу 0x08000000
    */
#ifdef MODE_Z80
    
    Z80_Run();
    
#else
    
    Log_EnableLoggerUSB(true);
    main3();
    VCP_Init();
    Settings_Load(false);
    FPGA_Init();
    Timer_PauseOnTime(250);
    FPGA_OnPressStartStop();
    Display_Init();
    Ethernet_Init();
    Menu_Init();

    if (set.service.screenWelcomeEnable)
    {
        Display_SetDrawMode(DrawMode_Hand, DrawWelcomeScreen);
        Timer2_SetAndStartOnce(kTemp, StopDrawWelcomeScreen, 5000);
    }

    bool run = true;
    while(run)
    {
        Timer_StartMultiMeasurement();      // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.
        Ethernet_Update(0);
        FDrive_Update();
        FPGA_Update();                      // Обновляем аппаратную часть.
        ProcessingSignal();
        Panel_Update();
        Menu_UpdateInput();                 // Обновляем состояние меню
        Display_Update();                   // Рисуем экран.
        Disable_IfNessessary();
    }
    
#endif
}


#ifdef MODE_NORMAL

//------------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessingSignal(void)
{
    uint8 **dataA = &gDataA;
    uint8 **dataB = &gDataB;
    DataSettings **ds = &gDSet;

    if (DS_NumElementsInStorage() == 0)
    {
        *dataA = 0;
        *dataB = 0;
        *ds = 0;
        return;
    }

    int first = 0;
    int last = 0;
    sDisplay_PointsOnDisplay(&first, &last);
    if (WORK_DIRECT)
    {
        if (sTime_P2PModeEnabled() &&                   // Находимся в режиме поточечного вывода
            set.trig.startMode == StartMode_Wait &&     // в режиме ждущей синхронизации
            DS_NumElementsWithCurrentSettings() > 1     // и в хранилище уже есть считанные сигналы с такими настройками
            )
        {
            DS_GetDataFromEnd_RAM(1, &gDSet, (uint16**)&gDataA, (uint16**)&gDataB);
        }
        else
        {
            DS_GetDataFromEnd_RAM(0, &gDSet, (uint16**)&gDataA, (uint16**)&gDataB);
        }

        if (sDisplay_NumAverage() != 1 || sTime_RandomizeModeEnabled())
        {
            ModeFSMC mode = FSMC_GetMode();
            FSMC_SetMode(ModeFSMC_RAM);
            gDataA = DS_GetAverageData(A);
            gDataB = DS_GetAverageData(B);
            FSMC_SetMode(mode);
        }
    }
    else if (WORK_LAST)
    {
        dataA = &gDataAmemLast;
        dataB = &gDataBmemLast;
        ds = &gDSmemLast;
        DS_GetDataFromEnd_RAM(gMemory.currentNumLatestSignal, &gDSmemLast, (uint16**)&gDataAmemLast, (uint16**)&gDataBmemLast);
    }
    else if (WORK_INT)
    {
        dataA = &gDataAmemInt;
        dataB = &gDataBmemInt;
        ds = &gDSmemInt;
        FLASH_GetData(gMemory.currentNumIntSignal, &gDSmemInt, &gDataAmemInt, &gDataBmemInt);
    }

    if (WORK_INT)
    { 
        if (SHOW_IN_INT_BOTH || SHOW_IN_INT_DIRECT)
        {
            Processing_SetSignal(gDataA, gDataB, gDSet, first, last);
        }
    }
    else
    {
        Processing_SetSignal(*dataA, *dataB, *ds, first, last);
    }

    CursorsUpdate();    // В случае, если находимся в режиме курсорных измерений, обновляем их положение, если нужно.
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawWelcomeScreen(void)
{
    Painter_BeginScene(gColorBack);
    Painter_SetColor(gColorFill);
    Painter_DrawRectangle(0, 0, 319, 239);
    Painter_DrawBigText(32, 50, 9, "МНИПИ");
    Painter_DrawStringInCenterRect(0, 180, 320, 20, "Для получения помощи нажмите и удерживайте кнопку ПОМОЩЬ");
    Painter_DrawStringInCenterRect(0, 205, 320, 20, "Отдел маркетинга: тел./факс. 8-017-262-57-50");
    Painter_DrawStringInCenterRect(0, 220, 320, 20, "Разработчики: e-mail: mnipi-24(@)tut.by, тел. 8-017-262-57-51");
    Painter_EndScene();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void StopDrawWelcomeScreen(void)
{
    Display_SetDrawMode(DrawMode_Auto, 0);
}

#endif
