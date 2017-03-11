// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "ProcessingSignal.h"
#include "Math.h"
#include "GlobalFunctions.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Hardware/FSMC.h"
#include "Hardware/RAM.h"
#include "Menu/Pages/Memory/PageMemory.h"
#include "Settings/Settings.h"
#include <math.h>
#include <string.h>
#include <limits.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
static int   CalculatePeriodAccurately(Channel ch);   // Точно вычисляет период или целое число периодов в точках сигнала.
static float CalculateFreq(Channel ch);
static float CalculateTimeNarastaniya(Channel ch);
static float CalculateTimeSpada(Channel ch);
static float CalculateDurationPlus(Channel ch);
static float CalculateDurationMinus(Channel ch);
static float CalculateSkvaznostPlus(Channel ch);
static float CalculateSkvaznostMinus(Channel ch);


static float CalculateMinRel(Channel ch);           // Возвращает минимальное значение относительного сигнала
static float CalculateMinSteadyRel(Channel ch);     // Возвращает минимальное установившееся значение относительного сигнала
static float CalculateMaxRel(Channel ch);           // Возвращает максимальное значение относительного сигнала
static float CalculateMaxSteadyRel(Channel ch);     // Возвращает максимальное установившееся значение относительного сигнала
static float CalculateAverageRel(Channel ch);       // Возвращает среденее значение относительного сигнала
static float CalculatePicRel(Channel ch);
static float CalculateDelayPlus(Channel ch);
static float CalculateDelayMinus(Channel ch);
static float CalculatePhazaPlus(Channel ch);
static float CalculatePhazaMinus(Channel ch);
static float FindIntersectionWithHorLine(Channel ch, int numIntersection, bool downToUp, uint8 yLine);  // Найти точку пересечения сигнала с горизонтальной линией, проведённой на уровне yLine. numItersection - порядковый номер пересечения, начинается с 1. downToTop - если true, ищем пересечение сигнала со средней линией при прохождении из "-" в "+".

static void CountedToCurrentSettings(void);
static void CountedTShift(void);
static void CountedRShift(Channel ch);
static void CountedRange(Channel ch);

static uint16 *dataOutA_RAM = 0;
static uint16 *dataOutB_RAM = 0;

static DataSettings ds;
static bool isSet = false;  // Если true, то сигнал назначен

static uint8 *dataInA = 0;
static uint8 *dataInB = 0;

static int firstPoint = 0;
static int lastPoint = 0;
static int numPoints = 0;

