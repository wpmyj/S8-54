#include "FPGA.h"
#include "FPGATypes.h"
#include "Log.h"
#include "Data/Data.h"
#include "Data/DataStorage.h"
#include "Display/Display.h"
#include "FPGA/FPGAtypes.h"
#include "FPGA/FPGAextensions.h"
#include "Hardware/FSMC.h"
#include "Hardware/Hardware.h"
#include "Hardware/RAM.h"
#include "Hardware/Timer.h"
#include "Hardware/RTC.h"
#include "Menu/Pages/PageMemory.h"
#include "Panel/Panel.h"
#include "Panel/Controls.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/ProcessingSignal.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Debug.h"


/** @addtogroup FPGA
  * @{
  */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define FPGA_IN_PAUSE                   (bf.pause)
#define FPGA_CAN_READ_DATA              (bf.canRead)
#define FPGA_FIRST_AFTER_WRITE          (bf.firstAfterWrite)
#define NEED_STOP_AFTER_READ_FRAME_2P2  (bf.needStopAfterReadFrame2P2)


static struct BitFieldFPGA
{
    uint pause                     : 1;
    uint canRead                   : 1;
    uint firstAfterWrite           : 1;     ///< \brief Используется в режиме рандомизатора. После записи любого параметра в альтеру нужно не 
                                            ///<        использовать первое считанное данное с АЦП, потому что оно завышено и портит ворота.
    uint needStopAfterReadFrame2P2 : 1;
} bf = {0, 1, 0, 0};


#define NULL_TSHIFT 1000000

extern const int Kr[];

#define N_KR 100
const int Kr[] = {N_KR / 1, N_KR / 2, N_KR / 5, N_KR / 10, N_KR / 20};

StateWorkFPGA fpgaStateWork = StateWorkFPGA_Stop;
volatile static int numberMeasuresForGates = 1000;
static DataSettings ds;
static uint timeCompletePredTrig = 0;   ///< Здесь окончание счёта предзапуска. Если == 0, то предзапуск не завершён.
static uint8 *dataRandA = 0;
static uint8 *dataRandB = 0;
static uint timeStart = 0;
static uint timeSwitchingTrig = 0;
static bool readingPointP2P = false;    ///< Признак того, что точка и последнего прерывания поточечного вывода прочитана.
uint16 adcValueFPGA = 0;                ///< Здесь хранится значение считанное с АЦП для правильной расстановки точек.

int gRandStat[281];                     ///< Здесь будут храниться статистики.
float gScaleRandStat = 0.0f;
bool gFPGAisCalibrateAddRshift = false;      ///< Происходит процедура калибровки смещения и поэтому засылать смещение в АЦП надо без учёта добавок


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool ReadPoint(void);                                        ///< Чтение точки в поточечном режиме.
static void Write(TypeRecord type, uint16 *address, uint data);     ///< Запись в регистры и альтеру.
static void InitADC(void);
static void ProcessingAfterReadData(void);                          ///< Действия, которые нужно предпринять после успешного считывания данных.
       bool ProcessingData(void);                                   ///< Возвращает true, если считаны данные.
