#include "Math.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Math_GetIntersectionWithHorizontalLine(int x0, int y0, int x1, int y1, int yHorLine)
{
    if(y0 == y1)
    {
        return (float)x1;
    }

    return (yHorLine - y0) / ((float)(y1 - y0) / (float)(x1 - x0)) + x0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Math_FloatsIsEquals(float value0, float value1, float epsilonPart)
{
    float max = fabsf(value0) > fabsf(value1) ? fabsf(value0) : fabsf(value1);

    float epsilonAbs = max * epsilonPart;

    return fabsf(value0 - value1) < epsilonAbs;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float Math_MinFrom3float(float value1, float value2, float value3)
{
    float retValue = value1;
    if(value2 < retValue)
    {
        retValue = value2;
    }
    if(value3 < retValue)
    {
        retValue = value3;
    }
    return retValue;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int Math_MaxInt(int val1, int val2)
{
    return val1 > val2 ? val1 : val2;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Math_MinInt(int val1, int val2)
{
    return val1 < val2 ? val1 : val2;
}



//----------------------------------------------------------------------------------------------------------------------------------------------------
float Math_RandFloat(float min, float max)
{
    float delta = max - min;
    return min + ((rand() / (float)RAND_MAX) * delta);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int8 Math_AddInt8WithLimitation(int8 value, int8 delta, int8 min, int8 max)
{
    int8 retValue = value + delta;
    if (retValue < 0)
    {
        return 0;
    }
    if (retValue > max)
    {
        return max;
    }
    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Math_Sign(int value)
{
    if (value > 0)
    {
        return 1;
    }
    if (value < 0)
    {
        return -1;
    }
    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Math_Pow10(int pow)
{
    int retValue = 1;

    while (pow--)
    {
        retValue *= 10;
    }

    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Math_NumDigitsInNumber(int value)
{
    value = Math_FabsInt(value);
    int num = 1;
    while ((value /= 10) > 0)
    {
        num++;
    }
    return num;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Math_FabsInt(int value)
{
    return value >= 0 ? value : -value;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 Math_GetMinFromArray(const uint8 *data, int firstPoint, int lastPoint)
{
#define MIN_IF_LESS if(d < min) { min = d; }

    uint8 min = 255;
    const uint8 *pointer = &data[firstPoint];

    for (int i = firstPoint; i < lastPoint; i += 2)
    {
        uint8 d = *pointer++;
        MIN_IF_LESS
        d = *pointer++;
        MIN_IF_LESS
    }
    if ((lastPoint - firstPoint + 1) & 1)
    {
        uint8 d = *pointer;
        MIN_IF_LESS
    }

    return min;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 Math_GetMinFromArray_RAM(const uint16 *data, int firstPoint, int lastPoint)
{
    uint8 min = 255;

    const uint16 *pointer = &data[firstPoint];

    const int endPoint = lastPoint / 2;

    for (int i = firstPoint; i < endPoint; i++)
    {
        uint16 d16 = *pointer++;

        uint8 d8 = (uint8)d16;
        if (d8 < min)
        {
            min = d8;
        }
        d8 = (uint8)(d16 >> 8);
        if (d8 < min)
        {
            min = d8;
        }
    }
    
    return min;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 Math_GetMaxFromArray(const uint8 *data, int firstPoint, int lastPoint)
{
#define MAX_IF_ABOVE if(d > max) { max = d; }

    uint8 max = 0;
    const uint8 *pointer = &data[firstPoint];

    for (int i = firstPoint; i < lastPoint; i += 2)
    {
        uint8 d = *pointer++;
        MAX_IF_ABOVE;
        d = *pointer++;
        MAX_IF_ABOVE;
    }
    if ((lastPoint - firstPoint + 1) & 1)
    {
        uint8 d = *pointer;
        MAX_IF_ABOVE
    }

    return max;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 Math_GetMaxFromArray_RAM(const uint16 *data, int firstPoint, int lastPoint)
{
    uint8 max = 0;

    const uint16 *pointer = &data[firstPoint];

    const int endPoint = lastPoint / 2;

    for (int i = firstPoint; i < endPoint; i++)
    {
        uint16 d16 = *pointer++;

        uint8 d8 = (uint8)d16;
        if (d8 > max)
        {
            max = d8;
        }

        d8 = (uint8)(d16 >> 8);
        if (d8 > max)
        {
            max = d8;
        }
    }

    return max;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
#define LIMIT                           \
    if (value <= min) { return min; }   \
    if (value >= max) { return max; }   \
    return value;


//----------------------------------------------------------------------------------------------------------------------------------------------------
int LimitationInt(int value, int min, int max)
{
    LIMIT
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 LimitationUInt8(uint8 value, uint8 min, uint8 max)
{
    LIMIT
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float LimitationFloat(float value, float min, float max)
{
    LIMIT
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 Math_CalculateFiltr(const uint8 *data, int x, int numPoints, int numSmoothing)
{
    if (numSmoothing < 2)
    {
        return data[x];
    }

    int count = 1;
    int sum = data[x];
    int startDelta = 1;

    int endDelta = numSmoothing / 2;

    for (int delta = startDelta; delta <= endDelta; delta++)
    {
        if (((x - delta) >= 0) && ((x + delta) < (numPoints)))
        {
            sum += data[x - delta];
            sum += data[x + delta];
            count += 2;
        }
    }

    if ((numSmoothing % 2) == 1)
    {
        int delta = numSmoothing / 2;
        if ((x + delta) < numPoints)
        {
            sum += data[x + delta];
            count++;
        }
    }

    return (uint8)(sum / count);
}
