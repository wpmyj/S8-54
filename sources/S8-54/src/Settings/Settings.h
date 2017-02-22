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


#define SIZE_NONRESET_SETTINGS 200


// Настройки изображения
typedef struct
{
    int16               timeShowLevels;             // Время, в течение которого нужно показывать уровни смещения
    int16               shiftInMemory;              // Показывает смещение левого края сетки относительно нулевого байта памяти. Нужно для правильного отображения сигнала в окне. Для пикового детектора показывает количество байт, а не точек.
    int16               timeMessages;               // Время в секундах, на которое сообщение остаётся на экране
    int16               brightness;                 // Яркость дисплея (только для цветного)
    int16               brightnessGrid;             // Яркость сетки от 0 до 100
    uint16              colors[16];
    ModeDrawSignal      modeDrawSignal;             // Режим отрисовки сигнала
    TypeGrid            typeGrid;                   // Тип сетки
    NumAccumulation     numAccumulation;            // Число накоплений сигнала на экране
    NumAveraging        numAveraging;               // Число усреднений сигнала
    ModeAveraging       modeAveraging;              // Тип усреднений по измерениям
    NumMinMax           numMinMax;                  // Число измерений для определения минимумов и максимумов
    NumSmoothing        smoothing;                  // Число точек для скользящего фильтра
    NumSignalsInSec     numSignalsInSec;            // Число считываний сигнала в секунду
    Channel             lastAffectedChannel;        // Здесь хранится номер последнего канала, которым управляли ручками. Нужно для того, чтобы знать, какой сигнал рисовать наверху.
    ModeAccumulation    modeAccumulation;           // Задаёт режим накопления сигналов
    AltMarkers          altMarkers;                 // Режим отображения дополнительных боковых маркеров смещений
    MenuAutoHide        menuAutoHide;               // Через сколько времени после последнего нажатия клавиши прятать меню 
    ShowStrNavi         showStringNavigation;       // Режим показа строки навигации меню
    LinkingRShift       linkingRShift;              // Тип привязки к смещению по вертикали
    Background          background;                 // Цвет фона
} SettingsDisplay;


// Настройки каналов
typedef struct
{
    uint16          rShiftRel;
    ModeCouple      modeCouple;         	    // Режим по входу.
    Divider         divider;                    // Множитель.
    Range           range;              	    // Масштаб по напряжению.
    bool            enable;
    bool            inverse;
    int8            balanceShiftADC;    	    // Добавочное смещение для балансировки АЦП.
    Bandwidth       bandwidth;                  // Ограничение полосы
    Resistance      resistance;                 // Сопротивление входа
    CalibrationMode calibrationMode;            // Режим калибровки
    // некоторые настройки хранятся в SettingsNonReset
} SettingsChannel;


// Настройки синхронизации
typedef struct
{
    StartMode           startMode;          // Режим запуска.
    TrigSource          source;             // Источник.
    TrigPolarity        polarity;           // Тип синхронизации.
    TrigInput           input;              // Вход синхронизации.
    uint16              levelRel[3];        // Уровень синхронизации для трёх источников.
    TrigModeFind        modeFind;           // Поиск синхронизации - вручную или автоматически.
    uint16              timeDelay;
} SettingsTrig;


// ВременнЫе настройки
typedef struct
{
    TBase           tBase;                  // Масштаб по времени.
    int16           tShiftRel;              // Смещение по времени
    FunctionTime    timeDivXPos;
    TPos            tPos;
    SampleType      sampleType;
    SampleType      sampleTypeOld;
    PeackDetMode    peackDet;
} SettingsTime;


// Настройки курсоров
typedef struct
{
    CursCntrl           cntrlU[NumChannels];                    // Активные курсоры напряжения.
    CursCntrl           cntrlT[NumChannels];                    // Активные курсоры напряжения.
    Channel             source;                                 // Источник - к какому каналу относятся курсоры.
    float               posCurU[NumChannels][2];                // Текущие позиции курсоров напряжения обоих каналов.
    float               posCurT[NumChannels][2];                // Текущие позиции курсоров времени обоих каналов.
    float               deltaU100percents[2];                   // Расстояние между курсорами напряжения для 100%, для обоих каналов.
    float               deltaT100percents[2];                   // Расстояние между курсорами времени для 100%, для обоих каналов.
    CursMovement        movement;                               // Как перемещаться курсорам - по точкам или по процентам.
    CursActive          active;                                 // Какие курсоры сейчас активны.
    CursLookMode        lookMode[2];                            // Режимы слежения за курсорами для двух пар курсоров.
    bool                showFreq;                               // Установленное в true значение, что нужно показывать на экране значение 1/dT между курсорами.
    bool                showCursors;                            // Показывать ли курсоры
} SettingsCursors;


