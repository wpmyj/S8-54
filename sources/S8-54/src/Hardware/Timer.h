#pragma once 


#include "defines.h"
#include "Utils/GlobalFunctions.h"


typedef enum
{
    kPressKey,                  // Нужно устанавливать, когда приходит нажатие клавиши.
    kShowLevelRShiftA,          // Нужно устанавливать, когда изменяется положение ручки смещения канала 1.
    kShowLevelRShiftB,          // Нужно устанавливать, когда изменяется положение ручки смещения канала 2.
    kShowLevelTrigLev,          // Нужно устанавливать, когда изменяется положение ручки уровня синхронизации
    kNumSignalsInSec,           // Для установки количества считываний сигнала в секунду.
    kFlashDisplay,              // Таймер для мерцающих участков экрана чёрно-белого дисплея.
    kShowMessages,              // Таймер для засекания время показа информационных и предупреждающих сообщений.
    kMenuAutoHide,              // Таймер для отсчёта времени скрывания меню.
    kRShiftMarkersAutoHide,     // Таймер для отсчёта времени скрывания дополнительных боковых меток.
    kTemp,                      // Вспомогательный, для общих нужд.
    kStopSound,                 // Выключить звук
    kTemporaryPauseFPGA,        // Временная пауза для фиксации сигнала на экране после поворота ручек
    kTimerDrawHandFunction,     // Этот таймер могут использовать функции, использующие свою процедуру рисования
    kStrNaviAutoHide,           // Прятать строку навигации меню
    kTimerStartP2P,             // Таймер для запуска следующего цикла поточечного чтения
    kTimerMountFlash,           // Таймер для отрисовки дисплея при монтировании флешки
    kTimerBalanceChannel,       // Таймер для функции отрисовки балансировки
    TypeTimerSize               // Общее количество таймеров.
} TypeTimer;

typedef enum
{
    kDelayTrig                  // Для СИНХР -> Удержание
} TypePersonalTimer;

void    Timer_Init(void);
void    Timer_PauseOnTime(uint timeMS);
void    Timer_PauseOnTicks(uint numTicks);

void    Timer_Enable(TypeTimer type, int timeInMS, pFuncVV);
void    Timer_Disable(TypeTimer type);          // Остановить таймер
void    Timer_Pause(TypeTimer type);            // Приостановить таймер. Запустить снова можно функцией Timer_Continue()
void    Timer_Continue(TypeTimer type);         // Продолжить работу таймера, ранее приостановленного функцией Timer_Pause()
bool    Timer_IsRun(TypeTimer type);            // С помощью этой функции можно узнать, работает ли таймер

void    Timer_StartMultiMeasurement(void);      // Запускает счётчик для измерения малых отрезков времени.

void    Timer_StartLogging(void);               // Устанавливает стартовую точку логгирования. Далее вызовы Timer_LogPoint засекают временные интервалы от это точки.
uint    Timer_LogPointUS(char *name);
uint    Timer_LogPointMS(char *name);

void PersonalTimer_Start(TypePersonalTimer type, uint timeMS, bool repeat, pFuncVV func);
void PersonalTimer_Stop(TypePersonalTimer type);

//extern uint gTimerMS;

/**
  * gTimerTics - количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement(). Не более (1 << 32)
  * В одной секунде 120.000.000 тиков для С8-53 и 90.000.000 тиков для С8-54.
  * Максимальный отрезок времени, который можно отсчитать с её помощью - 35 сек.
***/
#define gTimerTics (TIM2->CNT)
#define TICS_IN_US(x) ((x) / 90)

// Время, прошедшее с момента инициализации таймера, в миллисекундах
#define gTimerMS (TIM5->CNT / 10)                    
