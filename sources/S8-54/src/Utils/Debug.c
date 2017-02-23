#include "Debug.h"
#include "defines.h"
#include "Hardware/Timer.h"
#include "Log.h"


Debug debug;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_NUM_POINTS 20

typedef struct
{
    char *point;
    uint time;
} LogStruct;

static LogStruct points[MAX_NUM_POINTS];
static uint timeStartLogMS = 0;
static uint timeStartLogTicks = 0;
static uint timeStopLogTics = 0;
static int nextPoint = 0;
static uint timePauseMS = 1000;
static bool running = false;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Sys_PrintSizeStack(void)
{
    uint value = 0;
    __asm
    {
        MRS value, MSP
    }
    value -= 0x10000000;
    LOG_WRITE("%d", value);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void DBG_Log_Start(void)
{
    if (gTimerMS - timeStartLogMS < timePauseMS)
    {
        return;
    }
    running = true;
    timeStartLogTicks = gTimerTics;
    timeStartLogMS = gTimerMS;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void DBG_Log_Point(char *point)
{
    if (running)
    {
        points[nextPoint].point = point;
        points[nextPoint].time = gTimerTics;
        nextPoint++;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void DBG_Log_Out(void)
{
    if (running)
    {
        timeStopLogTics = gTimerTics;
        LOG_WRITE("");

        float fullTimeTicks = (float)(points[nextPoint - 1].time - timeStartLogTicks);

        for (int i = 1; i < nextPoint; i++)
        {
            LOG_WRITE("%s - %d us, %.1f %%",
                      points[i].point,
                      (points[i].time - timeStartLogTicks) / 90,
                      (points[i].time - points[i - 1].time) / fullTimeTicks * 100.0f
                      );
        }
        LOG_WRITE("Полное время %.1f us", (timeStopLogTics - timeStartLogTicks) / 90.0f);
        running = false;
        nextPoint = 0;
    }
}
