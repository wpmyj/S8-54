#include "Settings.h"
#include "Hardware/FLASH.H"
#include "Display/Display.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGA_Types.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Display/Display.h"
#include "Menu/Menu.h"
#include "Menu/MenuFunctions.h"
#include "Log.h"


#include <string.h>


extern void OnChange_ADC_Stretch_Mode(bool active);
extern void OnChange_DisplayOrientation(bool);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
            MAKE_COLOR(1, 50, 31),      // COLOR_DATA_A
            MAKE_COLOR(0, 63, 0),       // COLOR_DATA_B
            MAKE_COLOR(26, 54, 26),     // COLOR_MENU_FIELD
            MAKE_COLOR(31, 44, 0),      // COLOR_MENU_TITLE
            MAKE_COLOR(15, 22, 0),      // COLOR_MENU_TITLE_DARK
            MAKE_COLOR(31, 63, 0),      // COLOR_MENU_TITLE_BRIGHT
            MAKE_COLOR(26, 34, 0),      // COLOR_MENU_ITEM
            MAKE_COLOR(13, 17, 0),      // COLOR_MENU_ITEM_DARK
            MAKE_COLOR(31, 51, 0),      // COLOR_MENU_ITEM_BRIGHT
            MAKE_COLOR(0, 25, 15),      // COLOR_DATA_A_WHITE
            MAKE_COLOR(0, 31, 0),       // COLOR_DATA_B_WHITE
            MAKE_COLOR(1, 40, 25),      
            MAKE_COLOR(0, 51, 0)
        },
        ModeDrawSignal_Lines,       // modeDrawSignal
        TypeGrid_1,                 // typeGrid
        NumAccumulation_1,          // numAccumulation
        NumAveraging_1,             // numAveraging
        Averaging_Accurately,       // modeAveraging
        NumMinMax_1,                // numMinMax
        NumSmooth_Disable,          // smoothing
        NumSignalsInSec_25,         // num signals in one second
        A,                          // lastAffectedChannel
        ModeAccumulation_NoReset,   // modeAccumulation
        AM_Show,                    // altMarkers
        MenuAutoHide_None,          // menuAutoHide
        ShowStrNavi_Temp,           // showStringNavigation
        LinkingRShift_Voltage,      // linkingRShift
        Background_Black            // background
    },
    // channels
    {
        {
            RShiftZero,             // rShiftRel
            ModeCouple_AC,          // ModCouple
            Multiplier_1,           // Divider
            Range_500mV,            // range
            true,                   // enable
            false,                  // inverse
			0,                      // balanceShiftADC
            Bandwidth_Full,         //
            Resistance_1Mom,        // resistance
            CalibrationMode_x1
        },
        {
            RShiftZero,             // rShiftRel
            ModeCouple_AC,          // ModCouple    
            Multiplier_1,           // Divider
            Range_500mV,            // range
            true,                   // enable
            false,                  // inverse
			5,                      // balanceShiftADC
            Bandwidth_Full,         //
            Resistance_1Mom,        // resistance
            CalibrationMode_x1
        }
        
    },
    // trig
    {
        StartMode_Auto,
        TrigSource_A,
        TrigPolarity_Front,
        TrigInput_AC,
        {TrigLevZero, TrigLevZero, TrigLevZero},
        TrigModeFind_Hand,
        45
    },
    // time
    {
        TBase_200us,
        0,                      // set.time.tShiftRel
        FunctionTime_Time,
        TPos_Center,
        SampleType_Equal,        // sampleType
        SampleType_Equal,
        PeackDet_Disable
    },
    // cursors
    {
        { CursCntrl_Disable, CursCntrl_Disable },   // CursCntrl U
        { CursCntrl_Disable, CursCntrl_Disable },   // CursCntrl T
        A,                                          // source
        { 60.0f,  140.0f, 60.0f, 140.0f },          // posCur U
        { 80.0f,  200.0f, 80.0f, 200.0f },          // posCur T
        { 80.0f,  80.0f },                          // расстояние между курсорами напряжения для 100%
        { 120.0f, 120.0f },                         // расстояние между курсорами времени для 100%
        CursMovement_Points,                        // CursMovement
        CursActive_None,                            // CursActive
        { CursLookMode_None, CursLookMode_None },   // Режим слежения курсоров.
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
        false,                  // flashAutoConnect
        ModeBtnMemory_Menu,
        ModeSaveSignal_BMP
    },
    // measures
    {
        MN_1,                       // measures number
        A_B,                        // source
        ModeViewSignals_AsIs,       // mode view signal
        {
            Measure_VoltageMax,     Measure_VoltageMin,     Measure_VoltagePic,         Measure_VoltageMaxSteady,   Measure_VoltageMinSteady,
            Measure_VoltageAverage, Measure_VoltageAmpl,    Measure_VoltageRMS,         Measure_VoltageVybrosPlus,  Measure_VoltageVybrosMinus,
            Measure_Period,         Measure_Freq,           Measure_TimeNarastaniya,    Measure_TimeSpada,          Measure_DurationPlus
        },
        false,                      // show
        MeasuresZone_Screen,        // брать для измерений значения, видимые на экране
        //{0, 200},                 // начальные значения курсоров напряжения для зоны измерений
        //{372, 652},               // начальные значения курсоров времени для зоны измерений
        {50, 150},                  // начальные значения курсоров напряжения для зоны измерений
        {422, 602},                 // начальные значения курсоров времени для зоны измерений
        CursCntrl_1,                // Управляем первым курсором напряжения
        CursCntrl_1,                // Управляем первым курсором времени
        CursActive_T,               // Активны курсоры времени.
        Measure_None                // marked Measure
    },
    // math
    {
        ScaleFFT_Log,
        SourceFFT_Channel0,
        WindowFFT_Rectangle,
        FFTmaxDB_60,
        Function_Sum,
        0,
        {100, 256 - 100},
        1,
        1,
        1,
        1,
        false,
        ModeDrawMath_Disable,       // modeDrawMath
        ModeRegSet_Range,           // modeRegSet
        Range_50mV,
        Multiplier_1,
        RShiftZero                  // rShift
    },
    // service
    {
        true,                       // screenWelcomeEnable
        true,                       // soundEnable
        15,                         // soundVolume
        Calibrator_Freq,            // calibrator
        0,                          // IP-адрес (временно)  WARN
        ColorScheme_WhiteLetters,   // colorScheme
        {false},                    // freqMeter
        false,                      // recorder
        FunctionPressRShift_ResetPosition,  // funcRShift
        1                           // speedRShift
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
    // menu
    {
        {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        true,
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LoadDefaultColors(void)
{
    for(int color = 0; color < NUM_COLORS; color++) 
    {
        COLOR(color) = defaultSettings.display.colors[color];
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Settings_Load(bool default_)
{
    SettingsNonReset setNonReset = set.nr;

    memcpy((void*)&set, (void*)(&defaultSettings), sizeof(set));
    if (default_)
    {
        set.nr = setNonReset;
    }
    else
    {
        FLASH_LoadSettings();
    }
    FPGA_LoadSettings();
    FPGA_SetNumSignalsInSec(sDisplay_NumSignalsInS());
    Panel_EnableLEDChannelA(sChannel_Enabled(A));
    Panel_EnableLEDChannelB(sChannel_Enabled(B));
    FPGA_SetNumberMeasuresForGates(set.debug.numMeasuresForGates);
    Menu_SetAutoHide(true);
    Display_ChangedRShiftMarkers(true);
    OnChange_ADC_Stretch_Mode(true);
    OnChange_DisplayOrientation(true);

    gBF.alreadyLoadSettings = 1;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Settings_Save(void)
{
    FLASH_SaveSettings();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool Settings_DebugModeEnable(void)
{
    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void SetMenuPosActItem(NamePage namePage, int8 pos)
{
    set.menu.posActItem[namePage] = pos;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int8 MenuCurrentSubPage(NamePage namePage)
{
    return set.menu.currentSubPage[namePage];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void SetMenuCurrentSubPage(NamePage namePage, int8 posSubPage)
{
    set.menu.currentSubPage[namePage] = posSubPage;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool MenuIsMinimize(void)
{
    return TypeOpenedItem() == Item_Page && GetNamePage(OpenedItem()) >= Page_SB_Curs;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool MenuPageDebugIsActive(void)
{
    return set.menu.pageDebugActive;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void SetMenuPageDebugActive(bool active)
{
    set.menu.pageDebugActive = active;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void CurrentPageSBregSet(int angle)
{
    Page *page = OpenedItem();
    if (page->funcRegSetSB)
    {
        page->funcRegSetSB(angle);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const SmallButton* GetSmallButton(PanelButton button)
{
    if(MenuIsMinimize() && button >= B_Menu && button <= B_F5)
    {
        Page *page = OpenedItem();
        SmallButton *sb = page->items[button - B_Menu];
        return sb;
    }
    return NULL;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Settings_SaveState(Settings *set_)
{
    gStateFPGA.stateWorkBeforeCalibration = fpgaStateWork;
    *set_ = set;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Settings_RestoreState(Settings *set_)
{
    int16 rShiftAdd[2][RangeSize][2];
    for (int ch = 0; ch < 2; ch++)
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < RangeSize; range++)
            {
                rShiftAdd[ch][range][mode] = set.nr.rShiftAdd[ch][range][mode];
            }
        }
    }
    set = *set_;
    for (int ch = 0; ch < 2; ch++)
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < RangeSize; range++)
            {
                set.nr.rShiftAdd[ch][range][mode] = rShiftAdd[ch][range][mode];
            }
        }
    }
    FPGA_LoadSettings();
    if (gStateFPGA.stateWorkBeforeCalibration != StateWorkFPGA_Stop)
    {
        gStateFPGA.stateWorkBeforeCalibration = StateWorkFPGA_Stop;
        FPGA_OnPressStartStop();
    }
}
