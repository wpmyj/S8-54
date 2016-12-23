#pragma once


#include "defines.h"


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
    NumTimers
} TypeTimer2;


void Timer2_Init(void);
void Timer2_Set(TypeTimer2 type, pFuncVV func, uint dTms);              // Назначает таймеру timer функцию и время срабатывания
void Timer2_SetAndStartOnce(TypeTimer2 type, pFuncVV func, uint dTms);
void Timer2_SetAndEnable(TypeTimer2 type, pFuncVV func, uint dTms);
void Timer2_StartOnce(TypeTimer2 type);
void Timer2_Enable(TypeTimer2 type);
void Timer2_Disable(TypeTimer2 type);
bool Timer2_IsRun(TypeTimer2 type);
