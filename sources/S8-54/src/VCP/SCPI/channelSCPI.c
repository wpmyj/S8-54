

#include "defines.h"
#include "SCPI.h"
#include "FPGA/FPGAextensions.h"
#include "Settings/Settings.h"
#include "Settings/SettingsChannel.h"
#include "Utils/Map.h"
#include "Utils/GlobalFunctions.h"
#include "VCP/VCP.h"
#include "FPGA/FPGA.h"
#include "Log.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Process_INPUT(uint8 *buffer);
static void Process_COUPLING(uint8 *buffer);
static void Process_BWLIMIT(uint8 *buffer);
static void Process_INVERT(uint8 *buffer);
static void Process_RANGE(uint8 *buffer);
static void Process_OFFSET(uint8 *buffer);
static void Process_PROBE(uint8 *buffer);
static void Process_BALANCE(uint8 *buffer);

static Channel ch = A;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Process_CHANNEL(uint8 *buffer) 
{
    static const StructCommand commands[] = 
    {
        {"INPUT",       Process_INPUT},
        {"PROBE",       Process_PROBE},
        {"PROB",        Process_PROBE},
        {"INVERT",      Process_INVERT},
        {"INV",         Process_INVERT},
        {"COUPLING",    Process_COUPLING},
        {"COUP",        Process_COUPLING},
        {"BWLIMIT",     Process_BWLIMIT},
        {"BWL",         Process_BWLIMIT},
        {"SCALE",       Process_RANGE},
        {"SCAL",        Process_RANGE},
        {"BALANCE",     Process_BALANCE},
        {"BAL",         Process_BALANCE},
        {"OFFSET",      Process_OFFSET},
        {"OFFS",        Process_OFFSET},
        {0}
    };

    ch = (char)(*(buffer - 2)) == '1' ? A : B;

    SCPI_ProcessingCommand(commands, buffer);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_INPUT(uint8 *buffer)
{
    static const MapElement map[] = 
    {
        {"ON",  0},
        {"1",   1},
        {"OFF", 2},
        {"0",   3},
        {"?",   4},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value || 1 == value)       { SET_ENABLED(ch) = true; }
        else if (2 == value || 3 == value)  { SET_ENABLED(ch) = false; }
        else if (4 == value)
        {
            SCPI_SEND(":CHANNEL%d:INPUT %s", Tables_GetNumChannel(ch), SET_ENABLED(ch) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_COUPLING(uint8 *buffer)
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
        if (0 == value)         { SET_COUPLE(ch) = ModeCouple_DC; FPGA_SetModeCouple(ch, SET_COUPLE(ch)); }
        else if (1 == value)    { SET_COUPLE(ch) = ModeCouple_AC; FPGA_SetModeCouple(ch, SET_COUPLE(ch)); }
        else if (2 == value)    { SET_COUPLE(ch) = ModeCouple_GND; FPGA_SetModeCouple(ch, SET_COUPLE(ch)); }
        else if (3 == value)
        {
            SCPI_SEND(":CHANNEL%d:COUPLING %s", Tables_GetNumChannel(ch), map[SET_COUPLE(ch)].key);
        }
    LEAVE_ANALYSIS
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_BWLIMIT(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ON",  0},
        {"1",   1},
        {"OFF", 2},
        {"0",   3},
        {"?",   4},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value || 1 == value)         { SET_BANDWIDTH(ch) = Bandwidth_20MHz; FPGA_SetBandwidth(ch); }
        else if (2 == value || 3 == value)    { SET_BANDWIDTH(ch) = Bandwidth_Full; FPGA_SetBandwidth(ch); }
        else if (4 == value)
        {
            SCPI_SEND(":CHANNEL%d:FILTR %s", Tables_GetNumChannel(ch), SET_BANDWIDTH(ch) == Bandwidth_20MHz ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_INVERT(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ON",  0},
        {"1",   1},
        {"OFF", 2},
        {"0",   3},
        {"?",   4},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value || 1 == value)       { SET_INVERSE(ch) = true; }
        else if (2 == value || 3 == value)  { SET_INVERSE(ch) = false; }
        else if (4 == value)
        {
            SCPI_SEND(":CHANNEL%d:INVERT %s", Tables_GetNumChannel(ch), SET_INVERSE(ch) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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
        if (value == (uint8)RangeSize)
        {
            SCPI_SEND(":CHANNEL%d:RANGE %s", Tables_GetNumChannel(ch), map[SET_RANGE(ch)].key);
        }
        else
        {
            FPGA_SetRange(ch, (Range)value);
        }
    LEAVE_ANALYSIS
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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
        int rShift = RShiftZero + (intVal * RSHIFT_IN_CELL / 20);
        FPGA_SetRShift(ch, (int16)rShift);
        return;
    }
    ENTER_ANALYSIS
        if (value == 0)
        {
            int retValue = (SET_RSHIFT(ch) - RShiftZero) / (RSHIFT_IN_CELL / 20);
            SCPI_SEND(":CHANNNEL%d:OFFSET %d", Tables_GetNumChannel(ch), retValue);
        }
        else
        {
            SCPI_SEND("COMMAND ERROR");
        }
    LEAVE_ANALYSIS

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_PROBE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1/1",  0},
        {"1/10", 1},
        {"?",    2},
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_BALANCE(uint8 *buffer)
{
    FPGA_BalanceChannel(ch);
}
