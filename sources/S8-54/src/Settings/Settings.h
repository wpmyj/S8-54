#pragma once
#include "SettingsTypes.h"
#include "Utils/Measures.h"
#include "Menu/MenuItems.h"
#include "Panel/Controls.h"
#include "SettingsChannel.h"
#include "SettingsDisplay.h"
#include "SettingsMemory.h"
#include "SettingsTime.h"
#include "SettingsTrig.h"
#include "SettingsService.h"
#include "SettingsCursors.h"
#include "SettingsDebug.h"
#include "SettingsMeasures.h"
#include "SettingsMath.h"
#include "SettingsNRST.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup Settings
 *  @{
 */


#define ETH_ENABLED         (set.eth_enable)
#define ETH_PORT            (set.eth_port)


#define SIZE_NONRESET_SETTINGS 200


#pragma anon_unions

typedef union
{
    struct
    {
        // 
        int16               disp_TimeShowLevels;
        int16               disp_ShiftInMemory;
        int16               disp_TimeMessages;
        int16               disp_Brightness;
        int16               disp_BrightnessGrid;
        uint16              disp_Colors[16];
        ModeDrawSignal      disp_ModeDrawSignal;
        TypeGrid            disp_TypeGrid;
        NumAccumulation     disp_NumAccumulation;
        NumAveraging        disp_NumAveraging;
        ModeAveraging       disp_ModeAveraging;
        ENumMinMax          disp_ENumMinMax;
        NumSmoothing        disp_NumSmoothing;
        NumSignalsInSec     disp_NumSignalsInSec;
        Channel             disp_LastAffectedChannel;
        ModeAccumulation    disp_ModeAccumulation;
        AltMarkers          disp_AltMarkers;
        MenuAutoHide        disp_MenuAutoHide;
        ShowStrNavi         disp_ShowStringNavigation;
        LinkingRShift       disp_LinkingRShift;
        Background          disp_Background;
        // Channels
        uint16          chan_RShiftRel[2];
        ModeCouple      chan_ModeCouple[2];
        Divider         chan_Divider[2];
        Range           chan_Range[2];
        bool            chan_Enable[2];
        bool            chan_Inverse[2];
        int8            chan_BalanceShiftADC[2];
        Bandwidth       chan_Bandwidth[2];
        Resistance      chan_Resistance[2];
        CalibrationMode chan_CalibrationMode[2];
        // Trig
        uint16          trig_LevelRel[3];
        uint16          trig_TimeDelay;
        StartMode       trig_StartMode;
        TrigSource      trig_Source;
        TrigPolarity    trig_Polarity;
        TrigInput       trig_Input;
        TrigModeFind    trig_ModeFind;
        // TBase
        TBase           time_TBase;
        int16           time_TShiftRel;
        FunctionTime    time_TimeDivXPos;
        TPos            time_TPos;
        SampleType      time_SampleType;
        SampleType      time_SampleTypeOld;
        PeackDetMode    time_PeackDet;
        // Курсоры
        CursCntrl       curs_CntrlU[NumChannels];
        CursCntrl       curs_CntrlT[NumChannels];
        Channel         curs_Source;
        float           curs_PosCurU[NumChannels][2];
        float           curs_PosCurT[NumChannels][2];
        float           curs_DeltaU100percents[2];
        float           curs_DeltaT100percents[2];
        CursMovement    curs_Movement;
        CursActive      curs_Active;
        CursLookMode    curs_LookMode[2];
        bool            curs_ShowFreq;
        bool            curs_ShowCursors;
        // Memory
#define MAX_SYMBOLS_IN_FILE_NAME 35
        NumPoinstFPGA   mem_FpgaNumPoints;
        ModeWork        mem_ModeWork;
        FileNamingMode  mem_FileNamingMode;
        char            mem_FileNameMask[MAX_SYMBOLS_IN_FILE_NAME];
        char            mem_FileName[MAX_SYMBOLS_IN_FILE_NAME];
        int8            mem_IndexCurSymbolNameMask;
        ModeShowIntMem  mem_ModeShowIntMem;
        bool            mem_FlashAutoConnect;
        ModeBtnMemory   mem_ModeBtnMemory;
        ModeSaveSignal  mem_ModeSaveSignal;
        // ИЗМЕРЕНИЯ
        MeasuresNumber  meas_Number;
        Channel         meas_Source;
        ModeViewSignals meas_ModeViewSignals;
        Measure         meas_Measures[15];
        bool            meas_Show;
        MeasuresZone    meas_Zone;
        int16           meas_PosCurU[2];
        int16           meas_PosCurT[2];
        CursCntrl       meas_CntrlU;
        CursCntrl       meas_CntrlT;
        CursActive      meas_CursActive;
        Measure         meas_Marked;
        // SettingsMath
        ScaleFFT        math_ScaleFFT;
        SourceFFT       math_SourceFFT;
        WindowFFT       math_WindowFFT;
        FFTmaxDB        math_FFTmaxDB;
        Function        math_Function;
        uint8           math_CurrentCursor;
        uint8           math_PosCur[2];
        int8            math_koeff1add;
        int8            math_koeff2add;
        int8            math_koeff1mul;
        int8            math_koeff2mul;
        bool            math_EnableFFT;
        ModeDrawMath    math_ModeDraw;
        ModeRegSet      math_ModeRegSet;
        Range           math_Range;
        Divider         math_Divider;
        uint16          math_RShift;
        // Частотомер
        bool            freq_Enable;
        TimeCounting    freq_TimeCounting;
        FreqClc         freq_FreqClc;
        NumberPeriods   freq_NumberPeriods;
        // СЕРВИС
        int16               serv_SoundVolume;
        int16               serv_SpeedRShift;
        bool                serv_SoundEnable;
        bool                serv_Recorder;
        int8                serv_IPaddress;
        CalibratorMode      serv_CalibratorMode;
        ColorScheme         serv_ColorScheme;
        FunctionPressRShift serv_FuncRShift;
        // Ethernet
        uint8       eth_mac0;
        uint8       eth_mac1;
        uint8       eth_mac2;
        uint8       eth_mac3;
        uint8       eth_mac4;
        uint8       eth_mac5;
        uint8       eth_ip0;
        uint8       eth_ip1;
        uint8       eth_ip2;
        uint8       eth_ip3;
        uint16      eth_port;
        uint8       eth_mask0;
        uint8       eth_mask1;
        uint8       eth_mask2;
        uint8       eth_mask3;
        uint8       eth_gw0;
        uint8       eth_gw1;
        uint8       eth_gw2;
        uint8       eth_gw3;
        bool        eth_enable;
        // Common
        int         com_CountEnables;
        int         com_CountErasedFlashData;
        int         com_CountErasedFlashSettings;
        int         com_WorkingTimeInSecs;
        Language    com_Lang;
        // Debug
        bool        dbg_ShowRandInfo;
        bool        dbg_ShowRandStat;
        bool        dbg_ModeEMS;
        bool        dbg_ShowStats;
        bool        dbg_ModePauseConsole;
        int8        dbg_SizeFont;
        DisplayOrientation  dbg_Orientation;
        int16               dbg_NumStrings;
        int16               dbg_NumMeasuresForGates;
        int16               dbg_TimeCompensation;
        int16               dbg_AltShift;
        int16               dbg_Pretriggered;
        Bandwidth           dbg_Bandwidth[2];
        bool                dbg_ShowAll;
        bool                dbg_ShowFlag;
        bool                dbg_ShowRShift[2];
        bool                dbg_ShowTrigLev;
        bool                dbg_ShowRange[2];
        bool                dbg_ShowChanParam[2];
        bool                dbg_ShowTrigParam;
        bool                dbg_ShowTShift;
        bool                dbg_ShowTBase;
        // SettingsMenu
        int8                menu_PosActItem[Page_NumPages];
        int8                menu_CurrentSubPage[Page_NumPages];
        bool                menu_PageDebugActive;
        int8                menu_IsShown;
        // Настройки, которые задаются единожды при наладке на заводе
        int16               nrst_RShiftAdd[NumChannels][RangeSize][2];
        int16               nrst_CorrectionTime;
        int16               nrst_BalanceADC[NumChannels];
        int16               nrst_NumAveForRand;
        BalanceADCtype      nrst_BalanceADCtype;
        StretchADCtype      nrst_StretchADCtype;
        int16               nrst_StretchADC[NumChannels][3];
        int16               nrst_AddStretch20mV[NumChannels];
        int16               nrst_AddStretch50mV[NumChannels];
        int16               nrst_AddStretch100mV[NumChannels];
        int16               nrst_AddStretch2V[NumChannels];
        int16               nrst_NumSmoothForRand;
    };
    int _size_[256];
} Settings;

extern Settings set;

void SetMenuPosActItem(NamePage namePage, int8 pos);            // Установить позицию активного пункта на странице namePage.
int8 MenuCurrentSubPage(NamePage namePage);                     // Возвращает номер текущей подстраницы страницы namePage.
void SetMenuCurrentSubPage(NamePage namePage, int8 posSubPage); // Устанавливает номер текущей подстраницы в странице namePage.
bool MenuIsMinimize(void);                                      // Если true - меню находится в дополнительном режиме.
bool MenuPageDebugIsActive(void);                               // Активна ли страница отладки.
void SetMenuPageDebugActive(bool active);                       // Сделать/разделать активной страницу отладки.
void CurrentPageSBregSet(int angle);                            // Повернуть ручку УСТАНОВКА на текущей странице малых кнопок.
const SmallButton*  GetSmallButton(PanelButton button);         // Вернуть указатель на малую кнопку, соответствующую данной кнопки панели.

// Возвращает позицию активного пункта на странице namePage.
#define MENU_POS_ACT_ITEM(namePage) (set.menu_PosActItem[namePage])


void Settings_Load(bool _default);                  // Загрузить настройки. Если default == true, загружаются настройки по умолчанию, иначе пытается загрузить настройки из ПЗУ, а в случае неудачи - тоже настройки по умолчанию.
void Settings_Save(void);                           // Сохранить настройки во флеш-память.
bool Settings_DebugModeEnable(void);                // Возвращает true, если включён режим отладки.
void Settings_SaveState(Settings *set_);            // Сохраняет текущие настройки в set_.
void Settings_RestoreState(const Settings *set_);   // Восстанавливает ранее записанные настройки в set_.

/** @}
 */
