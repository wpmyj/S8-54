#include "defines.h"
#include "Timer.h"
#include "Log.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Timer_PauseOnTicks(uint numTicks)
{
    uint startTicks = gTimerTics;
    while (gTimerTics - startTicks < numTicks) {};
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_PauseOnTime(uint timeMS)
{
    uint time = gTimerMS;
    while(gTimerMS - time < timeMS) {};
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_StartMultiMeasurement(void)
{
    TIM2->CR1 &= (uint)~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN; 
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static uint timeStartLogging = 0;
static uint timePrevPoint = 0;


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_StartLogging(void)
{
    timeStartLogging = gTimerTics;
    timePrevPoint = timeStartLogging;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint Timer_LogPointUS(char *name)
{
    uint interval = gTimerTics - timePrevPoint;
    timePrevPoint = gTimerTics;
    LOG_WRITE("%s %.2f us", name, interval / 120.0f);
    return interval;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint Timer_LogPointMS(char *name)
{
    uint interval = gTimerTics - timePrevPoint;
    timePrevPoint = gTimerTics;
    LOG_WRITE("%s %.2f ms", name, interval / 120e3);
    return interval;
}
