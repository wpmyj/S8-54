#include "../../defines.h"


#include "SCPI.h"
#include "../../Globals.h"
#include "../../VCP/VCP.h"
#include "../../Settings/Settings.h"
#include "../../Utils/GlobalFunctions.h"
#include "../../Utils/Map.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Process_AUTOSEND(uint8 *buffer);
static void Process_MAPPING(uint8 *buffer);
static void Process_ACCUM(uint8 *buffer);
static void Process_AVERAGE(uint8 *buffer);
static void Process_MINMAX(uint8 *buffer);
static void Process_FILTR(uint8 *buffer);
static void Process_FPS(uint8 *buffer);
static void Process_GRID(uint8 *buffer);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Process_DISPLAY(uint8 *buffer)
{
    static const StructCommand commands[] =
    {
        {"AUTOSEND",    Process_AUTOSEND},
        {"MAPPING",     Process_MAPPING},   // Отображение
        {"ACCUMULATION",Process_ACCUM},     // Накопление
        {"ACCUM",       Process_ACCUM},     
        {"AVERAGE",     Process_AVERAGE},   // Усреднение
        {"AVE",         Process_AVERAGE},
        {"MINMAX",      Process_MINMAX},    // Мин Макс
        {"FILTR",       Process_FILTR},     // Сглаживание
        {"FPS",         Process_FPS},       // Частота обновл
        {"GRID",        Process_GRID},      // СЕТКА
        {0}
    };

    SCPI_ProcessingCommand(commands, buffer);
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Process_AUTOSEND(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1", 1},
        {"2", 2},
        {0}
    };
    ENTER_ANALYSIS
        if (1 == value)         { Painter_SendFrame(true); }
        else if (2 == value)    { Painter_SendFrame(false); } 
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_MAPPING(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"POINTS",  1},
        {"LINES",   2},
        {"?",       3},
        {0}
    };
    ENTER_ANALYSIS
        if (1 == value)         { set.display.modeDrawSignal = ModeDrawSignal_Points; } 
        else if (2 == value)    { set.display.modeDrawSignal = ModeDrawSignal_Lines; } 
        else if (3 == value)    { SCPI_SEND(":DISPLAY:MAPPING %s", ModeDrawSignal_Lines == set.display.modeDrawSignal ? "LINES" : "POINTS"); }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
extern void OnPress_Accum_Clear(void);


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_ACCUM(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1",   0},
        {"2",   1},
        {"4",   2},
        {"8",   3},
        {"16",  4},
        {"32",  5},
        {"64",  6},
        {"128", 7},
        {"NORESET",     8},
        {"RESET",       9},
        {"INFINITY",    10},
        {"CLEAR",       11},
        {"?",           12},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= 7)         { set.display.numAccumulation = (NumAccumulation)value; }
        else if (8 == value)    { set.display.modeAccumulation = ModeAccumulation_NoReset; }
        else if (9 == value)    { set.display.modeAccumulation = ModeAccumulation_Reset; }
        else if (10 == value)   { set.display.numAccumulation = NumAccumulation_Infinity; } 
        else if (11 == value)   { OnPress_Accum_Clear(); }
        else if (12 == value)
        {
            SCPI_SEND(":DISPLAY:ACCUMULATION %s %s", map[set.display.numAccumulation].key, map[set.display.modeAccumulation + 8].key);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_AVERAGE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1", 0},
        {"2", 1},
        {"4", 2},
        {"8", 3},
        {"16", 4},
        {"32", 5},
        {"64", 6},
        {"128", 7},
        {"256", 8},
        {"512", 9},
        {"ACCURACY", 10},
        {"APPROXIMATE", 11},
        {"?", 12},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= 9) {set.display.numAveraging = (NumAveraging)value; }
        else if (10 == value) { set.display.modeAveraging = Averaging_Accurately; }
        else if (11 == value) { set.display.modeAveraging = Averaging_Around; }
        else if (12 == value)
        {
            SCPI_SEND(":DISPLAY:AVERAGE %s %s", map[set.display.numAveraging].key, map[set.display.modeAveraging + 10].key);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_MINMAX(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1", 0},
        {"2", 1},
        {"4", 2},
        {"8", 3},
        {"16", 4},
        {"32", 5},
        {"64", 6},
        {"128", 7},
        {"?", 8},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= 7) { set.display.numMinMax = (NumMinMax)value; }
        else if (8 == value)
        {
            SCPI_SEND(":DISPLAY:MINMAX %s", map[set.display.numMinMax].key);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_FILTR(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1", 0},
        {"2", 1},
        {"3", 2},
        {"4", 3},
        {"5", 4},
        {"6", 5},
        {"7", 6},
        {"8", 7},
        {"9", 8},
        {"10", 9},
        {"?", 10},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= 9) { set.display.smoothing = (NumSmoothing)value; }
        else if (10 == value)
        {
            SCPI_SEND(":DISPLAY:FILTR %s", map[set.display.smoothing].key);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_FPS(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"25",  0},
        {"10",  1},
        {"5",   2},
        {"2",   3},
        {"1",   4},
        {"?",   5},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 5) { set.display.numSignalsInSec = (NumSignalsInSec)value; }
        else if (5 == value)
        {
            SCPI_SEND(":DISPLAY:FPS %s", map[set.display.numSignalsInSec].key);
        }
    LEAVE_ANALYSIS
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
extern void OnChange_Grid_Brightness(void);


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_GRID(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"TYPE1", 0},
        {"TYPE2", 1},
        {"TYPE3", 2},
        {"TYPE4", 3},
        {"?", 4},
        {0}
    };

    int intVal = 0;
    if (SCPI_FirstIsInt(buffer, &intVal, 0, 100))
    {
        set.display.brightnessGrid = (int16)intVal;
        OnChange_Grid_Brightness();
        return;
    }

    ENTER_ANALYSIS
        if (value < 4) { set.display.typeGrid = (TypeGrid)value; }
        else if (4 == value)
        {
            extern ColorType colorTypeGrid;
            Color_Init(&colorTypeGrid, false);
            SCPI_SEND(":DISPLAY:GRID %s %d", map[set.display.typeGrid].key, (int)(colorTypeGrid.brightness * 100.0f));
        }
    LEAVE_ANALYSIS
}
