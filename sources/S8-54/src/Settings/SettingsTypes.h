#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsTypes Settings Types
 *  @{
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SDisplay
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    Direct,
    Back
} DisplayOrientation;

/// Тип привязки к смещению по вертикали
typedef enum
{
    LinkingRShift_Voltage,      ///< Смещение привязано к напряжению
    LinkingRShift_Position      ///< Смещение привязано к позиции
} LinkingRShift;

typedef enum
{
    ColorScheme_WhiteLetters,   ///< В этом случае заголовки элементов меню пишутся белым - не очень хорошо видно снизу
    ColorScheme_BlackLetters    ///< В этом случае заголовки элементов меню пишутся чёрным - не очень красиво выглядит
} ColorScheme;

/// Режим отрисовки сигнала.
typedef enum
{
    ModeDrawSignal_Lines = 0,   ///< линиями
    ModeDrawSignal_Points = 1   ///< точками
} ModeDrawSignal;

/// Тип сетки на экране.
typedef enum
{
    TypeGrid_1,
    TypeGrid_2,
    TypeGrid_3,
    TypeGrid_4,
    TG_Size
} TypeGrid;

/// Количество накоплений.
typedef enum
{
    NumAccumulation_1,
    NumAccumulation_2,
    NumAccumulation_4,
    NumAccumulation_8,
    NumAccumulation_16,
    NumAccumulation_32,
    NumAccumulation_64,
    NumAccumulation_128,
    NumAccumulation_Infinity
} NumAccumulation;

/// Количество усреднений по измерениям.
typedef enum
{
    ENumAverages_1,
    ENumAverages_2,
    ENumAverages_4,
    ENumAverages_8,
    ENumAverages_16,
    ENumAverages_32,
    ENumAverages_64,
    ENumAverages_128,
    ENumAverages_256,
    ENumAverages_512
} ENumAverages;

/// Тип усреднений по измерениям.
typedef enum
{
    Averaging_Accurately,   ///< Усреднять точно.
    Averaging_Around        ///< Усреднять приблизительно.
} ModeAveraging;

/// Количество измерений для расчёта минимального и максимального значений.
typedef enum
{
    NumMinMax_1,
    NumMinMax_2,
    NumMinMax_4,
    NumMinMax_8,
    NumMinMax_16,
    NumMinMax_32,
    NumMinMax_64,
    NumMinMax_128
} ENumMinMax;

/// Количество точек для расчёта сглаживания.
typedef enum
{
    NumSmooth_Disable,
    NumSmooth_2points = 1,
    NumSmooth_3points = 2,
    NumSmooth_4points = 3,
    NumSmooth_5points = 4,
    NumSmooth_6points = 5,
    NumSmooth_7points = 6,
    NumSmooth_8points = 7,
    NumSmooth_9points = 8,
    NumSmooth_10points = 9
} ENumSmoothing;

/// Ограничение FPS.
typedef enum
{
    ENumSignalsInSec_25,
    ENumSignalsInSec_10,
    ENumSignalsInSec_5,
    ENumSignalsInSec_2,
    ENumSignalsInSec_1
} ENumSignalsInSec;

/// Режим отображения пропущенных сигналов при ограничении частоты кадров.
typedef enum
{
    Missed_Hide,    ///< Не выводить на экран.
    Missed_Show,    ///< Выводить на экран.
    Missed_Average  ///< Устреднять и выводить на экран.
} MissedSignals;

typedef enum
{
    ModeAccumulation_NoReset,   /// В этом режиме показываются строго N последних.
    ModeAccumulation_Reset      /// В этом режиме набираются N последних и потом сбрасываются.
} ModeAccumulation;

/// Режим отображения дополнительных боковых маркеров смещения по напряжению.
typedef enum
{
    AM_Hide,        /// Никода не выводить.
    AM_Show,        /// Всегда выводить.
    AM_AutoHide     /// Выводить и прятать через timeShowLevels.
} AltMarkers;

/// Через какое время после последнего нажатия кнопки скрывать меню.
typedef enum
{
    MenuAutoHide_None    = 0,   ///< Никогда.
    MenuAutoHide_5       = 5,   ///< Через 5 секунд.
    MenuAutoHide_10      = 10,  ///< Через 10 секунд.
    MenuAutoHide_15      = 15,  ///< Через 15 секунд.
    MenuAutoHide_30      = 30,  ///< Через 30 секунд.
    MenuAutoHide_60      = 60   ///< Через 60 секунд.
} MenuAutoHide;

