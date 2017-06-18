// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Debug.h"
#include "defines.h"
#include "Log.h"
#include "FPGA/Data.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"


Debug debug;

int numLine = 0;
char* nameFile = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DBG_PrintSizeStack(void)
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
    if (gTimeMS - timeStartLogMS < timePauseMS)
    {
        return;
    }
    running = true;
    timeStartLogTicks = gTimerTics;
    timeStartLogMS = gTimeMS;
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DBG_LogFirstBytes(uint8 *buffer)
{
    LOG_WRITE("%d %d %d %d %d %d %d %d %d %d",
              buffer[0], buffer[1], buffer[2], buffer[3], buffer[4],
              buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
    LOG_WRITE("%d %d %d %d %d %d %d %d %d %d", 
              buffer[10], buffer[11], buffer[12], buffer[13], buffer[14],
              buffer[15], buffer[16], buffer[17], buffer[18], buffer[19]);
}
