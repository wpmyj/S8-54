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
static uint8 *dataDir[2] = {0, 0};      ///< Последние считанные данные.
static uint8 *dataRAM[2] = {0, 0};     ///< Данные, которые должны выводиться в режиме "ПОСЛЕДНИЕ".
static uint8 *dataROM[2] = {0, 0};      ///< \brief Данные, которые должны выводиться в режиме "ВНУТР ЗУ" или нормальном режиме при соотв. 
                                            ///< настройке "ПАМЯТЬ-ВНУТР ЗУ-Показывать всегда".

static DataSettings *pDSDir = 0;
static DataSettings *pDSRAM = 0;
static DataSettings *pDSROM = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Clear(void);
/// Считать данные 
static void GetDataFromStorage(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_GetFromIntMemory(void)
{
    FLASH_GetData(NUM_ROM_SIGNAL, &pDSROM, &dataROM[A], &dataROM[B]);
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

    DS_GetDataFromEnd_RAM(fromEnd, &pDSDir, (uint16**)&dataDir[A], (uint16**)&dataDir[B]);

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

    if (MODE_WORK_DIR)                          // Если находимся в реальном режиме
    {   
        GetDataFromStorage();                   // Считываем данные из хранилища

        if (ALWAYS_SHOW_ROM_SIGNAL)             // И, если нужно показывать сигнал из ППЗУ и в основном режиме
        {
            Data_GetFromIntMemory();            // то из хранилща
        }
    }
    else if (MODE_WORK_RAM)                     // Если находимся в режиме отображения последних
    {
        DS_GetDataFromEnd_RAM(NUM_RAM_SIGNAL, &pDSRAM, (uint16**)&dataRAM[A], (uint16**)&dataRAM[B]);
    }
    else if (MODE_WORK_ROM)
    {
        Data_GetFromIntMemory();                // Считываем данные из ППЗУ

        if (!SHOW_IN_INT_SAVED)                 // И, если нужно
        {
            GetDataFromStorage();               // из хранилища
        }
    }

    int first = 0;
    int last = 0;
    sDisplay_PointsOnDisplay(&first, &last);
    
    Data_PrepareToUse(MODE_WORK);

    Processing_SetSignal(DATA_A, DATA_B, DS, first, last);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Clear(void)
{
    pDS = pDSDir = pDSRAM = pDSROM = 0;
    dataChan[A] = dataChan[B] = 0;
    dataRAM[A] = dataRAM[B] = 0;
    dataROM[A] = dataROM[B] = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_PrepareToUse(ModeWork mode)
{
    /// \todo переделать цепочку условий на выбор из массива
    if (MODE_WORK_DIR)
    {
        DS = pDSDir;
        DATA_A = dataDir[A];
        DATA_B = dataDir[B];
    }
    else if (MODE_WORK_RAM)
    {
        DS = pDSRAM;
        DATA_A = dataRAM[A];
        DATA_B = dataRAM[B];
    }
    else if (MODE_WORK_ROM)
    {
        DS = pDSROM;
        DATA_A = dataROM[A];
        DATA_B = dataROM[B];
    }
}

/** @}  @}
 */