/// Режим показа строки навигации.
typedef enum
{
    ShowStrNavi_Temp,   ///< Показывать на несколько секунд.
    ShowStrNavi_All,    ///< Всегда показывать.
    ShowStrNavi_None    ///< Никогда не показывать.
} ShowStrNavi;

/// Выбор цвета фона.
typedef enum
{
    Background_Black,
    Background_White
} Background;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ChannelX
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Режим калибровки.
typedef enum
{
    CalibrationMode_x1,
    CalibrationMode_x10,
    CalibrationMode_Disable
}CalibrationMode;

typedef enum
{
    Bandwidth_Full,     ///< Если это значение выбрано в меню КАНАЛ, то при этом положение устанавливается полоса из ОТЛАДКА-КАНАЛЫ-Полоса.
    Bandwidth_20MHz,
    Bandwidth_100MHz,
    Bandwidth_200MHz,
    Bandwidth_350MHz,
    Bandwidth_650MHz,
    Bandwidth_750MHz
} Bandwidth;

typedef enum
{
    Resistance_1Mom,
    Resistance_50Om
} Resistance;

typedef enum
{
    A,
    B,
    A_B,
    Math
} Channel;

#define NumChannels 2

/// Режим канала по входу.
typedef enum
{
    ModeCouple_DC,      ///< Открытый вход.
    ModeCouple_AC,      ///< Закрытый вход.
    ModeCouple_GND      ///< Вход заземлён.
} ModeCouple;

/// Делитель.
typedef enum
{
    Divider_1,
    Divider_10
} Divider;

/// Масштаб по напряжению.
typedef enum
{
    Range_2mV,
    Range_5mV,
    Range_10mV,
    Range_20mV,
    Range_50mV,
    Range_100mV,
    Range_200mV,
    Range_500mV,
    Range_1V,
    Range_2V,
    Range_5V,
    RangeSize
} Range;

/// Режим запуска.
typedef enum
{
    StartMode_Auto,     ///< Автоматический.
    StartMode_Wait,     ///< Ждущий.
    StartMode_Single    ///< Однократный.
} StartMode;

/// Источник синхронизации.
typedef enum
{
    TrigSource_A,       ///< Канал 1.
    TrigSource_B,       ///< Канал 2.
    TrigSource_Ext      ///< Внешняя.
} TrigSource;

/// Тип синхронизацц.
typedef enum
{
    TrigPolarity_Front,     ///< По фронту.
    TrigPolarity_Back       ///< По срезу.
} TrigPolarity;

/// Вход синхронизации.
typedef enum
{
    TrigInput_Full,         ///< Полный сиганл.
    TrigInput_AC,           ///< Переменный.
    TrigInput_LPF,          ///< ФНЧ.
    TrigInput_HPF           ///< ФВЧ.
} TrigInput;

typedef enum
{
    TrigModeFind_Hand,      ///< Уровень синхронизации устанавливается вручную или автоматически - однократным нажажтием кнопки.
    TrigModeFind_Auto       ///< Подстройки уровня синхронизации производится автоматически после каждого нового считанного сигнала.
} TrigModeFind;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Time
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Функция ВР/ДЕЛ.
typedef enum
{
    FunctionTime_Time,
    FunctionTime_ShiftInMemory
} FunctionTime;

/// Масштаб по времени.
typedef enum 
{
    TBase_1ns,
    TBase_2ns,
    TBase_5ns,
    TBase_10ns,
    TBase_20ns,
    TBase_50ns,
    TBase_100ns,
    TBase_200ns,
    TBase_500ns,
    TBase_1us, 
    TBase_2us,
    TBase_5us,
    TBase_10us,
    TBase_20us,
    TBase_50us,
    TBase_100us,
    TBase_200us,
    TBase_500us,
    TBase_1ms,
    TBase_2ms,
    TBase_5ms,
    TBase_10ms,
    TBase_20ms,
    TBase_50ms,
    TBase_100ms,
    TBase_200ms,
    TBase_500ms,
    TBase_1s,
    TBase_2s,
    TBase_5s,
    TBase_10s,
    TBaseSize
} TBase;

