// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "ProcessingSignal.h"
#include "Globals.h"
#include "Math.h"
#include "GlobalFunctions.h"
#include "Log.h"
#include "Data/Data.h"
#include "Data/DataBuffer.h"
#include "Hardware/Timer.h"
#include "Hardware/FSMC.h"
#include "Hardware/RAM.h"
#include "Menu/Pages/PageMemory.h"
#include "Settings/Settings.h"
#include "Utils/Debug.h"
#include <math.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    float value[2];
} MeasureValue;


static float CalculateVoltageMax(Channel ch);
static float CalculateVoltageMin(Channel ch);
static float CalculateVoltagePic(Channel ch);
static float CalculateVoltageMaxSteady(Channel ch);
static float CalculateVoltageMinSteady(Channel ch);
static float CalculateVoltageAmpl(Channel ch);
static float CalculateVoltageAverage(Channel ch);
static float CalculateVoltageRMS(Channel ch);
static float CalculateVoltageVybrosPlus(Channel ch);
static float CalculateVoltageVybrosMinus(Channel ch);
static float CalculatePeriod(Channel ch);
static int   CalculatePeriodAccurately(Channel ch); ///< “очно вычисл€ет период или целое число периодов в точках сигнала.
static float CalculateFreq(Channel ch);
static float CalculateTimeNarastaniya(Channel ch);
static float CalculateTimeSpada(Channel ch);
static float CalculateDurationPlus(Channel ch);
static float CalculateDurationMinus(Channel ch);
static float CalculateSkvaznostPlus(Channel ch);
static float CalculateSkvaznostMinus(Channel ch);


static float CalculateMinRel(Channel ch);           ///< ¬озвращает минимальное значение относительного сигнала.
static float CalculateMinSteadyRel(Channel ch);     ///< ¬озвращает минимальное установившеес€ значение относительного сигнала.
static float CalculateMaxRel(Channel ch);           ///< ¬озвращает максимальное значение относительного сигнала.
static float CalculateMaxSteadyRel(Channel ch);     ///< ¬озвращает максимальное установившеес€ значение относительного сигнала.
static float CalculateAverageRel(Channel ch);       ///< ¬озвращает среденее значение относительного сигнала.
static float CalculatePicRel(Channel ch);
static float CalculateDelayPlus(Channel ch);
static float CalculateDelayMinus(Channel ch);
static float CalculatePhazaPlus(Channel ch);
static float CalculatePhazaMinus(Channel ch);
/// \brief Ќайти точку пересечени€ сигнала с горизонтальной линией, проведЄнной на уровне yLine. numItersection - пор€дковый номер пересечени€, 
/// начинаетс€ с 1. downToTop - если true, ищем пересечение сигнала со средней линией при прохождении из "-" в "+".
static float FindIntersectionWithHorLine(Channel ch, int numIntersection, bool downToUp, uint8 yLine);

static void CountedToCurrentSettings(void);
static void CountedRange(Channel ch);

static bool isSet = false;          ///< ≈сли true, то сигнал назначен.

static int firstByte = 0;
static int lastByte = 0;
static int numBytes = 0;

typedef struct
{
    char        *name;
    pFuncFU8    FuncCalculate;
    pFuncCFBC   FucnConvertate;
    bool        showSign;           ///< ≈сли true, нужно показывать знак.
} MeasureCalculate;


static const MeasureCalculate measures[Measure_NumMeasures] =
{
    {"", 0, 0, false},
    {"CalculateVoltageMax",         CalculateVoltageMax,           Voltage2String, true},
    {"CalculateVoltageMin",         CalculateVoltageMin,           Voltage2String, true},
    {"CalculateVoltagePic",         CalculateVoltagePic,           Voltage2String, false},
    {"CalculateVoltageMaxSteady",   CalculateVoltageMaxSteady,     Voltage2String, true},
    {"CalculateVoltageMinSteady",   CalculateVoltageMinSteady,     Voltage2String, true},
    {"CalculateVoltageAmpl",        CalculateVoltageAmpl,          Voltage2String, false},
    {"CalculateVoltageAverage",     CalculateVoltageAverage,       Voltage2String, true},
    {"CalculateVoltageRMS",         CalculateVoltageRMS,           Voltage2String, false},
    {"CalculateVoltageVybrosPlus",  CalculateVoltageVybrosPlus,    Voltage2String, false},
    {"CalculateVoltageVybrosMinus", CalculateVoltageVybrosMinus,   Voltage2String, false},
    {"CalculatePeriod",             CalculatePeriod,               Time2String, false},
    {"CalculateFreq",               CalculateFreq,                 Freq2String, false},
    {"CalculateTimeNarastaniya",    CalculateTimeNarastaniya,      Time2String, false},
    {"CalculateTimeSpada",          CalculateTimeSpada,            Time2String, false},
    {"CalculateDurationPlus",       CalculateDurationPlus,         Time2String, false},
    {"CalculateDurationPlus",       CalculateDurationMinus,        Time2String, false},
    {"CalculateSkvaznostPlus",      CalculateSkvaznostPlus,        FloatFract2String, false},
    {"CalculateSkvaznostMinus",     CalculateSkvaznostMinus,       FloatFract2String, false},
    {"CalculateDelayPlus",          CalculateDelayPlus,            Time2String, false},
    {"CalculateDelayMinus",         CalculateDelayMinus,           Time2String, false},
    {"CalculatePhazaPlus",          CalculatePhazaPlus,            Phase2String, false},
    {"CalculatePhazaMinus",         CalculatePhazaMinus,           Phase2String, false}
};


