#pragma once   
#include "defines.h"


#ifdef DEBUG
#define LOG_WRITE(...)          Log_Write(TypeTrace_Info, __VA_ARGS__)
#define LOG_ERROR(...)          Log_Write(TypeTrace_Error, __VA_ARGS__)
#define LOG_WRITE_TRACE(...)    Log_Trace(TypeTrace_Info, __MODULE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_ERROR_TRACE(...)    Log_Trace(TypeTrace_Error, __MODULE__, __FUNCTION__, __LINE__, __VA_ARGS__)
// Когда нужен вспомогательный буфер для преобразования числа в строку, можно пользоваться этой функцией
#define LOG_WRITE_BUF(...)      { char buffer[100]; Log_Write(TypeTrace_Info, __VA_ARGS__); }
#define LOG_FUNC_ENTER          Log_Write(TypeTrace_Info, "%s enter", __FUNCTION__);
#define LOG_FUNC_LEAVE          Log_Write(TypeTrace_Info, "%s leave", __FUNCTION__);
#define LOG_TRACE               Log_Write(TypeTrace_Info, "%s : %d", __MODULE__, __LINE__);
#define ASSEERT(cond, ...)      if(cond)(LOG_ERROR_TRACE(__VA_ARGS__));
#define ASSERT_RET(cond, ...)   if(cond) {LOG_ERROR_TRACE(__VA_ARGS__); return; }
#else
#define LOG_WRITE(...)
#define LOG_ERROR(...)
#define LOG_WRITE_TRACE(...)
#define LOG_ERROR_TRACE(...)
#define LOG_WRITE_BUF(...)
#define LOG_FUNC_ENTER
#define LOG_FUNC_LEAVE
#define LOG_TRACE
#define ASSERT(const, ...)
#define ASSERT_RET(cont, ...)
#endif


typedef enum
{
    TypeTrace_Info,
    TypeTrace_Error
} TypeTrace;


void Log_Trace(TypeTrace type, const char *module, const char *func, int numLine, char *format, ...);
void Log_Write(TypeTrace type, char *format, ...);
void Log_DisconnectLoggerUSB(void);
void Log_EnableLoggerUSB(bool enable);
