// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "SCPI.h"
#include "Settings/Settings.h"
#include "Settings/SettingsChannel.h"
#include "Utils/Map.h"
#include "Utils/GlobalFunctions.h"
#include "VCP/VCP.h"
#include "FPGA/FPGA.h"
#include "Log.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Process_INPUT(uint8 *buffer);
static void Process_COUPLE(uint8 *buffer);
static void Process_FILTR(uint8 *buffer);
static void Process_INVERSE(uint8 *buffer);
static void Process_RANGE(uint8 *buffer);
static void Process_OFFSET(uint8 *buffer);
static void Process_PROBE(uint8 *buffer);

static Channel ch = A;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Process_CHANNEL(uint8 *buffer) 
{
    static const StructCommand commands[] = 
    {
        {"INPUT",   Process_INPUT},
        {"COUPLE",  Process_COUPLE},
        {"FILTR",   Process_FILTR},
        {"INVERSE", Process_INVERSE},
        {"INV",     Process_INVERSE},
        {"RANGE",   Process_RANGE},
        {"OFFSET",  Process_OFFSET},
        {"PROBE",   Process_PROBE},
        {0}
    };

    ch = (char)(*(buffer - 2)) == '1' ? A : B;

    SCPI_ProcessingCommand(commands, buffer);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_INPUT(uint8 *buffer)
{
    LOG_FUNC_ENTER;
    static const MapElement map[] = 
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_ENABLED(ch) = true; }
        else if (1 == value)    { SET_ENABLED(ch) = false; }
        else if (2 == value)
        {
            SCPI_SEND(":CHANNEL%d:INPUT %s", Tables_GetNumChannel(ch), sChannel_Enabled(ch) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_COUPLE(uint8 *buffer)
{
    static const MapElement map[] = 
    {
        {"DC",  0},
        {"AC",  1},
        {"GND", 2},
        {"?",   3},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_COUPLE(ch) = ModeCouple_DC; }
        else if (1 == value)    { SET_COUPLE(ch) = ModeCouple_AC; }
        else if (2 == value)    { SET_COUPLE(ch) = ModeCouple_GND; }
        else if (3 == value)
        {
            SCPI_SEND(":CHANNEL%d:COUPLE %s", Tables_GetNumChannel(ch), map[SET_COUPLE(ch)].key);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_FILTR(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
/*      // WARN это для C8-53, сделать для С8-54
        if (0 == value)         { set.chan[ch].filtr = true; }
        else if (1 == value)    { set.chan[ch].filtr = false; }
        else if (2 == value)
        {
            SCPI_SEND(":CHANNEL%d:FILTR %s", Tables_GetNumChannel(ch), set.chan[ch].filtr == true ? "ON" : "OFF");
        }
*/
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_INVERSE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_INVERSE(ch) = true; }
        else if (1 == value)    { SET_INVERSE(ch) = false; }
        else if (2 == value)
        {
            SCPI_SEND(":CHANNEL%d:INVERSE %s", Tables_GetNumChannel(ch), SET_INVERSE(ch) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_RANGE(uint8 *buffer)
{
    static const MapElement map[] = 
    {
        {"2MV",     (uint8)Range_2mV},
        {"5MV",     (uint8)Range_5mV},
        {"10MV",    (uint8)Range_10mV},
        {"20MV",    (uint8)Range_20mV},
        {"50MV",    (uint8)Range_50mV},
        {"100MV",   (uint8)Range_100mV},
        {"200MV",   (uint8)Range_200mV},
        {"500MV",   (uint8)Range_500mV},
        {"1V",      (uint8)Range_1V},
        {"2V",      (uint8)Range_2V},
        {"5V",      (uint8)Range_5V},
        {"?",       (uint8)RangeSize},
        {0}
    };
    ENTER_ANALYSIS
        if (value < (uint8)RangeSize)      { FPGA_SetRange(ch, (Range)value); }
        else if (value == (uint8)RangeSize)
        {
            SCPI_SEND(":CHANNEL%d:RANGE %s", Tables_GetNumChannel(ch), map[SET_RANGE(ch)].key);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_OFFSET(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"?", 0},
        {0}
    };
    int intVal = 0;
    if (SCPI_FirstIsInt(buffer, &intVal, -240, 240))
    {
        int rShift = RShiftZero + 2 * intVal;
        FPGA_SetRShift(ch, (int16)rShift);
        return;
    }
    ENTER_ANALYSIS
        if (value == 0)
        {
            int retValue = (SET_RSHIFT(ch) - RShiftZero) / 2;
            SCPI_SEND(":CHANNNEL%d:OFFSET %d", Tables_GetNumChannel(ch), retValue);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_PROBE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"X1",  0},
        {"X10", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (value == 0)         { SET_DIVIDER(ch) = Divider_1; }
        else if (value == 1)    { SET_DIVIDER(ch) = Divider_10; }
        else if (value == 2)
        {
            SCPI_SEND(":CHANNEL%d:PROBE %s", Tables_GetNumChannel(ch), map[SET_DIVIDER(ch)].key);
        }
    LEAVE_ANALYSIS
}
