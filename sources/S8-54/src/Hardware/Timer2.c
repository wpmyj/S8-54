#include "Timer2.h"
#include "Timer.h"
#include "Log.h"


typedef struct
{
    pFuncVV func;       // Функция таймера
    uint dTms;          // Период срабатывания, мс
    uint timeFirstMS;   // Время первого срабатывания
    uint timeNextMS;    // Время следующего срабатывания. Если == 0xffffffff, то таймер неактивен
    bool repeat;        // Если true, будет срабатывать, пока не будет вызвана функция Timer2_Disable()
} TimerStruct;


static TimerStruct timers[NumTimers];

static TIM_HandleTypeDef timHandle;


#undef TIME_NEXT
#define TIME_NEXT(type) (timers[type].timeNextMS)


static void StartTIM(uint timeStop);    // Завести таймр, который остановится в timeStop мс
static void StopTIM(void);
static uint NearestTime(void);          // Возвращает время срабатывания ближайщего таймера, либо 0, если таймеров нет
static void TuneTIM(TypeTimer2 type);   // Настроить систему на таймер


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Timer2_Init(void)
{
    for(uint i = 0; i < NumTimers; i++)
    {
        timers[i].timeNextMS = 0xffffffff;
    }

#ifndef _MS_VS
    __HAL_RCC_TIM3_CLK_ENABLE();
#endif

    HAL_NVIC_EnableIRQ(TIM3_IRQn);
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);

    timHandle.Instance = TIM3;
    timHandle.Init.Period = 1;
    uint prescaler = 42000 - 1;
    timHandle.Init.Prescaler = prescaler;
    timHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timHandle.Init.CounterMode = TIM_COUNTERMODE_UP;

    HAL_TIM_Base_Init(&timHandle);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&timHandle);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Timer2_Set(TypeTimer2 type, pFuncVV func, uint dTms)
{
    TimerStruct *timer = &timers[type];
    timer->func = func;
    timer->dTms = dTms;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Timer2_SetAndStartOne(TypeTimer2 type, pFuncVV func, uint dTms)
{
    Timer2_Set(type, func, dTms);
    Timer2_StartOnce(type);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Timer2_SetAndEnable(TypeTimer2 type, pFuncVV func, uint dTms)
{
    Timer2_Set(type, func, dTms);
    Timer2_Enable(type);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Timer2_StartOnce(TypeTimer2 type)
{
    timers[type].repeat = false;
    TuneTIM(type);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Timer2_Enable(TypeTimer2 type)
{
    timers[type].repeat = true;
    TuneTIM(type);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void TuneTIM(TypeTimer2 type)
{
    TimerStruct *timer = &timers[type];
    timer->timeFirstMS = gTimerMS;

    uint timeNearest = NearestTime();

    uint timeNext = timer->timeFirstMS + timer->dTms;
    timer->timeNextMS = timeNext;

    if(timeNext < timeNearest)      // Если таймер должен сработать раньше текущего
    {
        StartTIM(timeNext);         // то заводим таймер на наше время
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Timer2_Disable(TypeTimer2 type)
{
    timers[type].timeNextMS = 0xffffffff;
    timers[type].repeat = false;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static uint NearestTime(void)
{
    uint time = 0xffffffff;

    for(uint type = 0; type < NumTimers; type++)
    {
        if(TIME_NEXT(type) < time)
        {
            time = TIME_NEXT(type);
        }
    }
    
    return time;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void StartTIM(uint timeStopMS)
{
    StopTIM();

    if(timeStopMS == 0xffffffff)
    {
        return;
    }

    uint dT = timeStopMS - gTimerMS;

    timHandle.Instance = TIM3;
    timHandle.Init.Period = (dT * 2) - 1;      // 10 соответствует 0.1мс. Т.е. если нам нужна 1мс, нужно засылать (100 - 1)
    timHandle.Init.Prescaler = 45000 - 1;
    timHandle.Init.ClockDivision = 0;
    timHandle.Init.CounterMode = TIM_COUNTERMODE_UP;

    HAL_TIM_Base_Init(&timHandle);
    HAL_TIM_Base_Start_IT(&timHandle);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    uint time = gTimerMS;
    uint nearestTime = NearestTime();

    if (nearestTime > time)
    {
        return;
    }

    StopTIM();

    for (uint type = 0; type < NumTimers; type++)
    {
        if (TIME_NEXT(type) <= time)            // Если пришло время срабатывания
        {
            TimerStruct *timer = &timers[type];
            timer->func();
            if (timer->repeat)
            {
                timer->timeNextMS += timer->dTms;
            }
            else
            {
                timer->timeNextMS = 0xffffffff;
            }
        }
    }

    StartTIM(NearestTime());
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void StopTIM(void)
{
    HAL_TIM_Base_Stop_IT(&timHandle);
}


#undef TIME_NEXT
