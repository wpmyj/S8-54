#include "defines.h"


#include "SCPI.h"
#include "Settings/Settings.h"
#include "Utils/Map.h"
#include "VCP/VCP.h"
#include "FPGA/FPGA.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Process_RANGE(uint8 *buffer);
static void Process_OFFSET(uint8 *buffer);
static void Process_SAMPLING(uint8 *buffer);
static void Process_PEACKDET(uint8 *buffer);
static void Process_TPOS(uint8 *buffer);
static void Process_SELFRECORDER(uint8 *buffer);
static void Process_FUNCTIMEDIV(uint8 *buffer);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Process_TBASE(uint8 *buffer)
{
    static const StructCommand commands[] =
    {
        {"RANGE",       Process_RANGE},
        {"OFFSET",      Process_OFFSET},
        {"SAMPLING",    Process_SAMPLING},
        {"SAMPL",       Process_SAMPLING},
        {"PEACKDET",    Process_PEACKDET},
        {"PEACK",       Process_PEACKDET},
        {"TPOS",        Process_TPOS},
        {"SELFRECORDER",Process_SELFRECORDER},
        {"FUNCTIMEDIV", Process_FUNCTIMEDIV},
        {0}
    };
    SCPI_ProcessingCommand(commands, buffer);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_RANGE(uint8 *buffer)
{
    static const MapElement map[] = 
    {
        {"1ns",     (uint8)TBase_1ns},
        {"2ns",     (uint8)TBase_2ns},
        {"5ns",     (uint8)TBase_5ns},
        {"10ns",    (uint8)TBase_10ns},
        {"20ns",    (uint8)TBase_20ns},
        {"50ns",    (uint8)TBase_50ns},
        {"100ns",   (uint8)TBase_100ns},
        {"200ns",   (uint8)TBase_200ns},
        {"500ns",   (uint8)TBase_500ns},
        {"1us",     (uint8)TBase_1us},
        {"2us",     (uint8)TBase_2us},
        {"5us",     (uint8)TBase_5us},
        {"10us",    (uint8)TBase_10us},
        {"20us",    (uint8)TBase_20us},
        {"50us",    (uint8)TBase_50us},
        {"100us",   (uint8)TBase_100us},
        {"200us",   (uint8)TBase_200us},
        {"500us",   (uint8)TBase_500us},
        {"1ms",     (uint8)TBase_1ms},
        {"2ms",     (uint8)TBase_2ms},
        {"5ms",     (uint8)TBase_5ms},
        {"10ms",    (uint8)TBase_10ms},
        {"20ms",    (uint8)TBase_20ms},
        {"50ms",    (uint8)TBase_50ms},
        {"100ms",   (uint8)TBase_100ms},
        {"200ms",   (uint8)TBase_200ms},
        {"500ms",   (uint8)TBase_500ms},
        {"1s",      (uint8)TBase_1s},
        {"2s",      (uint8)TBase_2s},
        {"5s",      (uint8)TBase_5s},
        {"10s",     (int8)TBase_10s},
        {"?",       255},
        {0}
    };
    ENTER_ANALYSIS
        if (TBaseSize > value) { FPGA_SetTBase((TBase)value); }
        else if (255 == value)
        {
            SCPI_SEND(":TBASE:RANGE %s", Tables_GetTBaseStringEN(TBASE));
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
    if (SCPI_FirstIsInt(buffer, &intVal, -1024, 15000))
    {
        int tShift = intVal + 1024;
        FPGA_SetTShift(tShift);
        return;
    }

    ENTER_ANALYSIS
        if (0 == value)
        {
            int retValue = TSHIFT - 1024;
            SCPI_SEND(":TBASE:OFFSET %d", retValue);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_SAMPLING(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"EQUAL", 0},
        {"REAL",  1},
        {"?",     2},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 2) { set.time.sampleType = (SampleType)value; }
        else if (2 == value)
        {
            SCPI_SEND(":TBASE:SAMPLING %s", map[set.time.sampleType].key);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
extern void OnPeacDetChanged(bool active);  // WARN ¬ообще-то это нехорошо, как нехорошо и дублировать. Ќадо бы подумать.


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_PEACKDET(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 2) { PEACKDET = (value == 0) ? PeackDet_Disable : PeackDet_Enable; OnPeacDetChanged(true); } // WARN SCPI дл€ пикового детектора переделать
        else if (2 == value)
        {
            SCPI_SEND(":TBASE:PEACKDET %s", PEACKDET ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
extern void OnTPosChanged(bool active);


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_TPOS(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"LEFT",   0},
        {"CENTER", 1},
        {"RIGHT",  2},
        {"?",      3},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 3)      { TPOS = (TPos)value; OnTPosChanged(true); }
        else if (4 == value)
        {
            SCPI_SEND(":TBASE:TPOS %s", map[TPOS].key);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_SELFRECORDER(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ON", 0},
        {"OFF", 1},
        {"?", 2},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 2) { set.service.recorder = (value == 0); }
        else if (2 == value)
        {
            SCPI_SEND(":TBASE:RECORDER %s", set.service.recorder == true ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_FUNCTIMEDIV(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"TIME",   0},
        {"MEMORY", 1},
        {"?",      2},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 2) { TIME_DIVXPOS = (FunctionTime)value; }
        else if (2 == value)
        {
            SCPI_SEND(":TBASE:FUNCTIMEDIV %s", map[TIME_DIVXPOS].key);
        }
    LEAVE_ANALYSIS
}
