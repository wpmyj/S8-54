#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Utils
 *  @{
 *  @defgroup Math
 *  @{
 */

#define SET_IF_LARGER(x, max, newValue)     if((x) > (max)) { (x) = (newValue); }

#define LIMIT_BELOW(x, min)                 if((x) < (min)) { (x) = (min); }

#define LIMIT_ABOVE(x, max)                 if((x) > (max)) { (x) = (max); }

#define SET_MIN_IF_LESS(x, min)             if((x) < (min)) { (min) = (x); }

#define SET_MAX_IF_LARGER(x, max)           if((x) > (max)) { (max) = (x); }

#define LIMITATION_ABOVE(var, value, max)   var = (value); if((var) > (max)) { (var) = (max); }

#define LIMITATION(var, min, max)           if(var < (min)) { (var) = (min); } else if(var > (max)) { var = (max); };

#define MAX_VOLTAGE_ON_SCREEN(range)        (tableScalesRange[(range)] * 5)

#define RSHIFT_2_ABS(rShift, range)         (-((RShiftZero - ((int)rShift)) * absStepRShift[(range)]))

#define POINT_2_VOLTAGE(value, range, rShift) (((value) - MIN_VALUE) * voltsInPoint[(range)] - MAX_VOLTAGE_ON_SCREEN((range)) - RSHIFT_2_ABS((rShift), (range)))

#define TSHIFT_2_ABS(tShift, tBase)         (absStepTShift[(tBase)] * (tShift) * 2.0f)

#define TSHIFT_2_REL(tShiftAbs, tBase)      ((tShiftAbs) / absStepTShift[(tBase)] / 2.0f)

#define ROUND(x)                            ((x) += 0.5f, (x))

#define IN_RANGE(x, min, max)               ((x) >= (min) && (x) <= (max))


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const float tableScalesRange[RangeSize];
extern const float absStepRShift[];
extern const float voltsInPixel[];
extern const float voltsInPoint[];
extern const float absStepTShift[];


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Math
{
public:
    int RShift2Rel(float rShiftAbs, Range range);
    /// Смещение относительно нулевого в пикселях экрана
    int RShift2Pixels(uint16 rShift, int heightGrid);

    float VoltageCursor(float shiftCurU, Range range, uint16 rShift);

    float TimeCursor(float shiftCurT, TBase tBase);

    void DataExtrapolation(uint8 *data, uint8 *there, int size);

    void PointsRelToVoltage(const uint8 *points, int numPoints, Range range, uint16 rShift, float *voltage);

    uint8 VoltageToPoint(float voltage, Range range, uint16 rShift);

    void PointsVoltageToRel(const float *voltage, int numPoints, Range range, uint16 rShift, uint8 *points);

    uint8 GetMaxFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint);

    uint8 GetMinFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint);

    uint8 GetMaxFromArray(const uint8 *data, int firstPoint, int lastPoint);

    uint8 GetMaxFromArray_RAM(const uint16 *data, int firstPoint, int lastPoint);

    uint8 GetMinFromArray(const uint8 *data, int firstPoint, int lastPoint);
    /// Работает с внешним ОЗУ - считывать можно 16-битные значения
    uint8 GetMinFromArray_RAM(const uint16 *data, int firstPoint, int lastPoint);
    /// Возвращает координату x пересечения линии, проходящей через (x0, y0), (x1, y1), с горизонтальной линией, проходящей через точку с ординатой yHorLine
    float GetIntersectionWithHorizontalLine(int x0, int y0, int x1, int y1, int yHorLine);
    /// Сравнивает два числа. Возвращает true, если числа отличаются друг от друга не более, чем на epsilonPart. При этом для расчёта epsilonPart используется большее в смысле модуля число
    bool FloatsIsEquals(float value0, float value1, float epsilonPart);

    float MinFrom3float(float value1, float value2, float value3);

    int MinInt(int val1, int val2);

    int MaxInt(int val1, int val2);

    void CalculateFFT(float *data, int numPoints, float *result, float *freq0, float *density0, float *freq1, float *density1, int *y0, int *y1);

    void CalculateMathFunction(float *data0andResult, float *dataB, int numPoints);

    float RandFloat(float min, float max);

    int8 AddInt8WithLimitation(int8 value, int8 delta, int8 min, int8 max);

    int Sign(int vlaue);
    /// Вычисляет 10**pow
    int Pow10(int pow);
    /// Вычисляет число разрядов в целом типа int
    int NumDigitsInNumber(int value);
    /// Возвращает модуль value
    int FabsInt(int value);

    int LimitationInt(int value, int min, int max);

    uint8 LimitationUInt8(uint8 value, uint8 min, uint8 max);

    float LimitationFloat(float value, float min, float max);

    uint8 CalculateFiltr(const uint8 *data, int x, int numPoints);

    void CalculateFiltrArray(const uint8 *dataIn, uint8 *dataOut, int numPoints, bool needSmoothing);
};


extern Math math;


/** @}  @}
 */