/// Положение точки синхронизация на сигнале.
typedef enum
{
    TPos_Left,          ///< Привязка к левому краю.
    TPos_Center,        ///< Привязка к центру.
    TPos_Right          ///< Привязка к правому краю.
} TPos;

/// Тип выборки для режима рандомизатора.
typedef enum
{
    SampleType_Real,    ///< реальное время - в построении участвуют только реально считанные точки, ничего не рассчитывается.
    SampleType_Equal    ///< эквивалентная - сигнал строится по последним точкам, полученным от рандомизатора.
} SampleType;

typedef enum
{
    PeackDet_Disable,
    PeackDet_Enable,
    PeackDet_Average
} PeackDetMode;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cursors
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Каким курсором управлять.
typedef enum
{
    CursCntrl_1,        ///< первым.
    CursCntrl_2,        ///< вторым.
    CursCntrl_1_2,      ///< обоими.
    CursCntrl_Disable   ///< никаким.
} CursCntrl;

/// Дискретность перемещения курсоров.
typedef enum
{
    CursMovement_Points,    ///< По точкам.
    CursMovement_Percents   ///< По процентам.
} CursMovement;

/// Какие курсоры сейчас активны. Какие активны, те и будут перемещаться по вращению ручки УСТАНОВКА.
typedef enum
{
    CursActive_U,
    CursActive_T,
    CursActive_None
} CursActive;

/// Режим слежения курсоров.
typedef enum
{
    CursLookMode_None,      ///< Курсоры не следят.
    CursLookMode_Voltage,   ///< Курсоры следят за напряжением автоматически.
    CursLookMode_Time,      ///< Курсоры следят за временем автоматически.
    CursLookMode_Both       ///< Курсоры следят за временем и напряжением, в зависимости от того, какой курсоры вращали последним.
} CursLookMode;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Определяет вид сигнала при переключении ручек управления сигналом.
typedef enum
{
    MemDataScale_Recalculated,  ///< Сигнал пересчитывается к текущим настройкам.
    MemDataScale_Original       ///< Сигнал на экране остаётся неизменным, в тех же точках, в которых считан.
} MemDataScale;

/// Число точек сигнала, с которым идёт работа.
typedef enum
{
    FNP_512,
    FNP_1k,
    FNP_2k,
    FNP_4k,
    FNP_8k,
    FNP_16k,
    FNP_32k,                /// \todo В этом режиме только один канал
    FPGA_ENUM_POINTS_SIZE
} ENumPointsFPGA;

/// Режим работы.
typedef enum
{
    ModeWork_Dir,           ///< Основной режим.
    ModeWork_RAM,           ///< В этом режиме можно просмотреть последние сохранённые измерения.
    ModeWork_ROM,           ///< В этом режиме можно сохранять во flash-памяти измерения просматривать ранее сохранённые.
    ModeWork_None           ///< Используется в модуле Data.c. Нужен, чтобы указать, что мудуль не настроен ни на какой режим.
} ModeWork;

/// Что показывать в режиме Внутр ЗУ - считанный или записанный сигнал.
typedef enum
{
    ModeShowIntMem_Direct,  ///< Показывать данные реального времени.
    ModeShowIntMem_Saved,   ///< Показывать сохранённые данные.
    ModeShowIntMem_Both     ///< Показывать оба сигнала.
} ModeShowIntMem;

/// Что делать при нажатии кнопки ПАМЯТЬ.
typedef enum
{
    ModeBtnMemory_Menu,     ///< Будет открывааться соответствующая страница меню.
    ModeBtnMemory_Save      ///< Сохранение содержимого экрана на флешку.
} ModeBtnMemory;

/// Режим наименования файлов.
typedef enum
{
    FileNamingMode_Mask,        ///< Именовать по маске.
    FileNamingMode_Manually     ///< Именовать вручную.
} FileNamingMode;

/// Как сохранять данные на флешку.
typedef enum
{
    ModeSaveSignal_BMP,
    ModeSaveSignal_TXT
} ModeSaveSignal;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Measures
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Сжимать ли сигналы при выводе измерений.
typedef enum
{
    ModeViewSignals_AsIs,       ///< Показывать сигналы как есть.
    ModeViewSignals_Compress    ///< Сжимать сетку с сигналами.
} ModeViewSignals;