typedef  struct
{
    bool isActiveModeSelect;
} StructMemoryLast;


// Настройки МЕНЮ->ПАМЯТЬ
typedef struct
{
#define MAX_SYMBOLS_IN_FILE_NAME 35
    NumPoinstFPGA   fpgaNumPoints;              // Число точек.
    ModeWork        modeWork;                   // Режим работы.
    FileNamingMode  fileNamingMode;             // Режим именования файлов.
    char            fileNameMask[MAX_SYMBOLS_IN_FILE_NAME]; // Здесь маска для автоматического именования файлов\n
         // Правила именования.\n
         // %y('\x42') - год, %m('\x43') - месяц, %d('\x44') - день, %H('\x45') - часы, %M('\x46') - минуты, %S('\x47') - секунды\n
         // %Nn('\x48''n') - порядковый номер, котрый занимает не менее n знакомест, например, 7 в %3N будет преобразовано в 007\n
         // Примеры\n
         // name_%4N_%y_%m_%d_%H_%M_%S будет генерировать файлы вида name_0043_2014_04_25_14_45_32\n
         // При этом обратите внимание, что если спецификатор %4N стоИт после временнЫх параметров, то, скорее всего, этот параметр будет всегда равен 0001, т.к. для определения номера просматриваются только символы ДО него.
    char                fileName[MAX_SYMBOLS_IN_FILE_NAME]; // Имя файла для режима ручного задания
    int8                indexCurSymbolNameMask;             // Индекс текущего символа в режиме задания маски или выбора имени.
    StructMemoryLast    strMemoryLast;
    ModeShowIntMem      modeShowIntMem;
    bool                flashAutoConnect;                   // Если true, при подлючении флеш автоматически выводится NC (Нортон Коммандер)
    ModeBtnMemory       modeBtnMemory;
    ModeSaveSignal      modeSaveSignal;                     // В каком виде сохранять сигнал.
} SettingsMemory;


typedef struct
{
    MeasuresNumber  number;             // Сколько измерений выводить.
    Channel         source;             // Для каких каналов выводить измерения.
    ModeViewSignals modeViewSignals;    // Сжимать ли сигналы при выводе измерений.
    Measure         measures[15];       // Выбранные для индикации измерения.
    bool            show;               // Показывать ли измерения.
    MeasuresZone    zone;               // Задаёт область, из которой берутся значения для расчёта измерений.
    int16           posCurU[2];         // Позиции курсоров, которые задают область, из которой берутся значения для расчёта измерений при field == MeasuresField_Hand.
    int16           posCurT[2];         // Позиции курсоров, которые задают область, из которой берутся значения для расчёта измерений при field == MeasuresField_Hand.
    CursCntrl       cntrlU;             // Активные курсоры напряжения.
    CursCntrl       cntrlT;             // Активные курсоры времени.
    CursActive      cursActive;         // Какие курсоры активны - по времени или напряжению.
    Measure         marked;             // Измерение, на которое нужно выводить маркеры.
} SettingsMeasures;


typedef struct
{
    ScaleFFT        scaleFFT;  
    SourceFFT       sourceFFT;
    WindowFFT       windowFFT;
    FFTmaxDB        fftMaxDB;
    Function        mathFunc;
    uint8           currentCursor;      // Определяет, каким курсором спектра управляет ручка УСТАНОВКА
    uint8           posCur[2];          // Позиция курсора спектра. Изменятеся 0...256.
    int8            koeff1add;          // Коэффициент при первом слагаемом для сложения.
    int8            koeff2add;
    int8            koeff1mul;
    int8            koeff2mul;
    bool            enableFFT;
    ModeDrawMath    modeDraw;           // Раздельный или общий дисплей в режиме математической функции
    ModeRegSet      modeRegSet;         // Функция ручки УСТАНОВКА - масштаб по времени или смещение по вертикали
    Range           range;
    Divider         divider;
    uint16          rShift;
} SettingsMath;


