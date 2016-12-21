#pragma once

#include "defines.h"
#include "SettingsTypes.h"
#include "Panel/Controls.h"

#ifndef _MS_VS
#pragma anon_unions     // Разрешаем безымянные определения
#endif


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
    bool                showFullMemoryWindow;       // Показывать ли окно памяти вверху экрана
    ShowStrNavi         showStringNavigation;       // Режим показа строки навигации меню
    LinkingRShift       linkingRShift;              // Тип привязки к смещению по вертикали
    Background          background;                 // Цвет фона
} SettingsDisplay;


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
    FPGA_NUM_POINTS fpgaNumPoints;              // Число точек.
    ModeWork        modeWork;                   // Режим работы.
    FileNamingMode  fileNamingMode;             // Режим именования файлов.
    char            fileNameMask[MAX_SYMBOLS_IN_FILE_NAME]; // Здесь маска для автоматического именования файлов\n
         // Правила именования.\n
         // %y('\x42') - год, %m('\x43') - месяц, %d('\x44') - день, %H('\x45') - часы, %M('\x46') - минуты, %S('\x47') - секунды\n
         // %Nn('\x48''n') - порядковый номер, котрый занимает не менее n знакомест, например, 7 в %3N будет преобразовано в 007\n
         // Примеры\n
         // name_%4N_%y_%m_%d_%H_%M_%S будет генерировать файлы вида name_0043_2014_04_25_14_45_32\n
         // При этом обратите внимание, что если спецификатор %4N стоИт после временнЫх параметров, то, скорее всего, этот параметр будет всегда равен 0001, т.к. для определения номера просматриваются только символы ДО него.
    char            fileName[MAX_SYMBOLS_IN_FILE_NAME];     // Имя файла для режима ручного задания
    int8                indexCurSymbolNameMask; // Индекс текущего символа в режиме задания маски или выбора имени.
    StructMemoryLast    strMemoryLast;
    ModeShowIntMem      modeShowIntMem;
    bool                showBothSignalInIntMem; // Показывать ли оба сигнала в режиме внутреннего ЗУ
    bool                flashAutoConnect;       // Если true, при подлючении флеш автоматически выводится NC (Нортон Коммандер)
    ModeBtnMemory       modeBtnMemory;     
    ModeSaveSignal      modeSaveSignal;         // В каком виде сохранять сигнал.
} SettingsMemory;


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
    bool                screenWelcomeEnable;    // Будет ли показываться экран приглашения при включении прибора.
    bool                soundEnable;            // Включены ли звуки.
    int16               soundVolume;            // Громкость звука [0...100]
    CalibratorMode      calibrator;             // Режим работы калибратора.
    int8                IPaddress;              // IP-адрес (временно)
    ColorScheme         colorScheme;            //
    SettingsFreqMeter   freqMeter;              // Настройки частотомера
    bool                recorder;               // Включен ли режим регистратора.
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

    int16   numStrings;                 // Число строк в консоли.
    int8    sizeFont;                   // Размер шрифта консоли - 0 - 5, 1 - 8,
    bool    consoleInPause;             // Признак того, что консоль находится в режиме паузы. Режим паузы означает, что новые сообщения она не записывает и не сохраняет.
    int16   numMeasuresForGates;        // Число измерений для ворот.
    int16   timeCompensation;           // Дополнительное смещение по времени для данной развёртки режима рандомизатора.
    bool    viewAlteraWrittingData;     // Показывать ли данные, идущие в альтеру.
    bool    viewAllAlteraWrittingData;  // Показывать ли все данные, идущие в альтеру (если false, то постоянно идущие команды вроде START, STOP не показываются).
    int16   altShift;                   // Добавочное смещение для устранения эффекта горизонтальной помехи синхронизации.
    bool    showRandInfo;               // Выводить информацию по рандомизатору - ворота и считаннное значение
    bool    showRandStat;               // Вводить график статистики
    bool    modeEMS;                    // При включении этого режима принудительно включается фильтр 20МГц, усреднение по 8-ми точкам, сглаживание по 4-м
    bool    showStats;                  // Показывать статистику на экране (fps, например).
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
    DisplayOrientation orientation;
} SettingsDebug;


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


// Струкура хранит все настройки прибора.
typedef struct
{
    union
    {
        SettingsNonReset nr;                    // здесь хранятся настройки, которые не сбрасываются при СЕРВИС - Сброс настроек
        uint8 buf[SIZE_NONRESET_SETTINGS];
    };
    SettingsDisplay     display;                // настройки изображения          (меню ДИСПЛЕЙ)
    SettingsTime        time;                   // временнЫе настройки            (меню РАЗВЁРТКА)
    SettingsCursors     cursors;                // настройки курсорных измерений  (меню КУРСОРЫ)
    SettingsMemory      memory;                 // настройки режимов памяти       (меню ПАМЯТЬ)
    SettingsService     service;                // дополнительные настройки       (меню СЕРВИС)
    SettingsEthernet    eth;
    SettingsCommon      common;                 // системные настройки
    SettingsDebug       debug;
    int temp[14];
} Settings;


extern Settings set;


void Settings_Load(void);          // Загрузить настройки. Если default == true, загружаются настройки по умолчанию, иначе пытается загрузить настройки из ПЗУ, а в случае неудачи - тоже настройки по умолчанию.