typedef enum
{
    MN_1,                       ///< 1 измерение слева внизу.
    MN_2,                       ///< 2 измерения слева внизу.
    MN_1_5,                     ///< 1 строка с 5 измерениями.
    MN_2_5,                     ///< 2 строки по 5 измерений.
    MN_3_5,                     ///< 3 строки по 5 измерений.
    MN_6_1,                     ///< 6 строк по 1 измерению.
    MN_6_2                      ///< 6 строк по 2 измерения.
} MeasuresNumber;

/// Зона, по которой считаются измрения.
typedef enum
{
    MeasuresZone_Screen,        ///< Измерения будут производиться по той части сингала, которая видна на экране.
    MeasuresZone_AllMemory,     ///< Измерения будут производиться по всему сигналу.
    MeasuresZone_Hand           ///< Измерения будут производиться по окну, задаваемому пользователем.
} MeasuresZone;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
    Function_Sum,
    Function_Mul
} Function;

typedef enum
{
    ScaleMath_Channel0,     ///< Масштаб берётся из канала 1.
    ScaleMath_Channel1,     ///< Масштаб берётся из канала 2.
    ScaleMath_Hand          ///< Масштаб задаётся вручную.
} ScaleMath;

typedef enum
{
    ScaleFFT_Log,           ///< Это значение означает логарифмическую шкалу вывода спектра.
    ScaleFFT_Linear         ///< Это значение означает линейную шкалу вывода спектра.
} ScaleFFT;

typedef enum
{
    SourceFFT_ChannelA,
    SourceFFT_ChannelB,
    SourceFFT_Both
} SourceFFT;

typedef enum
{
    WindowFFT_Rectangle,
    WindowFFT_Hamming,
    WindowFFT_Blackman,
    WindowFFT_Hann
} WindowFFT;

typedef enum
{
    FFTmaxDB_40,
    FFTmaxDB_60,
    FFTmaxDB_80
} FFTmaxDB;

typedef enum
{
    ModeDrawMath_Disable,
    ModeDrawMath_Separate,
    ModeDrawMath_Together
} ModeDrawMath;

typedef enum
{
    ModeRegSet_Range,
    ModeRegSet_RShift
} ModeRegSet;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Service
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Язык меню.
typedef enum
{
    Russian,            ///< Русский.
    English             ///< Английский.
} Language;

typedef enum
{
    Calibrator_Freq,
    Calibrator_DC,
    Calibrator_GND
} CalibratorMode;

typedef enum
{
    FunctionPressRShift_SwitchingSpeed, ///< \brief Нажатие на ручку переключает скорость - в режиме просмотра памяти - навигации по памяти, в 
                                        ///< рабочем режиме - скорость вращения ручки смещения.
    FunctionPressRShift_ResetPosition   ///< В этом режиме нажатие ручки сбрасывает смещение в ноль.
} FunctionPressRShift;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Debug
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// \brief Тип балансировки АЦП каналов.
/// Дело в том, что уровни АЦП не совпадают из-за отличия характеристик ( ? ), поэтому мы вводим дополнительное смещение для одного из АЦП канала.
typedef enum
{
    BalanceADC_Disable,                 ///< Балансировка выключена.
    BalanceADC_Settings,                ///< Используются значения балансировки, которые получены автоматически.
    BalanceADC_Hand                     ///< Используются значения балансировки, заданные вручную.
} BalanceADCtype;

/// Тип растяжки АЦП.
typedef enum
{
    StretchADC_Disable,
    StretchADC_Settings,
    StretchADC_Hand
} StretchADCtype;

/// Тип дополнительного смещения.
typedef enum
{
    RShiftADC_Disable,
    RShiftADC_Settings,
    RShiftADC_Hand
} RShiftADCtype;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Частотомер
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Время счёта периода.
typedef enum
{
    TimeCounting_100ms,
    TimeCounting_1s,
    TimeCounting_10s
} TimeCounting;

/// Частота заполняющих импульсов для счёта частоты.
typedef enum
{
    FreqClc_100kHz,
    FreqClc_1MHz,
    FreqClc_10MHz,
    FreqClc_100MHz
} FreqClc;

/// Количество периодов.
typedef enum
{
    NumberPeriods_1,
    NumberPeriods_10,
    NumberPeriods_100
} NumberPeriods;


/** @}   @}
 */
