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


/** \todo Этот экземпляр введён потому, что при непосредсвенный ссылке через pDSROM на настройки в EPROM  происходят глюки.
        Поэтому настройки просто копируются в эту структуру и потому берутся отсюда.
        Разбораться и сделать как надо - без этой структуры. */
static DataSettings dataSettingsROM;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8 *dataDirA = 0;     ///< Последние считанные данные.
static uint8 *dataDirB = 0;

static uint8 *dataRAMA = 0;     ///< Данные, которые должны выводиться в режиме "ПОСЛЕДНИЕ".
static uint8 *dataRAMB = 0;

static uint8 *dataROMA = 0;     ///< \brief Данные, которые должны выводиться в режиме "ВНУТР ЗУ" или нормальном режиме при соотв. 
static uint8 *dataROMB = 0;     ///< настройке "ПАМЯТЬ-ВНУТР ЗУ-Показывать всегда".

static DataSettings *pDSDir = 0;
static DataSettings *pDSRAM = 0;
static DataSettings *pDSROM = 0;

static ModeWork currentModeWork = ModeWork_Dir;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Clear(void);
/// Считать данные 
static void GetDataFromStorage(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_GetFromIntMemory(void)
{
    if(FLASH_GetData(NUM_ROM_SIGNAL, &pDSROM, &dataROMA, &dataROMB))
    {
        memcpy(&dataSettingsROM, (void*)pDSROM, sizeof(DataSettings));
    }
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

    DS_GetDataFromEnd_RAM(fromEnd, &pDSDir, (uint16**)&dataDirA, (uint16**)&dataDirB);

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

    if (MODE_WORK_DIR)                              // Если находимся в реальном режиме
    {   
        GetDataFromStorage();                       // Считываем данные из хранилища

        if (ALWAYS_SHOW_ROM_SIGNAL)                 // И, если нужно показывать сигнал из ППЗУ и в основном режиме
        {
            Data_GetFromIntMemory();                // то из хранилща
        }
    }
    else if (MODE_WORK_RAM)                         // Если находимся в режиме отображения последних
    {
        DS_GetDataFromEnd_RAM(NUM_RAM_SIGNAL, &pDSRAM, (uint16**)&dataRAMA, (uint16**)&dataRAMB);
    }
    else if (MODE_WORK_ROM)
    {
        if (!SHOW_IN_INT_DIRECT)                    // Есил покажывается не только непосредственный сигнал
        {
            Data_GetFromIntMemory();                // то читаем сохранённый из ППЗУ
        }

        if (SHOW_IN_INT_DIRECT || SHOW_IN_INT_BOTH) // И, если нужно
        {
            GetDataFromStorage();                   // из хранилища
        }
    }
    
    Data_PrepareToUse(MODE_WORK);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Clear(void)
{
    pDS = pDSDir = pDSRAM = pDSROM = 0;
    dataChan[A] = dataChan[B] = 0;
    dataRAMA = dataRAMB = 0;
    dataROMA = dataROMB = 0;
    dataDirA = dataDirB = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_PrepareToUse(ModeWork mode)
{
    currentModeWork = mode;
    
    typedef DataSettings* pDataSettings;
    static const pDataSettings *ds[3] = {&pDSDir, &pDSRAM, &pDSROM};

    typedef uint8* pUINT8;
    static const pUINT8 *dA[3] = {&dataDirA, &dataRAMA, &dataROMA};
    static const pUINT8 *dB[3] = {&dataDirB, &dataRAMB, &dataROMB};
    
    DS = *ds[mode];
    if(mode == ModeWork_ROM)
    {
        DS = pDSROM ? &dataSettingsROM : 0;
    }
    DATA_A = *dA[mode];
    DATA_B = *dB[mode];

    int first = 0;
    int last = 0;
    sDisplay_PointsOnDisplay(&first, &last);

    Processing_SetSignal(DATA_A, DATA_B, DS, first, last);
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
            Data_PrepareToUse(ModeWork_ROM);        // то его и рисуем
        }
        else                                        // А если на экране есть и другой сигнал
        {
            if (EXIT_FROM_ROM_TO_RAM)               // Если сигнал из ОЗУ
            {
                Data_PrepareToUse(ModeWork_RAM);    
            }
            else                                    // Если непосредственный сигнал
            {
                Data_PrepareToUse(ModeWork_Dir);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
ModeWork Data_GetUsedModeWork(void)
{
    return currentModeWork;
}


/** @}  @}
 */
