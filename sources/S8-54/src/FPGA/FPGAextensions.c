// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "DataStorage.h"
#include "fpgaExtensions.h"
#include "Log.h"
#include "Display/DisplayPrimitives.h"
#include "Display/PainterC.h"
#include "FPGA/fpga.h" 
#include "Hardware/FSMC.h"
#include "Hardware/Timer.h"
#include "Panel/Panel.h"
#include "Utils/Debug.h"
#include "Utils/Dictionary.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"


/** @addtogroup FPGA
 *  @{
 *  @addtogroup FPGA_Extensions
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern uint16* AddressRead(Channel ch);
extern bool ProcessingData(void);
extern uint8 ReadFlag(void);
extern TBase CalculateTBase(float freq);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsCalibrateChannel(Channel ch);
static void CreateCalibrationStruct(void);
static void DeleteCalibrationStruct(void);
static void LoadSettingsCalcAddRShift(Channel ch);
static void ReadPeriod(void);
/// \brief Чтение счётчика частоты производится после того, как бит 4 флага RD_FL установится в едицину. После чтения автоматически запускается 
/// новый цикл счёта.
static void ReadFreq(void);
static float PeriodSetToFreq(const BitSet32 *period);
static float FreqSetToFreq(const BitSet32 *freq);
static void RestoreSettingsForCalibration(const Settings *savedSettings);
static void WriteAdditionRShifts(Channel ch);
static void CalibrateChannel(Channel ch);
static void CalibrateAddRShift(Channel ch);
static float CalculateDeltaADC(Channel ch, float *avgADC1, float *avgADC2, float *delta);
/// Функция обновления экрана в режиме калибровки.
static void FuncAttScreen(void);
static void DrawMessageErrorCalibrate(Channel ch);
static void WriteStretch(Channel ch, int x, int y);
static void FuncDrawAdditionRShift(int x, int y, const int16 *addRShift);
static void DrawParametersChannel(Channel ch, int eX, int eY, bool inProgress);
static void AlignmentADC(void);
/// Измерить коэффициент калибровки канала по напряжению.
static float CalculateStretchADC(Channel ch);
/// Измерить добавочное смещение канала по напряжению.
static int16 CalculateAdditionRShift(Channel ch, Range range);
static bool RunFuncAndWaitFlag(pFuncVV func, uint8 flag);


/** @addtogroup AutoFind
 *  @{
 */
static bool FindWave(Channel ch);
static bool AccurateFindParams(Channel ch);
static bool FindParams(Channel ch, TBase *tBase);
static Range FindRange(Channel ch);                 ///< Возвращает RangeSize, если масштаб не найден.
static void FuncDrawAutoFind(void);
static void CalibrateStretch(Channel ch);
/** @}
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup AutoFind
 *  @{
 */

///  Структура используется для отрисовки прогресс-бара во время автоматического поиска сигнала.
typedef struct
{
    uint startTime;
    Settings settings;
} StrForAutoFind;

/** @}
 */

typedef struct
{
    float deltaADC[2];
    float deltaADCPercents[2];
    float avrADC1[2];
    float avrADC2[2];

    float deltaADCold[2];
    float deltaADCPercentsOld[2];
    float avrADC1old[2];
    float avrADC2old[2];

    bool isCalculateStretch[2];

    int8 shiftADCA;
    int8 shiftADCB;

    ProgressBar barA;       // Прогресс-бар для калибровки первого канала.
    ProgressBar barB;       // Прогресс-бар для калибровки второго канала.
    
    uint startTimeChanA;    // Время начала калибровки первого канала.
    uint startTimeChanB;    // Время начала калибровки второго канала.
} CalibrationStruct;

CalibrationStruct *cal;

static float freq = 0.0f;                   ///< Частота, намеренная альтерой.
static float prevFreq = 0.0f;
static volatile bool readPeriod = false;    ///< Установленный в true флаг означает, что частоту нужно считать по счётчику периода.
static BitSet32 freqSet;
static BitSet32 periodSet;

static BitSet32 freqActual;                 ///< Здесь хранятся последние действительные.
static BitSet32 periodActual;               ///< значения. Для вывода в режиме честотомера.

