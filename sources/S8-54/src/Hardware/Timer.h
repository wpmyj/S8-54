#pragma once 


#include "defines.h"
#include "Utils/GlobalFunctions.h"


void    Timer_PauseOnTime(uint timeMS);
void    Timer_PauseOnTicks(uint numTicks);

void    Timer_StartMultiMeasurement(void);      // Запускает счётчик для измерения малых отрезков времени.

void    Timer_StartLogging(void);               // Устанавливает стартовую точку логгирования. Далее вызовы Timer_LogPoint засекают временные интервалы от это точки.
uint    Timer_LogPointUS(char *name);
uint    Timer_LogPointMS(char *name);

//extern uint gTimerMS;

/**
  * gTimerTics - количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement(). Не более (1 << 32)
  * В одной секунде 120.000.000 тиков для С8-53 и 90.000.000 тиков для С8-54.
  * Максимальный отрезок времени, который можно отсчитать с её помощью - 35 сек.
***/
#define gTimerTics (TIM2->CNT)
#define gTimerUS (TIM2->CNT / 90)
    
// Время, прошедшее с момента инициализации таймера, в миллисекундах
#define gTimerMS (TIM5->CNT / 2)                    
