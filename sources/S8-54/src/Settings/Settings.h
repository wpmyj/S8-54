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


// Включаем выравнивание, чтобы однозначно происходило определение новой версии структуры Settings

#pragma pack(push, 1)

typedef struct
{
    uint16              size;                       ///< \brief В первом байте всегда размер структуры, чтобы при загрузке можно было определить 
                                                    /// изменение структуры Settings.
    // SettingsDisplay
    int16               disp_TimeShowLevels;        ///< Время, в течение которого нужно показывать уровин смещения.
    int16               disp_ShiftInMemory;         ///< \brief Показывает смещение левого края стеки относительно нулевого байта памяти в байтах
                                                    ///< Т.е. для пикового детектора будет в два раза больше количества точек на экране.
    int16               disp_TimeMessages;          ///< Время в секундах, на которое сообщение остаётся на экрне.
    int16               disp_Brightness;            ///< Яркость дисплея.
    int16               disp_BrightnessGrid;        ///< Яркость сетки от 0 до 100.
    uint16              disp_Colors[16];
    ModeDrawSignal      disp_ModeDrawSignal;        ///< Режим отрисовки сигнала.
    TypeGrid            disp_TypeGrid;              ///< Тип сетки
    ENumAccum           disp_ENumAccum;             ///< Число накоплений сигнала на экране.
    ENumAverages        disp_ENumAverages;          ///< Число усреднений сигнала.
    ModeAveraging       disp_ModeAveraging;         ///< Тип усреднений по измерениям.
    ENumMinMax          disp_ENumMinMax;            ///< Перечисление количества измерений для определения минимумов и масимумов.
    ENumSmoothing       disp_ENumSmoothing;         ///< Перечисление количества точек для скользящего фильтра.
    ENumSignalsInSec    disp_ENumSignalsInSec;      ///< Перечисление числа считываний сигнала в секунда.
    Channel             disp_LastAffectedChannel;   ///< \brief Здесь хранится номер последнего канала, которым управляли ручками. Нужно для того, 
                                                    /// чтобы знать, какой сигнал рисовать наверху.
    ModeAccumulation    disp_ModeAccumulation;      ///< Режим накопления сигналов.
    AltMarkers          disp_AltMarkers;            ///< Режим отображения дополнительных боковых маркеров смещений.
    MenuAutoHide        disp_MenuAutoHide;          ///< Через сколько времени после последнего нажатия клавиши прятать меню.
    ShowStrNavi         disp_ShowStringNavigation;  ///< Режим показа строки навигации меню.
    LinkingRShift       disp_LinkingRShift;         ///< Тип привязки к смещению по вертикали.
    Background          disp_Background;            ///< Цвет фона.
    // Channels
    uint16          chan_RShiftRel[2];
    ModeCouple      chan_ModeCouple[2];             ///< Режим по входу.
    Divider         chan_Divider[2];                ///< Множитель.
    Range           chan_Range[2];                  ///< Масштаб по напряжению.
    bool            chan_Enable[2];
    bool            chan_Inverse[2];
    int8            chan_BalanceShiftADC[2];        ///< Добавочное смещение для балансировки АЦП.
    Bandwidth       chan_Bandwidth[2];              ///< Ограничение полосы.
    Resistance      chan_Resistance[2];             ///< Сопротивление входа.
    CalibrationMode chan_CalibrationMode[2];        ///< Режим калибровки.
    // Trig
    uint16          trig_LevelRel[3];               ///< Уровень синхронизации для трёх каналов.
    uint16          trig_TimeDelay;
    StartMode       trig_StartMode;                 ///< Режим запуска.
    TrigSource      trig_Source;                    ///< Источник.
    TrigPolarity    trig_Polarity;                  ///< Тип синхронизации.
    TrigInput       trig_Input;                     ///< Вход синхронизации.
    TrigModeFind    trig_ModeFind;                  ///< Поиск синхронизации - вручную или автоматически.
    // TBase
    TBase           time_TBase;                     ///< Масштаб по времени.
    int16           time_TShiftRel;                 ///< Смещение по времени.
    FunctionTime    time_TimeDivXPos;
    TPos            time_TPos;
    SampleType      time_SampleType;
    SampleType      time_SampleTypeOld;
    PeakDetMode    time_PeakDet;
    // Курсоры
    CursCntrl       curs_CntrlU[NumChannels];       ///< Активные курсоры напряжения.
    CursCntrl       curs_CntrlT[NumChannels];       ///< Активные курсоры времени.
    Channel         curs_Source;                    ///< Источник - к какому каналу относятся курсоры.
    float           curs_PosCurU[NumChannels][2];   ///< Текущие позиции курсоров напряжения обоих каналов.
    float           curs_PosCurT[NumChannels][2];   ///< Текущие позиции курсоров времени обоих каналов.
    float           curs_DeltaU100percents[2];      ///< Расстояние между курсорами напряжения для 100%, для обоих каналов.
    float           curs_DeltaT100percents[2];      ///< Расстояние между курсорами времени для 100%, для обоих каналов.
    CursMovement    curs_Movement;                  ///< Как перемещаться курсорам - по точкам или процентам.
    CursActive      curs_Active;                    ///< Какие курсоры сейас активны.
    CursLookMode    curs_LookMode[2];               ///< Режимы слежения за курсорами для двух пар курсоров.
    bool            curs_ShowFreq;                  ///< Установленное в true значение, что нужно показывать на экране 1/dT между курсорами.
    bool            curs_ShowCursors;               ///< Показывать ли курсоры.
    // Memory
#define MAX_SYMBOLS_IN_FILE_NAME 35
    ENumPointsFPGA  mem_ENumPointsFPGA;             ///< Число точек.
    ModeWork        mem_ModeWork;                   ///< Режим работы.
    FileNamingMode  mem_FileNamingMode;             ///< Режим именования файлов.
    char            mem_FileNameMask[MAX_SYMBOLS_IN_FILE_NAME]; ///< \brief Здесь маска для автоматического именования файлов.
                ///< \details Правила именования.\n
                /// \code
                /// %y('\x42') - год, %m('\x43') - месяц, %d('\x44') - день, %H('\x45') - часы, %M('\x46') - минуты, %S('\x47') - секунды
                /// %Nn('\x48''n') - порядковый номер, котрый занимает не менее n знакомест, например, 7 в %3N будет преобразовано в 007
                /// Примеры
                /// name_%4N_%y_%m_%d_%H_%M_%S будет генерировать файлы вида name_0043_2014_04_25_14_45_32
                /// При этом обратите внимание, что если спецификатор %4N стоИт после временнЫх параметров, то, скорее всего, этот параметр 
                /// будет всегда равен 0001, т.к. для определения номера просматриваются.
                /// \endcode
    char            mem_FileName[MAX_SYMBOLS_IN_FILE_NAME]; ///< Имя файла для режима ручного задания.
    int8            mem_IndexCurSymbolNameMask; ///< Индекс текущего символа в режиме задания маски или выбора имени.
    ModeShowIntMem  mem_ModeShowIntMem;         ///< Что показывать в режиме ВНУТР ЗУ - считанный или записанный сигнал.
    bool            mem_FlashAutoConnect;       ///< Если true, при подключении флешки автоматически выводится Файл-Менеджер.
    ModeBtnMemory   mem_ModeBtnMemory;
    ModeSaveSignal  mem_ModeSaveSignal;         ///< В каком виде сохранять сигнал.
    // ИЗМЕРЕНИЯ
    MeasuresNumber  meas_Number;                ///< Сколько измерений выводить.
    Channel         meas_Source;                ///< Для каких каналов выводить измерения.
    ModeViewSignals meas_ModeViewSignals;       ///< Сжимать ли сигналы при выводе измерений.
    Meas            meas_Measures[15];          ///< Выбранные для индикации измерения.
    bool            meas_Show;                  ///< Показывать ли измерения.
    MeasuresZone    meas_Zone;                  ///< Задаёт область, из которой берутся значения для расчёта измерений.
    int16           meas_PosCurU[2];            ///< \brief Позиции курсоров, которые задают область, из которой берутся значения для расчёта 
                                                ///< измерений при field == MeasuresField_Hand.
    int16           meas_PosCurT[2];            ///< \brief Позиции курсоров, которые задают область, из которой берутся значения для расчёта
                                                ///< измерений при field == MeasuresField_Hand.
    CursCntrl       meas_CntrlU;                ///< Активные курсоры напряжения.
    CursCntrl       meas_CntrlT;                ///< Активные курсоры времени.
    CursActive      meas_CursActive;            ///< Какие курсоры активны - по времени или напряжению.
    Meas            meas_Marked;                ///< Измерение, на которое нужно выводить маркеры.
    // SettingsMath
    ScaleFFT        math_ScaleFFT;
    SourceFFT       math_SourceFFT;
    WindowFFT       math_WindowFFT;
    FFTmaxDB        math_FFTmaxDB;
    Function        math_Function;
    uint8           math_CurrentCursor;         ///< Определяет, каким курсором спектра управляет ручка УСТАНОВКА.
    uint8           math_PosCur[2];             ///< Позиция курсора спектра. Изменяется 0...256.
    int8            math_koeff1add;             ///< Коэффициент при первом слагаемом для сложения.
    int8            math_koeff2add;
    int8            math_koeff1mul;
    int8            math_koeff2mul;
    bool            math_EnableFFT;
    ModeDrawMath    math_ModeDraw;              ///< Раздельный или общий дисплей в режиме математической функции.
    ModeRegSet      math_ModeRegSet;            ///< Функция ручки УСТАНОВКА - масштаб по времени или смещение по вертикали.
    Range           math_Range;
    Divider         math_Divider;
    uint16          math_RShift;
    // Частотомер
    bool            freq_Enable;
    TimeCounting    freq_TimeCounting;          ///< Время счёта частоты.
    FreqClc         freq_FreqClc;               ///< Частота заполнения.
    NumberPeriods   freq_NumberPeriods;         ///< Количество периодов.
    // СЕРВИС
    int16               serv_SoundVolume;       ///< Громкость звука [0...100].
    int16               serv_SpeedRShift;       ///< Относительная скорость смещения по вращению ручки RShift.
    bool                serv_SoundEnable;       ///< Включены ли звуки.
    bool                serv_Recorder;          ///< Включён ли режим регистратора.
    CalibratorMode      serv_CalibratorMode;    ///< Режим работы калибратора.
    ColorScheme         serv_ColorScheme;
    FunctionPressRShift serv_FuncRShift;        ///< Функция, выполняемая по нажатию на ручку RShift.
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
    int         com_CountEnables;                   ///< Количество включений. Инкрементируется при каждом включении.
    int         com_CountErasedFlashData;           ///< Сколько раз стирался первый сектор с ресурсами.
    int         com_CountErasedFlashSettings;       ///< Сколько раз стирался сектор с настройками.
    int         com_WorkingTimeInSecs;              ///< Время работы в секундах.
    Language    com_Lang;                           ///< Язык меню.
    // Debug
    bool                dbg_ShowRandInfo;           ///< Выводить информацию по рандомизатору - ворота и считанное значение.
    bool                dbg_ShowRandStat;           ///< Выводить график статистики.
    bool                dbg_ModeEMS;                ///< \brief При включении этого режима принудительно включается фильтр 20МГц, усреднение по 
                                                    /// 8-ми точкам, сглаживание по 4-м.
    bool                dbg_ShowStats;              ///< Показывать статистику на экране (fps, например).
    bool                dbg_ModePauseConsole;       ///< Если true, то вывод в консоль останавливается нажатием кнопки ПУСК/СТОП.
    int8                dbg_SizeFont;               ///< Размер шрифта консоли - 0 - 5, 1 - 8.
    DisplayOrientation  dbg_Orientation;
    int16               dbg_NumStrings;             ///< Число строк в консоли.
    int16               dbg_NumMeasuresForGates;    ///< Число измерений для ворот.
    int16               dbg_TimeCompensation;       ///< Дополнительное смещение по времени для данной развёртки режима рандомизатора.
    int16               dbg_AltShift;               ///< Добавочное смещение для устранения эффекта горизонтальной помехи синхронизации.
    int16               dbg_Pretriggered;           ///< Регулируемая величина предзапуска для исследования рандомизатора.
    Bandwidth           dbg_Bandwidth[2];           ///< \brief Здесь задаётся полоса, которая будет включаться при выборе в "КАНАЛ-Полоса" 
                                                    ///< значения "Полная".
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
    int8                menu_PosActItem[Page_NumPages];             ///< \brief Позиция активного пункта. bit7 == 1 - item is opened, 0x7f - нет 
                                                                    ///< активного пункта.
    int8                menu_CurrentSubPage[Page_NumPages];         ///< Номер текущей подстраницы.
    bool                menu_PageDebugActive;                       ///< Активна ли кнопка отладки в меню.
    int8                menu_IsShown;                               ///< \brief Меню показано. Если == false, и при этом какой-либо элемент меню раскрыт,
                                                                    ///< то он будет показан на экране.
    // Настройки, которые задаются единожды при наладке на заводе
    int16               nrst_RShiftAdd[NumChannels][RangeSize][2];  ///< Добавочное смещение, которое пишется сюда при калибровке и балансировке
    int16               nrst_CorrectionTime;                        ///< Коэффициент коррекции времени.
    int16               nrst_BalanceADC[NumChannels];               ///< Значение дополнительного смещения АЦП для ручной балансировки.
    int16               nrst_NumAveForRand;                         ///< По скольким измерениям усреднять сигнал в режиме рандомизатора.
    BalanceADCtype      nrst_BalanceADCtype;                        ///< Тип балансировки.
    StretchADCtype      nrst_StretchADCtype;                        ///< Тип растяжки канала.
    int16               nrst_StretchADC[NumChannels][3];            ///< \brief Поправочный коэффициент для ручного, калибровочного и
                        ///< отключенного режимов. Здесь хранится в целом виде, чтобы получить реальный коэффициент, нужно разделить на 1000 и
                        ///< прибавить единицу.
    int16               nrst_AddStretch20mV[NumChannels];
    int16               nrst_AddStretch50mV[NumChannels];
    int16               nrst_AddStretch100mV[NumChannels];
    int16               nrst_AddStretch2V[NumChannels];
    int16               nrst_NumSmoothForRand;                      ///< Число точек для скользящего фильта в рандомизаторе.
    MemDataScale        mem_DataScale;                              ///< \brief Определяет поведение сохранённого сигнала при переключении ручек
                                                                    ///< управления сигналом.
    PlaceOfSaving       rec_PlaceOfSaving;                          ///< Куда будут сохраняться данные регистратора
    int8                rec_NumCursor;                              ///< Номер активного курсора
    int16               nrst_RShiftAddStable[NumChannels][3];       ///< Добавочное смещение для трёх самых чувствительных диапазонов. Задаётся единожды при настройке
    LinkingTShift       time_LinkingTShift;                         ///< Тип привязки смещения по горизонтали
} Settings;

