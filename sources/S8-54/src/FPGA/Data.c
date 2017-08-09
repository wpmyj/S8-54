// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _INCLUDE_DATA_
#include "Data.h"
#undef _INCLUDE_DATA_
#include "DataBuffer.h"
#include "Globals.h"
#include "Hardware/FLASH.h"
#include "Hardware/FSMC.h"
#include "Settings/SettingsMemory.h"
#include "Utils/ProcessingSignal.h"


/** @addtogroup FPGA
 *  @{
 *  @addtogroup Data
 *  @{
 */


static DataSettings dataSettings;   ///< Здесь хранятся настройки для текущего рисуемого сигнала


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_Clear(void)
{
    pDS = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_ReadDataRAM(int fromEnd)
{
    Data_Clear();

    bool readed = false;                // Признак того, что данные считаны

    if (IN_AVERAGING_MODE               // Если включено усреднение
        && fromEnd == 0)                // И запрашиваем псоледний считанный сигнал
    {
        Data_GetAverageFromDataStorage();
        readed = true;
    }
    else
    {
        DS_GetDataFromEnd(fromEnd, &dataSettings, inA, inB);
        readed = true;
    }

    if (readed)
    {
        if (ENUM_POINTS(&dataSettings) == FPGA_ENUM_POINTS) /** \todo Это временно. Нужно сделать пересчёт к установленной длине памяти в
                                                            в ProcessingSignal_SetData(), чтобы не мелькало на экране. */
        {
            pDS = &dataSettings;

            Processing_SetData();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_ReadDataROM(void)
{
    Data_Clear();

    if (FLASH_GetData(NUM_ROM_SIGNAL, &dataSettings, inA, inB))
    {
        pDS = &dataSettings;

        Processing_SetData();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_GetAverageFromDataStorage(void)
{
    if(G_ENABLED_A)
    {
        memcpy(inA, DS_GetAverageData(A), BYTES_IN_CHANNEL(DS));
    }
    if(G_ENABLED_B)
    {
        memcpy(inB, DS_GetAverageData(B), BYTES_IN_CHANNEL(DS));
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_PrepareToDrawSettings(void)
{
    if (MODE_WORK_DIR)
    {

    }
    else if (MODE_WORK_RAM)
    {

    }
    else if (MODE_WORK_ROM)
    {
        if (SHOW_IN_INT_SAVED)                      // Если выводится только сохранённый сигнал
        {
        }
        else                                        // А если на экране есть и другой сигнал
        {
            if (EXIT_FROM_ROM_TO_RAM)               // Если сигнал из ОЗУ
            {
            }
            else                                    // Если непосредственный сигнал
            {
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
/*
static void GetDataFromStorage(void)
{
    int fromEnd = 0;

    if (IN_P2P_MODE &&                              // Находимся в режиме поточечного вывода
    START_MODE_WAIT &&                          // в режиме ждущей синхронизации
    DS_NumElementsWithCurrentSettings() > 1)    // и в хранилище уже есть считанные сигналы с такими настройками
    {
    fromEnd = 1;
    }

    DS_GetDataFromEnd_RAM(fromEnd, &pDSDir, (uint16**)&dataDirA, (uint16**)&dataDirB);

    if (sDisplay_NumAverage() != 1 || IN_RANDOM_MODE)
    {
    ModeFSMC mode = FSMC_GetMode();
    FSMC_SetMode(ModeFSMC_RAM);
    Data_GetAverageFromDataStorage();
    FSMC_SetMode(mode);
    }
}
*/


/** @}  @}
 */
