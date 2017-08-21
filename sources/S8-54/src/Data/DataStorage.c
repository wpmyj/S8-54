// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "DataSettings.h"
#include "DataStorage.h"
#include "Log.h"
#include "FPGA/FPGAtypes.h"
#include "FPGA/FPGA.h"
#include "Hardware/Timer.h"
#include "Hardware/RTC.h"
#include "Hardware/FSMC.h"
#include "Hardware/RAM.h"
#include "Menu/Pages/PageMemory.h"
#include "Settings/Settings.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/Debug.h"
#include "Utils/GlobalFunctions.h"
#include <string.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static void ReplaceLastFrame(DataSettings *ds, uint8 *dataA, uint8 *dataB);

static int SIZE_POOL = 0;

static uint *sumA_RAM = 0;        // Сумма первого канала
static uint *sumB_RAM = 0;

static uint8 *limitUpA_RAM = 0;
static uint8 *limitUpB_RAM = 0;
static uint8 *limitDownA_RAM = 0;
static uint8 *limitDownB_RAM = 0;

static int iFirst = 0;          // Номер в массиве datas первого сохранённого сигнала
static int iLast = 0;           // Номер в массиве datas последнего сохранённого сигнала

static float *aveDataA_RAM = 0; // В этих массивах хранятся усреднённые значения, подсчитанные по приблизительному алгоритму.
static float *aveDataB_RAM = 0;

static bool newSumCalculated[NumChannels] = {true, true};   // Если true, то новые суммы рассчитаны, и нужно повторить расчёт среднего
static int numElementsInStorage = 0;