// Настройки частотомера
typedef struct
{
    bool            enable;
    TimeCounting    timeCounting;       // Время счёта частоты
    FreqClc         freqClc;            // Частота заполения
    NumberPeriods   numberPeriods;      // Количество периодов
} SettingsFreqMeter;


// Эти настройки меняются через МЕНЮ -> СЕРВИС
typedef struct
{
    bool                soundEnable;            // Включены ли звуки.
    int16               soundVolume;            // Громкость звука [0...100]
    CalibratorMode      calibrator;             // Режим работы калибратора.
    int8                IPaddress;              // IP-адрес (временно)
    ColorScheme         colorScheme;            //
    SettingsFreqMeter   freqMeter;              // Настройки частотомера
    bool                recorder;               // Включен ли режим регистратора.
    FunctionPressRShift funcRShift;             // Функция, выполняемая по нажатию на ручку RShift
    int16               speedRShift;            // Относительная скорость смещения по вращению ручки RShift
} SettingsService;


typedef struct
{
    uint8 mac0;
    uint8 mac1;
    uint8 mac2;
    uint8 mac3;
    uint8 mac4;
    uint8 mac5;

    uint8 ip0;
    uint8 ip1;
    uint8 ip2;
    uint8 ip3;

    uint16 port;

    uint8 mask0;
    uint8 mask1;
    uint8 mask2;
    uint8 mask3;

    uint8 gw0;
    uint8 gw1;
    uint8 gw2;
    uint8 gw3;

    bool enable;
} SettingsEthernet; 


typedef struct
{
    int     countEnables;               // Количество включений. Увеличивается при каждом включении
    int     countErasedFlashData;       // Сколько раз стирался первый сектор с ресурсами
    int     countErasedFlashSettings;   // Сколько раз стирался сектор с настройкаи
    int     workingTimeInSecs;          // Время работы в секундах
    Language lang;                      // Язык меню
} SettingsCommon;


// Настройки отладчика
typedef  struct
{
    // некоторые настройки хранятся в SettingsNonReset

    bool    showRandInfo;               // Выводить информацию по рандомизатору - ворота и считаннное значение
    bool    showRandStat;               // Вводить график статистики
    bool    modeEMS;                    // При включении этого режима принудительно включается фильтр 20МГц, усреднение по 8-ми точкам, сглаживание по 4-м
    bool    showStats;                  // Показывать статистику на экране (fps, например).
    bool    viewAlteraWrittingData;     // Показывать ли данные, идущие в альтеру.
    bool    viewAllAlteraWrittingData;  // Показывать ли все данные, идущие в альтеру (если false, то постоянно идущие команды вроде START, STOP не показываются).
    bool    modePauseConsole;           // Если true, то вывод в консоль останавливается нажатием кнопки ПУСК/СТОП
    int8    sizeFont;                   // Размер шрифта консоли - 0 - 5, 1 - 8,
    DisplayOrientation orientation;
    int16   numStrings;                 // Число строк в консоли.
    int16   numMeasuresForGates;        // Число измерений для ворот.
    int16   timeCompensation;           // Дополнительное смещение по времени для данной развёртки режима рандомизатора.
    int16   altShift;                   // Добавочное смещение для устранения эффекта горизонтальной помехи синхронизации.
    int16   pretriggered;               // Регулируемая величина предзапуска для исследования рандомизатора
    Bandwidth bandwidth[2];             // Здесь задаётся полоса, которая будет включатся при выборе в КАНАЛ-Полоса значения "Полная"
    struct Show
    {
        bool all;
        bool flag;
        bool rShift[NumChannels];
        bool trigLev;
        bool range[NumChannels];
        bool chanParam[NumChannels];
        bool trigParam;
        bool tShift;
        bool tBase;
    } show;
    
} SettingsDebug;


typedef struct
{
    int8            posActItem[Page_NumPages];      // Позиция ативного пункта. bit7 == 1 - item is opened, 0x7f - нет активного пункта
    int8            currentSubPage[Page_NumPages];  // Номер текущей подстраницы.
    bool            pageDebugActive;                // Активна ли кнопка отладки в меню.
    int8            isShown;                        // Меню показано. Если == false, и при этом какой-либо элемент меню расрыт, то он будет показан на экране
} SettingsMenu;


