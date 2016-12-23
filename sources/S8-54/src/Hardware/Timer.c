#include "defines.h"
#include "Timer.h"
#include "Log.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void (*f[TypeTimerSize])(void) = {0};
static uint reactionTimeMS[TypeTimerSize] = {0};
static int timePrevExecuteMS[TypeTimerSize] = {0};
static bool isRun[TypeTimerSize] = {false};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Timer_Init(void)
{
#ifndef _MS_VS
    __TIM2_CLK_ENABLE();        // Для тиков
    __TIM5_CLK_ENABLE();
#endif

    TIM_HandleTypeDef handleTIM2 =
    {
        TIM2,
        {
            0,
            TIM_COUNTERMODE_UP,
            0xffffffff,
            TIM_CLOCKDIVISION_DIV1
        }
    };

    HAL_TIM_Base_Init((TIM_HandleTypeDef*)&handleTIM2);

    HAL_TIM_Base_Start((TIM_HandleTypeDef*)&handleTIM2);

    TIM_HandleTypeDef handleTIM5 =
    {
        TIM5,
        {
            44999,             // WARN Так и не разобрался, как настроить таймер на 1мс. При 44999 период счета - 2мс.
            TIM_COUNTERMODE_UP,
            0xffffffff,
            TIM_CLOCKDIVISION_DIV1
        }
    };

    HAL_TIM_Base_Init((TIM_HandleTypeDef*)&handleTIM5);

    HAL_TIM_Base_Start((TIM_HandleTypeDef*)&handleTIM5);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_Enable(TypeTimer type, int timeInMS, void(*eF)(void))
{
    f[type] = eF;
    reactionTimeMS[type] = timeInMS;
    timePrevExecuteMS[type] = gTimerMS;
    isRun[type] = true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_Disable(TypeTimer type)
{
    isRun[type] = false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_Pause(TypeTimer type)
{
    isRun[type] = false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_Continue(TypeTimer type)
{
    isRun[type] = true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool Timer_IsRun(TypeTimer type)
{
    return isRun[type];
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_Update(void)
{
    uint curTimeMS = gTimerMS;

    for(int num = 0; num < TypeTimerSize; num++)
    {
        if(isRun[num])
        {
            uint delta = curTimeMS - timePrevExecuteMS[num];
            if(delta >= reactionTimeMS[num])
            {
                f[num]();
                timePrevExecuteMS[num] = curTimeMS;
            }
            
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------------------------------
void PersonalTimer_Start(TypePersonalTimer type, uint timeMS, bool repeat, pFuncVV func)
{

}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void PersonalTimer_Stop(TypePersonalTimer type)
{

}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void TIM2_IRQHandler(void)
{

}
