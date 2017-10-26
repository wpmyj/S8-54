

#include "../../defines.h"
#include "SCPI.h"
#include "../../Globals.h"
#include "../../VCP/VCP.h"
#include "../../Settings/Settings.h"
#include "../../Utils/GlobalFunctions.h"
#include "../../Utils/Map.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Process_AUTOSEND(uint8 *buffer);
static void Process_MAPPING(uint8 *buffer);
static void Process_ACCUM(uint8 *buffer);
static void Process_AVERAGE(uint8 *buffer);
static void Process_MINMAX(uint8 *buffer);
static void Process_BWLIMIT(uint8 *buffer);
static void Process_FPS(uint8 *buffer);
static void Process_GRID(uint8 *buffer);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        {"FILTR",       Process_BWLIMIT},     // Сглаживание
        {"FPS",         Process_FPS},       // Частота обновл
        {"GRID",        Process_GRID},      // СЕТКА
        {0}
    };

    SCPI_ProcessingCommand(commands, buffer);
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
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
        if (1 == value)         { MODE_DRAW_SIGNAL = ModeDrawSignal_Points; } 
        else if (2 == value)    { MODE_DRAW_SIGNAL = ModeDrawSignal_Lines; } 
        else if (3 == value)    { SCPI_SEND(":DISPLAY:MAPPING %s", MODE_DRAW_SIGNAL_LINES ? "LINES" : "POINTS"); }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_ACCUM(uint8 *buffer)
{
    extern void OnPress_Accum_Clear(void);
    
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
        if (value <= 7)         { ENUM_ACCUM = (ENumAccum)value; }
        else if (8 == value)    { MODE_ACCUM = ModeAccumulation_NoReset; }
        else if (9 == value)    { MODE_ACCUM = ModeAccumulation_Reset; }
        else if (10 == value)   { ENUM_ACCUM = ENumAccum_Infinity; } 
        else if (11 == value)   { OnPress_Accum_Clear(); }
        else if (12 == value)
        {
            SCPI_SEND(":DISPLAY:ACCUMULATION %s %s", map[ENUM_ACCUM].key, map[MODE_ACCUM + 8].key);
        }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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
        if (value <= 9) {ENUM_AVE = (ENumAverages)value; }
        else if (10 == value) { MODE_AVERAGING = Averaging_Accurately; }
        else if (11 == value) { MODE_AVERAGING = Averaging_Around; }
        else if (12 == value)
        {
            SCPI_SEND(":DISPLAY:AVERAGE %s %s", map[ENUM_AVE].key, map[MODE_AVERAGING + 10].key);
        }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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
        if (value <= 7) { ENUM_MIN_MAX = (ENumMinMax)value; }
        else if (8 == value)
        {
            SCPI_SEND(":DISPLAY:MINMAX %s", map[ENUM_MIN_MAX].key);
        }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_BWLIMIT(uint8 *buffer)
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
        if (value <= 9) { ENUM_SMOOTHING = (ENumSmoothing)value; }
        else if (10 == value)
        {
            SCPI_SEND(":DISPLAY:FILTR %s", map[ENUM_SMOOTHING].key);
        }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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
        if (value < 5) { ENUM_SIGNALS_IN_SEC = (ENumSignalsInSec)value; }
        else if (5 == value)
        {
            SCPI_SEND(":DISPLAY:FPS %s", map[ENUM_SIGNALS_IN_SEC].key);
        }
    LEAVE_ANALYSIS
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_GRID(uint8 *buffer)
{
    extern void OnChanged_Grid_Brightness(void);
    
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
        BRIGHTNESS_GRID = (int16)intVal;
        OnChanged_Grid_Brightness();
        return;
    }

    ENTER_ANALYSIS
        if (value < 4) { TYPE_GRID = (TypeGrid)value; }
        else if (4 == value)
        {
            extern ColorType colorTypeGrid;
            Color_Init(&colorTypeGrid, false);
            SCPI_SEND(":DISPLAY:GRID %s %d", map[TYPE_GRID].key, (int)(colorTypeGrid.brightness * 100.0f));
        }
    LEAVE_ANALYSIS
}
