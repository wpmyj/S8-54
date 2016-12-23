#include "FPGA_Calibration.h"
#include "FPGA/FPGA.h" 
#include "FPGA/FPGA_Types.h"
#include "Display/Display.h"
#include "Display/DisplayPrimitives.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Panel/Panel.h"
#include "Hardware/FSMC.h"
#include "Hardware/Timer2.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/Timer.h"
#include "Log.h"


#include <limits.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

extern uint16* AddressRead(Channel ch);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsCalibrateChannel(Channel ch)
{
    return set.chan[ch].calibrationMode != CalibrationMode_Disable;
}

static void OnTimerDraw(void)
{
    Display_Update();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CreateCalibrationStruct(void)
{
    cal = malloc(sizeof(CalibrationStruct));
    memset(cal, 0, sizeof(CalibrationStruct));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void DeleteCalibrationStruct(void)
{
    free(cal);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void LoadSettingsCalcAddRShift(Channel ch)
{
    FPGA_SetRShift(ch, RShiftZero);
    FPGA_SetTBase(TBase_200us);
    FPGA_SetTrigSource(ch == A ? TrigSource_ChannelA : TrigSource_ChannelB);
    FPGA_SetTrigPolarity(TrigPolarity_Front);
    FPGA_SetTrigLev((TrigSource)ch, TrigLevZero);

    FPGA_SetCalibratorMode(Calibrator_GND);                 // Устанавливаем выход калибратора в ноль
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static bool RunFuncAndWaitFlag(pFuncVV func, uint8 flag)
{
    func();

    const uint timeWait = 1000;
    uint startTime = gTimerMS;

    while (GetBit(FSMC_READ(RD_FL), flag) == 0 && (gTimerMS - startTime > timeWait))
    {
    };
    if (gTimerMS - startTime > timeWait)
    {
        return false;
    }

    return true;
}

extern uint16 ReadNStop(void);

//------------------------------------------------------------------------------------------------------------------------------------------------------
// Измерить добавочное смещение канала по напряжению.
int16 CalculateAdditionRShift(Channel ch, Range range)
{
    FPGA_SetModeCouple(ch, ModeCouple_GND);
    FPGA_SetRange(ch, range);

    int numMeasures = 8;
    int sum = 0;
    int numPoints = 0;
    

    //Timer_PauseOnTime(200);

    for(int i = 0; i < numMeasures; i++)
    {
        uint startTime = gTimerMS;
        const uint timeWait = 5000;

        FPGA_WriteStartToHardware();

        while(GetBit(FSMC_READ(RD_FL), FL_PRED_READY) == 0 && (gTimerMS - startTime < timeWait)) {};
        if(gTimerMS - startTime > timeWait)         // Если прошло слишком много времени -
        {
            return ERROR_VALUE_INT16;               // выход с ошибкой
        }

        FPGA_SwitchingTrig();

        startTime = gTimerMS;

        while(GetBit(FSMC_READ(RD_FL), FL_DATA_READY) == 0 && (gTimerMS - startTime < timeWait)) {};
        if(gTimerMS - startTime > timeWait)         // Если прошло слишком много времени - 
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

    if(retValue < -(GRID_DELTA * 4) || retValue > (GRID_DELTA * 4)) // Проверка на выход за пределы двух клеток вверх и вниз
    {
        return ERROR_VALUE_INT16;
    }

    return retValue;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Измерить коэффициент калибровки канала по напряжению.
float CalculateStretchADC(Channel ch)
{
    FPGA_Write(RecordFPGA, WR_UPR, BINARY_U8(00000100), false);

    FPGA_SetRange(ch, (set.chan[ch].calibrationMode == CalibrationMode_x1) ? Range_500mV : Range_50mV);
    FPGA_SetRShift(ch, RShiftZero - 2700 * 4);    // Смещаем сигнал на 4 клетки вниз
    FPGA_SetModeCouple(ch, ModeCouple_DC);
    FPGA_SetTrigSource((TrigSource)ch);
    FPGA_SetTrigLev((TrigSource)ch, TrigLevZero + 40 * 4);

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
    
    float retValue = 160.0f / (aveMax - aveMin);

    if(retValue < 0.5f || retValue > 1.5f || numMIN < NUM_POINTS / 4 || numMAX < NUM_POINTS / 4 || numNot > NUM_POINTS / 4)
    {
        return ERROR_VALUE_FLOAT;
    }

    return retValue;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void AlignmentADC(void)
{
    cal->shiftADCA = (cal->deltaADCold[0] > 0) ? (int8)(cal->deltaADCold[0] + 0.5f) : (int8)(cal->deltaADCold[0] - 0.5f);
    set.chan[A].balanceShiftADC = cal->shiftADCA;
    cal->shiftADCB = (cal->deltaADCold[1] > 0) ? (int8)(cal->deltaADCold[1] + 0.5f) : (int8)(cal->deltaADCold[1] - 0.5f);
    set.chan[B].balanceShiftADC = cal->shiftADCB;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawParametersChannel(Channel ch, int eX, int eY, bool inProgress)
{
    Painter_SetColor(gColorFill);
    if(inProgress)
    {
        Painter_DrawText(eX, eY + 4, ch == 0 ? "КАНАЛ 1" : "КАНАЛ 2");
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
        char buffer[100] = {0};
        sprintf(buffer, "АЦП1 = %.2f/%.2f, АЦП2 = %.2f/%.2f, d = %.2f/%.2f", cal->avrADC1old[ch] - AVE_VALUE, cal->avrADC1[ch] - AVE_VALUE,
                cal->avrADC2old[ch] - AVE_VALUE, cal->avrADC2[ch] - AVE_VALUE,
                cal->deltaADCold[ch], cal->deltaADC[ch]);
        y += 10;
        Painter_DrawText(x, y, buffer);
        buffer[0] = 0;
        sprintf(buffer, "Расхождение AЦП = %.2f/%.2f %%", cal->deltaADCPercentsOld[ch], cal->deltaADCPercents[ch]);
        Painter_DrawText(x, y + 11, buffer);
        buffer[0] = 0;
        sprintf(buffer, "Записано %d", set.chan[ch].balanceShiftADC);
        Painter_DrawText(x, y + 19, buffer);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void FuncDrawAdditionRShift(int x, int y, int16 *addRShift)
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

    char buffer[100];
    sprintf(buffer, "Канал %d не скалиброван.", (int)ch + 1);
    Painter_DrawBigText(50, 70, 2, buffer);

    Painter_DrawStringInCenterRectC(0, 200, 319, 40, "Для продолжения нажмите кнопку ПУСК/СТОП", gColorFill);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция обновления экрана в режиме калибровки.
void FuncAttScreen(void)
{
    Painter_BeginScene(COLOR_BLACK);

    static bool first = true;
    static uint startTime = 0;
    if(first)
    {
        first = false;
        startTime = gTimerMS;
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
                    FuncDrawAdditionRShift(x, 55 + dY, &set.nr.rShiftAdd[A][i][ModeCouple_DC]);
                    FuncDrawAdditionRShift(x, 65 + dY, &set.nr.rShiftAdd[A][i][ModeCouple_AC]);
                }
                if (drawB)
                {
                    FuncDrawAdditionRShift(x, 80 + dY, &set.nr.rShiftAdd[B][i][ModeCouple_DC]);
                    FuncDrawAdditionRShift(x, 90 + dY, &set.nr.rShiftAdd[B][i][ModeCouple_AC]);
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
    char buffer[100];
    sprintf(buffer, "%.1f", (gTimerMS - startTime) / 1000.0f);
    Painter_DrawTextC(0, 0, buffer, COLOR_BLACK);

    Painter_EndScene();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateDeltaADC(Channel ch, float *avgADC1, float *avgADC2, float *delta)
{
    uint *startTime = (ch == A) ? &cal->startTimeChanA : &cal->startTimeChanB;
    *startTime = gTimerMS;

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

        bar->passedTime = (float)(gTimerMS - *startTime);
        bar->fullTime = bar->passedTime * (float)numCicles / (cicle + 1);
    }

    *avgADC1 /= (FPGA_MAX_POINTS * numCicles);
    *avgADC2 /= (FPGA_MAX_POINTS * numCicles);

    *delta = *avgADC1 - *avgADC2;

    return ((*avgADC1) - (*avgADC2)) / 255.0f * 100;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CalibrateStretch(Channel ch)
{
    float kStretch = CalculateStretchADC(ch);
    if (kStretch == ERROR_VALUE_FLOAT)
    {
        cal->isCalculateStretch[ch] = false;
        gStateFPGA.stateCalibration = (ch == A) ? StateCalibration_ErrorCalibrationA : StateCalibration_ErrorCalibrationB;
        Panel_WaitPressingButton();
    }
    else
    {
        cal->isCalculateStretch[ch] = true;
        set.nr.stretchADCtype = StretchADC_Real;
        SetStretchADC(ch, kStretch);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CalibrateAddRShift(Channel ch)
{
    LoadSettingsCalcAddRShift(ch);

    for (int range = 0; range < RangeSize; range++)
    {
        for (int i = 0; i < 2; i++)
        {
            set.nr.rShiftAdd[ch][range][i] = 0;
        }
        for (int i = 0; i < 2; i++)
        {
            FPGA_SetModeCouple(ch, (ModeCouple)i);    // 
        }
        for (int mode = 0; mode < 2; mode++)
        {
            set.nr.rShiftAdd[ch][range][mode] = CalculateAdditionRShift(ch, (Range)range);
            if (mode == ModeCouple_DC && range == Range_2mV)
            {
                set.nr.rShiftAdd[ch][range][mode] -= 5;
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
                if (set.nr.rShiftAdd[ch][range][mode] == ERROR_VALUE_INT16)
                {
                    set.nr.rShiftAdd[ch][range][mode] = 0;
                }
            }
        }
    }
}

extern void OnChange_ADC_Stretch_Mode(bool active);

//---------------------------------------------------------------------------------------------------------------------------------------------------]
static void RestoreSettings(Settings *savedSettings)
{
    int16 stretch[2][3];
    
    for(int ch = 0; ch < 2; ch++)
    {
        for(int type = 0; type < 3; type++)
        {
            stretch[ch][type] = set.nr.stretchADC[ch][type];
        }
    }

    StretchADCtype type = set.nr.stretchADCtype;
    
    Settings_RestoreState(savedSettings);

    for(int ch = 0; ch < 2; ch++)
    {
        for(int type = 0; type < 3; type++)
        {
            set.nr.stretchADC[ch][type] = stretch[ch][type];
        }
    }

    set.nr.stretchADCtype = type;

    OnChange_ADC_Stretch_Mode(true);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_ProcedureCalibration(void)
{
    CreateCalibrationStruct();
    
    Settings storedSettings;
    
    bool chanAenable = sChannel_Enabled(A);
    bool chanBenable = sChannel_Enabled(B);

    set.chan[A].enable = set.chan[B].enable = true;
    
    Display_SetDrawMode(DrawMode_Hand, FuncAttScreen);
    Timer2_SetAndEnable(kTimerDrawHandFunction, OnTimerDraw, 100);
    
    cal->barA.fullTime = cal->barA.passedTime = cal->barB.fullTime = cal->barB.passedTime = 0;
    
    Settings_SaveState(&storedSettings);    // Сохраняем текущее состояние.
    Panel_Disable();                        // Отлкючаем панель управления.
    
    bool run = true;
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
    
    RestoreSettings(&storedSettings);
    
    set.chan[A].balanceShiftADC = cal->shiftADCA;
    set.chan[B].balanceShiftADC = cal->shiftADCB;
    
    FPGA_SetRShift(A, RSHIFT_A);
    FPGA_SetRShift(B, RSHIFT_B);
    
    gStateFPGA.stateCalibration = StateCalibration_None;
    
    Panel_WaitPressingButton();
    
    WriteAdditionRShifts(A);
    WriteAdditionRShifts(B);
    
    Panel_Enable();
    Timer2_Disable(kTimerDrawHandFunction);
    Display_SetDrawMode(DrawMode_Auto, 0);
    
    set.chan[A].enable = chanAenable;
    set.chan[B].enable = chanBenable;
    
    DeleteCalibrationStruct();
    
    FPGA_OnPressStartStop();
}


static uint timeStart = 0;

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncDrawBalance(void)
{
    uint time = ((gTimerMS - timeStart) / 50) % 50;

    int width = 200;
    int height = 80;
    int x = 160 - width / 2;
    int y = 120 - height / 2;

    Painter_FillRegionC(x, y, width, height, gColorBack);
    Painter_DrawRectangleC(x, y, width, height, gColorFill);
    Painter_DrawStringInCenterRect(x, y, width, height - 20, "Балансировка канала");
    char buffer[100];
    buffer[0] = 0;
    for (uint i = 0; i < time; i++)
    {
        strcat(buffer, ".");
    }
    Painter_DrawStringInCenterRect(x, y + 20, width, height - 20, buffer);
    Painter_EndScene();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_BalanceChannel(Channel ch)
{
    timeStart = gTimerMS;

    Display_SetDrawMode(DrawMode_Hand, FuncDrawBalance);
    Timer_Enable(kTimerBalanceChannel, 10, Display_Update);

    CreateCalibrationStruct();
    Settings storedSettings;
    Settings_SaveState(&storedSettings);
    Panel_Disable();

    CalibrateAddRShift(ch);

    RestoreSettings(&storedSettings);

    CalibrationMode mode = set.chan[ch].calibrationMode;
    set.chan[ch].calibrationMode = CalibrationMode_x1;

    WriteAdditionRShifts(A);
    
    set.chan[ch].calibrationMode = mode;

    Panel_Enable();

    Timer_Disable(kTimerBalanceChannel);
    Display_SetDrawMode(DrawMode_Auto, 0);
    
    FPGA_OnPressStartStop();
}
