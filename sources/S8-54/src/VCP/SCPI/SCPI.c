

#include "defines.h"
#include "SCPI.h"
#include "commonSCPI.h"
#include "controlSCPI.h"
#include "Utils/Strings.h"
#include "Utils/GlobalFunctions.h"
#include <ctype.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    WAIT,
    SAVE_SYMBOLS
} StateProcessing;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SCPI_ParseNewCommand(uint8 *buffer, int length)
{
    static const StructCommand commands[] =
    {
        {"*IDN?",       Process_IDN},
        {"*RST",        Process_RESET},
        {"RUN",         Process_RUN},
        {"STOP",        Process_STOP},
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

    uint8 *addrRET = buffer;
    uint8 *addrLast = buffer + length;

    while (*addrRET != 0x0a && *addrRET != 0x0d)
    {
        ++addrRET;
        if (addrRET == addrLast) // Если вышли за границу массива
        {
            return 0;
        }
    }

    for (uint8 *address = buffer; address <= addrRET; address++)
    {
        *address = toupper(*address);
    }

    SCPI_ProcessingCommand(commands, buffer);

    return addrRET - buffer + 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI_ProcessingCommand(const StructCommand *commands, uint8 *buffer) 
{
    while(*buffer == ':')
    {
        ++buffer;
    }
    
    int numCommand = -1;
    char *name = 0;
    do 
    {
        numCommand++;   
        name = commands[numCommand].name;
    } while (name != 0 && (!EqualsStrings(name, (char*)buffer)));

    if (name) 
    {
        commands[numCommand].func(buffer + strlen(name) + 1); // Передаём в функцию указатель на первый символ за двоеточием, следующим после команды
    }
    else
    {
        SCPI_SEND("COMMAND ERROR");
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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