typedef struct
{
    /*
    ******************************************************************************************************************************
    !!!!!!!!!!!!!!!!!!!!!!! ВНИМАНИЕ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Не меняйте порядок следования и не удаляйте ничего.
    Только в этом случае гарантируется сохранность настроек при изменении размера структуры настроек и перезаписи программы в ППЗУ
    ******************************************************************************************************************************
    */

    // channel
    int16           rShiftAdd[NumChannels][RangeSize][2];    // Добавочное смещение для открытого (0) и закрытого (1) входов.

    // service
    int16           correctionTime;             // Коэффициент коррекции времени

    // debug
    int16           balanceADC[NumChannels];    // Значение дополнительного смещения АЦП для ручной балансировки.
    int16           numAveForRand;              // По скольким измерениям усреднять сигнал в режиме рандомизатора.
    BalanceADCtype  balanceADCtype;             // Тип балансировки.
    StretchADCtype  stretchADCtype;             // Тип растяжки канала.

    // channels
    int16           stretchADC[NumChannels][3]; // Поправочный коэффициент для ручного, калибровочного и отлючённого режимов
                                                // Здесь хранится в целовм виде, чтобы получить реалный коэффициент, нужно разделить на 1000 и прибавить единицу.

    int16           addStretch20mV[NumChannels];
    int16           addStretch50mV[NumChannels];
    int16           addStretch100mV[NumChannels];
    int16           addStretch2V[NumChannels];

    // debug
    int16           numSmoothForRand;           // Число точек для скользящего фильтра в рандомизаторе
} SettingsNonReset; // Здесь будут храниться несбрасываемые настройки


void    SetMenuPosActItem(NamePage namePage, int8 pos);             // Установить позицию активного пункта на странице namePage.
int8    MenuCurrentSubPage(NamePage namePage);                      // Возвращает номер текущей подстраницы страницы namePage.
void    SetMenuCurrentSubPage(NamePage namePage, int8 posSubPage);  // Устанавливает номер текущей подстраницы в странице namePage.
bool    MenuIsMinimize(void);                                       // Если true - меню находится в дополнительном режиме.
bool    MenuPageDebugIsActive(void);                                // Активна ли страница отладки.
void    SetMenuPageDebugActive(bool active);                        // Сделать/разделать активной страницу отладки.
void    CurrentPageSBregSet(int angle);                             // Повернуть ручку УСТАНОВКА на текущей странице малых кнопок.
bool    PressSmallButton(PanelButton button);                       // Нажать малую кнопку, соответствующую данной кнопке панели.
const SmallButton*  GetSmallButton(PanelButton button);             // Вернуть указатель на малую кнопку, соответствующую данной кнопки панели.

// Возвращает позицию активного пункта на странице namePage.
#define MENU_POS_ACT_ITEM(namePage) (set.menu.posActItem[namePage])

// Струкура хранит все настройки прибора.
typedef struct
{
    SettingsDisplay     display;                // настройки изображения          (меню ДИСПЛЕЙ)
    SettingsChannel     chan[NumChannels];      // настройки каналов              (меню КАНАЛ 1 и КАНАЛ 2)
    SettingsTrig        trig;                   // настройки синхронизации        (меню СИНХР)
    SettingsTime        time;                   // временнЫе настройки            (меню РАЗВЁРТКА)
    SettingsCursors     cursors;                // настройки курсорных измерений  (меню КУРСОРЫ)
    SettingsMemory      memory;                 // настройки режимов памяти       (меню ПАМЯТЬ)
    SettingsMeasures    measures;               // настройки измерений            (меню ИЗМЕРЕНИЯ)
    SettingsMath        math;                   // настройки режима математических измерений
    SettingsService     service;                // дополнительные настройки       (меню СЕРВИС)
    SettingsEthernet    eth;
    SettingsCommon      common;                 // системные настройки
    SettingsMenu        menu;                   // состояние меню
    SettingsDebug       debug;
    //int temp[4];
} Settings;


extern Settings         set;
extern SettingsNonReset setNR;

void Settings_Load(bool _default);          // Загрузить настройки. Если default == true, загружаются настройки по умолчанию, иначе пытается загрузить настройки из ПЗУ, а в случае неудачи - тоже настройки по умолчанию.
void Settings_Save(void);                   // Сохранить настройки во флеш-память.
bool Settings_DebugModeEnable(void);        // Возвращает true, если включён режим отладки.
void Settings_SaveState(Settings *set_);    // Сохраняет текущие настройки в set_.
void Settings_RestoreState(Settings *set_); // Восстанавливает ранее записанные настройки в set_.