#pragma pack(pop)

extern Settings set;

/// Установить позицию активного пункта на странице namePage.
void SetMenuPosActItem(NamePage namePage, int8 pos);
/// Возвращает номер текущей подстраницы страницы namePage.
int8 MenuCurrentSubPage(NamePage namePage);
/// Устанавливает номер текущей подстраницы в странице namePage.
void SetMenuCurrentSubPage(NamePage namePage, int8 posSubPage);
/// Если true - меню находится в дополнительном режиме.
bool MenuIsMinimize(void);
/// Активна ли страница отладки.
bool MenuPageDebugIsActive(void);
/// Сделать/разделать активной страницу отладки.
void SetMenuPageDebugActive(bool active);
/// Повернуть ручку УСТАНОВКА на текущей странице малых кнопок.
void CurrentPageSBregSet(int angle);
/// Вернуть указатель на малую кнопку, соответствующую данной кнопки панели.
const SButton*  GetSmallButton(PanelButton button);

/// Возвращает позицию активного пункта на странице namePage.
#define MENU_POS_ACT_ITEM(namePage) (set.menu_PosActItem[namePage])

/// \brief Загрузить настройки. Если default == true, загружаются настройки по умолчанию, иначе пытается загрузить настройки из ПЗУ, а в случае 
/// неудачи - тоже настройки по умолчанию.
void Settings_Load(bool _default);
/// Сохранить настройки во флеш-память.
void Settings_Save(void);
/// Возвращает true, если включён режим отладки.
bool Settings_DebugModeEnable(void);
/// Сохраняет текущие настройки в set_.
void Settings_SaveState(Settings *set_);
/// Восстанавливает ранее записанные настройки в set_.
void Settings_RestoreState(const Settings *set_);

/** @}
 */
