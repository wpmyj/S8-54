#pragma once
#include "defines.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Utils
 *  @{
 *  @defgroup Debug
 *  @{
 */

void DBG_PrintSizeStack(void);

#define PROFILER_INIT()   uint timeProfiler = gTimerTics; uint time_ = 0;

#define PROFILER_POINT(NamePoint)                       \
    time_ = gTimerTics - timeProfiler;                  \
    Log_Write("%s = %d ìêñ", NamePoint, time_ / 90);    \
    timeProfiler = gTimerTics;

#define PROFILER_POINT_TICS()                                       \
    time_ = gTimerTics - timeProfiler;                              \
    Log_Write("%s %d = %d tics", __FUNCTION__, __LINE__, time_);    \
    timeProfiler = gTimerTics;

//#define WRITE_STACK debugStruct.numberLine = __LINE__; debugStruct.function = __FUNCTION__;  PrintSizeStack();
//#define WRITE_POINT(x) debugStruct.step = x;

typedef struct
{
    uint count;
    uint timeDataReadFull;
    uint timeStartDataReadOne;
    uint timeDataReadOne;

    uint timeDataReadRandomizeFull;
    uint timeStartDataReadRandomizeOnec;

    uint timeCopyMemoryFull;
    uint timeStartCopyMemory;
} Debug;

//extern Debug debug;
//void DBG_Log_Start(void);
//void DBG_Log_Point(char *point);
//void DBG_Log_Out(void);


void DBG_Breakpoint(int numBreakpoint);

extern int breakpoint;

/** @}  @}
 */
