// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "FPGA.h"
#include "Log.h"
#include "Display/Display.h"
#include "FPGA/DataStorage.h"
#include "FPGA/FPGAtypes.h"
#include "FPGA/FPGAextensions.h"
#include "Hardware/FSMC.h"
#include "Hardware/Hardware.h"
#include "Hardware/RAM.h"
#include "Hardware/Timer.h"
#include "Hardware/RTC.h"
#include "Menu/Pages/Memory/PageMemory.h"
#include "Panel/Panel.h"
#include "Panel/Controls.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/ProcessingSignal.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Debug.h"
#include "structures.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NULL_TSHIFT 1000000


#define N_KR 100
const int Kr[] = {N_KR / 1, N_KR / 2, N_KR / 5, N_KR / 10, N_KR / 20};


StateWorkFPGA fpgaStateWork = StateWorkFPGA_Stop;
volatile static int numberMeasuresForGates = 1000;

static DataSettings ds;

static uint timeCompletePredTrig = 0;   // Здесь окончание счёта предзапуска. Если == 0, то предзапуск не завершён.

static uint8* dataRandA = 0;
static uint8* dataRandB = 0;

static uint timeStart = 0;
static uint timeSwitchingTrig = 0;

static bool readingPointP2P = false;    // Признак того, что точка и последнего прерывания поточечного вывода прочитана.

