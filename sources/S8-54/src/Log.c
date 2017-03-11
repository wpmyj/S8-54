// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Log.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "VCP/VCP.h"
#include <stdarg.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool loggerUSB = false;


#define SIZE_BUFFER_LOG 200

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Log_Write(TypeTrace type, char *format, ...)
{
    char buffer[SIZE_BUFFER_LOG];
    char *pointer = buffer;

    if (type == TypeTrace_Error)
    {
        buffer[0] = 0;
        strcat(buffer, "!!! ERROR !!! ");
        while (*pointer++) {};
        ++pointer;
    }
    __va_list args;
    va_start(args, format);
    vsprintf(pointer, format, args);
    va_end(args);
    Display_AddStringToIndicating(buffer);
    if(loggerUSB)
    {
        VCP_SendFormatStringAsynch(buffer);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
void Log_Trace(TypeTrace type, const char *module, const char *func, int numLine, char *format, ...)
{
    char buffer[SIZE_BUFFER_LOG];
    char message[SIZE_BUFFER_LOG];
    __va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    const int SIZE = 20;
    char numBuffer[SIZE];
    snprintf(numBuffer, 100, ":%d", numLine);
    message[0] = 0;
    if (type == TypeTrace_Error)
    {
        strcat(message, "!!!ERROR!!! ");
    }
    else if (type == TypeTrace_Info)
    {
        strcat(message, "            ");
    }
    strcat(message, module);
    strcat(message, " ");
    strcat(message, func);
    strcat(message, numBuffer);
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
