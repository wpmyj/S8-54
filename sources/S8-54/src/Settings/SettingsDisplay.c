#include "Settings.h"
#include "SettingsDisplay.h"
#include "Utils/Math.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int sDisplay_NumSignalsInS(void)
{
    return Tables_ENUMtoNumSignalsInSec(set.display.numSignalsInSec);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void sDisplay_SetNumSignalsInS(int numSignalsInS)
{
    set.display.numSignalsInSec = Tables_NumSignalsInSecToENUM(numSignalsInS);
}
 

//------------------------------------------------------------------------------------------------------------------------------------------------------
int sDisplay_NumPointSmoothing(void)
{
    if(set.debug.modeEMS && (set.display.smoothing < NumSmooth_4points))
    {
        return 4;
    }

    int retValue = 0;
    if (set.display.smoothing != NumSmooth_Disable)
    {
        retValue = set.display.smoothing + 1;
    }

    if (IN_RANDOM_MODE)
    {
        int numRand = 0;
        if (set.nr.numSmoothForRand > 1)
        {
            numRand = set.nr.numSmoothForRand;
        }

        LIMIT_BELOW(retValue, numRand);
    }


    return retValue;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int sDisplay_NumAverage(void)
{
    if (set.debug.modeEMS && (set.display.numAveraging < NumAveraging_8))
    {
        return 8;
    }

    return (1 << (int)set.display.numAveraging);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void sDisplay_PointsOnDisplay(int *firstPoint, int *lastPoint)
{
    *firstPoint = SHIFT_IN_MEMORY;
    if (PEACKDET_EN)
    {
        *firstPoint /= 2;
    }
    *lastPoint = *firstPoint + 281;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int sDisplay_ShiftInMemoryInPoints(void)
{
    if (PEACKDET_DIS)
    {
        return SHIFT_IN_MEMORY;
    }
    return SHIFT_IN_MEMORY / 2;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int sDisplay_TimeMenuAutoHide(void)
{
    static const int times[] =
    {
        0, 5, 10, 15, 30, 60
    };
    return times[set.display.menuAutoHide] * 1000;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sDisplay_IsSeparate(void)
{
    return FUNC_SEPARATE || FFT_ENABLED;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void sDisplay_SetBrightness(int16 brightness)
{
    set.display.brightness = brightness;
    Painter_SetBrightnessDisplay(brightness);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
ModeAveraging sDisplay_GetModeAveraging(void)
{
    if (IN_RANDOM_MODE)
    {
        return Averaging_Accurately;
    }
    return set.display.modeAveraging;
}