/// \brief Прочитать данные.
/// \param first          Нужно для режима рандомизматора - чтобы подготовить память.
/// \param saveToStorage  Нужно в режиме рандомизатора для указания, что пора сохранять измерение.
/// \param onlySave       Только сохранить в хранилище.
static void DataReadSave(bool first, bool saveToStorage, bool onlySave);
/// Сдвигает данные в массиве на одну точку вправо
static void ShiftOnePoint2Right(uint8 *data, int size);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint16 READ_DATA_ADC_16(const uint16 *address, Channel ch )
{
    float delta = AVE_VALUE - (RShiftZero - SET_RSHIFT(ch)) / (RSHIFT_IN_CELL / 20.0f);
    BitSet16 point;
    point.halfWord = *address;
    int byte0 = (int)(((float)point.byte[0] - delta) * GetStretchADC(ch) + delta + 0.5f);
    LIMITATION(byte0, 0, 255);
    point.byte[0] = (uint8)byte0;
    int byte1 = (int)(((float)point.byte[1] - delta) * GetStretchADC(ch) + delta + 0.5f);
    LIMITATION(byte1, 0, 255);
    point.byte[1] = (uint8)byte1;
    return point.halfWord;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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
    dS.Clear();
    HardwareInit();     /// \todo Пока не получается чтение флага сделать на прерывании
    FreqMeter_Init();
    InitADC();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// Функция вызывается, когда можно считывать очередной сигнал.
static void OnTimerCanReadData(void)
{
    FPGA_CAN_READ_DATA = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetENumSignalsInSec(int numSigInSec) 
{
    Timer_SetAndEnable(kENumSignalsInSec, OnTimerCanReadData, (int)(1000.f / numSigInSec));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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
    timeSwitchingTrig = gTimeMS;
    panel.EnableLEDTrig(false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint16 ReadFlag(void)
{
    uint16 flag = FSMC_READ(RD_FL);

    if (!RECORDER_MODE)
    {
        if(GetBit(flag, FL_TRIG_READY) == 1 && timeStart > timeSwitchingTrig)
        {
            panel.EnableLEDTrig(true);
        }
    }

    FreqMeter_Update(flag);

    return flag;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_ReadPoint(void)
{
    readingPointP2P = false;
    ReadPoint();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

        int16 balanceA = set.nrst_BalanceADC[A];
        int16 balanceB = set.nrst_BalanceADC[B];

        BitSet16 bsA;
        BitSet16 bsB;

        bsA.halfWord = READ_DATA_ADC_16(RD_ADC_A, A);
        bsB.halfWord = READ_DATA_ADC_16(RD_ADC_B, B);

        int16 byte0 = (int16)bsA.byte0 + balanceA;
        LIMITATION(byte0, 0, 255);
        bsA.byte0 = (uint8)byte0;
        LIMITATION(bsA.byte0, MIN_VALUE, MAX_VALUE);
        LIMITATION(bsA.byte1, MIN_VALUE, MAX_VALUE);
        byte0 = (int16)bsB.byte0 + balanceB;
        LIMITATION(byte0, 0, 255);
        bsB.byte0 = (uint8)byte0;
        LIMITATION(bsB.byte0, MIN_VALUE, MAX_VALUE);
        LIMITATION(bsB.byte1, MIN_VALUE, MAX_VALUE);

        dS.AddPointsP2P(bsA.halfWord, bsB.halfWord);

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

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Start(void)
{
    NEED_STOP_AFTER_READ_FRAME_2P2 = 0;

    FPGA_WriteStartToHardware();

    timeCompletePredTrig = 0;

    DataSettings_Fill(&ds);

    timeStart = gTimeMS;

    if (!IN_P2P_MODE)
    {
        HAL_NVIC_DisableIRQ(EXTI2_IRQn);    // Выключаем чтение одиночной точки
    }
    else
    {
        HAL_NVIC_EnableIRQ(EXTI2_IRQn);     // Вклюение чтения одиночной точки
        dS.NewFrameP2P(&ds);
    }

    fpgaStateWork = StateWorkFPGA_Work;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool CalculateGate(uint16 rand, uint16 *eMin, uint16 *eMax)
{
    if (FPGA_FIRST_AFTER_WRITE)   // Если первый запуск после записи в альтеру -
    {
        FPGA_FIRST_AFTER_WRITE = 0;    // пропускаем его, потому что оно дудет портить нам статистику
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

    if (gScaleRandStat != 0.0f)
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

    if (minGate == 0.0f)
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int CalculateShift(void)             /// \todo Не забыть восстановить функцию
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
        int retValue = (int)(tin / 10e-9f * Kr[SET_TBASE]);
        return retValue;
    }

    if (SET_TBASE == TBase_100ns && adcValueFPGA < (min + max) / 2)
    {
        return 0;
    }

    /// \todo Остановились на жёстком задании дополнительного смещения. На PageDebug выбор закомментирован, можно раскомментировать при необходимости

    return -1;  // set.debug.altShift;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
#define WRITE_AND_OR_INVERSE(addr, data, ch)                                                \
    if(SET_INVERSE(ch))                                                                     \
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

    uint16 *addr = ADDRESS_READ(ch);

    uint16 newData = 0;

    for (int i = 0; i < numSkipped; i++)
    {
        newData = *addr;
    }

    if (SET_INVERSE(ch))
    {
        while (data <= last)
        {
            newData = READ_DATA_ADC_16(addr, ch);
            *data = (uint8)((int)(2 * AVE_VALUE) - math.LimitationUInt8((uint8)newData, MIN_VALUE, MAX_VALUE));
            data += step;
        }
    }
    else
    {
        while (data <= last)
        {
            *data = (uint8)READ_DATA_ADC_16(addr, ch);
            data += step;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// first - если true, это первый вызов из последовательности, нужно подготовить память
// last - если true, это последний вызов из последовательности, нужно записать результаты в память.
static bool ReadRandomizeModeSave(bool first, bool last, bool onlySave)
{
    int bytesInChannel = BytesInChannel(&ds);

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
            if (ENABLED_DS_A)
            {
                memcpy(dataRandA, OUT_A, bytesInChannel);
            }
            if (ENABLED_DS_B)
            {
                memcpy(dataRandB, OUT_B, bytesInChannel);
            }
        }

        if (Tsm == NULL_TSHIFT)
        {
            return false;
        };

        if (START_MODE_SINGLE || SAMPLE_TYPE_IS_REAL)
        {
            FPGA_ClearData();

            // Очищаем массив для данных. После чтения точек несчитанные позиции будут равны нулю, что нужно для экстраполяции
            memset(dataRandA, 0, bytesInChannel);
            memset(dataRandB, 0, bytesInChannel);
        }

        // Теперь считаем данные
        TBase tBase = SET_TBASE;
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
        // Но считывать будем с адреса на 3 меньшего, чем расчётный. Лишние данные нужны, чтобы достроить те точки вначале, 
        // которые выпадают при программном сдвиге.
        // Процедуре чтения мы укажем сколько первых точек выбросить через параметр numSkipped
        uint16 addrFirstRead = *RD_ADDR_NSTOP + 16384 - (uint16)(bytesInChannel / step) - 1 - NUM_ADD_STEPS;

        //uint startRead = gTimerTics;

        ReadRandomizeChannel(B, addrFirstRead, &dataRandB[index], &dataRandB[FPGA_MAX_POINTS - 1], step, numSkipped);
        ReadRandomizeChannel(A, addrFirstRead, &dataRandA[index], &dataRandA[FPGA_MAX_POINTS - 1], step, numSkipped);
        
        if (START_MODE_SINGLE || SAMPLE_TYPE_IS_REAL)
        {
            processing.InterpolationSinX_X(dataRandA, bytesInChannel, tBase);
            processing.InterpolationSinX_X(dataRandB, bytesInChannel, tBase);
        }
    }

    // Теперь сохраняем данные обратно во внешнее ОЗУ
    if (last)
    {
        RAM_MemCpy16(dataRandA, RAM8(FPGA_DATA_A), bytesInChannel);
        RAM_MemCpy16(dataRandB, RAM8(FPGA_DATA_B), bytesInChannel);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// balance - свдиг точки вверх/вниз для балансировки
static void ReadChannel(uint8 *data, Channel ch, int length, uint16 nStop, bool shift, int balance)
{
    if (length == 0)
    {
        return;
    }
    *WR_PRED = nStop;
    *WR_ADDR_NSTOP = 0xffff;

    uint16 *p = (uint16 *)data;
    uint16 *endP = (uint16 *)&data[length];

    uint16 *address = ADDRESS_READ(ch);

    volatile uint16 dat = *address;

    if (shift)
    {
        *((uint8 *)p) = ((*address) >> 8);

        p = (uint16 *)(((uint8 *)p) + 1);
        endP -= 8;                          // Это нужно, чтбы не выйти за границу буфера - ведь мы сдвигаем данные на один байт
    }

    while (p < endP && FPGA_IN_PROCESS_OF_READ)
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

    if (shift)                              ///  \todo Во-первых, теряется один байт. Во-вторых, не очень-то красиво выглядит
    {
        while (p < (uint16 *)&data[length - 1])
        {
            *p++ = READ_DATA_ADC_16(address, ch);
        }
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

    ShiftOnePoint2Right(data, length);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Кажется, рассчитываем адрес последней записи
uint16 ReadNStop(void)
{
    return *RD_ADDR_NSTOP + 16384 - (uint16)BytesInChannel(&ds) / 2 - 1 - (uint16)gAddNStop;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ReadRealMode(uint8 *dataA, uint8 *dataB)
{
    FPGA_IN_PROCESS_OF_READ = 1;

    uint16 nStop = ReadNStop();

    bool shift = GetBit(ReadFlag(), FL_LAST_RECOR) == 0;    // Если true, будем сдвигать точки на одну

    int balanceA = 0;
    int balanceB = 0;

    if (NRST_BALANCE_ADC_TYPE_IS_HAND && 
        SET_PEAKDET_DIS)               // При включённом пиковом детекторе балансировка не нужна
    {
        balanceA = NRST_BALANCE_ADC_A;
        balanceB = NRST_BALANCE_ADC_B;
    }

    ReadChannel(dataA, A, BytesInChannel(&ds), nStop, shift, balanceA);
    
    ReadChannel(dataB, B, BytesInChannel(&ds), nStop, shift, balanceB);

    RAM_MemCpy16(dataA, RAM8(FPGA_DATA_A), FPGA_MAX_POINTS);
    RAM_MemCpy16(dataB, RAM8(FPGA_DATA_B), FPGA_MAX_POINTS);

    FPGA_IN_PROCESS_OF_READ = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ShiftOnePoint2Right(uint8 *data, int size)
{
    for (int i = size - 1; i >= 1; i--)
    {
        data[i] = data[i - 1];
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void InverseDataIsNecessary(Channel ch, uint8 *data)
{
    if (SET_INVERSE(ch))
    {
        for (int i = 0; i < FPGA_MAX_POINTS; i++)
        {
            data[i] = (uint8)((int)(2 * AVE_VALUE) - math.LimitationUInt8(data[i], MIN_VALUE, MAX_VALUE));
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DataReadSave(bool first, bool saveToStorage, bool onlySave)
{
    // В этой функции испльзуем память, предназначенную для хранения выходного сигнала, в качестве временного буфера.

    FPGA_IN_PROCESS_OF_READ = 1;
    if (IN_RANDOM_MODE)
    {
        ReadRandomizeModeSave(first, saveToStorage, onlySave);
    }
    else
    {
        ReadRealMode(OUT_A, OUT_B);
    }

    int numBytes = BytesInChannel(&ds);

    RAM_MemCpy16(RAM8(FPGA_DATA_A), OUT_A, numBytes);
    RAM_MemCpy16(RAM8(FPGA_DATA_B), OUT_B, numBytes);

    for (int i = 0; i < numBytes; i++)
    {
        LIMITATION(OUT_A[i], MIN_VALUE, MAX_VALUE);
        LIMITATION(OUT_B[i], MIN_VALUE, MAX_VALUE);
    }

    if (!IN_RANDOM_MODE)
    {
        InverseDataIsNecessary(A, OUT_A);
        InverseDataIsNecessary(B, OUT_B);
    }
    
    if (saveToStorage)
    {
        dS.AddData(OUT_A, OUT_B, ds);
    }

    if (TRIG_MODE_FIND_AUTO)
    {
        FPGA_FindAndSetTrigLevel();
    }

    FPGA_IN_PROCESS_OF_READ = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool ProcessingData(void)
{
    bool retValue = false;                          // Здесь будет true, когда данные считаются

    static const int numRead[] = {100, 50, 20, 10, 5};

    int num = IN_RANDOM_MODE ? numRead[SET_TBASE] / 2 : 1;

    if (IN_RANDOM_MODE)
    {
        dataRandA = (uint8 *)AllocMemForChannelFromHeap(A, 0);
        dataRandB = (uint8 *)AllocMemForChannelFromHeap(B, 0);

        if (SAMPLE_TYPE_IS_REAL)
        {
            num = 1;                                // Для реальной выборки ограничим количество считываний - нам надо только одно измерение.
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
            timeCompletePredTrig = gTimeMS;         // записываем время, когда оно произошло.
        }

        if (i > 0)
        {
            uint time = gTimeMS;
            // В рандомизаторных развёртках при повторных считываниях нужно подождать флага синхронизации
            while (GetBit(flag, FL_TRIG_READY) == 0 && GetBit(flag, FL_DATA_READY) == 0 && (gTimeMS - time) < 10)
            {                                                       // Это нужно для низких частот импульсов на входе
                flag = ReadFlag();
            }
            if (GetBit(flag, FL_DATA_READY) == 0) 
            {
                i = num;
            }
        }

        if (GetBit(flag, FL_TRIG_READY))                            // Если прошёл импульс синхронизации
        {
            if (GetBit(flag, FL_DATA_READY) == 1)                   // Проверяем готовность данных
            {
                fpgaStateWork = StateWorkFPGA_Stop;                 // И считываем, если данные готовы
                HAL_NVIC_DisableIRQ(EXTI2_IRQn);                    // Отключаем чтение точек
                DataReadSave(i == 0, i == num - 1, false);
                ProcessingAfterReadData();
                retValue = true;
            }
        }
        else if (START_MODE_AUTO)  // Если имупльса синхронизации нету, а включён автоматический режим синхронизации
        {
            if (gTimeMS - timeCompletePredTrig > TSHIFT_2_ABS(2, SET_TBASE) * 1000) // Если прошло больше времени, чем помещается в десяти клетках
            {
                if (IN_P2P_MODE)
                {
                    panel.EnableLEDTrig(false);                                     // В поточечном режиме просто тушим лампочку синхронизации
                }
                else
                {
                    FPGA_SwitchingTrig();                                           // В непоточечном даём принудительно даём синхронизацю
                }
            }
        }

        if (i == num)
        {
            DataReadSave(false, true, true);
            retValue = true;
            break;
        }

        if (PANEL_CONTROL_RECEIVE && IN_RANDOM_MODE)
        {
            DataReadSave(false, true, true);
            retValue = true;
            break;
        }
    }

    SAFE_FREE(dataRandA);
    SAFE_FREE(dataRandB);

    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ProcessingAfterReadData(void)
{
    if(!START_MODE_SINGLE)
    {
        if(IN_P2P_MODE && START_MODE_AUTO)                              // Если находимся в режиме поточечного вывода при автоматической синхронизации
        {
            if(!NEED_STOP_AFTER_READ_FRAME_2P2)
            {
                Timer_SetAndStartOnce(kTimerStartP2P, FPGA_Start, 1000);    // то откладываем следующий запуск, чтобы зафиксировать сигнал на экране
            }
        }
        else
        {
            FPGA_Start();
        }
    }
    else
    {
        panel.EnableLEDTrig(false);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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
    while (str->freq != 0.0f)
    {
        if (freq >= str->freq)
        {
            return str->tBase;
        }
        ++str;
    }

    return TBase_200ms;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Update(void)
{
    if (FPGA_IN_STATE_STOP)
    {
        return;
    }

    ReadFlag();

    if (gStateFPGA.needCalibration)              // Если вошли в режим калибровки -
    {
        FPGA_ProcedureCalibration();            // выполняем её.
        gStateFPGA.needCalibration = false;
    }
    if (FPGA_IN_PAUSE)
    {
        return;
    }

	if(FPGA_NEED_AUTO_FIND)
    {
		FPGA_AutoFind();
		return;
	}

    if(!FPGA_CAN_READ_DATA)
    {
        return;
    }

    if (!RECORDER_MODE)
    {
        ProcessingData();
    }

    FPGA_CAN_READ_DATA = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPressStartStopInP2P(void)
{
    if(Timer_IsRun(kTimerStartP2P))             // Если находимся в режиме поточечного вывода и в данный момент пауза после считывания очередного 
    {                                           // полного сигнала
        Timer_Disable(kTimerStartP2P);          // то останавливаем таймер, чтобы просмотреть сигнал
    }
    else                                        // Если идёт процесс сбора информации
    {
        if(FPGA_IN_STATE_STOP)
        {
            FPGA_Start();
        }
        else
        {   // то устанавливаем признак того, что после окончания не надо запускать следующий цикл
            NEED_STOP_AFTER_READ_FRAME_2P2 = !NEED_STOP_AFTER_READ_FRAME_2P2;
            FPGA_Stop(false);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_OnPressStartStop(void)
{
    if (!MODE_WORK_DIR || CONSOLE_IN_PAUSE)           // Если находимся не в режиме непосредственного считывания сигнала
    {
        return;
    }

    if (IN_P2P_MODE)
    {
        OnPressStartStopInP2P();
    }
    else if(FPGA_IN_STATE_STOP) 
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Stop(bool pause) 
{
    panel.EnableLEDTrig(false);
    HAL_NVIC_DisableIRQ(EXTI2_IRQn);        // Выключаем прерывание на чтение считанной точки
    fpgaStateWork = pause ? StateWorkFPGA_Pause : StateWorkFPGA_Stop;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Reset(void)
{
    bool needStart = FPGA_IS_RUNNING;
    FPGA_Stop(false);
    if (needStart)
    {
        FPGA_Start();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA_IsRunning(void)
{
    return !FPGA_IN_STATE_STOP;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_ClearData(void)
{
    RAM_MemClear(RAM8(FPGA_DATA_A), FPGA_MAX_POINTS);
    RAM_MemClear(RAM8(FPGA_DATA_B), FPGA_MAX_POINTS);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetNumberMeasuresForGates(int number)
{
    numberMeasuresForGates = number;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void StopTemporaryPause(void)
{
    FPGA_IN_PAUSE = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_TemporaryPause(void)
{
    FPGA_IN_PAUSE = 1;
    Timer_SetAndStartOnce(kTemporaryPauseFPGA, StopTemporaryPause, 100);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_FindAndSetTrigLevel(void)
{
    if (dS.NumElementsInStorage() == 0 || TRIGSOURCE_EXT)
    {
        return;
    }

    uint16 *dataA = 0;
    uint16 *dataB = 0;
    DataSettings *ds_ = 0;

    dS.GetDataFromEnd_RAM(0, &ds_, &dataA, &dataB);

    const uint16 *data = TRIGSOURCE_A ? dataA : dataB;

    int lastPoint = BytesInChannel(ds_) - 1;

    uint8 min = math.GetMinFromArray_RAM(data, 0, lastPoint);
    uint8 max = math.GetMaxFromArray_RAM(data, 0, lastPoint);

    uint8 aveValue = ((int)min + (int)max) / 2;

    static const float scale = (float)(TrigLevMax - TrigLevZero) / (float)(MAX_VALUE - AVE_VALUE) / 2.4f;

    int trigLev = (int)(TrigLevZero + scale * ((int)aveValue - AVE_VALUE) - (SET_RSHIFT(TRIGSOURCE) - RShiftZero));

    FPGA_SetTrigLev(TRIGSOURCE, (int16)trigLev);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_Write(TypeRecord type, uint16 *address, uint data, bool restart)
{
    // Если необходимо, сохраняем установленный режим на шине, чтобы затем его восстановить
    ModeFSMC modePrev = FSMC_GetMode();
    bool needRestore = modePrev != ModeFSMC_FPGA;
    if (type == RecordFPGA && needRestore)
    {
        FSMC_SetMode(ModeFSMC_FPGA);
    }

    
    FPGA_FIRST_AFTER_WRITE = 1;
    if (restart)
    {
        if (FPGA_IN_PROCESS_OF_READ)
        {
            FPGA_Stop(true);
            FPGA_IN_PROCESS_OF_READ = 0;
            Write(type, address, data);
            FPGA_Start();
        }
        else
        {
            if (!FPGA_IN_STATE_STOP)
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

    panel.EnableLEDTrig(false); // После каждой засылки выключаем лампочку синхронизации
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint16 PinSelect(uint16 *addrAnalog)
{
    const uint16 pins[4] = {GPIO_PIN_3, GPIO_PIN_6, GPIO_PIN_10, GPIO_PIN_15};
    return pins[(int)addrAnalog];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static GPIO_TypeDef* AddrGPIO(uint16 *addrAnalog)
{
    GPIO_TypeDef *gpio[4] = {GPIOD, GPIOD, GPIOG, GPIOG};
    return gpio[(int)addrAnalog];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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
        for (int i = ((int)addrAnalog <= (int)CS2 ? 15 : 23); i >= 0; i--)      // Хотя данных всего 16 бит, но передаём 24 - первые восемь нули - 
        {                                                                       // первый из них указывает на то, что производится запись
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTPos(TPos tPos)
{
extern void OnChanged_TPos(bool active);

    TPOS = tPos;
    OnChanged_TPos(true);
}

/** @}
 */
