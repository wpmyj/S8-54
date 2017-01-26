/*
*************** Описание внизу ****************************
*/


#include "main.h"
#include "globals.h"
#include "FlashDrive/FlashDrive.h"
#include "Hardware/Hardware.h"
#include "Settings/Settings.h"
#include "Display/Painter.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Panel/Panel.h"
#include "Hardware/Flash.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define FILE_NAME "S8-54.bin"

typedef void(*pFunction)(void);

MainStruct *ms;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint jumpAddress = 0;
pFunction JumpToApplication;
State state = State_Start;

float percentUpdate = 0.0f;     // Сколько времени обновления завершено


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Upgrade(void);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    ms = malloc(sizeof(MainStruct));
    
    HAL_Init();

    Hardware_Init();

    Settings_Load();

    Timer_PauseOnTime(250);

    Display_Init();

    state = State_Start;

    Timer_Enable(kTemp, 10, Display_Update);

    uint timeStart = gTimerMS;

    FDrive_Init();

    while (gTimerMS - timeStart < 2000 && !FDrive_Update())
    {
    }

    if ((ms->connection && ms->active == 0) || (ms->active && state != State_Mount))
    {
        free(ms);
        NVIC_SystemReset();
    }

    if (state == State_Mount)   // Это означает, что диск удачно примонтирован
    {
        if (FDrive_FileExist(FILE_NAME))
        {
            state = State_RequestAction;
            
            while (1)
            {
                PanelButton button = Panel_PressedButton();
                if (button == B_F1)
                {
                    state = State_Upgrade;
                    Upgrade();
                    break;
                }
                else if (button == B_F5)
                {
                    state = State_Ok;
                    break;
                }
            }
        }
        else
        {
            state = State_NotFile;
        }
    }
    else if (state == State_WrongFlash) // Диск не удалось примонтировать
    {
        Timer_PauseOnTime(5000);
    }

    Panel_DeInit();

    Timer_Disable(kTemp);

    while (Display_IsRun())
    {
    }

    Display_Update();
    
    HAL_DeInit();

    free(ms);
    
    __disable_irq();
    // Теперь переходим на основную программу
    JumpToApplication = (pFunction)(*(__IO uint*)(MAIN_PROGRAM_START_ADDRESS + 4));
    __set_MSP(*(__IO uint*)MAIN_PROGRAM_START_ADDRESS);
    __enable_irq();
    JumpToApplication();
}



/*
    По адресу 0x08000000 находится загрузчик.
    После сброса он проверяет наличие флешки.
        Если флешка есть:
            проверяет наличие файла S8-54.bin. Если таковой имеется, выводит сообщение "На USB-диске обнаружено новоое программное обеспечение. Установить?"
            Если нажато "Да":
                1. Стираются сектора:
                    5 - 0x08020000
                    6 - 0x08040000
                    7 - 0x08060000
                2. На их место записывается содержимое файла S8-54.bin
        Если флешку примонтировать не удалось:
            Вывести сообщение "Не удалось примонтировать флешку. Убедитесь, что на ней файловая система fat32"
    Далее выполняется переход по адресу, указанному в 0x0802004
*/


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Upgrade(void)
{
    const int sizeSector = 1 * 1024;
    
    static uint8 buffer[sizeSector];
    
    FLASH_Prepare();
    
    int size = FDrive_OpenFileForRead(FILE_NAME);
    int fullSize = size;
    uint address = ADDR_SECTOR_PROGRAM_0;

    while (size)
    {
        int readedBytes = FDrive_ReadFromFile(sizeSector, buffer);
        WriteData(address, buffer, readedBytes);
        size -= readedBytes;
        address += readedBytes;

        percentUpdate = 1.0f - (float)size / fullSize;
    }
    
    FDrive_CloseOpenedFile();
}
