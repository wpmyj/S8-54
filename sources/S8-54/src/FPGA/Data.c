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
static bool readedRAM = false;      ///< Если true, то в inA(B) хранятся данные, считанные из ОЗУ.
static bool fromEndRAM = 0;         ///< Номер текущего сигнала из ОЗУ, если readedRAM == true.

static bool readedROM = false;      ///< Если true, то в inA(B) хранятся данные, считанные из ППЗУ.
static bool numFromROM = 0;         ///< Номер считанного из ППЗУ сигнала, если readedROM == true.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Считать данные 
static void GetDataFromStorage(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_Clear(void)
{
    pDS = 0;
    readedRAM = readedROM = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_ReadDataRAM(int fromEnd)
{
    if(readedRAM && fromEnd == fromEndRAM)
    {
        return;
    }

    Data_Clear();

    if(DS_GetDataFromEnd(fromEnd, &dataSettings, inA, inB))
    {
        if (ENUM_POINTS(&dataSettings) == FPGA_ENUM_POINTS) /** \todo Это временно. Нужно сделать пересчёт к установленной длине памяти в 
        {                                                       в ProcessingSignal_SetData(), чтобы не мелькало на экране. */
            readedRAM = true;
            fromEndRAM = fromEnd;
            pDS = &dataSettings;

            Processing_SetData();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_ReadDataROM(void)
{
    if(readedROM && numFromROM == NUM_ROM_SIGNAL)
    {
        return;
    }

    Data_Clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_GetAverageFromDataStorage(void)
{
    if(DS)
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
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_Load(void)
{
    Data_Clear();

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
//        DS_GetDataFromEnd_RAM(NUM_RAM_SIGNAL, &pDSRAM, (uint16**)&dataRAMA, (uint16**)&dataRAMB);
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
void Data_PrepareToUse(ModeWork mode)
{ 
    /*
    typedef DataSettings* pDataSettings;
    static const pDataSettings *ds[3] = {&pDSDir, &pDSRAM, &pDSROM};
   
    DS = *ds[mode];
    if(mode == ModeWork_ROM)
    {
        DS = pDSROM ? &dataSettingsROM : 0;
    }
    */

    //Processing_SetData();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void GetDataFromStorage(void)
{
    /*
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
    */
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_GetFromIntMemory(void)
{
    /*
    if(FLASH_GetData(NUM_ROM_SIGNAL, &pDSROM, &dataROMA, &dataROMB))
    {
        memcpy(&dataSettingsROM, (void*)pDSROM, sizeof(DataSettings));
    }
    */
}


/** @}  @}
 */
