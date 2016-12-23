#include "DataStorage.h"
#include "Log.h"
#include "FPGA/FPGA_Types.h"
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

static int numPointsP2P = 0;                                // Когда в последнем фрейме хранятся точки для поточечного вывода, то здесь хранится количество точек


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClearLimitsAndSums(void)
{
    int numBytesA = RequestBytesForChannel(A, 0);
    int numBytesB = RequestBytesForChannel(B, 0);

    RAM_MemClear(limitUpA_RAM, numBytesA);
    RAM_MemClear(limitUpB_RAM, numBytesB);
    RAM_MemClear(limitDownA_RAM, numBytesA);
    RAM_MemClear(limitDownB_RAM, numBytesB);
    RAM_MemClear(sumA_RAM, numBytesA * 4);
    RAM_MemClear(sumB_RAM, numBytesB * 4);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DS_Clear(void)
{
    SIZE_POOL = RAM(DS_POOL_END) - RAM(DS_POOL_BEGIN);

    sumA_RAM = (uint*)RAM(DS_SUM_A);
    sumB_RAM = (uint*)RAM(DS_SUM_B);

    limitUpA_RAM = RAM(DS_LIMIT_UP_A);
    limitUpB_RAM = RAM(DS_LIMIT_UP_B);
    limitDownA_RAM = RAM(DS_LIMIT_DOWN_A);
    limitDownB_RAM = RAM(DS_LIMIT_DOWN_B);

    aveDataA_RAM = (float*)RAM(DS_AVE_DATA_A);
    aveDataB_RAM = (float*)RAM(DS_AVE_DATA_B);

    iFirst = 0;
    iLast = 0;
    gDatas[iFirst].addrData = gDatas[iLast].addrData = 0;

    for(int i = 0; i < NUM_DATAS; i++)
    {
        gDatas[i].addrData = 0;  // Пишем признак того, что ячейка свободна
    }

    numElementsInStorage = 0;

    ClearLimitsAndSums();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void CalculateAroundAverage(uint8 *dataA, uint8 *dataB, DataSettings *dss)
{
    int numAveData = DS_NumElementsWithCurrentSettings();

    int size = NumBytesInChannel(dss);

    if (numAveData == 1)
    {
        for (int i = 0; i < size; i++)
        {
            aveDataA_RAM[i] = dataA[i];
            aveDataB_RAM[i] = dataB[i];
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
        float* aDataA = &aveDataA_RAM[0];
        float* aDataB = &aveDataB_RAM[0];
        uint8* dA = &dataA[0];
        uint8* dB = &dataB[0];
        float* endData = &aveDataA_RAM[size];

        do 
        {
            *aDataA++ = ((*aDataA) * numAveDataFless + (float)(*dA++)) * numAveDataInv;
            *aDataB++ = ((*aDataB) * numAveDataFless + (float)(*dB++)) * numAveDataInv;
        } while (aDataA != endData);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Возвращает количество байт, требуемых для записи данных с настройками ds
static int SizeData(DataSettings *ds)
{
    int size = 0;

    int numBytesInChannel = NumBytesInChannel(ds);

    if(ds->enableChA == 1)
    {
        size += numBytesInChannel;
        if(ds->peackDet != PeackDet_Disable)
        {
            size += numBytesInChannel;
        }
    }

    if(ds->enableChB == 1)
    {
        size += numBytesInChannel;
        if(ds->peackDet != PeackDet_Disable)
        {
            size += numBytesInChannel;
        }
    }
    return size;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DeleteFirst(void)
{
    gDatas[iFirst].addrData = 0;
    iFirst++;
    if(iFirst == NUM_DATAS)
    {
        iFirst = 0;
    }
    numElementsInStorage--;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Находит место для записи данных во внешнее ОЗУ.
// По выходу из функции элемент datas[iLast] содержит ds, a ds содержит адрес для записи во внешнее ОЗУ
// При этом функция сама модернизируе iFirst, iLast, addrData элементов datas (0 указывает на то, что элемент свободен)
static void PrepareLastElemForWrite(DataSettings *ds)
{
    // Если хранилище пустое
    if(gDatas[iFirst].addrData == 0)
    {
        iFirst = iLast = 0;
        ds->addrData = RAM(DS_POOL_BEGIN);
        gDatas[iFirst] = *ds;
        return;
    }

    // Если в хранилище один элемент
    if(iFirst == iLast)
    {
        iLast = iFirst + 1;
        ds->addrData = gDatas[iFirst].addrData + SizeData(&gDatas[iFirst]);
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

    bool run = true;
    while(run)
    {
        uint8 *addrFirst = gDatas[iFirst].addrData;
        uint8 *addrLast = gDatas[iLast].addrData;

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
                    addrWrite = gDatas[iLast].addrData + SizeData(&gDatas[iLast]);
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
    ds->addrData = addrWrite;
    gDatas[iLast] = *ds;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void PushData(DataSettings *ds, uint8 *dataA, uint8 *dataB)
{
    PrepareLastElemForWrite(ds);

    int numPoints = NumBytesInChannel(ds);

    if(dataA)
    {
        if (ds->enableChA)
        {
            RAM_MemCpy16(dataA, AddressChannel(ds, A), numPoints);
        }
    }
    else
    {
        RAM_MemSet_Sinch(AddressChannel(ds, A), AVE_VALUE, numPoints);  // Для режима поточечного вывода - заполняем одним значением
    }

    if(dataB)
    {
        if (ds->enableChB)
        {
            RAM_MemCpy16(dataB, AddressChannel(ds, B), numPoints);
        }
    }
    else
    {
        RAM_MemSet_Sinch(AddressChannel(ds, B), AVE_VALUE, numPoints);  // Для режима поточечного вывода - заполянем одним значением
    }

    numElementsInStorage++;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void ReplaceLastFrame(DataSettings *ds, uint8 *dataA, uint8 *dataB)
{
    DataSettings *lastDS = DS_DataSettingsFromEnd(0);
    lastDS->time = ds->time;    // Нужно скопировать время, потому что во фрейме последних точек оно нулевое.

    int numBytes = NumBytesInChannel(ds);

    FSMC_SET_MODE(ModeFSMC_RAM);

    if (ds->enableChA)
    {
        RAM_MemCpy16(dataA, AddressChannel(lastDS, A), numBytes);
    }

    if (ds->enableChB)
    {
        RAM_MemCpy16(dataB, AddressChannel(lastDS, B), numBytes);
    }

    FSMC_RESTORE_MODE();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void BeginLimits(uint8 *dataA, uint8 *dataB, int numElements)
{
    uint16* datA = (uint16*)dataA;
    uint16* datB = (uint16*)dataB;

    uint16* limitUpA = (uint16*)limitUpA_RAM;
    uint16* limitDownA = (uint16*)limitDownA_RAM;

    uint16* limitUpB = (uint16*)limitUpB_RAM;
    uint16* limitDownB = (uint16*)limitDownB_RAM;

    for(int i = 0; i < numElements / 2; i++)
    {
        limitUpA[i] = limitDownA[i] = datA[i];
        limitUpB[i] = limitDownB[i] = datB[i];
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
DataSettings* DS_DataSettingsFromEnd(int indexFromEnd)
{
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
DataSettings* GetSettingsDataFromEnd(int fromEnd)
{
    return DS_DataSettingsFromEnd(fromEnd);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void CalculateLimits(uint8 *dataA, uint8 *dataB, DataSettings *dss)
{
    int numElements = NumBytesInChannel(dss);

    if(DS_NumElementsInStorage() == 0 || DISPLAY_NUM_MIN_MAX == 1 || (!DataSettings_IsEquals(dss, GetSettingsDataFromEnd(0))))
    {
        BeginLimits(dataA, dataB, numElements);
    }
    else
    {
        int allDatas = DS_NumElementsWithSameSettings();
        LIMITATION(allDatas, allDatas, 1, DISPLAY_NUM_MIN_MAX);

        if(DS_NumElementsWithSameSettings() >= DISPLAY_NUM_MIN_MAX)
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
                SET_LIMIT((uint8*)dataA, limitUpA_RAM, limitDownA_RAM);
                SET_LIMIT((uint8*)dataB, limitUpB_RAM, limitDownB_RAM);
            }
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void CalculateSums(void)
{
    DataSettings *ds = 0;
    uint16 *dataA = 0;
    uint16 *dataB = 0;

    DS_GetDataFromEnd_RAM(0, &ds, &dataA, &dataB);

    int numPoints = NumBytesInChannel(ds);

    int numAveragings = 0;

    if(sTime_RandomizeModeEnabled())
    {
        numAveragings = set.nr.numAveForRand;
    }

    if(sDisplay_NumAverage() > numAveragings)
    {
        numAveragings = sDisplay_NumAverage();
    }

    uint16* sumA16 = (uint16*)sumA_RAM;
    uint16* sumB16 = (uint16*)sumB_RAM;

    int iMax = numPoints / 2;

    for(int i = 0; i < iMax; i++)
    {
        uint16 data16 = dataA[i];
        *sumA16 = (uint8)data16;
        sumA16 += 2;
        *sumA16 = (uint8)(data16 >> 8);
        sumA16 += 2;

        data16 = dataB[i];
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

            sumA16 = (uint16*)sumA_RAM;
            uint16* dA = dataA;

            sumB16 = (uint16*)sumB_RAM;
            uint16* dB = dataB;

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
                uint16 data16 = *dA++;                      // Считываем первые два отсчёта данных

#ifdef _MS_VS
#pragma warning(push)
#pragma warning(disable:4333)
#endif
                ALTERNATE_ADD(sumA16, 0);                   // WARN Похоже, sum неправильно вычисляется, из-за чего артефаты при больших накоплениях. Нужно (loSum - data16) заменить на старое значение (*data16)

                ALTERNATE_ADD(sumA16, 8);

                data16 = *dB++;

                ALTERNATE_ADD(sumB16, 0);

                ALTERNATE_ADD(sumB16, 8);
#ifdef _MS_VS
#pragma warning(pop)
#endif
            }
        }
    }
    newSumCalculated[A] = newSumCalculated[B] = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
void DS_AddData(uint8 *dataA, uint8 *dataB, DataSettings dss)
{
    if (dss.enableChA == 0 && dss.enableChB == 0)
    {
        return;
    }

    dss.time = RTC_GetPackedTime();


    CalculateLimits(dataA, dataB, &dss);

    if (numPointsP2P)                           // Если находимся в поточечном выводе
    {
        ReplaceLastFrame(&dss, dataA, dataB);   // Заменим последний фрейм данных (в котором находятся текущие точки) считанными
        numPointsP2P = 0;
    }
    else
    {
        PushData(&dss, dataA, dataB);
    }

    CalculateSums();

    CalculateAroundAverage(dataA, dataB, &dss);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Возвращает true, если настройки измерений с индексами elemFromEnd0 и elemFromEnd1 совпадают, и false в ином случае.
static bool SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1)
{
    DataSettings* dp0 = DS_DataSettingsFromEnd(elemFromEnd0);
    DataSettings* dp1 = DS_DataSettingsFromEnd(elemFromEnd1);
    return DataSettings_IsEquals(dp0, dp1);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
int DS_NumElementsWithCurrentSettings(void)
{
    DataSettings dp;
    FPGA_FillDataPointer(&dp);
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
int DS_NumElementsInStorage(void)
{
    return numElementsInStorage;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Копирует данные канала ch из, определяемые ds, в одну из двух строк массива dataImportRel. Возвращаемое значение false означает, что данный канал выключен.
static bool CopyData(DataSettings *ds, Channel ch, uint8 *dataImportRel)
{
    if((ch == A && ds->enableChA == 0) || (ch == B && ds->enableChB == 0))
    {
        return false;
    }

    uint8* address = ds->addrData;

    int length = NumBytesInChannel(ds);

    if(ch == B && ds->enableChB && ds->enableChA)
    {
        address += length;
    }

    RAM_MemCpy16(address, dataImportRel, length);

    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint8* DS_GetData_RAM(Channel ch, int fromEnd)
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool DS_GetDataFromEnd_RAM(int fromEnd, DataSettings **ds, uint16 **dataA, uint16 **dataB)
{
    uint8 *dataImportRelA = RAM(DS_DATA_IMPORT_REL_A);
    uint8 *dataImportRelB = RAM(DS_DATA_IMPORT_REL_B);

    DataSettings* dp = DS_DataSettingsFromEnd(fromEnd);

    if(dp == 0)
    {
        return false;
    }

    if(dataA != 0)
    {
        *dataA = CopyData(dp, A, dataImportRelA) ? (uint16*)dataImportRelA : 0;
    }
    if(dataB != 0)
    {
        *dataB = CopyData(dp, B, dataImportRelB) ? (uint16*)dataImportRelB : 0;
    }
    *ds = dp;
    
    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint8* DS_GetAverageData(Channel ch)
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

    int numPoints = NumBytesInChannel(ds);

    if (sDisplay_GetModeAveraging() == Averaging_Around)
    {
        float *floatAveData = (ch == A) ? aveDataA_RAM : aveDataB_RAM;
        
        for (int i = 0; i < numPoints; i++)
        {
            gDataAve[ch][i] = (uint8)(floatAveData[i] + 0.5f);
        }
        return &gDataAve[ch][0];
    }

    int numAveraging = 0;
    if (sTime_RandomizeModeEnabled())
    {
        numAveraging = set.nr.numAveForRand;
    }

    if (sDisplay_NumAverage() > numAveraging)
    {
        numAveraging = sDisplay_NumAverage();
    }

    LIMIT_ABOVE(numAveraging, DS_NumElementsWithSameSettings());

    for(int i = 0; i < numPoints; i++)
    {
        gDataAve[A][i] = (uint8)((float)RAM_ReadWord(sumA_RAM + i) / numAveraging + 0.5f);
        gDataAve[B][i] = (uint8)((float)RAM_ReadWord(sumB_RAM + i) / numAveraging + 0.5f);
    }

    return &gDataAve[ch][0];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint8* DS_GetLimitation(Channel ch, int direction)
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
int DS_NumberAvailableEntries(void)
{
    if(gDatas[iFirst].addrData == 0)
    {
        return 0;
    }

    int numElems = 0;
    LIMITATION_ABOVE(numElems, SIZE_POOL / SizeData(&gDatas[iLast]), NUM_DATAS);

    return numElems;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DS_NewFrameP2P(DataSettings dss)
{
    if (dss.enableChA == 0 && dss.enableChB == 0)
    {
        return;
    }

    PushData(&dss, 0, 0);

    numPointsP2P = 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DS_AddPointsP2P(uint16 dataA, uint16 dataB)
{
    DataSettings* ds = DS_DataSettingsFromEnd(0);

    int length = NumBytesInChannel(ds);

    if (!ds->enableChA && !ds->enableChB)
    {
        return;
    }

    FSMC_SET_MODE(ModeFSMC_RAM);

    if (numPointsP2P >= length)                         // Если место во фрейме заполнено полностью
    {
        uint8 *address = ds->addrData;

        if (ds->enableChA)                              // То сдвинем все точки во фрейме влево
        {
            RAM_MemShiftLeft(address + 2, length - 2, 2);

            address += length;
        }
        if (ds->enableChB)
        {
            RAM_MemShiftLeft(address + 2, length - 2, 2);
        }
    }

    int dNumPoints = numPointsP2P - length;
    if (dNumPoints < 0)
    {
        dNumPoints = 0;
    }
    else
    {
        dNumPoints += 2;
    }

    uint8 *addrWrite = ds->addrData + numPointsP2P - dNumPoints;

    if (ds->enableChA)
    {
        *((uint16*)addrWrite) = dataA;
        addrWrite += length;
    }

    if (ds->enableChB)
    {
        *((uint16*)addrWrite) = dataB;
    }

    numPointsP2P += 2;

    FSMC_RESTORE_MODE();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int DS_GetLastFrameP2P_RAM(DataSettings **ds, uint8 **dataA, uint8 **dataB)
{
    DataSettings *dp = DS_DataSettingsFromEnd(0);

    *ds = dp;

    if (dp->tBase < MIN_TBASE_P2P)
    {
        *dataA = 0;
        *dataB = 0;
        return 0;
    }

    if (numPointsP2P)
    {
        if (dataA)
        {
            *dataA = AddressChannel(dp, A);
        }
        if (dataB)
        {
            *dataB = AddressChannel(dp, B);
        }
    }

    return numPointsP2P;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool DataSettings_IsEquals(DataSettings *ds1, DataSettings *ds2)
{
    bool equals = (ds1->enableChA == ds2->enableChA) &&
        (ds1->enableChB == ds2->enableChB) &&
        (ds1->inverseChA == ds2->inverseChA) &&
        (ds1->inverseChB == ds2->inverseChB) &&
        (ds1->range[A] == ds2->range[A]) &&
        (ds1->range[B] == ds2->range[B]) &&
        (ds1->rShift[A] == ds2->rShift[A]) &&
        (ds1->rShift[B] == ds2->rShift[B]) &&
        (ds1->tBase == ds2->tBase) &&
        (ds1->tShift == ds2->tShift) &&
        (ds1->modeCoupleA == ds2->modeCoupleA) &&
        (ds1->modeCoupleB == ds2->modeCoupleB) &&
        (ds1->trigLev[A] == ds2->trigLev[A]) &&
        (ds1->trigLev[B] == ds2->trigLev[B]) &&
        (ds1->multiplierA == ds2->multiplierA) &&
        (ds1->multiplierB == ds2->multiplierB) &&
        (ds1->peackDet == ds2->peackDet);

    return equals;
}