static MeasureValue values[Measure_NumMeasures] = {{0.0f, 0.0f}};

int markerHor[NumChannels][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};
int markerVert[NumChannels][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};

static bool maxIsCalculating[2] = {false, false};
static bool minIsCalculating[2] = {false, false};
static bool maxSteadyIsCalculating[2] = {false, false};
static bool minSteadyIsCalculating[2] = {false, false};
static bool aveIsCalculating[2] = {false, false};
static bool periodIsCaclulating[2] = {false, false};
static bool periodAccurateIsCalculating[2];
static bool picIsCalculating[2] = {false, false};

#define EXIT_IF_ERROR_FLOAT(x)      if((x) == ERROR_VALUE_FLOAT || isnan(x))                                            return ERROR_VALUE_FLOAT;
#define EXIT_IF_ERRORS_FLOAT(x, y)  if((x) == ERROR_VALUE_FLOAT || (y) == ERROR_VALUE_FLOAT || isnan(x) || isnan(y))    return ERROR_VALUE_FLOAT;
//#define EXIT_IF_ERROR_UINT8(x)      if((x) == ERROR_VALUE_UINT8)                                                      return ERROR_VALUE_FLOAT;
//#define EXIT_IF_ERRORS_UINT8(x, y)  if((x) == ERROR_VALUE_UINT8 || (y) == ERROR_VALUE_UINT8)                          return ERROR_VALUE_FLOAT;
#define EXIT_IF_ERROR_INT(x)        if((x) == ERROR_VALUE_INT)                                                          return ERROR_VALUE_FLOAT;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Processing_CalculateMeasures(void)
{
    if(!SHOW_MEASURES || !isSet)
    {
        return;
    }
    
    int length = BYTES_IN_CHANNEL_DS;

    maxIsCalculating[0] = maxIsCalculating[1] = maxSteadyIsCalculating[0] = maxSteadyIsCalculating[1] = false;
    minIsCalculating[0] = minIsCalculating[1] = minSteadyIsCalculating[0] = minSteadyIsCalculating[1] = false;
    aveIsCalculating[0] = aveIsCalculating[1] = false;
    periodIsCaclulating[0] = periodIsCaclulating[1] = false;
    periodAccurateIsCalculating[0] = periodAccurateIsCalculating[1] = false;
    picIsCalculating[0] = picIsCalculating[1] = false;

    for(int str = 0; str < Measure_NumRows(); str++)
    {
        for(int elem = 0; elem < Measure_NumCols(); elem++)
        {
            Measure meas = Measure_Type(str, elem);
            pFuncFU8 func = measures[meas].FuncCalculate;
            if(func)
            {
                if(meas == MARKED_MEAS || MARKED_MEAS == Measure_None)
                {
                    markerVert[A][0] = markerVert[A][1] = markerVert[B][0] = markerVert[B][1] = ERROR_VALUE_INT;
                    markerHor[A][0] = markerHor[A][1] = markerHor[B][0] = markerHor[B][1] = ERROR_VALUE_INT;
                }
                if((SOURCE_MEASURE_A || SOURCE_MEASURE_A_B) && SET_ENABLED_A)
                {
                    values[meas].value[A] = func(A);
                }
                if((SOURCE_MEASURE_B || SOURCE_MEASURE_A_B) && SET_ENABLED_B)
                {
                    values[meas].value[B] = func(B);
                }
            }
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageMax(Channel ch)
{
    float max = CalculateMaxRel(ch);
    EXIT_IF_ERROR_FLOAT(max);
    if(MARKED_MEAS == Measure_VoltageMax)
    {
        markerHor[ch][0] = (int)max;                           // «десь не округл€ем, потому что max может быть только целым
    }

    return POINT_2_VOLTAGE(max, RANGE_DS(ch), RSHIFT_DS(ch));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageMin(Channel ch)
{
    float min = CalculateMinRel(ch);
    EXIT_IF_ERROR_FLOAT(min);
    if(MARKED_MEAS == Measure_VoltageMin)
    {
        markerHor[ch][0] = (int)min;                           // «десь не округл€ем, потому что min может быть только целым
    }
    
    return POINT_2_VOLTAGE(min, RANGE_DS(ch),RSHIFT_DS(ch));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltagePic(Channel ch)
{
    float max = CalculateVoltageMax(ch);
    float min = CalculateVoltageMin(ch);

    EXIT_IF_ERRORS_FLOAT(min, max);

    if(MARKED_MEAS == Measure_VoltagePic)
    {
        markerHor[ch][0] = (int)CalculateMaxRel(ch);
        markerHor[ch][1] = (int)CalculateMinRel(ch);
    }
    return max - min;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageMinSteady(Channel ch)
{
    float min = CalculateMinSteadyRel(ch);
    EXIT_IF_ERROR_FLOAT(min);
    if(MARKED_MEAS == Measure_VoltageMinSteady)
    {
        markerHor[ch][0] = (int)ROUND(min);
    }

    return POINT_2_VOLTAGE(min, RANGE_DS(ch), RSHIFT_DS(ch));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageMaxSteady(Channel ch)
{
    float max = CalculateMaxSteadyRel(ch);

    EXIT_IF_ERROR_FLOAT(max);

    if(MARKED_MEAS == Measure_VoltageMaxSteady)
    {
        markerHor[ch][0] = (int)max;
    }

    return POINT_2_VOLTAGE(max, RANGE_DS(ch), RSHIFT_DS(ch));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageVybrosPlus(Channel ch)
{
    float max = CalculateMaxRel(ch);
    float maxSteady = CalculateMaxSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(max, maxSteady);

    if (MARKED_MEAS == Measure_VoltageVybrosPlus)
    {
        markerHor[ch][0] = (int)max;
        markerHor[ch][1] = (int)maxSteady;
    }

    uint rShift = RSHIFT_DS(ch);
    return fabsf(POINT_2_VOLTAGE(maxSteady, RANGE_DS(ch), rShift) - POINT_2_VOLTAGE(max, RANGE_DS(ch), rShift));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageVybrosMinus(Channel ch)
{
    float min = CalculateMinRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);
    EXIT_IF_ERRORS_FLOAT(min, minSteady);

    if (MARKED_MEAS == Measure_VoltageVybrosMinus)
    {
        markerHor[ch][0] = (int)min;
        markerHor[ch][1] = (int)minSteady;
    }

    uint16 rShift = RSHIFT_DS(ch);
    return fabsf(POINT_2_VOLTAGE(minSteady, RANGE_DS(ch), rShift) - POINT_2_VOLTAGE(min, RANGE_DS(ch), rShift));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageAmpl(Channel ch)
{
    float max = CalculateVoltageMaxSteady(ch);
    float min = CalculateVoltageMinSteady(ch);

    EXIT_IF_ERRORS_FLOAT(min, max);

    if(MARKED_MEAS == Measure_VoltageAmpl)
    {
        markerHor[ch][0] = (int)CalculateMaxSteadyRel(ch);
        markerHor[ch][1] = (int)CalculateMinSteadyRel(ch);
    }
    return max - min;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
#define CHOICE_BUFFER (dataIN[ch])


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageAverage(Channel ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    int sum = 0;

    uint8 *data = &CHOICE_BUFFER[firstByte];

    for(int i = 0; i < period; i++)
    {
        sum += *data++;
    }

    uint8 aveRel = (uint8)((float)sum / period);

    if(MARKED_MEAS == Measure_VoltageAverage)
    {
        markerHor[ch][0] = aveRel;
    }

    return POINT_2_VOLTAGE(aveRel, RANGE_DS(ch), RSHIFT_DS(ch));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageRMS(Channel ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    float rms = 0.0f;
    uint16 rShift = RSHIFT_DS(ch);

    uint8 *dataIn = CHOICE_BUFFER;

    for(int i = firstByte; i < firstByte + period; i++)
    {
        float volts = POINT_2_VOLTAGE(dataIn[i], RANGE_DS(ch), rShift);
        rms +=  volts * volts;
    }

    if(MARKED_MEAS == Measure_VoltageRMS)
    {
        markerHor[ch][0] = Math_VoltageToPoint(sqrtf(rms / period), RANGE_DS(ch), rShift);
    }

    return sqrtf(rms / period);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculatePeriod(Channel ch)
{
    static float period[2] = {0.0f, 0.0f};

    if(!periodIsCaclulating[ch])
    {
        float aveValue = CalculateAverageRel(ch);
        if(aveValue == ERROR_VALUE_UINT8)
        {
            period[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            float intersectionDownToTop = FindIntersectionWithHorLine(ch, 1, true, (uint8)aveValue);
            float intersectionTopToDown = FindIntersectionWithHorLine(ch, 1, false, (uint8)aveValue);

            EXIT_IF_ERRORS_FLOAT(intersectionDownToTop, intersectionTopToDown);

            bool firstDownToTop = intersectionDownToTop < intersectionTopToDown;
            float firstIntersection = firstDownToTop ? intersectionDownToTop : intersectionTopToDown;
            float secondIntersection = FindIntersectionWithHorLine(ch, 2, firstDownToTop, (uint8)aveValue);

            EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

            float per = TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, SET_TBASE);
            
            if(SET_PEAKDET_EN)
            {
                per *= 0.5f;
            }

            period[ch] = per;

            periodIsCaclulating[ch] = true;
        }
    }

    return period[ch];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
#define EXIT_FROM_PERIOD_ACCURACY           \
    period[ch] = ERROR_VALUE_INT;           \
    periodAccurateIsCalculating[ch] = true; \
    free(sums);                             \
    return period[ch];


//----------------------------------------------------------------------------------------------------------------------------------------------------
int CalculatePeriodAccurately(Channel ch)
{
    static int period[2];

    int *sums = malloc(FPGA_MAX_POINTS);

    uint8 *dataIn = CHOICE_BUFFER;

    if(!periodAccurateIsCalculating[ch])
    {
        period[ch] = 0;
        float pic = CalculatePicRel(ch);

        if(pic == ERROR_VALUE_FLOAT)
        {
            EXIT_FROM_PERIOD_ACCURACY
        }
        int delta = (int)(pic * 5.0f);
        sums[firstByte] = dataIn[firstByte];

        int i = firstByte + 1;
        int *sum = &sums[i];
        uint8 *data = &dataIn[i];
        uint8 *end = &dataIn[lastByte];

        while (data < end)
        {
            uint8 point = *data++;
            if(point < MIN_VALUE || point >= MAX_VALUE)
            {
                EXIT_FROM_PERIOD_ACCURACY
            }
            *sum = *(sum - 1) + point;
            sum++;
        }

        int addShift = firstByte - 1;
        int maxPeriod = (int)(numBytes * 0.95f);

        for(int nextPeriod = 10; nextPeriod < maxPeriod; nextPeriod++)
        {
            int sum = sums[addShift + nextPeriod];

            int maxDelta = 0;
            int maxStart = numBytes - nextPeriod;

            int *pSums = &sums[firstByte + 1];
            for(int start = 1; start < maxStart; start++)
            {
                int nextSum = *(pSums + nextPeriod) - (*pSums);
                pSums++;

                int nextDelta = nextSum - sum;
                if (nextSum < sum)
                {
                    nextDelta = -nextDelta;
                }

                if(nextDelta > delta)
                {
                    maxDelta = delta + 1;
                    break;
                }
                else if(nextDelta > maxDelta)
                {
                    maxDelta = nextDelta;
                }
            }

            if(maxDelta < delta)
            {
                delta = maxDelta;
                period[ch] = nextPeriod;
            }
        }

        if(period[ch] == 0)
        {
            period[ch] = ERROR_VALUE_INT;
        }
        periodAccurateIsCalculating[ch] = true;
    }

    free(sums);

    return period[ch];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateFreq(Channel ch)
{
    float period = CalculatePeriod(ch);
    return period == ERROR_VALUE_FLOAT ? ERROR_VALUE_FLOAT : 1.0f / period;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float FindIntersectionWithHorLine(Channel ch, int numIntersection, bool downToUp, uint8 yLine)
{
    int num = 0;
    int x = firstByte;
    int compValue = lastByte - 1;
    int step = SET_PEAKDET_EN ? 2 : 1;

    uint8 *data = &CHOICE_BUFFER[0];

    if(downToUp)
    {
        while((num < numIntersection) && (x < compValue))
        {
            if(data[x] < yLine && data[x + step] >= yLine)
            {
                num++;
            }
            x += step;
        }
    }
    else
    {
        while((num < numIntersection) && (x < compValue))
        {
            if(data[x] > yLine && data[x + step] <= yLine)
            {
                num++;
            }
            x += step;
        }
    }
    x -= step;

    if (num < numIntersection)
    {
        return ERROR_VALUE_FLOAT;
    }
    
    return Math_GetIntersectionWithHorizontalLine(x, data[x], x + step, data[x + step], yLine);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateDurationPlus(Channel ch)
{
    float aveValue = CalculateAverageRel(ch);
    EXIT_IF_ERROR_FLOAT(aveValue);

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, true, (uint8)aveValue);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, false, (uint8)aveValue);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, false, (uint8)aveValue);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, TBASE_DS);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateDurationMinus(Channel ch)
{
    float aveValue = CalculateAverageRel(ch);
    EXIT_IF_ERROR_FLOAT(aveValue);

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, false, (uint8)aveValue);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, true, (uint8)aveValue);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, true, (uint8)aveValue);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, TBASE_DS);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateTimeNarastaniya(Channel ch)   /** \todo «десь, возможно, нужно увеличить точность - брать не целые значени рассто€ний между 
                                              отсчЄтами по времени, а рассчитывать пересечени€ линий. */
{
    float maxSteady = CalculateMaxSteadyRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(maxSteady, minSteady);

    float value01 = (maxSteady - minSteady) * 0.1f;
    float max09 = maxSteady - value01;
    float min01 = minSteady + value01;

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, true, (uint8)min01);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, true, (uint8)max09);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);
    
    if (secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, true, (uint8)max09);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    float retValue = TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, TBASE_DS);

    if (MARKED_MEAS == Measure_TimeNarastaniya)
    {
        markerHor[ch][0] = (int)max09;
        markerHor[ch][1] = (int)min01;
        markerVert[ch][0] = (int)((int16)firstIntersection - SHIFT_IN_MEMORY);
        markerVert[ch][1] = (int)((int16)secondIntersection - SHIFT_IN_MEMORY);
    }

    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateTimeSpada(Channel ch)        /// \todo јналогично времени нарастани€
{
    float maxSteady = CalculateMaxSteadyRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(maxSteady, minSteady);

    float value01 = (maxSteady - minSteady) * 0.1f;
    float max09 = maxSteady - value01;
    float min01 = minSteady + value01;

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, false, (uint8)max09);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, false, (uint8)min01);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if (secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, false, (uint8)min01);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    float retValue = TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, TBASE_DS);

    if (MARKED_MEAS == Measure_TimeSpada)
    {
        markerHor[ch][0] = (int)max09;
        markerHor[ch][1] = (int)min01;
        markerVert[ch][0] = (int)((int16)firstIntersection - SHIFT_IN_MEMORY);
        markerVert[ch][1] = (int)((int16)secondIntersection - SHIFT_IN_MEMORY);
    }

    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateSkvaznostPlus(Channel ch)
{
    float period = CalculatePeriod(ch);
    float duration = CalculateDurationPlus(ch);

    EXIT_IF_ERRORS_FLOAT(period, duration);

    return period / duration;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateSkvaznostMinus(Channel ch)
{
    float period = CalculatePeriod(ch);
    float duration = CalculateDurationMinus(ch);

    EXIT_IF_ERRORS_FLOAT(period, duration);

    return period / duration;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateMinSteadyRel(Channel ch)
{
    static float min[2] = {255.0f, 255.0f};

    uint8 *dataIn = CHOICE_BUFFER;

    if(!minSteadyIsCalculating[ch])
    {
        float aveValue = CalculateAverageRel(ch);
        if(aveValue == ERROR_VALUE_FLOAT)
        {
            min[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            int sum = 0;
            int numSums = 0;

            uint8 *data = &dataIn[firstByte];
            const uint8 * const end = &dataIn[lastByte];
            while(data <= end)
            {
                uint8 d = *data++;
                if(d < aveValue)
                {
                    sum += d;
                    numSums++;
                }
            }
            min[ch] = (float)sum / numSums;
            int numMin = numSums;

            int numDeleted = 0;

            float pic = CalculatePicRel(ch);
            if (pic == ERROR_VALUE_FLOAT)
            {
                min[ch] = ERROR_VALUE_FLOAT;
            }
            else
            {
                float value = pic / 9.0f;

                data = &dataIn[firstByte];
                float _min = min[ch];
                while (data <= end)
                {
                    uint8 d = *data++;
                    if (d < aveValue)
                    {
                        if (d < _min)
                        {
                            if (_min - d > value)
                            {
                                sum -= d;
                                --numSums;
                                ++numDeleted;
                            }
                        }
                        else if (d - _min > value)
                        {
                            sum -= d;
                            --numSums;
                            ++numDeleted;
                        }
                    }
                }
                min[ch] = (numDeleted > numMin / 2.0f) ? CalculateMinRel(ch) : (float)sum / numSums;
            }
        }
        minSteadyIsCalculating[ch] = true;
    }

    return min[ch];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateMaxSteadyRel(Channel ch)
{
    static float max[2] = {255.0f, 255.0f};

    if(!maxSteadyIsCalculating[ch])
    {
        uint8 *dataIn = CHOICE_BUFFER;

        float aveValue = CalculateAverageRel(ch);
        
        if(aveValue == ERROR_VALUE_FLOAT)
        {
            max[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            int sum = 0;
            int numSums = 0;
            uint8 *data = &dataIn[firstByte];
            const uint8 * const end = &dataIn[lastByte];
            while (data <= end)
            {
                uint8 d = *data++;
                if(d > aveValue)
                {
                    sum += d;
                    numSums++;
                }
            }
            max[ch] = (float)sum / numSums;
            int numMax = numSums;

            int numDeleted = 0;

            float pic = CalculatePicRel(ch);

            if (pic == ERROR_VALUE_FLOAT)
            {
                max[ch] = ERROR_VALUE_FLOAT;
            }
            else
            {
                float value = pic / 9.0f;

                data = &dataIn[firstByte];
                uint8 _max = (uint8)max[ch];
                while (data <= end)
                {
                    uint8 d = *data++;
                    if (d > aveValue)
                    {
                        if (d > _max)
                        {
                            if (d - _max > value)
                            {
                                sum -= d;
                                numSums--;
                                numDeleted++;
                            }
                        }
                        else if (_max - d > value)
                        {
                            sum -= d;
                            numSums--;
                            numDeleted++;
                        }
                    }
                }
                max[ch] = (numDeleted > numMax / 2) ? CalculateMaxRel(ch) : (float)sum / numSums;
            }
        }
        maxSteadyIsCalculating[ch] = true;
    }

    return max[ch];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateMaxRel(Channel ch)
{
    static float max[2] = {0.0f, 0.0f};

    if(!maxIsCalculating[ch])
    {
        uint8 val = Math_GetMaxFromArrayWithErrorCode(CHOICE_BUFFER, firstByte, lastByte);
        max[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        maxIsCalculating[ch] = true;
    }

    return max[ch];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateMinRel(Channel ch)
{
    static float min[2] = {255.0f, 255.0f};

    if (!minIsCalculating[ch])
    {
        uint8 val = Math_GetMinFromArrayWithErrorCode(CHOICE_BUFFER, firstByte, lastByte);
        min[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        minIsCalculating[ch] = true;
    }

    return min[ch];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateAverageRel(Channel ch)
{
    static float ave[2] = {0.0f, 0.0f};

    if(!aveIsCalculating[ch])
    {
        float min = CalculateMinRel(ch);
        float max = CalculateMaxRel(ch);
        ave[ch] = (min == ERROR_VALUE_FLOAT || max == ERROR_VALUE_FLOAT) ? ERROR_VALUE_FLOAT : (min + max) / 2.0f;
        aveIsCalculating[ch] = true;
    }
    return ave[ch];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculatePicRel(Channel ch)
{
    static float pic[2] = {0.0f, 0.0f};

    if(!picIsCalculating[ch])
    {
        float min = CalculateMinRel(ch);
        float max = CalculateMaxRel(ch);
        pic[ch] = (min == ERROR_VALUE_FLOAT || max == ERROR_VALUE_FLOAT) ? ERROR_VALUE_FLOAT : max - min;
        picIsCalculating[ch] = true;
    }
    return pic[ch];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateDelayPlus(Channel ch) //-V2008
{
    float period0 = CalculatePeriod(A);
    float period1 = CalculatePeriod(B);

    EXIT_IF_ERRORS_FLOAT(period0, period1);
    if(!Math_FloatsIsEquals(period0, period1, 1.05f))
    {
        return ERROR_VALUE_FLOAT;
    }

    float average0 = CalculateAverageRel(A);
    float average1 = CalculateAverageRel(B);

    EXIT_IF_ERRORS_FLOAT(average0, average1);

    float firstIntersection = 0.0f;
    float secondIntersection = 0.0f;
    float averageFirst = ch == A ? average0 : average1;
    float averageSecond = ch == A ? average1 : average0;
    Channel firstChannel = ch == A ? A : B;
    Channel secondChannel = ch == A ? B : A;

    firstIntersection = FindIntersectionWithHorLine(firstChannel, 1, true, (uint8)averageFirst);
    secondIntersection = FindIntersectionWithHorLine(secondChannel, 1, true, (uint8)averageSecond);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(secondChannel, 2, true, (uint8)averageSecond);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, TBASE_DS);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateDelayMinus(Channel ch) //-V2008
{
    float period0 = CalculatePeriod(A);
    float period1 = CalculatePeriod(B);

    EXIT_IF_ERRORS_FLOAT(period0, period1);

    if(!Math_FloatsIsEquals(period0, period1, 1.05f))
    {
        return ERROR_VALUE_FLOAT;
    }

    float average0 = CalculateAverageRel(A);
    float average1 = CalculateAverageRel(B);

    EXIT_IF_ERRORS_FLOAT(average0, average1);

    float firstIntersection = 0.0f;
    float secondIntersection = 0.0f;
    float averageFirst = ch == A ? average0 : average1;
    float averageSecond = ch == A ? average1 : average0;
    Channel firstChannel = ch == A ? A : B;
    Channel secondChannel = ch == A ? B : A;

    firstIntersection = FindIntersectionWithHorLine(firstChannel, 1, false, (uint8)averageFirst);
    secondIntersection = FindIntersectionWithHorLine(secondChannel, 1, false, (uint8)averageSecond);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(secondChannel, 2, false, (uint8)averageSecond);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, TBASE_DS);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculatePhazaPlus(Channel ch)
{
    float delay = CalculateDelayPlus(ch);
    float period = CalculatePeriod(ch);
    if(delay == ERROR_VALUE_FLOAT || period == ERROR_VALUE_FLOAT)
    {
        return ERROR_VALUE_FLOAT;
    }
    return delay / period * 360.0f;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculatePhazaMinus(Channel ch)
{
    float delay = CalculateDelayMinus(ch);
    float period = CalculatePeriod(ch);
    if(delay == ERROR_VALUE_FLOAT || period == ERROR_VALUE_FLOAT)
    {
        return ERROR_VALUE_FLOAT;
    }
    return delay / period * 360.0f; 
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Processing_SetData(bool needSmoothing)
{
    isSet = true;

    BitSet64 points = sDisplay_BytesOnDisplay();
    firstByte = points.word0;
    lastByte = points.word1;

    numBytes = lastByte - firstByte;
    
    int length = BYTES_IN_CHANNEL_DS;

    if (ENABLED_DS_A)
    {
        Math_CalculateFiltrArray(IN_A, OUT_A, length);
        memcpy(IN_A, OUT_A, length);
    };
    if (ENABLED_DS_B)
    {
        Math_CalculateFiltrArray(IN_B, OUT_B, length);
        memcpy(IN_B, OUT_B, length);
    };
  
    CountedToCurrentSettings();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float Processing_GetCursU(Channel ch, float posCurT)
{
    if(!CHOICE_BUFFER)
    {
        return 0;
    }
    
    BitSet64 points = sDisplay_PointsOnDisplay();

    float retValue = 0.0f;
    LIMITATION(retValue, (float)(200 - (CHOICE_BUFFER)[points.word0 + (int)posCurT] + MIN_VALUE), 0.0f, 200.0f);
    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float Processing_GetCursT(Channel ch, float posCurU, int numCur)
{
    uint8 *dataIn = CHOICE_BUFFER;

    if(!dataIn)
    {
        return 0;
    }

#define FIRST_POINT (points.word0)
#define LAST_POINT  (points.word1)
    
    BitSet64 points = sDisplay_PointsOnDisplay();

    int prevData = 200 - dataIn[FIRST_POINT] + MIN_VALUE;

    int numIntersections = 0;

    for(int i = FIRST_POINT + 1; i < LAST_POINT; i++)
    {
        int curData = 200 - (dataIn)[i] + MIN_VALUE;

        if(curData <= posCurU && prevData > posCurU)
        {
            if(numCur == 0)
            {
                return (float)(i - FIRST_POINT);
            }
            else
            {
                if(numIntersections == 0)
                {
                    numIntersections++;
                }
                else
                {
                    return (float)(i - FIRST_POINT);
                }
            }
        }

        if(curData >= posCurU && prevData < posCurU)
        {
            if(numCur == 0)
            {
                return (float)(i - FIRST_POINT);
            }
            else
            {
                if(numIntersections == 0)
                {
                    numIntersections++;
                }
                else
                {
                    return (float)(i - FIRST_POINT);
                }
            }
        }
        prevData = curData;
    }
    return 0;

#undef LAST_POINT
#undef FIRST_POINT
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Processing_InterpolationSinX_X(uint8 *data, int numPoints, TBase tBase)
{
/*
     ѕоследовательности x в sin(x)   // Ёто, наверное, неправильно
2    1. 20нс : pi/2, -pi/2 ...
8    2. 10нс : pi/5, pi/5 * 2, pi/5 * 3, pi/5 * 4, -pi/5 * 4, -pi/5 * 3, -pi/5 * 2, -pi/5 ...
18   3. 5нс  : pi/10, pi/10 * 2 ... pi/10 * 9, -pi/10 * 9 .... -pi/10 * 2, -pi/10 ...
38   4. 2нс  : pi/20, pi/20 * 2 ... pi/20 * 19, -pi/20 * 19 ... -pi/20 * 2, -pi/20 ...
98   5. 1нс  : pi/50, pi/50 * 2 ... pi/50 * 49, -pi/50 * 49 ... -pi/50 * 2, -pi/50 ...
*/

#define MUL_SIN 1e7f
#define MUL     1e6f
#define KOEFF   (MUL / MUL_SIN)

    static const int deltas[5] = {100, 50, 20, 10, 5};
    int delta = deltas[tBase];

    uint8 *signedData = malloc(numPoints / 2);
    int numSignedPoints = 0;
    
    for (int pos = 0; pos < numPoints; pos++)
    {
        if (data[pos] > 0)
        {
            signedData[numSignedPoints++] = data[pos];
        }
    }

    // ЌайдЄм смещение первой значащей точки

    int shift = 0;
    for (int pos = 0; pos < numPoints; pos++)
    {
        if (data[pos] > 0)
        {
            shift = pos;
            break;
        }
    }

    float deltaX = PI;
    float stepX0 = PI / (float)delta;
    float x0 = PI - stepX0;
    int num = 0;
    
    for(int i = 0; i < numPoints; i++)
    {
        x0 += stepX0;
        if((i % delta) == 0)
        {
            data[i] = signedData[i / delta];
        }
        else
        {
            int part = num % ((delta - 1) * 2);
            num++;
            float sinX = (part < delta - 1) ? sinf(PI / delta * (part + 1)) : sinf(PI / delta * (part - (delta - 1) * 2));

            if (tBase > TBase_5ns)                 // «десь используем более быструю, но более неправильную арифметику целвых чисел
            {
                int sinXint = (int)(sinX * MUL_SIN);
                int value = 0;
                int x = (int)((x0 - deltaX) * MUL);
                int deltaXint = (int)(deltaX * MUL);

                for (int n = 0; n < numSignedPoints; n++)
                {
                    value += signedData[n] * sinXint / (x - n * deltaXint);
                    sinXint = -sinXint;
                }
                data[i] = (uint8)(value * KOEFF);
            }
            else                                    // Ќа этих развЄртках арифметика с плавающей зап€той даЄт приемлемое быстродействие
            {
                float value = 0.0f;
                float x = x0;

                for (int n = 0; n < numSignedPoints; n++)
                {
                    x -= deltaX;
                    value += signedData[n] * sinX / x;
                    sinX = -sinX;
                }
                data[i] = (uint8)value;
            }
        }
    }
    
    int pos = numPoints - 1;
    while (pos > shift)
    {
        data[pos] = data[pos - shift];
        pos--;
    }

    free(signedData);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char* Processing_GetStringMeasure(Measure measure, Channel ch, char* buffer, int lenBuf)
{
    if (!SET_ENABLED(ch))
    {
        return "";
    }
    buffer[0] = '\0';
    snprintf(buffer, 20, ch == A ? "1: " : "2: ");
    if(!isSet || values[measure].value[ch] == ERROR_VALUE_FLOAT)
    {
        strcat(buffer, "-.-");
    }
    else if((ch == A && !ENABLED_DS_A) || (ch == B && !ENABLED_DS_B))
    {
    }
    else if(measures[measure].FuncCalculate)
    {
        char bufferForFunc[20];
        pFuncCFBC func = measures[measure].FucnConvertate;
        float value = values[measure].value[ch];
        if (SET_DIVIDER_10(ch) && func == Voltage2String)
        {
            value *= 10.0f;                         // ƒомножаем, если включЄн делитель
        }
        char *text = func(value, measures[measure].showSign, bufferForFunc);
        int len = strlen(text) + strlen(buffer) + 1;
        if (len + 1 <= lenBuf)
        {
            strcat(buffer, text);
        }
        else
        {
            LOG_ERROR_TRACE("Ѕуфер слишком мал");
        }
    }
    else
    {
        return buffer;
    }
    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void CountedToCurrentSettings(void)
{
    int numBytes = BYTES_IN_CHANNEL_DS;

    int16 dTShift = SET_TSHIFT - TSHIFT_DS;

    int rShiftA = ((int)SET_RSHIFT_A - (int)RSHIFT_DS_A) / (float)STEP_RSHIFT * 1.25f;   /// \todo магические числа

    int rShiftB = ((int)SET_RSHIFT_B - (int)RSHIFT_DS_B) / (float)STEP_RSHIFT * 1.25f;   /// \todo избавитьс€ от этого непон€тного коэффициента

    if (SET_RANGE_A !=  RANGE_DS_A)
    {
        CountedRange(A);
    }
    else if (SET_RANGE_B != RANGE_DS_B)
    {
        CountedRange(B);
    }
    else if (dTShift || rShiftA || rShiftB)
    {
        int startIndex = -dTShift;
        for (int i = 0; i <= startIndex; i++)
        {
            OUT_A[i] = AVE_VALUE;
            OUT_B[i] = AVE_VALUE;
        };

        int endIndex = numBytes / 2 - dTShift;
        if (endIndex < numBytes / 2 - 1)
        {
            for (int i = endIndex; i < numBytes / 2; i++)
            {
                OUT_A[i] = AVE_VALUE;
                OUT_B[i] = AVE_VALUE;
            }
        }

        for (int i = 0; i < numBytes; i += 2)
        {
            int index = i / 2 - dTShift;
            if (index >= 0 && index < numBytes)
            {
                int dA0 = IN_A[i];
                int dA1 = IN_A[i + 1];
                if (rShiftA)
                {
                    if (dA0)                            // “олько если значение в этой точке есть
                    {
                        dA0 += rShiftA;
                        LIMITATION(dA0, dA0, 1, 255);
                    }
                    if (dA1)                            // “олько если значение в этой точке есть
                    {
                        dA1 += rShiftA;
                        LIMITATION(dA1, dA1, 1, 255);
                    }
                }
                ((uint16 *)OUT_A)[index] = (uint16)((dA0 | (dA1 << 8)));

                int dB0 = IN_B[i];
                int dB1 = IN_B[i + 1];
                if (rShiftB)
                {
                    if (dB0)                            // “олько если значение в этой точке есть
                    {
                        dB0 += rShiftB;
                        LIMITATION(dB0, dB0, 1, 255);
                    }
                    if (dB1)                            // “олько если значение в этой точке есть
                    {
                        dB1 += rShiftB;
                        LIMITATION(dB1, dB1, 1, 255);
                    }
                }
                ((uint16 *)OUT_B)[index] = (uint16)((dB0 | (dB1 << 8)));
            }
        }
    }
    else
    {
        memcpy(OUT_A, IN_A, numBytes);
        memcpy(OUT_B, IN_B, numBytes);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalcAve(uint16 *data, Range range, uint16 rShift)
{
    float sum = 0.0;
    int num = 100;
    for (int i = 0; i < num; i++)
    {
        uint8 val = (uint8)data[i];
        sum += POINT_2_VOLTAGE(val, range, rShift);
    }

    return sum / num;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void CountedRange(Channel ch)
{
    Range rangeIn = RANGE_DS(ch);
    Range rangeOut = SET_RANGE(ch);
    int rShiftIn = RSHIFT_DS(ch);
    int rShiftOut = SET_RSHIFT(ch);
    
    uint8 *in = dataIN[ch];
    uint8 *out = dataOUT[ch];
    
    int numBytes = BYTES_IN_CHANNEL_DS;

    for (int i = 0; i < numBytes; ++i)
    {
        int d = in[i];
        if (d)
        {
            float abs = POINT_2_VOLTAGE(d, rangeIn, rShiftIn);
            d = Math_VoltageToPoint(abs, rangeOut, (uint16)rShiftOut);
            LIMITATION(d, d, MIN_VALUE, MAX_VALUE);
            out[i] = d;
        }
        else
        {
            out[i] = 0;
        }
    }
}
