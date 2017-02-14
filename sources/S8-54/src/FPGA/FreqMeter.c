#include "defines.h"
#include "FreqMeter.h"
#include "FPGA/FPGAtypes.h"
#include "Hardware/FSMC.h"
#include "Display/Painter.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static float freq = 0.0f;                   // Частота, намеренная альтерой.
static float prevFreq = 0.0f;
static volatile bool readPeriod = false;    // Установленный в true флаг означает, что частоту нужно считать по счётчику периода
static BitSet32 freqSet;
static BitSet32 periodSet;

static BitSet32 freqActual;                 // Здесь хранятся последние действительные
static BitSet32 periodActual;               // значения. Для вывода в режиме честотомера

static uint16 flag = 0;
static bool drawFreq = false;
static bool drawPeriod = false;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

        if (set.service.freqMeter.enable)
        {
            const uint16 maskTime[3] = {0, 1, 2};
            const uint16 maskFreqClc[4] = {0, (1 << 2), (1 << 3), ((1 << 3) + (1 << 2))};
            const uint16 maskPeriods[3] = {0, (1 << 4), (1 << 5)};

            data |= maskTime[set.service.freqMeter.timeCounting];
            data |= maskFreqClc[set.service.freqMeter.freqClc];
            data |= maskPeriods[set.service.freqMeter.numberPeriods];
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
static float FreqSetToFreq(BitSet32 *freq)
{
    const float k[3] = {10.0f, 1.0f, 0.1f};
    return set.service.freqMeter.enable ? (freq->word * k[set.service.freqMeter.timeCounting]) : (freq->word * 10.0f);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static float PeriodSetToFreq(BitSet32 *period)
{
    if (period->word == 0)
    {
        return 0.0f;
    }

    const float k[4] = {10e4f, 10e5f, 10e6f, 10e7f};
    const float kP[3] = {1.0f, 10.0f, 100.0f};

    return set.service.freqMeter.enable ? (k[set.service.freqMeter.freqClc] * kP[set.service.freqMeter.numberPeriods] / (float)period->word) : (10e5f / (float)period->word);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FreqMeter_Draw(int x, int y)
{
    if (set.service.freqMeter.enable == 0)
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

    bool condFreq = GetBit(flag, FL_OVERFLOW_FREQ) == 1 || drawFreq == false || freq == 0.0f;

    Painter_DrawText(x + 17, y + 1, condFreq ? EMPTY_STRING : Freq2StringAccuracy(freq, buffer, 6));

    freq = PeriodSetToFreq(&periodActual);

    bool condPeriod = GetBit(flag, FL_OVERFLOW_PERIOD) == 1 || drawPeriod == false || freq == 0.0f;

    Painter_SetColor(ColorTrig());
    Painter_DrawText(x + 17, y + 10, condPeriod ? EMPTY_STRING : Time2StringAccuracy(1.0f / freq, false, buffer, 6));
    Painter_DrawText(x + 71, y + 10, "");   // TODO Эта строка оставлена, потому что без неё артефакт изображения
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float FreqMeter_GetFreq(void)
{
    return freq;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FreqMeter_ReadFreq(void)
{

}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FreqMeter_ReadPeriod(void)
{

}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void ReadFreq(void)             // Чтение счётчика частоты производится после того, как бит 4 флага RD_FL установится в едицину
{                                      // После чтения автоматически запускается новый цикл счёта
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
void ReadPeriod(void)
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
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
