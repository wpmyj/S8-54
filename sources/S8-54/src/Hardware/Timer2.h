#pragma once


#include "defines.h"


typedef enum
{
    kPressKey,
    kShowLevelRShiftA,
    kShowLevelRShiftB,
    kShowLevelTrigLev,
    NumTimers
} TypeTimer2;


void Timer2_Init(void);
void Timer2_Set(TypeTimer2 type, pFuncVV func, uint dTms);              // Назначает таймеру timer функцию и время срабатывания
void Timer2_SetAndStartOne(TypeTimer2 type, pFuncVV func, uint dTms);
void Timer2_SetAndEnable(TypeTimer2 type, pFuncVV func, uint dTms);
void Timer2_StartOnce(TypeTimer2 type);
void Timer2_Enable(TypeTimer2 type);
void Timer2_Disable(TypeTimer2 type);
