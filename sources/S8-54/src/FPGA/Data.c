// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _INCLUDE_DATA_
#include "Data.h"
#undef _INCLUDE_DATA_
#include "DataBuffer.h"
#include "Globals.h"
#include "Log.h"
#include "Hardware/FLASH.h"
#include "Hardware/FSMC.h"
#include "Settings/SettingsMemory.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/ProcessingSignal.h"


/** @addtogroup FPGA
 *  @{
 *  @addtogroup Data
 *  @{
 */


/// Заполняет структуру dataStruct данными для отрисовки
static void PrepareDataForDraw(DataStruct *dataStruct);


static DataSettings dataSettings;   ///< Здесь хранятся настройки для текущего рисуемого сигнала


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_Clear(void)
{
    pDS = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_ReadFromRAM(int fromEnd, DataStruct *dataStruct)
{
    Data_Clear();

    bool readed = false;                // Признак того, что данные считаны

    if (IN_AVERAGING_MODE               // Если включено усреднение
        && fromEnd == 0)                // И запрашиваем псоледний считанный сигнал
    {
        dataSettings = *DS_DataSettingsFromEnd(0);
        pDS = &dataSettings;
        if (ENABLED_DS_A)
        {
            memcpy(inA, DS_GetAverageData(A), BYTES_IN_CHANNEL(DS));
        }
        if (ENABLED_DS_B)
        {
            memcpy(inB, DS_GetAverageData(B), BYTES_IN_CHANNEL(DS));
        }
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

            PrepareDataForDraw(dataStruct);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_ReadFromROM(DataStruct *dataStruct)
{
    Data_Clear();

    if (FLASH_GetData(NUM_ROM_SIGNAL, &dataSettings, inA, inB))
    {
        pDS = &dataSettings;

        Processing_SetData();

        PrepareDataForDraw(dataStruct);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
#define CYCLE(in, out, num)         \
    uint8 *dest = in;               \
    uint8 *src = out;               \
    for(int i = 0; i < num; i++)    \
    {                               \
        *dest++ = *src++;           \
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PrepareDataForDraw(DataStruct *dataStruct)
{
    if (!dataStruct)
    {
        return;
    }

    int pointFirst = 0;
    int pointLast = 0;

    sDisplay_PointsOnDisplay(&pointFirst, &pointLast);

    dataStruct->needDraw[A] = ENABLED_DS_A && SET_ENABLED_A;
    dataStruct->needDraw[B] = ENABLED_DS_B && SET_ENABLED_B;

    int numBytes = 281;
    int firstByte = pointFirst;

    if (PEACKDET_DS)
    {
        numBytes *= 2;
        firstByte *= 2;
    }

    if (dataStruct->needDraw[A])
    {
        CYCLE(dataStruct->data[A], &outA[firstByte], numBytes);
    }

    if (dataStruct->needDraw[B])
    {
        CYCLE(dataStruct->data[B], &outB[firstByte], numBytes);
    }
}


/** @}  @}
 */
