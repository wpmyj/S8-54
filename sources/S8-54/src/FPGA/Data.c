// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _INCLUDE_DATA_
#include "Data.h"
#undef _INCLUDE_DATA_
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8 *dataChanDir[2] = {0, 0};      ///< Последние считанные данные.
static uint8 *dataChanLast[2] = {0, 0};     ///< Данные, которые должны выводиться в режиме "ПОСЛЕДНИЕ".
static uint8 *dataChanInt[2] = {0, 0};      ///< \brief Данные, которые должны выводиться в режиме "ВНУТР ЗУ" или нормальном режиме при соотв. 
                                            ///< настройке "ПАМЯТЬ-ВНУТР ЗУ-Показывать всегда".

static DataSettings *pDSDir = 0;
static DataSettings *pDSLast = 0;
static DataSettings *pDSInt = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Clear(void);
/// Считать данные 
static void GetDataFromStorage(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_GetFromIntMemory(void)
{
    FLASH_GetData(gMemory.currentNumIntSignal, &pDSInt, &dataChanInt[A], &dataChanInt[B]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void GetDataFromStorage(void)
{
    int fromEnd = 0;

    if (IN_P2P_MODE &&                              // Находимся в режиме поточечного вывода
        START_MODE_WAIT &&                          // в режиме ждущей синхронизации
        DS_NumElementsWithCurrentSettings() > 1)    // и в хранилище уже есть считанные сигналы с такими настройками
    {
        fromEnd = 1;
    }

    DS_GetDataFromEnd_RAM(fromEnd, &pDS, (uint16**)&DATA_A, (uint16**)&DATA_B);

    if (sDisplay_NumAverage() != 1 || IN_RANDOM_MODE)
    {
        ModeFSMC mode = FSMC_GetMode();
        FSMC_SetMode(ModeFSMC_RAM);
        Data_GetAverageFromDataStorage();
        FSMC_SetMode(mode);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_GetAverageFromDataStorage(void)
{
    dataChan[A] = DS_GetAverageData(A);
    dataChan[B] = DS_GetAverageData(B);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_Load(void)
{
    Clear();

    if (DS_NumElementsInStorage() == 0)
    {
        return;
    }

    if (WORK_DIRECT)                        // Если находимся в реальном режиме
    {   
        GetDataFromStorage();               // Считываем данные из хранилища

        if (ALWAYS_SHOW_MEM_INT_SIGNAL)     // И, если нужно показывать сигнал из ППЗУ и в основном режиме
        {
            Data_GetFromIntMemory();        // то из хранилща
        }
    }
    else if (WORK_LAST)                     // Если находимся в режиме отображения последних
    {

    }
    else if (WORK_EEPROM)
    {
        Data_GetFromIntMemory();            // Считываем данные из ППЗУ

        if (!SHOW_IN_INT_SAVED)             // И, если нужно
        {
            GetDataFromStorage();           // из хранилища
        }
    }

    int first = 0;
    int last = 0;
    sDisplay_PointsOnDisplay(&first, &last);

    Processing_SetSignal(DATA_A, DATA_B, DS, first, last);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Clear(void)
{
    pDS = pDSDir = pDSLast = pDSInt = 0;
    dataChan[A] = dataChan[B] = 0;
    dataChanLast[A] = dataChanLast[B] = 0;
    dataChanInt[A] = dataChanInt[B] = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_PrepareToUse(ModeWork mode)
{
    if (mode == ModeWork_Direct)
    {
        DS = pDSDir;
    }
    else if (mode == ModeWork_Latest)
    {
        DS = pDSLast;
    }
    else if (mode == ModeWork_EEPROM)
    {
        DS = pDSInt;
    }
}

/** @}  @}
 */