// Для поточечного режима фрейм бегущего кадра
static uint8 *frameP2P = 0;
static int numPointsP2P = 0;            // Когда в последнем фрейме хранятся точки для поточечного вывода, то здесь хранится количество точек
static DataSettings dsP2P = {0};        // Настройки поточечного режима    
static bool inFrameP2Pmode = false;     // Если true - сейчас поточечный режим

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClearLimitsAndSums(void)
{
    int numBytesA = RequestBytesForChannel(A, 0);
    int numBytesB = RequestBytesForChannel(B, 0);

    RAM_MemClear(limitUpA_RAM, numBytesA);
    RAM_MemClear(limitUpB_RAM, numBytesB);
    RAM_MemClear(limitDownA_RAM, numBytesA);
    RAM_MemClear(limitDownB_RAM, numBytesB);
    RAM_MemClear(sumA_RAM, numBytesA * 4); //-V112
    RAM_MemClear(sumB_RAM, numBytesB * 4); //-V112
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void DS_Clear(void)
{
    SIZE_POOL = RAM(DS_POOL_END) - RAM(DS_POOL_BEGIN);

    sumA_RAM = (uint *)RAM(DS_SUM_A); //-V206
    sumB_RAM = (uint *)RAM(DS_SUM_B); //-V206

    limitUpA_RAM = RAM(DS_LIMIT_UP_A);
    limitUpB_RAM = RAM(DS_LIMIT_UP_B);
    limitDownA_RAM = RAM(DS_LIMIT_DOWN_A);
    limitDownB_RAM = RAM(DS_LIMIT_DOWN_B);
    frameP2P = RAM(DS_P2P_FRAME);

    aveDataA_RAM = (float*)RAM(DS_AVE_DATA_A);
    aveDataB_RAM = (float*)RAM(DS_AVE_DATA_B);

    iFirst = 0;
    iLast = 0;
    ADDRESS_DATA(&gDatas[iFirst]) = ADDRESS_DATA(&gDatas[iLast]) = 0;

    for(int i = 0; i < NUM_DATAS; i++)
    {
        ADDRESS_DATA(&gDatas[i]) = 0;  // Пишем признак того, что ячейка свободна
    }

    numElementsInStorage = 0;

    ClearLimitsAndSums();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void CalculateAroundAverage(uint8 *dataA, uint8 *dataB, DataSettings *dss)
{
    if (!dataA && !dataB)
    {
        return;
    }

    int numAveData = DS_NumElementsWithCurrentSettings();

    int size = BYTES_IN_CHANNEL(dss);

    if (numAveData <= 1)
    {
        for (int i = 0; i < size; i++)
        {
            aveDataA_RAM[i] = dataA[i]; //-V108
            aveDataB_RAM[i] = dataB[i]; //-V108
        }
    }
    else
    {
        if (numAveData > sDisplay_NumAverage())
        {
            numAveData = sDisplay_NumAverage();
        }

        float numAveDataF = (float)numAveData;
        float numAveDataFless = numAveDataF - 1.0f;
        float numAveDataInv = 1.0f / numAveDataF;
        float *aDataA = &aveDataA_RAM[0];
        float *aDataB = &aveDataB_RAM[0];
        uint8 *dA = &dataA[0];
        uint8 *dB = &dataB[0];
        float *endData = &aveDataA_RAM[size]; //-V108

        do 
        {
            *aDataA = ((*aDataA) * numAveDataFless + (float)(*dA++)) * numAveDataInv;
            aDataA++;
            *aDataB = ((*aDataB) * numAveDataFless + (float)(*dB++)) * numAveDataInv;
            aDataB++;
        } while (aDataA != endData);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Возвращает количество байт, требуемых для записи данных с настройками ds
static int SizeData(DataSettings *ds)
{
    /// \todo должна быть глобальная функция для расчёта размера данных. Какжется, где-то она есть в функциях DataSettings.

    int size = 0;

    int numBytesInChannel = BYTES_IN_CHANNEL(ds);

    if(ENABLED_A(ds))
    {
        size += numBytesInChannel;
        if(PEAKDET(ds) != PeakDet_Disable)
        {
            size += numBytesInChannel;
        }
    }

    if(ENABLED_B(ds))
    {
        size += numBytesInChannel;
        if(PEAKDET(ds) != PeakDet_Disable)
        {
            size += numBytesInChannel;
        }
    }
    return size;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DeleteFirst(void)
{
    ADDRESS_DATA(&gDatas[iFirst]) = 0;
    iFirst++;
    if(iFirst == NUM_DATAS)
    {
        iFirst = 0;
    }
    numElementsInStorage--;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Находит место для записи данных во внешнее ОЗУ.
// По выходу из функции элемент datas[iLast] содержит ds, a ds содержит адрес для записи во внешнее ОЗУ
// При этом функция сама модернизируе iFirst, iLast, addrData элементов datas (0 указывает на то, что элемент свободен)
static void PrepareLastElemForWrite(DataSettings *ds)
{
    // Если хранилище пустое
    if(ADDRESS_DATA(&gDatas[iFirst]) == 0)
    {
        iFirst = iLast = 0;
        ADDRESS_DATA(ds) = RAM(DS_POOL_BEGIN);
        gDatas[iFirst] = *ds;
        return;
    }

    // Если в хранилище один элемент
    if(iFirst == iLast)
    {
        iLast = iFirst + 1;
        ADDRESS_DATA(ds) = ADDRESS_DATA(&gDatas[iFirst]) + SizeData(&gDatas[iFirst]);
        gDatas[iLast] = *ds;
        return;
    }

    // Если в хранилище максимально возможное число эелементов
    if(numElementsInStorage >= NUM_DATAS)
    {
        DeleteFirst();
    }

    uint8 *addrWrite = 0;

    int size = SizeData(ds);

    // Сначала найдём свободное место в ОЗУ

    volatile bool run = true;
    while(run)
    {
        uint8 *addrFirst = ADDRESS_DATA(&gDatas[iFirst]);
        uint8 *addrLast = ADDRESS_DATA(&gDatas[iLast]);

        if(addrLast > addrFirst)                                                   // Данные в памяти сохранены в порядке возрастания
        {
            int memFree = RAM(DS_POOL_END) - addrLast - SizeData(&gDatas[iLast]);     // Столько памяти осталось за последним элементом

            if(memFree >= size)                                                    // Памяти за последним элементом достаточно
            {
                addrWrite = addrLast + SizeData(&gDatas[iLast]);
                break;
            }
            else                                                                    // Памяти за последним элементом не хватает.
            {
                if(addrFirst - RAM(DS_POOL_BEGIN) < size)                       // Если в начале меньше памяти, чем необходимо
                {
                    DeleteFirst();                                              // Удаляем один элемент с начала
                }
                else
                {
                    addrWrite = RAM(DS_POOL_BEGIN);
                    break;
                }
            }
        }
        else
        {
            int memFree = addrFirst - addrLast - SizeData(&gDatas[iFirst]);

            if(memFree >= size)
            {
                addrWrite = addrLast + SizeData(&gDatas[iLast]);
                break;
            }
            else
            {
                if(addrFirst - addrLast - SizeData(&gDatas[iLast]) < size)
                {
                    DeleteFirst();
                }
                else
                {
                    addrWrite = ADDRESS_DATA(&gDatas[iLast]) + SizeData(&gDatas[iLast]);
                    break;
                }
            }
        }
    }

    // Теперь найдём последний элемент
    iLast++;
    if(iLast == NUM_DATAS)
    {
        iLast = 0;
    }
    ADDRESS_DATA(ds) = addrWrite;
    gDatas[iLast] = *ds;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PushData(DataSettings *ds, uint8 *dataA, uint8 *dataB)
{
    PrepareLastElemForWrite(ds);

    int numBytes = BYTES_IN_CHANNEL(ds);

    if(dataA)
    {
        if (ENABLED_A(ds))
        {
            RAM_MemCpy16(dataA, AddressChannel(ds, A), numBytes);
        }
    }
    else
    {
        RAM_MemSet_Sinch(AddressChannel(ds, A), NONE_VALUE, numBytes);  // Для режима поточечного вывода - заполняем одним значением
    }

    if(dataB)
    {
        if (ENABLED_B(ds))
        {
            RAM_MemCpy16(dataB, AddressChannel(ds, B), numBytes);
        }
    }
    else
    {
        RAM_MemSet_Sinch(AddressChannel(ds, B), NONE_VALUE, numBytes);  // Для режима поточечного вывода - заполянем одним значением
    }

    numElementsInStorage++;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
/*
static void ReplaceLastFrame(DataSettings *ds, uint8 *dataA, uint8 *dataB)
{
    DataSettings *lastDS = DS_DataSettingsFromEnd(0);
    TIME_TIME(lastDS) = TIME_TIME(ds);    // Нужно скопировать время, потому что во фрейме последних точек оно нулевое.

    int numBytes = BYTES_IN_CHANNEL(ds);

    FSMC_SET_MODE(ModeFSMC_RAM);

    if (ENABLED_A(ds))
    {
        RAM_MemCpy16(dataA, AddressChannel(lastDS, A), numBytes);
    }

    if (ENABLED_B(ds))
    {
        RAM_MemCpy16(dataB, AddressChannel(lastDS, B), numBytes);
    }

    FSMC_RESTORE_MODE();
}
*/


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void BeginLimits(uint8 *dataA, uint8 *dataB, int numElements)
{
    uint16 *datA = (uint16 *)dataA;
    uint16 *datB = (uint16 *)dataB;

    uint16 *limitUpA = (uint16 *)limitUpA_RAM;
    uint16 *limitDownA = (uint16 *)limitDownA_RAM;

    uint16 *limitUpB = (uint16 *)limitUpB_RAM;
    uint16 *limitDownB = (uint16 *)limitDownB_RAM;

    for(int i = 0; i < numElements / 2; i++)
    {
        limitUpA[i] = limitDownA[i] = datA[i]; //-V108
        limitUpB[i] = limitDownB[i] = datB[i]; //-V108
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
DataSettings* DS_DataSettingsFromEnd(int indexFromEnd)
{
    if (inFrameP2Pmode)
    {
        return &dsP2P;
    }

    int index = 0;

    if(indexFromEnd <= iLast)
    {
        index = iLast - indexFromEnd;
    }
    else
    {
        indexFromEnd -= iLast;
        index = NUM_DATAS - indexFromEnd;
    }

    return &gDatas[index];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
DataSettings* GetSettingsDataFromEnd(int fromEnd)
{
    return DS_DataSettingsFromEnd(fromEnd);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void CalculateLimits(uint8 *dataA, uint8 *dataB, DataSettings *dss)
{
    int numElements = BYTES_IN_CHANNEL(dss);

    if(DS_NumElementsInStorage() == 0 || NUM_MIN_MAX == 1 || (!DataSettings_IsEquals(dss, GetSettingsDataFromEnd(0))))
    {
        BeginLimits(dataA, dataB, numElements);
    }
    else
    {
        int allDatas = DS_NumElementsWithSameSettings();
        LIMITATION(allDatas, allDatas, 1, NUM_MIN_MAX);

        if(DS_NumElementsWithSameSettings() >= NUM_MIN_MAX)
        {
            BeginLimits(dataA, dataB, numElements);
            allDatas--;
        }

        for(int numData = 0; numData < allDatas; numData++)
        {
            const uint8 *dataA = DS_GetData_RAM(A, numData);
            const uint8 *dataB = DS_GetData_RAM(B, numData);

#define SET_LIMIT(d, up, down)          \
    data = RAM_ReadByte(d + i);         \
    limitUp = RAM_ReadByte(up + i);     \
    limitDown = RAM_ReadByte(down + i); \
    if(data > limitUp)                  \
    {                                   \
        RAM_WriteByte(up + i, data);    \
    }                                   \
    if(data < limitDown)                \
    {                                   \
        RAM_WriteByte(down + i, data);  \
    }
        uint8 data = 0;
        uint8 limitUp = 0;
        uint8 limitDown = 0;

            for(int i = 0; i < numElements; i++)
            {
                SET_LIMIT((uint8 *)dataA, limitUpA_RAM, limitDownA_RAM);
                SET_LIMIT((uint8 *)dataB, limitUpB_RAM, limitDownB_RAM);
            }
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void CalculateSums(void)
{
    DataSettings *ds = 0;
    uint16 *dataA = 0;
    uint16 *dataB = 0;

    DS_GetDataFromEnd_RAM(0, &ds, &dataA, &dataB);

    int numPoints = BYTES_IN_CHANNEL(ds);

    int numAveragings = sDisplay_NumAverage();

    uint16 *sumA16 = (uint16 *)sumA_RAM;
    uint16 *sumB16 = (uint16 *)sumB_RAM;

    int iMax = numPoints / 2;

    for(int i = 0; i < iMax; i++)
    {
        uint16 data16 = dataA[i]; //-V108
        *sumA16 = (uint8)data16;
        sumA16 += 2;
        *sumA16 = (uint8)(data16 >> 8);
        sumA16 += 2;

        data16 = dataB[i]; //-V108
        *sumB16 = (uint8)data16;
        sumB16 += 2;
        *sumB16 = (uint8)(data16 >> 8);
        sumB16 += 2;
    }

    if(numAveragings > 1)
    {
        int numSameSettins = DS_NumElementsWithSameSettings();
        if(numSameSettins < numAveragings)
        {
            numAveragings = numSameSettins;
        }

        for(int i = 1; i < numAveragings; i++)
        {
            DS_GetDataFromEnd_RAM(i, &ds, &dataA, &dataB);

            sumA16 = (uint16 *)sumA_RAM;
            uint16 *dA = dataA;

            sumB16 = (uint16 *)sumB_RAM;
            uint16 *dB = dataB;

/* Прибавляем к младшим 16 разрядам суммы значение сигнала */
/* И записываем сумму на место 16 младших разрядов суммы */
/* Если значение выходит за 16 разрядов */
/* То производим полное вычисление суммы */
#define ALTERNATE_ADD(addr, shift)                                                                          \
    loSum = (*addr) + (uint8)(data16 >> shift);                                                             \
    *addr = (uint16)loSum;                                                                                  \
    if (loSum > 0xffff)                                                                                     \
    {                                                                                                       \
        sum = (uint)((int)loSum - (int)((uint8)(data16 >> shift))) + ((*(addr + 1)) >> 16) + (uint8)data16; \
        *addr = (uint16)sum;                                                                                \
        *(addr + 1) = (uint16)(sum >> 16);                                                                  \
    }                                                                                                       \
    addr += 2;

            for(int i = 0; i < iMax; i++)
            {
                uint sum = 0;
                uint loSum = 0;
                uint16 data16 = *dA++;     // Считываем первые два отсчёта данных

                ALTERNATE_ADD(sumA16, 0);  /** \todo Похоже, sum неправильно вычисляется, из-за чего артефаты при больших накоплениях. Нужно 
                                           (loSum - data16) заменить на старое значение (*data16) */

                ALTERNATE_ADD(sumA16, 8);

                data16 = *dB++;

                ALTERNATE_ADD(sumB16, 0);

                ALTERNATE_ADD(sumB16, 8);
            }
        }
    }
    newSumCalculated[A] = newSumCalculated[B] = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DS_AddData(uint8 *dataA, uint8 *dataB, DataSettings dss)
{
    if (!ENABLED_A(&dss) && !ENABLED_B(&dss))
    {
        return;
    }

    inFrameP2Pmode = false;

    TIME_TIME(&dss) = RTC_GetPackedTime();

    CalculateLimits(dataA, dataB, &dss);

    PushData(&dss, dataA, dataB);

    CalculateSums();

    CalculateAroundAverage(dataA, dataB, &dss);

    LOG_WRITE("num elements in storage %d", numElementsInStorage);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Возвращает true, если настройки измерений с индексами elemFromEnd0 и elemFromEnd1 совпадают, и false в ином случае.
static bool SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1)
{
    DataSettings* dp0 = DS_DataSettingsFromEnd(elemFromEnd0);
    DataSettings* dp1 = DS_DataSettingsFromEnd(elemFromEnd1);
    return DataSettings_IsEquals(dp0, dp1);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int DS_NumElementsWithSameSettings(void)
{
    int retValue = 0;
    for(retValue = 1; retValue < numElementsInStorage; retValue++)
    {
        if(!SettingsIsIdentical(retValue, retValue - 1))
        {
            break;
        }
    }
    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int DS_NumElementsWithCurrentSettings(void)
{
    DataSettings dp;
    DataSettings_Fill(&dp);
    int retValue = 0;
    int numElements = DS_NumElementsInStorage();

    for(retValue = 0; retValue < numElements; retValue++)
    {
        if(!DataSettings_IsEquals(&dp, DS_DataSettingsFromEnd(retValue)))
        {
            break;
        }
    }

    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int DS_NumElementsInStorage(void)
{
    return numElementsInStorage;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Копирует данные канала ch из, определяемые ds, в одну из двух строк массива dataImportRel. Возвращаемое значение false означает, что данный канал 
// выключен.
static bool CopyData(DataSettings *ds, Channel ch, uint8 *dataImportRel)
{
    if((ch == A && !ENABLED_A(ds)) || (ch == B && !ENABLED_B(ds)))
    {
        return false;
    }

    uint8 *address = ADDRESS_DATA(ds);

    int length = BYTES_IN_CHANNEL(ds);

    if(ch == B && ENABLED_B(ds) && ENABLED_A(ds))
    {
        address += length; //-V102
    }

    RAM_MemCpy16(address, dataImportRel, length);

    return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *DS_GetData_RAM(Channel ch, int fromEnd)
{
    uint8 *dataImport = (ch == A) ? RAM(DS_DATA_IMPORT_REL_A) : RAM(DS_DATA_IMPORT_REL_B);

    DataSettings* dp = DS_DataSettingsFromEnd(fromEnd);

    if (dp == 0)
    {
        return 0;
    }

    if(CopyData(dp, ch, dataImport))
    {
        return dataImport;
    }

    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool DS_GetDataFromEnd(int fromEnd, DataSettings *ds, uint8 *dataA, uint8 *dataB)
{
    DataSettings *dataSettings = 0;
    uint16 *dA = 0;
    uint16 *dB = 0;
    
    if(DS_GetDataFromEnd_RAM(fromEnd, &dataSettings, &dA, &dB))
    {
        memcpy(ds, dataSettings, sizeof(DataSettings));
        if(dA)
        {
            RAM_MemCpy16(dA, dataA, BYTES_IN_CHANNEL(ds));
        }
        if(dB)
        {
            RAM_MemCpy16(dB, dataB, BYTES_IN_CHANNEL(ds));
        }

        return true;
    }

    return false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool DS_GetDataFromEnd_RAM(int fromEnd, DataSettings **ds, uint16 **dataA, uint16 **dataB)
{
    DataSettings *dp = DS_DataSettingsFromEnd(fromEnd);

    if(dp == 0)
    {
        return false;
    }
    
    uint8 *dataImportRelA = RAM(DS_DATA_IMPORT_REL_A);
    uint8 *dataImportRelB = RAM(DS_DATA_IMPORT_REL_B);

    if (inFrameP2Pmode)
    {
        dataImportRelA = RAM(DS_P2P_FRAME);
        dataImportRelB = dataImportRelA + BYTES_IN_CHANNEL(dp);
    }

    if(dataA != 0)
    {
        *dataA = CopyData(dp, A, dataImportRelA) ? (uint16 *)dataImportRelA : 0;
    }
    if(dataB != 0)
    {
        *dataB = CopyData(dp, B, dataImportRelB) ? (uint16 *)dataImportRelB : 0;
    }

    *ds = dp;

    return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *DS_GetAverageData(Channel ch)
{
    if (newSumCalculated[ch] == false)
    {
        return &gDataAve[ch][0];
    }

    newSumCalculated[ch] = false;

    DataSettings *ds = 0;
    uint16 *dA = 0;
    uint16 *dB = 0;
    DS_GetDataFromEnd_RAM(0, &ds, &dA, &dB);

    if (ds == 0)
    {
        return 0;
    }

    int numPoints = BYTES_IN_CHANNEL(ds);

    if (sDisplay_GetModeAveraging() == Averaging_Around)
    {
        float *floatAveData = (ch == A) ? aveDataA_RAM : aveDataB_RAM;
        
        for (int i = 0; i < numPoints; i++)
        {
            gDataAve[ch][i] = (uint8)(floatAveData[i] + 0.5f); //-V108
        }
        return &gDataAve[ch][0];
    }

    int numAveraging = sDisplay_NumAverage();

    LIMIT_ABOVE(numAveraging, DS_NumElementsWithSameSettings());

    for(int i = 0; i < numPoints; i++)
    {
        gDataAve[A][i] = (uint8)((float)RAM_ReadWord(sumA_RAM + i) / numAveraging + 0.5f);
        gDataAve[B][i] = (uint8)((float)RAM_ReadWord(sumB_RAM + i) / numAveraging + 0.5f);
    }

    return &gDataAve[ch][0];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *DS_GetLimitation(Channel ch, int direction)
{
    if(direction == 0)
    {
        uint8 *limit = A == ch ? limitDownA_RAM : limitDownB_RAM;
        return &(limit[0]);
    }
    else
    {
        uint8 *limit = A == ch ? limitUpA_RAM : limitUpB_RAM;
        return &(limit[0]);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int DS_NumberAvailableEntries(void)
{
    if(ADDRESS_DATA(&gDatas[iFirst]) == 0)
    {
        return 0;
    }

    int numElems = 0;
    LIMITATION_ABOVE(numElems, SIZE_POOL / SizeData(&gDatas[iLast]), NUM_DATAS);

    return numElems;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void DS_NewFrameP2P(DataSettings *dss)
{
    if (!ENABLED_A(dss) && !ENABLED_B(dss))
    {
        return;
    }

    inFrameP2Pmode = true;
    dsP2P = *dss;
    dsP2P.addr = RAM(DS_P2P_FRAME);
    RAM_MemClear(frameP2P, 2 * BYTES_IN_CHANNEL(dss));
    numPointsP2P = 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void DS_AddPointsP2P(uint16 dataA, uint16 dataB)
{
    if (!ENABLED_A(&dsP2P) && !ENABLED_B(&dsP2P))
    {
        return;
    }

    FSMC_SET_MODE(ModeFSMC_RAM);

    int length = BYTES_IN_CHANNEL(&dsP2P);

    if (numPointsP2P >= length)                         // Если место во фрейме заполнено полностью
    {
        if (ENABLED_A(&dsP2P))                           // То сдвинем все точки во фрейме влево
        {
            RAM_MemShiftLeft(frameP2P, length - 2, 2);
        }
        if (ENABLED_B(&dsP2P))
        {
            RAM_MemShiftLeft(frameP2P + length, length - 2, 2);
        }
    }

    int offsetWrite = (numPointsP2P >= length) ? length - 2 : numPointsP2P;

    if (ENABLED_A(&dsP2P))
    {
        uint16 *address = (uint16*)(frameP2P + offsetWrite);
        *address = dataA;
    }

    if (ENABLED_B(&dsP2P))
    {
        uint16 *address = (uint16 *)(frameP2P + length + offsetWrite);
        *address = dataB;
    }

    numPointsP2P += 2;

    FSMC_RESTORE_MODE();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int DS_NumPointsInLastFrameP2P(void)
{
    return numPointsP2P;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int DS_GetLastFrameP2P_RAM(DataSettings **ds, uint8 **dataA, uint8 **dataB)
{
    if (!inFrameP2Pmode)
    {
        *ds = 0;
        *dataA = 0;
        *dataB = 0;
        return -1;
    }

    *ds = &dsP2P;
    *dataA = frameP2P;
    *dataB = frameP2P + BYTES_IN_CHANNEL(&dsP2P);

    return numPointsP2P;
}