typedef struct
{
    char        *name;
    pFuncFU8    FuncCalculate;
    pFuncCFBC   FucnConvertate;
    bool        showSign;           // Если true, нужно показывать знак.
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

static int markerHor[NumChannels][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};
static int markerVert[NumChannels][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};

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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Processing_CalculateMeasures(void)
{
    if(!SHOW_MEASURES || !isSet)
    {
        return;
    }

    int length = NumBytesInChannel(&ds);

    // Вначале выделим память для данных из внешнего ОЗУ
    dataInA = malloc(length);
    dataInB = malloc(length);

    // Вначале перепишем данные из внешнего ОЗУ
    RAM_MemCpy16(RAM(PS_DATA_IN_A), dataInA, length);
    RAM_MemCpy16(RAM(PS_DATA_IN_B), dataInB, length);

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
                if(SOURCE_MEASURE_A || SOURCE_MEASURE_A_B)
                {
                    values[meas].value[A] = func(A);
                }
                if(SOURCE_MEASURE_B || SOURCE_MEASURE_A_B)
                {
                    values[meas].value[B] = func(B);
                }
            }
        }
    }

    // Теперь удалим выделенную память
    free(dataInA);
    free(dataInB);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageMax(Channel ch)
{
    float max = CalculateMaxRel(ch);
    EXIT_IF_ERROR_FLOAT(max);
    if(MARKED_MEAS == Measure_VoltageMax)
    {
        markerHor[ch][0] = (int)max;                           // Здесь не округляем, потому что max может быть только целым
    }

    return POINT_2_VOLTAGE(max, ds.range[ch], ds.rShift[ch]);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageMin(Channel ch)
{
    float min = CalculateMinRel(ch);
    EXIT_IF_ERROR_FLOAT(min);
    if(MARKED_MEAS == Measure_VoltageMin)
    {
        markerHor[ch][0] = (int)min;                           // Здесь не округляем, потому что min может быть только целым
    }
    
    return POINT_2_VOLTAGE(min, ds.range[ch], ds.rShift[ch]);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageMinSteady(Channel ch)
{
    float min = CalculateMinSteadyRel(ch);
    EXIT_IF_ERROR_FLOAT(min);
    if(MARKED_MEAS == Measure_VoltageMinSteady)
    {
        markerHor[ch][0] = (int)ROUND(min);
    }

    return POINT_2_VOLTAGE(min, ds.range[ch], ds.rShift[ch]);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageMaxSteady(Channel ch)
{
    float max = CalculateMaxSteadyRel(ch);

    EXIT_IF_ERROR_FLOAT(max);

    if(MARKED_MEAS == Measure_VoltageMaxSteady)
    {
        markerHor[ch][0] = (int)max;
    }

    return POINT_2_VOLTAGE(max, ds.range[ch], ds.rShift[ch]);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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

    uint rShift = ds.rShift[ch];
    return fabsf(POINT_2_VOLTAGE(maxSteady, ds.range[ch], rShift) - POINT_2_VOLTAGE(max, ds.range[ch], rShift));
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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

    uint16 rShift = ds.rShift[ch];
    return fabsf(POINT_2_VOLTAGE(minSteady, ds.range[ch], rShift) - POINT_2_VOLTAGE(min, ds.range[ch], rShift));
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
#define CHOICE_BUFFER (ch == A ? dataInA : dataInB)


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageAverage(Channel ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    int sum = 0;

    uint8 *data = &CHOICE_BUFFER[firstPoint];

    for(int i = 0; i < period; i++)
    {
        sum += *data++;
    }

    uint8 aveRel = (uint8)((float)sum / period);

    if(MARKED_MEAS == Measure_VoltageAverage)
    {
        markerHor[ch][0] = aveRel;
    }

    return POINT_2_VOLTAGE(aveRel, ds.range[ch], ds.rShift[ch]);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateVoltageRMS(Channel ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    float rms = 0.0f;
    uint16 rShift = ds.rShift[ch];

    uint8 *dataIn = CHOICE_BUFFER;

    for(int i = firstPoint; i < firstPoint + period; i++)
    {
        float volts = POINT_2_VOLTAGE(dataIn[i], ds.range[ch], rShift);
        rms +=  volts * volts;
    }

    if(MARKED_MEAS == Measure_VoltageRMS)
    {
        markerHor[ch][0] = Math_VoltageToPoint(sqrtf(rms / period), (Range)ds.range[ch], rShift);
    }

    return sqrtf(rms / period);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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

            float firstIntersection = intersectionDownToTop < intersectionTopToDown ? intersectionDownToTop : intersectionTopToDown;
            float secondIntersection = FindIntersectionWithHorLine(ch, 2, intersectionDownToTop < intersectionTopToDown, (uint8)aveValue);

            EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

            float per = TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, ds.tBase);

            period[ch] = per;
            periodIsCaclulating[ch] = true;
        }
    }

    return period[ch];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
#define EXIT_FROM_PERIOD_ACCURACY               \
    period[ch] = ERROR_VALUE_INT;             \
    periodAccurateIsCalculating[ch] = true;   \
    free(sums);                                 \
    return period[ch];


//------------------------------------------------------------------------------------------------------------------------------------------------------
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
        sums[firstPoint] = dataIn[firstPoint];

        int i = firstPoint + 1;
        int *sum = &sums[i];
        uint8 *data = &dataIn[i];
        uint8 *end = &dataIn[lastPoint];

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

        int addShift = firstPoint - 1;
        int maxPeriod = (int)(numPoints * 0.95f);

        for(int nextPeriod = 10; nextPeriod < maxPeriod; nextPeriod++)
        {
            int sum = sums[addShift + nextPeriod];

            int maxDelta = 0;
            int maxStart = numPoints - nextPeriod;

            int *pSums = &sums[firstPoint + 1];
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateFreq(Channel ch)
{
    float period = CalculatePeriod(ch);
    return period == ERROR_VALUE_FLOAT ? ERROR_VALUE_FLOAT : 1.0f / period;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float FindIntersectionWithHorLine(Channel ch, int numIntersection, bool downToUp, uint8 yLine)
{
    int num = 0;
    int x = firstPoint;
    int compValue = lastPoint - 1;

    uint8 *data = &CHOICE_BUFFER[0];

    if(downToUp)
    {
        while((num < numIntersection) && (x < compValue))
        {
            if(data[x] < yLine && data[x + 1] >= yLine)
            {
                num++;
            }
            x++;
        }
    }
    else
    {
        while((num < numIntersection) && (x < compValue))
        {
            if(data[x] > yLine && data[x + 1] <= yLine)
            {
                num++;
            }
            x++;
        }
    }
    x--;

    if (num < numIntersection)
    {
        return ERROR_VALUE_FLOAT;
    }
    return Math_GetIntersectionWithHorizontalLine(x, data[x], x + 1, data[x + 1], yLine);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, ds.tBase);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, ds.tBase);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateTimeNarastaniya(Channel ch)                    // WARN Здесь, возможно, нужно увеличить точность - брать не целые значени расстояний между отсчётами по времени, а рассчитывать пересечения линий
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

    float retValue = TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, ds.tBase);

    if (MARKED_MEAS == Measure_TimeNarastaniya)
    {
        markerHor[ch][0] = (int)max09;
        markerHor[ch][1] = (int)min01;
        markerVert[ch][0] = (int)((int16)firstIntersection - SHIFT_IN_MEMORY);
        markerVert[ch][1] = (int)((int16)secondIntersection - SHIFT_IN_MEMORY);
    }

    return retValue;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateTimeSpada(Channel ch)                          // WARN Аналогично времени нарастания
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

    float retValue = TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, ds.tBase);

    if (MARKED_MEAS == Measure_TimeSpada)
    {
        markerHor[ch][0] = (int)max09;
        markerHor[ch][1] = (int)min01;
        markerVert[ch][0] = (int)((int16)firstIntersection - SHIFT_IN_MEMORY);
        markerVert[ch][1] = (int)((int16)secondIntersection - SHIFT_IN_MEMORY);
    }

    return retValue;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateSkvaznostPlus(Channel ch)
{
    float period = CalculatePeriod(ch);
    float duration = CalculateDurationPlus(ch);

    EXIT_IF_ERRORS_FLOAT(period, duration);

    return period / duration;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateSkvaznostMinus(Channel ch)
{
    float period = CalculatePeriod(ch);
    float duration = CalculateDurationMinus(ch);

    EXIT_IF_ERRORS_FLOAT(period, duration);

    return period / duration;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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

            uint8 *data = &dataIn[firstPoint];
            const uint8 * const end = &dataIn[lastPoint];
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

                data = &dataIn[firstPoint];
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
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
            uint8 *data = &dataIn[firstPoint];
            const uint8 * const end = &dataIn[lastPoint];
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

                data = &dataIn[firstPoint];
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateMaxRel(Channel ch)
{
    static float max[2] = {0.0f, 0.0f};

    if(!maxIsCalculating[ch])
    {
        uint8 val = Math_GetMaxFromArrayWithErrorCode(CHOICE_BUFFER, firstPoint, lastPoint);
        max[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        maxIsCalculating[ch] = true;
    }

    return max[ch];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateMinRel(Channel ch)
{
    static float min[2] = {255.0f, 255.0f};

    if (!minIsCalculating[ch])
    {
        uint8 val = Math_GetMinFromArrayWithErrorCode(CHOICE_BUFFER, firstPoint, lastPoint);
        min[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        minIsCalculating[ch] = true;
    }

    return min[ch];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateDelayPlus(Channel ch)
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

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, ds.tBase);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateDelayMinus(Channel ch)
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

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, ds.tBase);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Processing_SetSignal(uint8 *dataA, uint8 *dataB, DataSettings *_ds, int _firstPoint, int _lastPoint)
{
    isSet = true;

    dataOutA_RAM = (uint16*)RAM(PS_DATA_OUT_A);
    dataOutB_RAM = (uint16*)RAM(PS_DATA_OUT_B);

    firstPoint = _firstPoint;
    lastPoint = _lastPoint;
    numPoints = lastPoint - firstPoint;
    
    int numSmoothing = sDisplay_NumPointSmoothing();

    int length = NumBytesInChannel(&ds);

    bool enableA = DS_ENABLED_A(&ds) == 1;
    bool enableB = DS_ENABLED_B(&ds) == 1;

    // Выделим память для данных
    if (enableA) { dataInA = malloc(FPGA_MAX_POINTS); };
    if (enableB) { dataInB = malloc(FPGA_MAX_POINTS); };

    if (enableA) { Math_CalculateFiltrArray(dataA, dataInA, length, numSmoothing); };
    if (enableB) { Math_CalculateFiltrArray(dataB, dataInB, length, numSmoothing); };

    ds = *_ds;
    
    CountedToCurrentSettings();

    // Теперь сохраним данные во внешнем ОЗУ
    if (enableA) { RAM_MemCpy16(dataInA, RAM(PS_DATA_IN_A), FPGA_MAX_POINTS); };
    if (enableB) { RAM_MemCpy16(dataInB, RAM(PS_DATA_IN_B), FPGA_MAX_POINTS); };

    // И вернём ранее запрошенную память
    if (enableA) { free(dataInA); };
    if (enableB) { free(dataInB); };
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Processing_GetData(uint8 **dataA, uint8 **dataB, DataSettings **_ds)
{
    if (dataA)
    {
        *dataA = (isSet && DS_ENABLED_A(&ds)) ? (uint8*)dataOutA_RAM : 0;
    }
    if (dataB)
    {
        *dataB = (isSet && DS_ENABLED_B(&ds)) ? (uint8*)dataOutB_RAM : 0;
    }
    if (_ds)
    {
        *_ds = isSet ? &ds : 0;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float Processing_GetCursU(Channel ch, float posCurT)
{
    if(!CHOICE_BUFFER)
    {
        return 0;
    }
    
    int firstPoint = 0;
    int lastPoint = 0;
    sDisplay_PointsOnDisplay(&firstPoint, &lastPoint);

    float retValue = 0.0f;
    LIMITATION(retValue, (float)(200 - (CHOICE_BUFFER)[firstPoint + (int)posCurT] + MIN_VALUE), 0.0f, 200.0f);
    return retValue;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
float Processing_GetCursT(Channel ch, float posCurU, int numCur)
{
    uint8 *dataIn = CHOICE_BUFFER;

    if(!dataIn)
    {
        return 0;
    }

    int firstPoint = 0;
    int lastPoint = 0;
    sDisplay_PointsOnDisplay(&firstPoint, &lastPoint);

    int prevData = 200 - dataIn[firstPoint] + MIN_VALUE;

    int numIntersections = 0;

    for(int i = firstPoint + 1; i < lastPoint; i++)
    {
        int curData = 200 - (dataIn)[i] + MIN_VALUE;

        if(curData <= posCurU && prevData > posCurU)
        {
            if(numCur == 0)
            {
                return (float)(i - firstPoint);
            }
            else
            {
                if(numIntersections == 0)
                {
                    numIntersections++;
                }
                else
                {
                    return (float)(i - firstPoint);
                }
            }
        }

        if(curData >= posCurU && prevData < posCurU)
        {
            if(numCur == 0)
            {
                return (float)(i - firstPoint);
            }
            else
            {
                if(numIntersections == 0)
                {
                    numIntersections++;
                }
                else
                {
                    return (float)(i - firstPoint);
                }
            }
        }
        prevData = curData;
    }
    return 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Processing_InterpolationSinX_X(uint8 *data, int numPoints, TBase tBase)
{
/*
     Последовательности x в sin(x)   // Это, наверное, неправильно
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

    // Найдём смещение первой значащей точки

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

            if (tBase > TBase_5ns)                 // Здесь используем более быструю, но более неправильную арифметику целвых чисел
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
            else                                    // На этих развёртках арифметика с плавающей запятой даёт приемлемое быстродействие
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
char* Processing_GetStringMeasure(Measure measure, Channel ch, char* buffer, int lenBuf)
{
    if (!sChannel_Enabled(ch))
    {
        return "";
    }
    buffer[0] = '\0';
    snprintf(buffer, 20, ch == A ? "1: " : "2: ");
    if(!isSet || values[measure].value[ch] == ERROR_VALUE_FLOAT)
    {
        strcat(buffer, "-.-");
    }
    else if((ch == A && !DS_ENABLED_A(&ds)) || (ch == B && !DS_ENABLED_B(&ds)))
    {
    }
    else if(measures[measure].FuncCalculate)
    {
        char bufferForFunc[20];
        pFuncCFBC func = measures[measure].FucnConvertate;
        float value = values[measure].value[ch];
        if (DIVIDER_10(ch) && func == Voltage2String)
        {
            value *= 10.0f;                         // Домножаем, если включён делитель
        }
        char *text = func(value, measures[measure].showSign, bufferForFunc);
        int len = strlen(text) + strlen(buffer) + 1;
        if (len + 1 <= lenBuf)
        {
            strcat(buffer, text);
        }
        else
        {
            LOG_ERROR_TRACE("Буфер слишком мал");
        }
    }
    else
    {
        return buffer;
    }
    return buffer;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int Processing_GetMarkerHorizontal(Channel ch, int numMarker)
{
    return markerHor[ch][numMarker] - MIN_VALUE;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int Processing_GetMarkerVertical(Channel ch, int numMarker)
{
    return markerVert[ch][numMarker];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void CountedToCurrentSettings(void)
{
    int numPoints = NumBytesInChannel(&ds);

    int16 dTShift = TSHIFT - ds.tShift;

    if (dTShift)
    {
        CountedTShift();
    }

    int rShiftA = ((int)RSHIFT_A - (int)ds.rShift[A]) / STEP_RSHIFT;

    if (rShiftA)
    {
        CountedRShift(A);
    }

    int rShiftB = ((int)RSHIFT_B - (int)ds.rShift[B]) / STEP_RSHIFT;

    if (rShiftB)
    {
        CountedRShift(B);
    }

    if (RANGE_A != ds.range[A])
    {
        CountedRange(A);
    }
    else if (RANGE_B != ds.range[B])
    {
        CountedRange(B);
    }
    else if (dTShift || rShiftA || rShiftB)
    {
        int startIndex = -dTShift;
        for (int i = 0; i <= startIndex; i++)
        {
            dataOutA_RAM[i] = AVE_VALUE;
            dataOutB_RAM[i] = AVE_VALUE;
        };

        int endIndex = numPoints / 2 - dTShift;
        if (endIndex < numPoints / 2 - 1)
        {
            for (int i = endIndex; i < numPoints / 2; i++)
            {
                dataOutA_RAM[i] = AVE_VALUE;
                dataOutB_RAM[i] = AVE_VALUE;
            }
        }

        for (int i = 0; i < numPoints; i += 2)
        {
            int index = i / 2 - dTShift;
            if (index >= 0 && index < numPoints)
            {
                int dA0 = dataInA[i];
                int dA1 = dataInA[i + 1];
                if (rShiftA)
                {
                    dA0 += rShiftA;
                    LIMITATION(dA0, dA0, 0, 255);
                    dA1 += rShiftA;
                    LIMITATION(dA1, dA1, 0, 255);
                }
                ((uint16*)dataOutA_RAM)[index] = (uint16)((dA0 | (dA1 << 8)));

                int dB0 = dataInB[i];
                int dB1 = dataInB[i + 1];
                if (rShiftB)
                {
                    dB0 += rShiftB;
                    LIMITATION(dB0, dB0, 0, 255);
                    dB1 += rShiftB;
                    LIMITATION(dB1, dB1, 0, 255);
                }
                ((uint16*)dataOutB_RAM)[index] = (uint16)((dB0 | (dB1 << 8)));
            }
        }
    }
    else
    {
        RAM_MemCpy16(dataInA, dataOutA_RAM, numPoints);
        RAM_MemCpy16(dataInB, dataOutB_RAM, numPoints);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void CountedTShift(void)
{

}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void CountedRShift(Channel ch)
{

}


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


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void CountedRange(Channel ch)
{
    uint8 *in = 0;
    uint16 *out = 0;
    Range rangeIn = (Range)ds.range[ch];
    Range rangeOut = RANGE(ch);
    int rShiftIn = ds.rShift[ch];
    int rShiftOut = RSHIFT(ch);

    if (ch == A)
    {
        in = dataInA;
        out = dataOutA_RAM;
    }
    else
    {
        in = dataInB;
        out = dataOutB_RAM;
    }

    int numPoints = NumBytesInChannel(&ds);

    for (int i = 0; i < numPoints; i += 2)
    {
        int d0 = in[i];
        float abs0 = POINT_2_VOLTAGE(d0, rangeIn, rShiftIn);
        d0 = Math_VoltageToPoint(abs0, rangeOut, (uint16)rShiftOut);
        LIMITATION(d0, d0, 0, 255);

        int d1 = in[i + 1];
        float abs1 = POINT_2_VOLTAGE(d1, rangeIn, rShiftIn);
        d1 = Math_VoltageToPoint(abs1, rangeOut, (uint16)rShiftOut);
        LIMITATION(d1, d1, 0, 255);

        out[i / 2] = (uint16)((d0 | (d1 << 8)));
    }
}