static uint16 flag = 0;
static bool drawFreq = false;
static bool drawPeriod = false;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsCalibrateChannel(Channel ch)
{
    return SET_CALIBR_MODE(ch) != CalibrationMode_Disable;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CreateCalibrationStruct(void)
{
    /** @todo перенести cal в extraMEM */
    cal = malloc(sizeof(CalibrationStruct));
    memset(cal, 0, sizeof(CalibrationStruct)); //-V575
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void DeleteCalibrationStruct(void)
{
    free(cal);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void LoadSettingsCalcAddRShift(Channel ch)
{
    FPGA_SetRShift(ch, RShiftZero);
    FPGA_SetTBase(TBase_200us);
    FPGA_SetTrigSource(ch == A ? TrigSource_A : TrigSource_B);
    FPGA_SetTrigPolarity(TrigPolarity_Front);
    FPGA_SetTrigLev((TrigSource)ch, TrigLevZero);

    FPGA_SetCalibratorMode(Calibrator_GND);                 // Устанавливаем выход калибратора в ноль
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static bool RunFuncAndWaitFlag(pFuncVV func, uint8 flag)
{
    func();

    const uint timeWait = 1000;
    uint startTime = gTimeMS; //-V101

    while (GetBit(FSMC_READ(RD_FL), flag) == 0 && (gTimeMS - startTime > timeWait))
    {
    };
    if (gTimeMS - startTime > timeWait)
    {
        return false;
    }

    return true;
}

extern uint16 ReadNStop(void);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int16 CalculateAdditionRShift(Channel ch, Range range)
{
    FPGA_SetModeCouple(ch, ModeCouple_GND);
    FPGA_SetRange(ch, range);

    int numMeasures = 8;
    int sum = 0;
    int numPoints = 0;
    
    for(int i = 0; i < numMeasures; i++)
    {
        volatile uint startTime = gTimeMS; //-V101
        const uint timeWait = 5000;

        FPGA_WriteStartToHardware();

        while(GetBit(FSMC_READ(RD_FL), FL_PRED_READY) == 0 && (gTimeMS - startTime < timeWait)) {};
        if(gTimeMS - startTime > timeWait)         // Если прошло слишком много времени -
        {
            return ERROR_VALUE_INT16;               // выход с ошибкой
        }

        FPGA_SwitchingTrig();

        startTime = gTimeMS; //-V101

        while(GetBit(FSMC_READ(RD_FL), FL_DATA_READY) == 0 && (gTimeMS - startTime < timeWait)) {};
        if(gTimeMS - startTime > timeWait)         // Если прошло слишком много времени - 
        {
            return ERROR_VALUE_INT16;               // выход с ошибкой.
        }

        *WR_PRED = ReadNStop();
        *WR_ADDR_NSTOP = 0xffff;

        uint16* addrRead = AddressRead(ch);

        for(int i = 0; i < 250; i += 2)
        {
            uint16 data = *addrRead;
            uint8 data0 = (uint8)data;
            uint8 data1 = (uint8)(data >> 8);
            sum += data0;
            sum += data1;
            numPoints += 2;
        }
    }

    float aveValue = (float)sum / numPoints;

    int16 retValue = (int16)(aveValue - (float)AVE_VALUE + (aveValue > AVE_VALUE ? 0.5f : -0.5f));

    if(retValue < -(GRID_DELTA * 4) || retValue > (GRID_DELTA * 4)) // Проверка на выход за пределы двух клеток вверх и вниз //-V112
    {
        return ERROR_VALUE_INT16;
    }

    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static float CalculateStretchADC(Channel ch)
{
    FPGA_Write(RecordFPGA, WR_UPR, BINARY_U8(00000100), false);

    FPGA_SetRange(ch, (SET_CALIBR_MODE(ch) == CalibrationMode_x1) ? Range_500mV : Range_50mV);
    FPGA_SetRShift(ch, RShiftZero - 2700 * 4);    // Смещаем сигнал на 4 клетки вниз //-V112
    FPGA_SetModeCouple(ch, ModeCouple_DC);
    FPGA_SetTrigSource((TrigSource)ch);
    FPGA_SetTrigLev((TrigSource)ch, TrigLevZero + 40 * 4); //-V112

    int numMeasures = 10;
    int sumMIN = 0;
    int numMIN = 0;
    int sumMAX = 0;
    int numMAX = 0;
    volatile int numNot = 0;

    Timer_PauseOnTime(100);

    for(int i = 0; i < numMeasures; i++)
    {
        if (!RunFuncAndWaitFlag(FPGA_WriteStartToHardware, FL_PRED_READY))
        {
            return ERROR_VALUE_FLOAT;
        }

        if (!RunFuncAndWaitFlag(FPGA_SwitchingTrig, FL_DATA_READY))
        {
            return ERROR_VALUE_FLOAT;
        }

        uint16 *addressRead = AddressRead(ch);

#define NUM_POINTS 1024

        for (int i = 0; i < NUM_POINTS / 2; i++)
        {
            uint16 value = *addressRead;

            for (int i = 0; i < 2; i++)
            {
                uint8 val = (uint8)value;
                if (val > AVE_VALUE + 60 && val <= MAX_VALUE)
                {
                    numMAX++;
                    sumMAX += val;
                }
                else if (val < AVE_VALUE - 60 && val >= MIN_VALUE)
                {
                    numMIN++;
                    sumMIN += val;
                }
                else
                {
                    numNot++;
                }               
                value = value >> 8;
            }
        }
    }

    float aveMin = (float)sumMIN / (float)numMIN;
    float aveMax = (float)sumMAX / (float)numMAX;
    
    float retValue = 160.0f / (aveMax - aveMin) * (MAX_VALUE - MIN_VALUE) / 200.0f;

    if(retValue < 0.5f || retValue > 1.5f || numMIN < NUM_POINTS / 4 || numMAX < NUM_POINTS / 4 || numNot > NUM_POINTS / 4) //-V112
    {
        return ERROR_VALUE_FLOAT;
    }

    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void AlignmentADC(void)
{
    cal->shiftADCA = (cal->deltaADCold[0] > 0) ? (int8)(cal->deltaADCold[0] + 0.5f) : (int8)(cal->deltaADCold[0] - 0.5f);
    SET_BALANCE_ADC_A = cal->shiftADCA;
    cal->shiftADCB = (cal->deltaADCold[1] > 0) ? (int8)(cal->deltaADCold[1] + 0.5f) : (int8)(cal->deltaADCold[1] - 0.5f);
    SET_BALANCE_ADC_B = cal->shiftADCB;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawParametersChannel(Channel ch, int eX, int eY, bool inProgress)
{
    Painter_SetColor(gColorFill);
    if(inProgress)
    {
        Painter_DrawText(eX, eY + 4, ch == 0 ? "КАНАЛ 1" : "КАНАЛ 2"); //-V112
        ProgressBar *bar = (ch == A) ? &cal->barA : &cal->barB;
        bar->width = 240;
        bar->height = 15;
        bar->y = eY;
        bar->x = 60;
        ProgressBar_Draw(bar);
    }

    if(Settings_DebugModeEnable())
    {
        int x = inProgress ? 5 : eX;
        int y = eY + (inProgress ? 110 : 0);
        Painter_DrawText(x, y, "Отклонение от нуля:");
        const int SIZE = 100;
        char buffer[SIZE] = {0};
        snprintf(buffer, SIZE, "АЦП1 = %.2f/%.2f, АЦП2 = %.2f/%.2f, d = %.2f/%.2f", cal->avrADC1old[ch] - AVE_VALUE, cal->avrADC1[ch] - AVE_VALUE,
                cal->avrADC2old[ch] - AVE_VALUE, cal->avrADC2[ch] - AVE_VALUE,
                cal->deltaADCold[ch], cal->deltaADC[ch]);
        y += 10;
        Painter_DrawText(x, y, buffer);
        buffer[0] = 0;
        snprintf(buffer, SIZE, "Расхождение AЦП = %.2f/%.2f %%", cal->deltaADCPercentsOld[ch], cal->deltaADCPercents[ch]);
        Painter_DrawText(x, y + 11, buffer);
        buffer[0] = 0;
        snprintf(buffer, SIZE, "Записано %d", SET_BALANCE_ADC(ch));
        Painter_DrawText(x, y + 19, buffer);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncDrawAdditionRShift(int x, int y, const int16 *addRShift)
{
    if (*addRShift == ERROR_VALUE_INT16)
    {
        Painter_DrawFormText(x, y, gColorFill, "Err");
    }
    else
    {
        Painter_DrawFormText(x, y, gColorFill, "%d", *addRShift);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteStretch(Channel ch, int x, int y)
{
    if (cal->isCalculateStretch[ch])
    {
        Painter_DrawFormText(x, y, gColorFill, "Коэффициент калибровки %dк : %f", (int)ch + 1, GetStretchADC(ch));
    }
    else
    {
        Painter_DrawFormText(x, y, COLOR_FLASH_01, "Калибровка %dк не прошла. Старый коэффициент %f", (int)ch + 1, GetStretchADC(ch));
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawMessageErrorCalibrate(Channel ch)
{
    Painter_SetColor(COLOR_FLASH_01);
    Painter_DrawBigText(100, 30, 2, "ВНИМАНИЕ !!!");

    const int SIZE = 100;
    char buffer[SIZE];
    snprintf(buffer, SIZE, "Канал %d не скалиброван.", (int)ch + 1);
    Painter_DrawBigText(50, 70, 2, buffer);

    Painter_DrawStringInCenterRectC(0, 200, 319, 40, "Для продолжения нажмите кнопку ПУСК/СТОП", gColorFill);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncAttScreen(void)
{
    Painter_BeginScene(COLOR_BLACK);

    static bool first = true;
    static uint startTime = 0;
    if(first)
    {
        first = false;
        startTime = gTimeMS; //-V101
    }
    int16 y = 10;
    Display_Clear();

    Painter_SetColor(gColorFill);

#define dX 20
#define dY -15

    switch(gStateFPGA.stateCalibration)
    {
        case StateCalibration_None:
        {

            Painter_DrawTextInRect(40 + dX, y + 25 + dY, SCREEN_WIDTH - 100, "Калибровка завершена. Нажмите любую кнопку, чтобы выйти из режима калибровки.");

            bool drawA = IsCalibrateChannel(A);
            bool drawB = IsCalibrateChannel(B);
            
            if (drawA) { Painter_DrawText(10 + dX, 55 + dY, "Поправка нуля 1к :"); }
            if (drawB) { Painter_DrawText(10 + dX, 80 + dY, "Поправка нуля 2к :"); }

            int x = 95 + dX;
            for(int i = 0; i < RangeSize; i++)
            {
                if (drawA)
                {
                    FuncDrawAdditionRShift(x, 55 + dY, &NRST_RSHIFT_ADD_A(i, ModeCouple_DC));
                    FuncDrawAdditionRShift(x, 65 + dY, &NRST_RSHIFT_ADD_A(i, ModeCouple_AC));
                }
                if (drawB)
                {
                    FuncDrawAdditionRShift(x, 80 + dY, &NRST_RSHIFT_ADD_B(i, ModeCouple_DC));
                    FuncDrawAdditionRShift(x, 90 + dY, &NRST_RSHIFT_ADD_B(i, ModeCouple_AC));
                }
                x += 16;
            }

            if (drawA)
            {
                WriteStretch(A, 10 + dX, 110 + dY);
            }
            if (drawB)
            {
                WriteStretch(B, 10 + dX, 130 + dY);
            }

            if (drawA) { DrawParametersChannel(A, 10 + dX, 150 + dY, false); }
            if (drawB) { DrawParametersChannel(B, 10 + dX, 200 + dY, false); }
        }
        break;

        case StateCalibration_ADCinProgress:
            if (IsCalibrateChannel(A)) { DrawParametersChannel(A, 5, 25, true); }
            if (IsCalibrateChannel(B)) { DrawParametersChannel(B, 5, 75, true); }
            break;

        case StateCalibration_RShiftAstart:
            Painter_DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, "Подключите ко входу канала 1 выход калибратора и нажмите кнопку ПУСК/СТОП. \
                                                                        Если вы не хотите калибровать первый канала, нажмите любую другую кнопку.");
            break;

        case StateCalibration_RShiftAinProgress:
            break;

        case StateCalibration_RShiftBstart:
            Painter_DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, "Подключите ко входу канала 2 выход калибратора и нажмите кнопку ПУСК/СТОП. \
                                                                        Если вы не хотите калибровать второй канал, нажмите любую другую кнопку.");
            break;

        case StateCalibration_RShiftBinProgress:
            break;

        case StateCalibration_ErrorCalibrationA:
            DrawMessageErrorCalibrate(A);
            break;

        case StateCalibration_ErrorCalibrationB:
            DrawMessageErrorCalibrate(B);
            break;
    }

    /*
    if(stateFPGA.stateCalibration == kNone || stateFPGA.stateCalibration == kRShift0start || stateFPGA.stateCalibration == kRShift1start) {
    x = 230;
    y = 187;
    int delta = 32;
    width = 80;
    height = 25;
    DrawRectangle(x, y, width, height, COLOR_BLACK);
    DrawStringInCenterRect(x, y, width, height, "ПРОДОЛЖИТЬ", COLOR_BLACK, false);
    DrawRectangle(x, y - delta, width, height, COLOR_BLACK);
    DrawStringInCenterRect(x, y - delta, width, height, "ОТМЕНИТЬ", COLOR_BLACK, false);
    }
    */
    const int SIZE = 100;
    char buffer[SIZE];
    snprintf(buffer, SIZE, "%.1f", (gTimeMS - startTime) / 1000.0f);
    Painter_DrawTextC(0, 0, buffer, COLOR_BLACK);

    Painter_EndScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static float CalculateDeltaADC(Channel ch, float *avgADC1, float *avgADC2, float *delta)
{
    uint *startTime = (ch == A) ? &cal->startTimeChanA : &cal->startTimeChanB;
    *startTime = gTimeMS; //-V101

    ProgressBar *bar = (ch == A) ? &cal->barA : &cal->barB;
    bar->passedTime = 0;
    bar->fullTime = 0;

    FPGA_SetTrigSource((TrigSource)ch);
    FPGA_SetTrigLev((TrigSource)ch, TrigLevZero);

    uint16 *address = AddressRead(ch);

    static const int numCicles = 10;
    for(int cicle = 0; cicle < numCicles; cicle++)
    {
        FPGA_WriteStartToHardware();
        while(GetBit(FSMC_READ(RD_FL), FL_PRED_READY) == 0) {};

        FPGA_SwitchingTrig();
        while(GetBit(FSMC_READ(RD_FL), FL_DATA_READY) == 0) {};

        for(int i = 0; i < 512; i++)
        {
            uint16 value = *address;
            *avgADC1 += (value & 0x0f);
            *avgADC2 += ((value >> 8) & 0x0f);
        }

        bar->passedTime = (float)(gTimeMS - *startTime);
        bar->fullTime = bar->passedTime * (float)numCicles / (cicle + 1);
    }

    *avgADC1 /= (FPGA_MAX_POINTS * numCicles);
    *avgADC2 /= (FPGA_MAX_POINTS * numCicles);

    *delta = *avgADC1 - *avgADC2;

    return ((*avgADC1) - (*avgADC2)) / 255.0f * 100;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CalibrateAddRShift(Channel ch)
{
    LoadSettingsCalcAddRShift(ch);

    for (int range = 0; range < RangeSize; range++)
    {
        for (int i = 0; i < 2; i++)
        {
            NRST_RSHIFT_ADD(ch, range, i) = 0;
        }
        for (int i = 0; i < 2; i++)
        {
            FPGA_SetModeCouple(ch, (ModeCouple)i);    // 
        }
        for (int mode = 0; mode < 2; mode++)
        {
            NRST_RSHIFT_ADD(ch, range, mode) = CalculateAdditionRShift(ch, (Range)range);
            if (mode == ModeCouple_DC && range == Range_2mV)
            {
                NRST_RSHIFT_ADD(ch, range, mode) -= 5;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CalibrateChannel(Channel ch)
{
    if (IsCalibrateChannel(ch))
    {
        if (Panel_WaitPressingButton() == B_Start)             // Ожидаем подтверждения или отмены процедуры калибровки канала.
        {
            gStateFPGA.stateCalibration = (ch == A) ? StateCalibration_RShiftAinProgress : StateCalibration_RShiftBinProgress;

            CalibrateAddRShift(ch);

            CalibrateStretch(ch);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteAdditionRShifts(Channel ch)
{
    if (IsCalibrateChannel(ch))
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < RangeSize; range++)
            {
                if (NRST_RSHIFT_ADD(ch, range, mode) == ERROR_VALUE_INT16)
                {
                    NRST_RSHIFT_ADD(ch, range, mode) = 0;
                }
            }
        }
    }
}

extern void OnChanged_ADC_Stretch_Mode(bool active);

//---------------------------------------------------------------------------------------------------------------------------------------------------]
static void RestoreSettingsForCalibration(const Settings *savedSettings)
{
    int16 stretch[2][3];
    
    for(int ch = 0; ch < 2; ch++)
    {
        for(int type = 0; type < 3; type++)
        {
            stretch[ch][type] = NRST_STRETCH_ADC(ch, type);
        }
    }

    StretchADCtype type = NRST_STRETCH_ADC_TYPE;
    
    Settings_RestoreState(savedSettings);

    for(int ch = 0; ch < 2; ch++)
    {
        for(int type = 0; type < 3; type++)
        {
            NRST_STRETCH_ADC(ch, type) = stretch[ch][type];
        }
    }

    NRST_STRETCH_ADC_TYPE = type;

    OnChanged_ADC_Stretch_Mode(true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_ProcedureCalibration(void)
{
    CreateCalibrationStruct();
    
    Settings storedSettings;
    
    bool chanAenable = sChannel_Enabled(A);
    bool chanBenable = sChannel_Enabled(B);

    SET_ENABLED_A = SET_ENABLED_B = true;
    
    Display_SetDrawMode(DrawMode_Hand, FuncAttScreen);
    
    cal->barA.fullTime = cal->barA.passedTime = cal->barB.fullTime = cal->barB.passedTime = 0;
    
    Settings_SaveState(&storedSettings);    // Сохраняем текущее состояние.
    Panel_Disable();                        // Отлкючаем панель управления.
    
    volatile bool run = true;
    while (run)
    {
        gStateFPGA.stateCalibration = StateCalibration_ADCinProgress;                  // Запускаем процедуру балансировки АЦП.
    
        FPGA_SetTBase(TBase_500us);
        FPGA_SetTShift(0);
        FPGA_SetRange(A, Range_500mV);
        FPGA_SetRange(B, Range_500mV);
        FPGA_SetRShift(A, RShiftZero);
        FPGA_SetRShift(B, RShiftZero);
        FPGA_SetModeCouple(A, ModeCouple_GND);
        FPGA_SetModeCouple(B, ModeCouple_GND);
    
        // Балансируем АЦП
        cal->deltaADCPercentsOld[A] = CalculateDeltaADC(A, &cal->avrADC1old[A], &cal->avrADC2old[A], &cal->deltaADCold[A]);
        cal->deltaADCPercentsOld[B] = CalculateDeltaADC(B, &cal->avrADC1old[B], &cal->avrADC2old[B], &cal->deltaADCold[B]);
    
        AlignmentADC();
    
        cal->deltaADCPercents[0] = CalculateDeltaADC(A, &cal->avrADC1[A], &cal->avrADC2[A], &cal->deltaADC[A]);
        cal->deltaADCPercents[1] = CalculateDeltaADC(B, &cal->avrADC1[B], &cal->avrADC2[B], &cal->deltaADC[B]);
    
        // Рассчитываем дополнительное смещение и растяжку первого канала
        gStateFPGA.stateCalibration = StateCalibration_RShiftAstart;
    
        CalibrateChannel(A);
    
        // Рассчитываем дополнительное смещение и растяжку второго канала
        gStateFPGA.stateCalibration = StateCalibration_RShiftBstart;
    
        CalibrateChannel(B);
    
        break;
    }
    
    RestoreSettingsForCalibration(&storedSettings);

    SET_BALANCE_ADC_A = cal->shiftADCA;
    SET_BALANCE_ADC_B = cal->shiftADCB;
    
    FPGA_SetRShift(A, SET_RSHIFT_A);
    FPGA_SetRShift(B, SET_RSHIFT_B);
    
    gStateFPGA.stateCalibration = StateCalibration_None;
    
    Panel_WaitPressingButton();
    
    WriteAdditionRShifts(A);
    WriteAdditionRShifts(B);
    
    Panel_Enable();
    Display_SetDrawMode(DrawMode_Auto, 0);
    
    SET_ENABLED_A = chanAenable;
    SET_ENABLED_B = chanBenable;
    
    DeleteCalibrationStruct();
    
    FPGA_OnPressStartStop();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_BalanceChannel(Channel ch)
{
    if (ch == A)
    {
        Display_FuncOnWaitStart(DICT(DBalance1ch), false);
    }
    else
    {
        Display_FuncOnWaitStart(DICT(DBalance2ch), false);
    }

    CreateCalibrationStruct();
    Settings storedSettings;
    Settings_SaveState(&storedSettings);
    Panel_Disable();

    CalibrateAddRShift(ch);

    RestoreSettingsForCalibration(&storedSettings);

    CalibrationMode mode = SET_CALIBR_MODE(ch);
    SET_CALIBR_MODE(ch) = CalibrationMode_x1;

    WriteAdditionRShifts(A);
    
    SET_CALIBR_MODE(ch) = mode;

    Panel_Enable();

    Display_FuncOnWaitStop();
    
    FPGA_OnPressStartStop();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FreqMeter_Init(void)
{
    drawFreq = false;
    drawPeriod = false;

    if (FSMC_InSetStateMode() || FSMC_GetMode() == ModeFSMC_Display)
    {
        FSMC_SetFuncitonAfterSetMode(FreqMeter_Init);
    }
    else
    {
        FSMC_SET_MODE(ModeFSMC_FPGA);

        uint16 data = 0;

        if (FREQ_METER_ENABLED)
        {
            const uint16 maskTime[3] = {0, 1, 2};
            const uint16 maskFreqClc[4] = {0, (1 << 2), (1 << 3), ((1 << 3) + (1 << 2))};
            const uint16 maskPeriods[3] = {0, (1 << 4), (1 << 5)};

            data |= maskTime[FREQ_METER_TIMECOUNTING];
            data |= maskFreqClc[FREQ_METER_FREQ_CLC];
            data |= maskPeriods[FREQ_METER_NUM_PERIODS];
        }
        else
        {
            SetBit_(data, 2);
        }

        *WR_FREQ_METER_PARAMS = data;

        FSMC_RESTORE_MODE();

        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static float FreqSetToFreq(const BitSet32 *freq)
{
    const float k[3] = {10.0f, 1.0f, 0.1f};
    return FREQ_METER_ENABLED ? (freq->word * k[FREQ_METER_TIMECOUNTING]) : (freq->word * 10.0f);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static float PeriodSetToFreq(const BitSet32 *period)
{
    if (period->word == 0)
    {
        return 0.0f;
    }

    const float k[4] = {10e4f, 10e5f, 10e6f, 10e7f};
    const float kP[3] = {1.0f, 10.0f, 100.0f};

    return FREQ_METER_ENABLED ? (k[FREQ_METER_FREQ_CLC] * kP[FREQ_METER_NUM_PERIODS] / (float)period->word) : (10e5f / (float)period->word);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FreqMeter_Draw(int x, int y)
{
    if (!FREQ_METER_ENABLED)
    {
        return;
    }

#define EMPTY_STRING "---.---"

    int width = 68;
    int height = 19;

    Painter_FillRegionC(x + 1, y + 1, width - 2, height - 2, gColorBack);
    Painter_DrawRectangleC(x, y, width, height, ColorTrig());

    Painter_DrawText(x + 2, y + 1, "F =");
    Painter_DrawText(x + 2, y + 10, "T");
    Painter_DrawText(x + 10, y + 10, "=");

    char buffer[30];
    float freq = FreqSetToFreq(&freqActual);

    bool condFreq = GetBit(flag, FL_OVERFLOW_FREQ) == 1 || drawFreq == false || freq == 0.0f; //-V550

    Painter_DrawText(x + 17, y + 1, condFreq ? EMPTY_STRING : Freq2StringAccuracy(freq, buffer, 6));

    freq = PeriodSetToFreq(&periodActual);

    bool condPeriod = GetBit(flag, FL_OVERFLOW_PERIOD) == 1 || drawPeriod == false || freq == 0.0f; //-V550

    Painter_SetColor(ColorTrig());
    Painter_DrawText(x + 17, y + 10, condPeriod ? EMPTY_STRING : Time2StringAccuracy(1.0f / freq, false, buffer, 6));

    /** @todo Последняя страка оставлена, потому что без неё получается артефакт изображения */
    Painter_DrawText(x + 71, y + 10, "");
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float FreqMeter_GetFreq(void)
{
    return freq;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ReadFreq(void)
{
    freqSet.halfWord[0] = *RD_FREQ_LOW;
    freqSet.halfWord[1] = *RD_FREQ_HI;

    if (freqSet.word < 1000)
    {
        readPeriod = true;
    }
    else
    {
        float fr = FreqSetToFreq(&freqSet);
        if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
        {
            freq = ERROR_VALUE_FLOAT;
        }
        else
        {
            freq = fr;
        }
        prevFreq = fr;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ReadPeriod(void)
{
    periodSet.halfWord[0] = *RD_PERIOD_LOW;
    periodSet.halfWord[1] = *RD_PERIOD_HI;

    float fr = PeriodSetToFreq(&periodSet);
    if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
    {
        freq = ERROR_VALUE_FLOAT;
    }
    else
    {
        freq = fr;
    }
    prevFreq = fr;
    readPeriod = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FreqMeter_Update(uint16 flag_)
{
    flag = flag_;

    bool freqReady = GetBit(flag, FL_FREQ_READY) == 1;
    bool periodReady = GetBit(flag, FL_PERIOD_READY) == 1;

    if (freqReady)
    {
        freqActual.halfWord[0] = *RD_FREQ_LOW;
        freqActual.halfWord[1] = *RD_FREQ_HI;
        drawFreq = true;

        if (!readPeriod)
        {
            ReadFreq();
        }
    }

    if (periodReady)
    {
        periodActual.halfWord[0] = *RD_PERIOD_LOW;
        periodActual.halfWord[1] = *RD_PERIOD_HI;
        drawPeriod = true;

        if (readPeriod)
        {
            ReadPeriod();
        }
    }
}

/** @addtogroup AutoFind
 *  @{
 */

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_AutoFind(void)
{
    /*
        Алгоритм поиска.
        1. Устанавливаем закрытый вход по каналу, синхронизация - ПС.
        2. Устанавливаем развёртку 20мс/дел, растяжку - 2мВ/дел.
        3. Запускаем АЦП.
        4. Ждём синхронизацию. Если синхронизации нет - переходим на второй канал. Если есть:
        5. Включаем пиковый детектр и считываем данные. Если считанные данные занимают по высоте меньше 2 клеток - переходим к поиску по второму 
            каналу. Если занимают больше 2 клеток:
        6. Устанавливаем вертикальный масштаб 5мВ, 10мВ, 20мВ и более, пока считанный сигнал полностью не впишется в экран.
        7. Переходим к поиску TBase.
    */

    /** \todo Оптимизировать алгоритм в плане скорости.\n
                1. Сначала находим время между флагами снихронизации.\n
                2. Затем устанавливаем такую развёртку, чтобы на длину памяти приходилось не более двух импульсов синхронизации.\n
                3. Это позволит быстрее считывать и обрабатывать данные.
    */

    /// \todo Сделать динамическую индикацию процесса поиска. Временно отключена, потому что глючит изображение. Иногда даже зависает прибор.

    MALLOC_EXTRAMEM(StrForAutoFind, p);             // Подготовим структуру, использующуюся для отрисовки прогресс-бара
    p->startTime = HAL_GetTick();
    p->settings = set;                              // Сохраняем текущие настройки - если сигнал найти не удастся, придётся восстановить их потом

    FuncDrawAutoFind();

    if (!FindWave(A))
    {
        if (!FindWave(B))
        {                                           // Если не удалось найти сигнал, то:
            Display_ShowWarning(SignalNotFound);    // выводим соотвествующее сообщение,
            set = p->settings;                      // восстанавливаем предыдущие настройки
            FPGA_LoadSettings();                    // и загружаем их в альтеру
        }
    }

    FREE_EXTRAMEM();

    NEED_AUTO_FIND = 0;

    FPGA_Start();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool FindWave(Channel ch)
{
    FPGA_SetTBase(TBase_20ms);
    sChannel_SetEnabled(ch, true);
    FPGA_SetTrigSource((TrigSource)ch);
    FPGA_SetTrigLev((TrigSource)ch, TrigLevZero);
    FPGA_SetRShift(ch, RShiftZero);
    FPGA_SetModeCouple(ch, ModeCouple_AC);
    FPGA_SetTrigInput(TrigInput_Full);

    Range range = FindRange(ch);

    if (range == RangeSize)
    {
        return false;
    }

    FPGA_SetRange(ch, range);

    return AccurateFindParams(ch);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
/// \brief Функция даёт старт АЦП и ждёт считывания информаии timeWait мс. Если данные получены, то функция возвращает true и их можно получить 
/// DS_GetData_RAM(ch, 0). Если данные не получены, функция возвращает false.
static bool ReadingCycle(uint timeWait)
{
    FPGA_Start();
    uint timeStart = HAL_GetTick();
    while(!ProcessingData())
    { 
        FuncDrawAutoFind();
        if ((HAL_GetTick() - timeStart) > timeWait)
        {
            FPGA_Stop(false);

            return false;
        }
    };
    FPGA_Stop(false);

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
/// Возвращает размах сигнала - разность между минимальным и максимальным значениями
static uint8 GetBound(uint8 data[512], uint8 *_min, uint8 *_max)
{
    uint8 min = 255;
    uint8 max = 0;
    for (int i = 0; i < 512; i++)
    {
        SET_MIN_IF_LESS(data[i], min);
        SET_MAX_IF_LARGER(data[i], max);
    }

    *_min = min;
    *_max = max;

    return max - min;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static Range FindRange(Channel ch)
{
    PeackDetMode peackDet = SET_PEACKDET;
    TPos tPos = TPOS;
    Range oldRange = SET_RANGE(ch);

    START_MODE = StartMode_Wait;                // Устанавливаем ждущий режим синхронизации, чтоб понять, есть ли сигнал

    FPGA_Stop(false);
    FPGA_SetPeackDetMode(PeackDet_Enable);
    FPGA_SetRange(ch, Range_2mV);
    FPGA_SetTPos(TPos_Left);

    int range = RangeSize;
    
    ReadingCycle(1000);

    if (ReadingCycle(2000))                                 // Если в течение 2 секунд не считан сигнал, то его нет на этом канале - выходим
    {
        uint8 min = 0;
        uint8 max = 0;

        uint8 bound = GetBound(DS_GetData_RAM(ch, 0), &min, &max);

        if (bound > (MAX_VALUE - MIN_VALUE) / 10.0 * 2)      // Если размах сигнала меньше двух клеток - тоже выходим
        {
            START_MODE = StartMode_Auto;

            for (range = 0; range < RangeSize; ++range)
            {
                /// \todo Этот алгоритм возвращает результат "Сигнал не найден", если при (range == RangeSize - 1) сигнал выходит за пределы экрана

                FPGA_SetRange(ch, (Range)range);

                ReadingCycle(10000);

                GetBound(DS_GetData_RAM(ch, 0), &min, &max);

                if (min > MIN_VALUE && max < MAX_VALUE)     // Если все значения внутри экрана
                {
                    break;                                  // То мы нашли наш Range - выходим из цикла
                }
            }
        }
    }

    FPGA_SetRange(ch, oldRange);
    FPGA_SetPeackDetMode(peackDet);
    FPGA_SetTPos(tPos);

    return (Range)range;
}

#undef NUM_MEASURES
#define NUM_MEASURES 3

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool AccurateFindParams(Channel ch)
{
    TBase tBase = TBaseSize;
    FindParams(ch, &tBase);
    
    return true;
}

#undef NUM_MEASURES

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncDrawAutoFind(void)
{
    ACCESS_EXTRAMEM(StrForAutoFind, s);

    int width = 220;
    int height = 60;
    int x = 160 - width / 2;
    int y = 120 - height / 2;
    Painter_BeginScene(gColorBack);
    Painter_FillRegionC(x, y, width, height, gColorBack);
    Painter_DrawRectangleC(x, y, width, height, gColorFill);
    Painter_DrawStringInCenterRect(x, y, width, height - 20, "Идёт поиск сигнала. Подождите");
    
    char buffer[101] = "";
    uint progress = ((HAL_GetTick() - s->startTime) / 20) % 80;
    for(int i = 0; i < progress; i++)
    {
        strcat(buffer, ".");
    }
    Painter_DrawStringInCenterRect(x, y + (height - 30), width, 20, buffer);

    Display_DrawConsole();

    Painter_EndScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool FindParams(Channel ch, TBase *tBase)
{
    FPGA_SetTrigInput(TrigInput_Full);

    FPGA_Start();
    while (GetBit(ReadFlag(), FL_FREQ_READY) == 0)
    {
        FuncDrawAutoFind();
    };
    FPGA_Stop(false);
    float freq = FreqMeter_GetFreq();

    FPGA_SetTrigInput(freq < 1e6f ? TrigInput_LPF : TrigInput_Full);

    FPGA_Start();
    while (GetBit(ReadFlag(), FL_FREQ_READY) == 0)
    {
    };
    FPGA_Stop(false);
    freq = FreqMeter_GetFreq();

    if (freq >= 50.0f)
    {
        *tBase = CalculateTBase(freq);
        if (*tBase >= MIN_TBASE_P2P)
        {
            *tBase = MIN_TBASE_P2P;
        }
        FPGA_SetTBase(*tBase);
        FPGA_Start();
        FPGA_SetTrigInput(freq < 500e3f ? TrigInput_LPF : TrigInput_HPF);
        return true;
    }
    else
    {
        FPGA_SetTrigInput(TrigInput_LPF);
        freq = FreqMeter_GetFreq();
        if (freq > 0.0f)
        {
            *tBase = CalculateTBase(freq);
            if (*tBase >= MIN_TBASE_P2P)
            {
                *tBase = MIN_TBASE_P2P;
            }
            FPGA_SetTBase(*tBase);
            Timer_PauseOnTime(10);
            FPGA_Start();
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CalibrateStretch(Channel ch)
{
    float kStretch = CalculateStretchADC(ch);
    if (kStretch == ERROR_VALUE_FLOAT) //-V550
    {
        cal->isCalculateStretch[ch] = false;
        gStateFPGA.stateCalibration = (ch == A) ? StateCalibration_ErrorCalibrationA : StateCalibration_ErrorCalibrationB;
        Panel_WaitPressingButton();
    }
    else
    {
        cal->isCalculateStretch[ch] = true;
        NRST_STRETCH_ADC_TYPE = StretchADC_Settings;
        SetStretchADC(ch, kStretch);
    }
}


/** @}  @}  @}
 */
