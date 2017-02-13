#pragma once


#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Tables.h"
#include "Painter.h"

/*
    ѕри поточечном режиме вывода текущие точки берутс€ из DS_GetData(0),
    а последний полный сигнал из DS_GetData(1);
*/

void    Display_Init(void);
void    Display_Update(void);
void    Display_RotateRShift(Channel ch);
void    Display_RotateTrigLev(void);
void    Display_Redraw(void);
void    Display_EnableTrigLabel(bool enable);
void    Display_SetDrawMode(DrawMode mode, pFuncVV func);
void    Display_SetAddDrawFunction(pFuncVV func);
pFuncVV Display_GetAddDrawFunction(void);
void    Display_RemoveAddDrawFunction(void);
void    Display_Clear(void);
void    Display_ShiftScreen(int delta);
void    Display_ChangedRShiftMarkers(bool active);
void    Display_AddStringToIndicating(const char *string);
void    Display_OneStringUp(void);
void    Display_OneStringDown(void);
void    Display_SetPauseForConsole(bool pause);
void    Display_SetOrientation(DisplayOrientation orientation);
void    Display_DrawConsole(void);                              // ¬ывести сообщеи€ отладочной консоли


typedef enum
{
    LimitChan1_Volts,
    LimitChan2_Volts,
    LimitSweep_Time,
    EnabledPeakDet,
    LimitChan1_RShift,
    LimitChan2_RShift,
    LimitSweep_Level,
    LimitSweep_TShift,
    TooSmallSweepForPeakDet,
    TooFastScanForRecorder,
    FileIsSaved,
    SignalIsSaved,
    SignalIsDeleted,
    MenuDebugEnabled,
    MovingData,
    TimeNotSet,
    SignalNotFound,
    SetTPosToLeft,
    VerySmallMeasures,
    NeedRebootDevice,
    ImpossibleEnableMathFunction,
    ImpossibleEnableFFT,
    WrongFileSystem,
    WrongModePeackDet,
    DisableChannel2,
    TooLongMemory,
    NoPeakDet32k,
    NoPeakDet16k,
    Warn50Ohms,
    WarnNeedForFlashDrive,
    Warning_NumWarnings
} Warning;

void    Display_ClearFromWarnings(void);
void    Display_ShowWarning(Warning warning);
