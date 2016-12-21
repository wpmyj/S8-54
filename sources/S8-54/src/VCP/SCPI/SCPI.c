#include "defines.h"
#include "SCPI.h"
#include "commonSCPI.h"
#include "controlSCPI.h"
#include "Utils/Strings.h"
#include "Utils/GlobalFunctions.h"


#include <ctype.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    WAIT,
    SAVE_SYMBOLS
} StateProcessing;

static int FindNumSymbolsInCommand(uint8 *buffer);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SCPI_ParseNewCommand(uint8 *buffer)
{
    static const StructCommand commands[] =
    {
        {"RUN",         Process_RUN},
        {"STOP",        Process_STOP},
        {"RESET",       Process_RESET},
        {"AUTOSCALE",   Process_AUTOSCALE}, 
        {"REQUEST ?",   Process_REQUEST},

        {"DISPLAY",     Process_DISPLAY},
        {"DISP",        Process_DISPLAY},

        {"CHANNEL1",    Process_CHANNEL},
        {"CHAN1",       Process_CHANNEL},

        {"CHANNEL2",    Process_CHANNEL},
        {"CHAN2",       Process_CHANNEL},

        {"TRIGGER",     Process_TRIG},
        {"TRIG",        Process_TRIG},

        {"TBASE",       Process_TBASE},
        {"TBAS" ,       Process_TBASE},

        {"KEY",         Process_KEY},
        {"GOVERNOR",    Process_GOVERNOR},
        {0}
    };

    SCPI_ProcessingCommand(commands, buffer);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI_ProcessingCommand(const StructCommand *commands, uint8 *buffer) 
{
    int sizeNameCommand = FindNumSymbolsInCommand(buffer);
    if (sizeNameCommand == 0) 
    {
        return;
    }
    for (int i = 0; i < sizeNameCommand; i++)
    {
        buffer[i] = (uint8)toupper(buffer[i]);
    }
    int numCommand = -1;
    char *name = 0;
    do 
    {
        numCommand++;   
        name = commands[numCommand].name;
    } while (name != 0 && (!EqualsStrings((char*)buffer, name, sizeNameCommand)));

    if (name != 0) 
    {
        commands[numCommand].func(buffer + sizeNameCommand + 1);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int FindNumSymbolsInCommand(uint8 *buffer)
{
    int pos = 0;
    while ((buffer[pos] != ':') && (buffer[pos] != ' ') && (buffer[pos] != '\x0d'))
    {
        pos++;
    }
    return pos;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool SCPI_FirstIsInt(uint8 *buffer, int *value, int min, int max)
{
    Word param;
    if (GetWord(buffer, &param, 0))
    {
        char n[256];
        memcpy(n, param.address, param.numSymbols);
        n[param.numSymbols] = '\0';
        if (String2Int(n, value) && *value >= min && *value <= max)
        {
            return true;
        }
    }
    return false;
}
