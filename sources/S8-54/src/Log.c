#include "Log.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "VCP/VCP.h"

#include <stdarg.h>

#ifdef _MS_VS
#pragma warning(push)
#pragma warning(disable:4100)
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool loggerUSB = false;


#define SIZE_BUFFER_LOG 200

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Log_Write(char *format, ...)
{
    char buffer[SIZE_BUFFER_LOG];
    __va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    Display_AddStringToIndicating(buffer);
    if(loggerUSB)
    {
        VCP_SendFormatStringAsynch(buffer);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
void Log_Error(const char *module, const char *func, int numLine, char *format, ...)
{
    char buffer[SIZE_BUFFER_LOG];
    char message[SIZE_BUFFER_LOG];
    __va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    char numBuffer[20];
    sprintf(numBuffer, ":%d", numLine);
    message[0] = 0;
#ifndef _MS_VS
    strcat(message, "!!!ERROR!!! ");
    strcat(message, module);
    strcat(message, " ");
    strcat(message, func);
    strcat(message, numBuffer);
#endif
    Display_AddStringToIndicating(message);
    Display_AddStringToIndicating(buffer);
    if(loggerUSB)
    {
        VCP_SendFormatStringAsynch(message);
        VCP_SendFormatStringAsynch(buffer);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Log_DisconnectLoggerUSB(void)
{
    //static uint8 data = 20;
    //Log_Write("посылаю %d", data);
    //VCP_SendData(&data, 1);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Log_EnableLoggerUSB(bool enable)
{
    loggerUSB = enable;
}

#ifdef _MS_VS
#pragma warning(pop)
#endif
