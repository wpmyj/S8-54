

#include "defines.h"
#include "SCPI.h"
#include "Settings/Settings.h"
#include "Utils/Map.h"
#include "Utils/GlobalFunctions.h"
#include "VCP/VCP.h"
#include "FPGA/FPGA.h"


/*
    TRIG:
        MODE {AUTO | WAIT | SINGLE | ?}
        SOURCE {CHAN1 | CHAN2 | EXT | ?}
        POLARITY {FRONT | BACK | ?}
        INPUT {FULL | AC | LPF | HPF | ?}
        OFFSET {-200...200 | ?}
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Process_MODE(uint8 *buffer);
static void Process_SOURCE(uint8 *buffer);
static void Process_POLARITY(uint8 *buffer);
static void Process_INPUT(uint8 *buffer);
static void Process_FIND(uint8 *buffer);
static void Process_OFFSET(uint8 *buffer);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Process_TRIG(uint8 *buffer)
{
    static const StructCommand commands[] =
    {
        {"MODE",        Process_MODE},
        {"SOURCE",      Process_SOURCE},
        {"POLARITY",    Process_POLARITY},
        {"POLAR",       Process_POLARITY},
        {"INPUT",       Process_INPUT},
        {"FIND",        Process_FIND},
        {"OFFSET",      Process_OFFSET},
        {0}
    };
    SCPI_ProcessingCommand(commands, buffer);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_MODE(uint8 *buffer)
{
    extern void OnChanged_TrigMode(bool active);
    
    static const MapElement map[] =
    {
        {"AUTO",   0},
        {"WAIT",   1},
        {"SINGLE", 2},
        {"?",      3},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { START_MODE = StartMode_Auto; OnChanged_TrigMode(true); }
        else if (1 == value)    { START_MODE = StartMode_Wait; OnChanged_TrigMode(true); }
        else if (2 == value)    { START_MODE = StartMode_Single; OnChanged_TrigMode(true); }
        else if (3 == value)
        {
            SCPI_SEND(":TRIGGER:MODE %s", map[START_MODE].key);
        }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_SOURCE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"CHAN1", 0},
        {"CHAN2", 1},
        {"EXT",   2},
        {"?",     3},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { FPGA_SetTrigSource(TrigSource_A); }
        else if (1 == value)    { FPGA_SetTrigSource(TrigSource_B); }
        else if (2 == value)    { FPGA_SetTrigSource(TrigSource_Ext); }
        else if (3 == value)
        {
            SCPI_SEND(":TRIGGER:SOUCRE %s", map[START_MODE].key);
        }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_POLARITY(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"FRONT", 0},
        {"BACK",  1},
        {"?",     2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { FPGA_SetTrigPolarity(TrigPolarity_Front); }
        else if (1 == value)    { FPGA_SetTrigPolarity(TrigPolarity_Back); }
        else if (2 == value)
        {
            SCPI_SEND(":TRIGGER:POLARITY %s", map[TRIG_POLARITY].key);
        }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_INPUT(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"FULL", 0},
        {"AC",   1},
        {"LPF",  2},
        {"HPF",  3},
        {"?",    4},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { FPGA_SetTrigInput(TrigInput_Full); }
        else if (1 == value)    { FPGA_SetTrigInput(TrigInput_AC); }
        else if (2 == value)    { FPGA_SetTrigInput(TrigInput_LPF); }
        else if (3 == value)    { FPGA_SetTrigInput(TrigInput_HPF); }
        else if (4 == value)
        {
            SCPI_SEND(":TRIGGER:INPUT %s", map[TRIG_INPUT].key);
        }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_FIND(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"HAND", 0},
        {"AUTO", 1},
        {"FIND", 2},
        {"?",    3},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { TRIG_MODE_FIND = TrigModeFind_Hand; }
        else if (1 == value)    { TRIG_MODE_FIND = TrigModeFind_Auto; }
        else if (2 == value)    { FPGA_FindAndSetTrigLevel(); }
        else if (3 == value)
        {
            SCPI_SEND(":TRIGGER:FIND %s", map[TRIG_MODE_FIND].key);
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
        int trigLev = RShiftZero + 2 * intVal;
        FPGA_SetTrigLev(TRIGSOURCE, (int16)trigLev);
        return;
    }

    ENTER_ANALYSIS
        if (value == 0)
        {
            int retValue = (SET_TRIGLEV(TRIGSOURCE) - RShiftZero) / 2;
            SCPI_SEND(":TRIGGER:OFFSET %d", retValue);
        }
    LEAVE_ANALYSIS
}