uint16 adcValueFPGA = 0;                // Здесь хранится значение считанное с АЦП для правильной расстановки точек

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool ReadPoint(void);                                        // Чтение точки в поточечном режиме
static void Write(TypeRecord type, uint16 *address, uint data);     // Запись в регистры и альтеру
static void InitADC(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16 READ_DATA_ADC_16(uint16 *address, Channel ch   )
{
    float delta = AVE_VALUE - (RShiftZero - RSHIFT(ch)) / (RSHIFT_IN_CELL / 20.0f);
    BitSet16 _data_;
    _data_.halfWord = *address;
    int byte0 = (int)(((float)_data_.byte[0] - delta) * GetStretchADC(ch) + delta + 0.5f);
    if (byte0 < 0) { byte0 = 0; };
    if (byte0 > 255) { byte0 = 255; };
    _data_.byte[0] = (uint8)byte0;
    int byte1 = (int)(((float)_data_.byte[1] - delta) * GetStretchADC(ch) + delta + 0.5f);
    if (byte1 < 0) { byte1 = 0; };
    if (byte1 > 255) { byte1 = 255; };
    _data_.byte[1] = (uint8)byte1;
    return _data_.halfWord;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void HardwareInit(void)
{
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // Настроим PD2 на внешнее прерывание EXTI2 - сюда будет приходить флаг считанной точки для поточечного вывода
    GPIO_InitTypeDef isGPIOD =
    {
        GPIO_PIN_2,
        GPIO_MODE_IT_RISING,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOD, &isGPIOD);

    // Включать прерывание будем только тогда, когда нужно. (FPGA_Start())
}

void FPGA_Init(void)
{
    DS_Clear();
    HardwareInit();  // WARN Пока не получается чтение флага сделать на прерывании
    FreqMeter_Init();
    InitADC();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
uint16* AddressRead(Channel ch)
{
    return ((ch == A) ? RD_ADC_A : RD_ADC_B);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция вызывается, когда можно считывать очередной сигнал.
static void OnTimerCanReadData(void)
{
    gBF.FPGAcanReadData = 1;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetNumSignalsInSec(int numSigInSec) 
{
    Timer_SetAndEnable(kNumSignalsInSec, OnTimerCanReadData, (int)(1000.f / numSigInSec));
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SwitchingTrig(void)
{
    if (TRIG_POLARITY_FRONT)
    {
        *WR_TRIG = 0;
        *WR_TRIG = 1;
    }
    else
    {
        *WR_TRIG = 1;
        *WR_TRIG = 0;
    }
    timeSwitchingTrig = gTimerMS;
    Panel_EnableLEDTrig(false);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint16 ReadFlag(void)
{
    uint16 flag = FSMC_READ(RD_FL);

    if (!RECORDER_MODE)
    {
        if(GetBit(flag, FL_TRIG_READY) == 1 && timeStart > timeSwitchingTrig)
        {
            Panel_EnableLEDTrig(true);
        }
    }

    FreqMeter_Update(flag);

    return flag;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_ReadPoint(void)
{
    readingPointP2P = false;
    ReadPoint();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static bool ReadPoint(void)
{
    if (readingPointP2P)
    {
        FSMC_RemoveFunctionAfterSetMode();
    }
    else if (FSMC_InSetStateMode()                  // Если шина находится в состоянии смены режима
             || FSMC_GetMode() == ModeFSMC_Display) // Или в режиме работы с дисплеем
    {
        FSMC_SetFuncitonAfterSetMode(ReadPoint);    // То назначаем вызов этой функции при следющей смене режима шины
    }
    else
    {
        FSMC_RemoveFunctionAfterSetMode();

        FSMC_SET_MODE(ModeFSMC_FPGA);

        int16 balanceA = setNR.balanceADC[A];
        int16 balanceB = setNR.balanceADC[B];

        BitSet16 bsA;
        BitSet16 bsB;

        bsA.halfWord = *RD_ADC_A;
        bsB.halfWord = *RD_ADC_B;

        int16 byte0 = 0;
        LIMITATION(byte0, (int16)bsA.byte0 + balanceA, 0, 255);
        LIMITATION(bsA.byte0, (uint8)byte0, MIN_VALUE, MAX_VALUE);
        LIMITATION(bsA.byte1, bsA.byte1, MIN_VALUE, MAX_VALUE);
        byte0 = 0;
        LIMITATION(byte0, (int16)bsB.byte0 + balanceB, 0, 255);
        LIMITATION(bsB.byte0, (uint8)byte0, MIN_VALUE, MAX_VALUE);
        LIMITATION(bsB.byte1, bsB.byte1, MIN_VALUE, MAX_VALUE);

        DS_AddPointsP2P(bsA.halfWord, bsB.halfWord);

        FSMC_RESTORE_MODE();

        readingPointP2P = true;
    }

    return readingPointP2P;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_WriteStartToHardware(void)
{
    *WR_POST = gPost;
    *WR_PRED = gPred;
    *WR_TRIG = TRIG_POLARITY_FRONT ? 1 : 0;
    *WR_START = 0xffff;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Start(void)
{
    FPGA_WriteStartToHardware();

    timeCompletePredTrig = 0;

    FPGA_FillDataPointer(&ds);
    timeStart = gTimerMS;
    gBF.FPGAcritiacalSituation = 0;

    if (!IN_P2P_MODE)
    {
        HAL_NVIC_DisableIRQ(EXTI2_IRQn);    // Выключаем чтение одиночной точки
    }
    else
    {
        HAL_NVIC_EnableIRQ(EXTI2_IRQn);     // Вклюение чтения одиночной точки
        DS_NewFrameP2P(ds);
    }

    fpgaStateWork = StateWorkFPGA_Work;
}


int gRandStat[281];          // Здесь будут храниться статистики
float gScaleRandStat = 0.0f;


//------------------------------------------------------------------------------------------------------------------------------------------------------
static bool CalculateGate(uint16 rand, uint16 *eMin, uint16 *eMax)
{
    if (gBF.FPGAfirstAfterWrite == 1)   // Если первый запуск после записи в альтеру -
    {
        gBF.FPGAfirstAfterWrite = 0;    // пропускаем его, потому что оно дудет портить нам статистику
        if (!START_MODE_SINGLE)         // И если не однократный режим -
        {
            return false;               // то выходим с ошибкой
        }
    }

    if (rand < 500 || rand > 3500)
    {
        LOG_ERROR_TRACE("ОШИБКА!!! считано %d", rand);
        return false;
    }

    static float minGate = 0.0f;
    static float maxGate = 0.0f;
    static int numElements = 0;
    static uint16 min = 0xffff;
    static uint16 max = 0;

    numElements++;
    if (rand < min)
    {
        min = rand;
    }
    if (rand > max)
    {
        max = rand;
    }

    static int stat[281];

    if (gScaleRandStat != 0.0f) //-V550
    {
        if (rand <= minGate)
        {
            stat[0]++;
        }
        else if (rand >= maxGate)
        {
            stat[280]++;
        }
        else
        {
            int step = (int)((rand - minGate) / gScaleRandStat);
            stat[step]++;
        }
    }

    if (minGate == 0.0f) //-V550
    {
        *eMin = min;
        *eMax = max;
        if (numElements < numberMeasuresForGates)
        {
            return true;
        }
        minGate = min;
        maxGate = max;
        numElements = 0;
        min = 0xffff;
        max = 0;
    }

    if (numElements >= numberMeasuresForGates)
    {
        minGate = (9.0f * minGate + min) * 0.1f;
        maxGate = (9.0f * maxGate + max) * 0.1f;
        numElements = 0;
        min = 0xffff;
        max = 0;

        gScaleRandStat = (maxGate - minGate) / 281.0f;
        for (int i = 0; i < 281; i++)
        {
            gRandStat[i] = stat[i];
            stat[i] = 0;
        }
    }
    *eMin = (uint16)minGate;
    *eMax = (uint16)maxGate;

    if ((rand - 5) < minGate || (rand + 5) > maxGate)
    {
        return false;
    }

    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static int CalculateShift(void)            // WARN Не забыть восстановить функцию
{
    uint16 min = 0;
    uint16 max = 0;

    if (!CalculateGate(adcValueFPGA, &min, &max))
    {
        return NULL_TSHIFT;
    }

    if (SHOW_RAND_INFO)
    {
        LOG_WRITE("rand = %d, ворота %d - %d", (int)adcValueFPGA, min, max);
    }

    if (IN_RANDOM_MODE)
    {
        float tin = (float)(adcValueFPGA - min) / (max - min) * 10e-9f;
        int retValue = (int)(tin / 10e-9f * Kr[TBASE]);
        return retValue;
    }

    if (TBASE == TBase_100ns && adcValueFPGA < (min + max) / 2)
    {
        return 0;
    }

    return -1;  // set.debug.altShift;      WARN Остановились на жёстком задании дополнительного смещения. На PageDebug выбор закомментирован, можно раскомментировать при необходимости
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
#define WRITE_AND_OR_INVERSE(addr, data, ch)                                              \
    if(INVERSE(ch))                                                           \
    {                                                                                       \
        data = (uint8)((int)(2 * AVE_VALUE) - LimitationUInt8(data, MIN_VALUE, MAX_VALUE)); \
    }                                                                                       \
    *(addr) = data;


//---------------------------------------------------------------------------------------------------------------------------------------------------
#define BALANCE_DATA(pData, balance)        \
    if (nowBalance)                         \
    {                                       \
        int n = ((int)(*pData)) + balance;  \
        if (n < 0)                          \
        {                                   \
            n = 0;                          \
        }                                   \
        if (n > 255)                        \
        {                                   \
            n = 255;                        \
        }                                   \
        *pData = (uint8)n;                  \
     }                                      \
    nowBalance = !nowBalance;


static void ReadRandomizeChannel(Channel ch, uint16 addrFirstRead, uint8 *data, const uint8 *last, int step, int numSkipped)
{
    *WR_PRED = addrFirstRead;
    *WR_ADDR_NSTOP = 0xffff;

    uint16 *addr = AddressRead(ch);

    uint16 newData = 0;

    for (int i = 0; i < numSkipped; i++)
    {
        newData = *addr;
    }

    if (INVERSE(ch))
    {
        while (data <= last)
        {
            newData = *addr;
            *data = (uint8)((int)(2 * AVE_VALUE) - LimitationUInt8((uint8)newData, MIN_VALUE, MAX_VALUE));
            data += step;
        }
    }
    else
    {
        while (data <= last)
        {
            *data = (uint8)(*addr);
            data += step;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
// first - если true, это первый вызов из последовательности, нужно подготовить память
// last - если true, это последний вызов из последовательности, нужно записать результаты в память.
static bool ReadRandomizeModeSave(bool first, bool last, bool onlySave)
{
    int bytesInChannel = sMemory_NumBytesInChannel(false);

    if (first)
    {
        memset(dataRandA, 0, bytesInChannel);
        memset(dataRandB, 0, bytesInChannel);
    }

    if (!onlySave)
    {
        int Tsm = CalculateShift();

        // Считываем из внешнего ОЗУ ранее записанные туда данные
        // Буфера dataRandA, dataRandB созданы заранее для ускорения, т.к. в режиме рандомизатора в FPGA_Update() выполняется несколько чтений
        if (first)
        {
            uint8 *dA = 0;
            uint8 *dB = 0;

            Processing_GetData(&dA, &dB, 0);

            if (dA)
            {
                RAM_MemCpy16(dA, dataRandA, bytesInChannel);
            }
            if (dB)
            {
                RAM_MemCpy16(dB, dataRandB, bytesInChannel);
            }
        }

        if (Tsm == NULL_TSHIFT)
        {
            return false;
        };

        if (START_MODE_SINGLE || SAMPLE_REAL)
        {
            FPGA_ClearData();

            // Очищаем массив для данных. После чтения точек несчитанные позиции будут равны нулю, что нужно для экстраполяции
            memset(dataRandA, 0, bytesInChannel);
            memset(dataRandB, 0, bytesInChannel);
        }

        // Теперь считаем данные
        TBase tBase = TBASE;
        int step = Kr[tBase];
        extern int addShiftForFPGA;
#define NUM_ADD_STEPS 2
        int index = Tsm - addShiftForFPGA - NUM_ADD_STEPS * step;

        int numSkipped = 0;

        while (index < 0)
        {
            index += step;
            numSkipped++;
        }

        // addrFirstRead - адрес, который мы должны записать в альтеру. Это адрес, с которого альтера начнёт чтение данных
        // Но считывать будем с адреса на 3 меньшего, чем расчётный. Лишние данные нужны, чтобы достроить те точки вначале, которые выпадают при программном сдвиге.
        // Процедуре чтения мы укажем сколько первых точек выбросить через параметр numSkipped
        uint16 addrFirstRead = *RD_ADDR_NSTOP + 16384 - (uint16)(bytesInChannel / step) - 1 - NUM_ADD_STEPS;

        //uint startRead = gTimerTics;

        ReadRandomizeChannel(B, addrFirstRead, &dataRandB[index], &dataRandB[FPGA_MAX_POINTS - 1], step, numSkipped);
        ReadRandomizeChannel(A, addrFirstRead, &dataRandA[index], &dataRandA[FPGA_MAX_POINTS - 1], step, numSkipped);
        
        if (START_MODE_SINGLE || SAMPLE_REAL)
        {
            Processing_InterpolationSinX_X(dataRandA, bytesInChannel, tBase);
            Processing_InterpolationSinX_X(dataRandB, bytesInChannel, tBase);
        }
    }

    // Теперь сохраняем данные обратно во внешнее ОЗУ
    if (last)
    {
        RAM_MemCpy16(dataRandA, RAM(FPGA_DATA_A), bytesInChannel);
        RAM_MemCpy16(dataRandB, RAM(FPGA_DATA_B), bytesInChannel);
    }

    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// balance - свдиг точки вверх/вниз для балансировки
static void ReadChannel(uint8 *data, Channel ch, int length, uint16 nStop, bool shift, int balance)
{
    if (length == 0)
    {
        return;
    }
    *WR_PRED = nStop;
    *WR_ADDR_NSTOP = 0xffff;

    uint16 *p = (uint16*)data;
    uint16 *endP = (uint16*)&data[length];

    uint16 *address = (ch == A) ? RD_ADC_A : RD_ADC_B;

    volatile uint16 dat = *address;

    if (shift)
    {
        *((uint8*)p) = ((*address) >> 8);

        p = (uint16*)(((uint8*)p) + 1);
    }

    while (p < endP && gBF.FPGAinProcessingOfRead == 1)
    {
        *p++ = READ_DATA_ADC_16(address, ch);
        *p++ = READ_DATA_ADC_16(address, ch);
        *p++ = READ_DATA_ADC_16(address, ch);
        *p++ = READ_DATA_ADC_16(address, ch);
        *p++ = READ_DATA_ADC_16(address, ch);
        *p++ = READ_DATA_ADC_16(address, ch);
        *p++ = READ_DATA_ADC_16(address, ch);
        *p++ = READ_DATA_ADC_16(address, ch);
    }

    if (balance != 0)
    {
        for (int i = shift ? 1 : 0; i < length; i += 2)
        {
            int newData = (int)data[i] + balance;
            if (newData < 0)
            {
                newData = 0;
            }
            if (newData > 255)
            {
                newData = 255;
            }
            data[i] = (uint8)newData;
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
// Кажется, рассчитываем адрес последней записи
uint16 ReadNStop(void)
{
    return *RD_ADDR_NSTOP + 16384 - (uint16)sMemory_NumBytesInChannel(false) / 2 - 1 - (uint16)gAddNStop;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void ReadRealMode(uint8 *dataA, uint8 *dataB, bool necessaryShift)
{
    gBF.FPGAinProcessingOfRead = 1;

    uint16 nStop = ReadNStop();

    bool shift = GetBit(ReadFlag(), FL_LAST_RECOR) == 0;    // Если true, будем сдвигать точки на одну

    int balanceA = 0;
    int balanceB = 0;

    if (setNR.balanceADCtype == BalanceADC_Hand && 
        PEACKDET_DIS)               // При включённом пиковом детекторе балансировка не нужна
    {
        balanceA = setNR.balanceADC[A];
        balanceB = setNR.balanceADC[B];
    }

    ReadChannel(dataA, A, sMemory_NumBytesInChannel(false), nStop, shift, balanceA);
    ReadChannel(dataB, B, sMemory_NumBytesInChannel(false), nStop, shift, balanceB);

    RAM_MemCpy16(dataA, RAM(FPGA_DATA_A), FPGA_MAX_POINTS);
    RAM_MemCpy16(dataB, RAM(FPGA_DATA_B), FPGA_MAX_POINTS);

    gBF.FPGAinProcessingOfRead = 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void InverseDataIsNecessary(Channel ch, uint8 *data)
{
    if (INVERSE(ch))
    {
        for (int i = 0; i < FPGA_MAX_POINTS; i++)
        {
            data[i] = (uint8)((int)(2 * AVE_VALUE) - LimitationUInt8(data[i], MIN_VALUE, MAX_VALUE));
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Прочитать данные.
// necessaryShift - признак того, что сигнал нужно смещать.
// saveToStorage - нужно в режиме рандомизатора для указания, что пора сохранять измерение
// first - нужно для режима рандомизматора - чтобы подготовить память.
// onlySave - только сохранить в хранилище
static void DataReadSave(bool necessaryShift, bool first, bool saveToStorage, bool onlySave)
{
    uint8 *dataA = malloc(FPGA_MAX_POINTS);
    uint8 *dataB = malloc(FPGA_MAX_POINTS);

    gBF.FPGAinProcessingOfRead = 1;
    if (IN_RANDOM_MODE)
    {
        ReadRandomizeModeSave(first, saveToStorage, onlySave);
    }
    else
    {
        ReadRealMode(dataA, dataB, necessaryShift);
    }

    int numBytes = sMemory_NumBytesInChannel(false);

    RAM_MemCpy16(RAM(FPGA_DATA_A), dataA, numBytes);
    RAM_MemCpy16(RAM(FPGA_DATA_B), dataB, numBytes);

    for (int i = 0; i < numBytes; i++)
    {
        LIMITATION(dataA[i], dataA[i], MIN_VALUE, MAX_VALUE);
        LIMITATION(dataB[i], dataB[i], MIN_VALUE, MAX_VALUE);
    }

    if (!IN_RANDOM_MODE)
    {
        InverseDataIsNecessary(A, dataA);
        InverseDataIsNecessary(B, dataB);
    }
    
    if (saveToStorage && ((NumPoinstFPGA)ds.indexLength == (NumPoinstFPGA)NumPoints_2_FPGA_NUM_POINTS(sMemory_NumBytesInChannel(false))))
    {
        DS_AddData(dataA, dataB, ds);
    }

    if (TRIG_MODE_FIND_AUTO)
    {
        FPGA_FindAndSetTrigLevel();
    }

    gBF.FPGAinProcessingOfRead = 0;

    free(dataA);
    free(dataB);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Возвращает true, если считаны данные
bool ProcessingData(void)
{
    bool retValue = false;          // Здесь будет true, когда данные считаются

    static const int numRead[] = {100, 50, 20, 10, 5};

    int num = IN_RANDOM_MODE ? numRead[TBASE] / 2 : 1;

    if (num > 1)
    {
        dataRandA = AllocMemForChannelFromHeap(A, 0);
        dataRandB = AllocMemForChannelFromHeap(B, 0);

        if (SAMPLE_REAL)
        {
            num = 1;        // Для реальной выборки ограничим количество считываний - нам надо только одно измерение.
        }
    }

    for (int i = 0; i < num; i++)
    {
        uint16 flag = ReadFlag();

        if (GetBit(flag, FL_PRED_READY) == 0)       // Если предзапуск не отсчитан - выходим
        {
            break;
        }

        if (timeCompletePredTrig == 0)              // Если окончание предзапуска ранее не было зафиксировано
        {
            timeCompletePredTrig = gTimerMS;        // записываем время, когда оно произошло.
        }

        if (i > 0)
        {
            uint time = gTimerMS;
            // В рандомизаторных развёртках при повторных считываниях нужно подождать флага синхронизации
            while (GetBit(flag, FL_TRIG_READY) == 0 && GetBit(flag, FL_DATA_READY) == 0 && (gTimerMS - time) < 10)
            {                                                                                   // Это нужно для низких частот импульсов на входе
                flag = ReadFlag();
            }
            if (GetBit(flag, FL_DATA_READY) == 0) 
            {
                i = num;
            }
        }

        if (GetBit(flag, FL_TRIG_READY))                                    // Если прошёл импульс синхронизации
        {
            if (GetBit(flag, FL_DATA_READY) == 1)                           // Проверяем готовность данных
            {
                fpgaStateWork = StateWorkFPGA_Stop;                         // И считываем, если данные готовы
                HAL_NVIC_DisableIRQ(EXTI2_IRQn);                            // Отключаем чтение точек

                DataReadSave(false, i == 0, i == num - 1, false);

                retValue = true;
                if (!START_MODE_SINGLE)
                {
                    if(IN_P2P_MODE && START_MODE_AUTO)  // Если находимся в режиме поточечного вывода при автоматической синхронизации
                    {
                        Timer_SetAndStartOnce(kTimerStartP2P, FPGA_Start, 1000);                 // то откладываем следующий запуск, чтобы зафиксировать сигнал на экране
                    }
                    else
                    {
                        FPGA_Start();
                    }
                }
                else
                {
                    Panel_EnableLEDTrig(false);
                }
            }
        }
        else if (START_MODE_AUTO)  // Если имупльса синхронизации нету, а включён автоматический режим синхронизации
        {
            if (gTimerMS - timeCompletePredTrig > TSHIFT_2_ABS(2, TBASE) * 1000)  // Если прошло больше времени, чем помещается в десяти клетках
            {
                if (IN_P2P_MODE)
                {
                    Panel_EnableLEDTrig(false);     // В поточечном режиме просто тушим лампочку синхронизации
                }
                else
                {
                    FPGA_SwitchingTrig();           // В непоточечном даём принудительно даём синхронизацю
                }
            }
        }

        if (i == num)
        {
                DataReadSave(false, false, true, true);
            retValue = true;
            break;
        }

        if (gBF.panelControlReceive && IN_RANDOM_MODE)
        {
            DataReadSave(false, false, true, true);
            retValue = true;
            break;
        }
    }

    SAFE_FREE(dataRandA);
    SAFE_FREE(dataRandB);

    return retValue;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
TBase CalculateTBase(float freq)
{
    typedef struct
    {
        float freq;
        TBase tBase;
    } STR;

    static const STR structs[] =
    {
        {200e6f,    TBase_1ns},
        {100e6f,    TBase_2ns},
        {40e6f,     TBase_5ns},
        {20e6f,     TBase_10ns},
        {10e6f,     TBase_20ns},
        {3e6f,      TBase_50ns},
        {2e6f,      TBase_100ns},
        {900e3f,    TBase_200ns},
        {200e3f,    TBase_1us},
        {400e3f,    TBase_500ns},
        {90e3f,     TBase_2us},
        {30e3f,     TBase_5us},
        {20e3f,     TBase_10us},
        {10e3f,     TBase_20us},
        {4e3f,      TBase_50us},
        {2e3f,      TBase_100us},
        {1e3f,      TBase_200us},
        {350.0f,    TBase_500us},
        {200.0f,    TBase_1ms},
        {100.0f,    TBase_2ms},
        {40.0f,     TBase_5ms},
        {20.0f,     TBase_10ms},
        {10.0f,     TBase_20ms},
        {4.0f,      TBase_50ms},
        {2.0f,      TBase_100ms},
        {0.0f,      TBaseSize}
    };


    const STR *str = &structs[0];
    while (str->freq != 0.0f) //-V550
    {
        if (freq >= str->freq)
        {
            return str->tBase;
        }
        ++str;
    }

    return TBase_200ms;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Update(void)
{
    if (fpgaStateWork == StateWorkFPGA_Stop)
    {
        return;
    }

    ReadFlag();

    if (gStateFPGA.needCalibration)              // Если вошли в режим калибровки -
    {
        FPGA_ProcedureCalibration();            // выполняем её.
        gStateFPGA.needCalibration = false;
    }
    if (gBF.FPGAtemporaryPause == 1)
    {
        return;
    }

	if(gBF.FPGAneedAutoFind == 1)
    {
		FPGA_AutoFind();
		return;
	}

    if(gBF.FPGAcanReadData == 0)
    {
        return;
    }

    if (!RECORDER_MODE)
    {
        ProcessingData();
    }

    gBF.FPGAcanReadData = 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_OnPressStartStop(void)
{
    if (!WORK_DIRECT || CONSOLE_IN_PAUSE)       // Если находимся не в режиме непосредственного считывания сигнала
    {
        return;
    }

    if (IN_P2P_MODE)
    {
        if (Timer_IsRun(kTimerStartP2P))        // Если находимся в режиме поточечного вывода и в данный момент пауза после считывания очередного полного сигнала
        {
            Timer_Disable(kTimerStartP2P);      // То останавливаем таймер, чтобы просмотреть сигнал
        }
        else
        {
            FPGA_Start();
        }
    }
    else if(fpgaStateWork == StateWorkFPGA_Stop) 
    {
        FPGA_Start();
        fpgaStateWork = StateWorkFPGA_Wait;
    } 
    else
    {
        FPGA_Stop(false);
        fpgaStateWork = StateWorkFPGA_Stop;
    } 
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Stop(bool pause) 
{
    Panel_EnableLEDTrig(false);
    HAL_NVIC_DisableIRQ(EXTI2_IRQn);        // Выключаем прерывание на чтение считанной точки
    fpgaStateWork = pause ? StateWorkFPGA_Pause : StateWorkFPGA_Stop;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Reset(void)
{
    bool needStart = FPGA_IsRunning();
    FPGA_Stop(false);
    if (needStart)
    {
        FPGA_Start();
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA_IsRunning(void)
{
    return fpgaStateWork != StateWorkFPGA_Stop;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_ClearData(void)
{
    RAM_MemClear(RAM(FPGA_DATA_A), FPGA_MAX_POINTS);
    RAM_MemClear(RAM(FPGA_DATA_B), FPGA_MAX_POINTS);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetNumberMeasuresForGates(int number)
{
    numberMeasuresForGates = number;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint8 CalculateMin(uint8 buffer[100])
{
    uint8 min = buffer[0];
    for (int i = 1; i < 100; i++)
    {
        if (buffer[i] < min)
        {
            min = buffer[i];
        }
    }
    return min;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint8 CalculateMax(uint8 buffer[100])
{
    uint8 max = buffer[0];
    for(int i = 1; i < 100; i++)
    {
        if(buffer[i] > max)
        {
            max = buffer[i];
        }
    }
    return max;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void StopTemporaryPause(void)
{
    gBF.FPGAtemporaryPause = 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_TemporaryPause(void)
{
    gBF.FPGAtemporaryPause = 1;
    Timer_SetAndStartOnce(kTemporaryPauseFPGA, StopTemporaryPause, 100);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_FillDataPointer(DataSettings *ds)
{
    ds->enableA = sChannel_Enabled(A) ? 1 : 0;
    ds->enableB = sChannel_Enabled(B) ? 1 : 0;
    ds->inverseA = INVERSE(A) ? 1 : 0;
    ds->inverseB = INVERSE(B) ? 1 : 0;
    ds->range[A] = RANGE_A; //-V2006
    ds->range[B] = RANGE_B; //-V2006
    ds->rShift[A] = RSHIFT_A;
    ds->rShift[B] = RSHIFT_B;
    ds->tBase = TBASE; //-V2006
    ds->tShift = TSHIFT;
    ds->modeCoupleA = COUPLE_A; //-V2006
    ds->modeCoupleB = COUPLE_B; //-V2006
    ds->indexLength = NumPoints_2_FPGA_NUM_POINTS(sMemory_NumBytesInChannel(false)); //-V2006
    ds->trigLev[A] = TRIGLEV(A);
    ds->trigLev[B] = TRIGLEV(B);
    ds->peackDet = (uint)PEACKDET;
    ds->multiplierA = DIVIDER_A; //-V2006
    ds->multiplierB = DIVIDER_B; //-V2006
    ds->time.timeMS = 0;                        // Это важно для режима поточеного вывода. Означает, что полный сигнал ещё не считан
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_FindAndSetTrigLevel(void)
{
    if (DS_NumElementsInStorage() == 0 || TRIGSOURCE_EXT)
    {
        return;
    }

    uint16 *dataA = 0;
    uint16 *dataB = 0;
    DataSettings *ds_ = 0;

    DS_GetDataFromEnd_RAM(0, &ds_, &dataA, &dataB);

    const uint16 *data = TRIGSOURCE_A ? dataA : dataB;

    int lastPoint = NumBytesInChannel(ds_) - 1;

    uint8 min = Math_GetMinFromArray_RAM(data, 0, lastPoint);
    uint8 max = Math_GetMaxFromArray_RAM(data, 0, lastPoint);

    uint8 aveValue = ((int)min + (int)max) / 2;

    static const float scale = (float)(TrigLevMax - TrigLevZero) / (float)(MAX_VALUE - AVE_VALUE) / 2.4f;

    int trigLev = (int)(TrigLevZero + scale * ((int)aveValue - AVE_VALUE) - (RSHIFT(TRIGSOURCE) - RShiftZero));

    FPGA_SetTrigLev(TRIGSOURCE, (int16)trigLev);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Write(TypeRecord type, uint16 *address, uint data, bool restart)
{
    // Если необходимо, сохраняем установленный режим на шине, чтобы затем его восстановить
    ModeFSMC modePrev = FSMC_GetMode();
    bool needRestore = modePrev != ModeFSMC_FPGA;
    if (type == RecordFPGA && needRestore)
    {
        FSMC_SetMode(ModeFSMC_FPGA);
    }

    
    gBF.FPGAfirstAfterWrite = 1;
    if (restart)
    {
        if (gBF.FPGAinProcessingOfRead == 1)
        {
            FPGA_Stop(true);
            gBF.FPGAinProcessingOfRead = 0;
            Write(type, address, data);
            FPGA_Start();
        }
        else
        {
            if (fpgaStateWork != StateWorkFPGA_Stop)
            {
                FPGA_Stop(true);
                Write(type, address, data);
                FPGA_Start();
            }
            else
            {
                Write(type, address, data);
            }
        }
    }
    else
    {
        Write(type, address, data);
    }


    // Восстанавливаем предыдущий режим на шине, если необходимо.
    if (type == RecordFPGA && needRestore)
    {
        FSMC_SetMode(modePrev);
    }

    Panel_EnableLEDTrig(false); // После каждой засылки выключаем лампочку синхронизации
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static uint16 PinSelect(uint16 *addrAnalog)
{
    const uint16 pins[4] = {GPIO_PIN_3, GPIO_PIN_6, GPIO_PIN_10, GPIO_PIN_15};
    return pins[(int)addrAnalog]; //-V205
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static GPIO_TypeDef* AddrGPIO(uint16 *addrAnalog)
{
    GPIO_TypeDef *gpio[4] = {GPIOD, GPIOD, GPIOG, GPIOG};
    return gpio[(int)addrAnalog]; //-V205
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
#define pinCLC      GPIO_PIN_10
#define pinData     GPIO_PIN_12
#define CHIP_SELECT_IN_LOW  HAL_GPIO_WritePin(AddrGPIO(addrAnalog), PinSelect(addrAnalog), GPIO_PIN_RESET);
#define CHIP_SELECT_IN_HI   HAL_GPIO_WritePin(AddrGPIO(addrAnalog), PinSelect(addrAnalog), GPIO_PIN_SET);
#define CLC_HI              HAL_GPIO_WritePin(GPIOC, pinCLC, GPIO_PIN_SET);
#define CLC_LOW             HAL_GPIO_WritePin(GPIOC, pinCLC, GPIO_PIN_RESET);
#define DATA_SET(x)         HAL_GPIO_WritePin(GPIOC, pinData, x);


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Write(TypeRecord type, uint16 *address, uint data)
{
    if (type == RecordFPGA)
    {
        *address = (uint16)data;
    }
    else if (type == RecordAnalog)
    {
        uint16 *addrAnalog = address;
        CHIP_SELECT_IN_LOW
        for (int i = ((int)addrAnalog <= (int)CS2 ? 15 : 23); i >= 0; i--)    // Хотя данных всего 16 бит, но передаём 24 - первые восемь нули - первый из них указывает на то, что производится запись //-V205
        {
            DATA_SET((data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            CLC_HI
            CLC_LOW
        }
        CHIP_SELECT_IN_HI;

        DATA_SET(GPIO_PIN_SET);
        CLC_HI
        CLC_LOW
    }
    else if (type == RecordDAC)
    {
        uint16 *addrAnalog = CS1;
        CHIP_SELECT_IN_LOW
        for (int i = 31; i >= 0; i--)
        {
            DATA_SET((data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            CLC_HI
            CLC_LOW
        }
        CHIP_SELECT_IN_HI;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void InitADC(void)
{
    /*
    АЦП для рандомизатора
    вход - ADC3 - 18 ADC3_IN4 - PF6
    тактовая частота 25МГц
    режим работы :
    -измерение по 1 регулярному каналу
    - одиночное измерение по фронту внешнего запуска(прерывание от 112 - EXT11 - PC11)
    */

    __ADC3_CLK_ENABLE();

    static GPIO_InitTypeDef isGPIO =
    {
        GPIO_PIN_6,
        GPIO_MODE_ANALOG,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOF, &isGPIO);

    GPIO_InitTypeDef isGPIOC =
    {
        GPIO_PIN_11,
        GPIO_MODE_IT_RISING,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOC, &isGPIOC);

    HAL_NVIC_SetPriority(ADC_IRQn, PRIORITY_FPGA_ADC);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

    handleADC.Instance = ADC3;
    handleADC.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    handleADC.Init.Resolution = ADC_RESOLUTION12b;
    handleADC.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    handleADC.Init.ScanConvMode = DISABLE;
    handleADC.Init.EOCSelection = ENABLE;
    handleADC.Init.ContinuousConvMode = DISABLE;
    handleADC.Init.DMAContinuousRequests = DISABLE;
    handleADC.Init.NbrOfConversion = 1;
    handleADC.Init.DiscontinuousConvMode = DISABLE;
    handleADC.Init.NbrOfDiscConversion = 0;
    handleADC.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    handleADC.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_Ext_IT11;

    if (HAL_ADC_Init(&handleADC) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(&handleADC, &sConfig) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    if (HAL_ADC_Start_IT(&handleADC) != HAL_OK)
    {
        HARDWARE_ERROR
    }
}
