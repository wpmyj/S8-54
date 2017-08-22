#include "Settings.h"
#include "Hardware/FLASH.H"
#include "Display/Display.h"
#include "Display/Colors.h"
#include "Panel/Panel.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Display/Display.h"

#include <string.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Settings defaultSettings =
{
    {
        // NonReset
        {
            // channel
            { {0}, {0} },           // rShiftAdd
            // service
            0,                      // correctionTime
            // debug
            { 0, 0 },               // balanceADC
            1,                      // numAveForRand
            BalanceADC_Settings,    // balanceADCtype
            StretchADC_Real,        // stretchADCtype
            {{0, 0, 0}, {0, 0, 0}}, // stretchADC
            {0, 0},                 // addStretch20mV
            {0, 0},                 // addStretch50mV
            {0, 0},                 // addStretch100mV
            {0, 0},                 // addStretch2V
            1                       // numSmoothForRand
        }
    },
    // Display
    {
        5,                          // timeShowLevels
        512 - 140,                  // shiftInMemory
        5,                          // timeMessages
        100,                        // brightness
        20,                         // brightnessGrid
        {
            MAKE_COLOR(0, 0, 0),        // COLOR_BLACK
            MAKE_COLOR(31, 63, 31),     // COLOR_WHITE
            MAKE_COLOR(15, 31, 15),     // COLOR_GRID
            MAKE_COLOR(1, 50, 31),      // COLOR_DATA_1
            MAKE_COLOR(0, 63, 0),       // COLOR_DATA_2
            MAKE_COLOR(26, 54, 26),     // COLOR_MENU_FIELD
            MAKE_COLOR(31, 44, 0),      // COLOR_MENU_TITLE
            MAKE_COLOR(15, 22, 0),      // COLOR_MENU_TITLE_DARK
            MAKE_COLOR(31, 63, 0),      // COLOR_MENU_TITLE_BRIGHT
            MAKE_COLOR(26, 34, 0),      // COLOR_MENU_ITEM
            MAKE_COLOR(13, 17, 0),      // COLOR_MENU_ITEM_DARK
            MAKE_COLOR(31, 51, 0),      // COLOR_MENU_ITEM_BRIGHT
            MAKE_COLOR(0, 25, 15),      // COLOR_DATA_A_WHITE_ACCUM
            MAKE_COLOR(0, 31, 0),       // COLOR_DATA_B_WHITE_ACCUM
            MAKE_COLOR(1, 40, 25),      
            MAKE_COLOR(0, 51, 0)
        },
        ModeDrawSignal_Lines,       // modeDrawSignal
        TypeGrid_1,                 // typeGrid
        ENumAccum_1,          // numAccumulation
        ENumAverages_1,             // numAveraging
        Averaging_Accurately,       // modeAveraging
        NumMinMax_1,                // numMinMax
        ENumSmooth_Disable,          // smoothing
        NumSignalsInSec_25,         // num signals in one second
        A,                          // lastAffectedChannel
        ModeAccumulation_NoReset,   // modeAccumulation
        AM_Show,                    // altMarkers
        MenuAutoHide_None,          // menuAutoHide
        true,                       // showFullMemoryWindow
        ShowStrNavi_Temp,           // showStringNavigation
        LinkingRShift_Voltage,      // linkingRShift
        Background_Black            // background
    },
    // time
    {
        TBase_200us,
        0,                      // set.time.tShiftRel
        FunctionTime_Time,
        TPos_Center,
        SampleType_Equal,        // sampleType
        SampleType_Equal,
        PeakDet_Disable
    },
    // cursors
    {
        { CursCntrl_Disable, CursCntrl_Disable },   // CursCntrl U
        { CursCntrl_Disable, CursCntrl_Disable },   // CursCntrl T
        A,                                          // source
        { 60.0f,  140.0f, 60.0f, 140.0f },          // posCur U
        { 80.0f,  200.0f, 80.0f, 200.0f },          // posCur T
        { 80.0f,  80.0f },                          // рассто€ние между курсорами напр€жени€ дл€ 100%
        { 120.0f, 120.0f },                         // рассто€ние между курсорами времени дл€ 100%
        CursMovement_Points,                        // CursMovement
        CursActive_None,                            // CursActive
        { CursLookMode_None, CursLookMode_None },   // –ежим слежени€ курсоров.
        false,                                      // showFreq
        false                                       // showCursors
    },
    // memory
    {
        FNP_1k,
        ModeWork_Direct,
        FileNamingMode_Mask,
        "Signal_\x07\x03",      // \x07 - означает, что здесь должен быть очередной номер, \x03 - минимум на три знакоместа
        "Signal01",
        0,
        {
            false
        },
        ModeShowIntMem_Direct,  // modeShowIntMem
        true,                   // showBothSignalInIntMem
        false,                  // flashAutoConnect
        ModeBtnMemory_Menu,
        ModeSaveSignal_BMP
    },
    // service
    {
        true,                       // screenWelcomeEnable
        false,                      // soundEnable
        100,                        // soundVolume
        Calibrator_Freq,            // calibrator
        /// \todo — IP-адресом нужно что-то делать
        0,                          // IP-адрес (временно)
        ColorScheme_WhiteLetters,   // colorScheme
        {false},                    // freqMeter
        false                       // recorder
    },
    {
        0x8b, 0x2e, 0xaf, 0x8f, 0x13, 0x00, // mac
        192, 168, 1, 200,                   // ip
        7,
        255, 255, 255, 0,                   // netmask
        192, 168, 1, 1                      // gateway
    },
    // common
    {
        0
    },
    // debug
    {
        0,          // numStrings
        0,          // размер шрифта - 5
        false,      // consoleInPause
        1000,       // numMeasuresForGates
        0,          // timeCompensation
        false,      // view altera writting data
        false,      // view all altera writting data
        0,          // alt shift
        false,      // showRandInfo
        false,      // showRandStat
        false,      // modeEMS
        false,      // showStats
        200,        // pretriggered
        {Bandwidth_Full, Bandwidth_Full}    // bandwidth[2]
    }
};

Settings set;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoadDefaultColors(void)
{
    for(int color = 0; color < NUM_COLORS; color++) 
    {
        set.display.colors[color] = defaultSettings.display.colors[color];
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Settings_Load(void)
{
    set = defaultSettings;
}
