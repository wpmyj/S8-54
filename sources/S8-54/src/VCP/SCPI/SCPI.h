#pragma once
#include "Globals.h"
#include "VCP/VCP.h"
#include "Ethernet/TcpSocket.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ENTER_ANALYSIS                                  \
    Word parameter;                                     \
    if (GetWord(buffer, &parameter, 0)) {               \
        uint8 value = GetValueFromMap(map, &parameter); \
        if (value < 255) {

#define LEAVE_ANALYSIS   }}


#define SCPI_SEND(...)                              \
    if(gConnectToHost)                              \
    {                                               \
        VCP_SendFormatStringAsynch(__VA_ARGS__);    \
    };                                              \
    if (gEthIsConnected)                            \
    {                                               \
        TCPSocket_SendFormatString(__VA_ARGS__);    \
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
    char        *name;
    pFuncpU8    func;
} StructCommand;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SCPI_ParseNewCommand(uint8 *buffer);
void SCPI_ProcessingCommand(const StructCommand *commands, uint8 *buffer);
void Process_DISPLAY(uint8 *buffer);
void Process_CHANNEL(uint8 *buffer);
void Process_TRIG(uint8 *buffer);
void Process_TBASE(uint8 *buffer);
bool SCPI_FirstIsInt(uint8 *buffer, int *value, int min, int max);
