#pragma once


#include "defines.h"


typedef enum
{
    kPressKey,
    kShowLevelRShiftA,
    kShowLevelRShiftB,
    kShowLevelTrigLev,
    kNumSignalsInSec,
    kFlashDisplay,
    kShowMessages,
    kMenuAutoHide,
    kRShiftMarkersAutoHide,
    kTemp,
    kStopSound,
    kTemporaryPauseFPGA,
    kTimerDrawHandFunction,
    NumTimers
} TypeTimer2;


void Timer2_Init(void);
void Timer2_Set(TypeTimer2 type, pFuncVV func, uint dTms);              // ��������� ������� timer ������� � ����� ������������
void Timer2_SetAndStartOnce(TypeTimer2 type, pFuncVV func, uint dTms);
void Timer2_SetAndEnable(TypeTimer2 type, pFuncVV func, uint dTms);
void Timer2_StartOnce(TypeTimer2 type);
void Timer2_Enable(TypeTimer2 type);
void Timer2_Disable(TypeTimer2 type);
