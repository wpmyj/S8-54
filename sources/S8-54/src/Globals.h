#pragma once


#include "FPGA/FPGAtypes.h"
#include "Panel/Controls.h"
#include "Settings/SettingsTypes.h"

#include <stm32f4xx_hal.h>

#include <stm32f4xx_hal_hcd.h>
#include <stm32f4xx_hal_pcd.h>
#include <stm32f4xx_hal_dma.h>
#include <stm32f4xx_hal_spi.h>
#include <stm32f4xx_hal_adc.h>
#include <stm32f4xx_hal_dac.h>


#include <usbd_def.h>
#include <usbh_def.h>


extern USBH_HandleTypeDef handleUSBH;
extern HCD_HandleTypeDef handleHCD;
extern SPI_HandleTypeDef handleSPI;
extern DMA_HandleTypeDef handleDMA_RAM;
extern ADC_HandleTypeDef handleADC;
extern DAC_HandleTypeDef handleDAC;


typedef struct
{
    // FPGA
    uint FPGAneedAutoFind           : 1;    // Если 1, то нужно найти сигнал
    uint FPGAtemporaryPause         : 1;
    uint FPGAinProcessingOfRead     : 1;
    uint FPGAcanReadData            : 1;
    uint FPGAcritiacalSituation     : 1;
    uint FPGAfirstAfterWrite        : 1;    // Используется в режиме рандомизатора. После записи любого параметра в альтеру нужно не использовать 
                                            // первое считанное данное с АЦП, потому что оно завышено и портит ворота

    uint cursorInDirs               : 1;    // Если 1, то ручка УСТАНОВКА перемещает по каталогам
    uint needRedrawFileManager      : 2;    // Если 1, то файл-менеджер нуждается в полной перерисовке
                                            // Если 2, то перерисовать только каталоги
                                            // Если 3, то перерисовать только файлы

    uint alreadyLoadSettings        : 1;    // Эта переменная нужна для того, чтобы исключить ложную запись пустых настроек из-за неправильного
                                            // включения прибора (при исключённом из схемы программном включении иногда сигнал от кнопки отключения
                                            // питания приходит быстрее, чем программа успевает настроить настройки).
    uint showHelpHints              : 1;    // Если 1, то при нажатии кнопки вместо выполнения её фунции выводится подсказка о её назначении

    uint showDebugMenu              : 1;
    
    uint tuneTime                   : 1;    // Если 1, то после загрузки настроек нужно вызвать страницу установки текущего времени

    int topMeasures                 : 9;    // Верх таблицы вывода измерений. Это значение нужно для нормального вывода сообщений на экран - чтобы они ничего не перекрывали

    uint disablePower               : 1;    // Если 1, то нужно выключить питание
    uint temporaryShowStrNavi       : 1;    // Признак того, что нужно временно показывать строку навигации меню
    uint needSetOrientation         : 1;    // Для установки ориентации дисплея
    uint cableEthIsConnected        : 1;    // 1, если подключён кабель LAN

    uint panelControlReceive        : 1;    // Если 1, то панель прислала команду
    uint needToMountFlash           : 1;    // Установленное в 1 значение означает, что подсоединена флешка. Надо её монтировать.

    uint soundIsBeep                : 1;    // Устанавливается в 1, когда идёт воспроизведение звука. Нужно как временный костыль - иначе зависает, когда сттирается флеш
} BitField;

extern volatile BitField gBF;               // Структура сделана volatile, потому что иначе при вклюённой оптимизации зависает во время выключения. Вероятно,
                                            // это связано с переменной soundIsBeep (перед стиранием сектора в цикле происходит ожидание, когда эта переменная
                                            // изменит своё состояние (каковое изменение происходит из прерывания, ясен перец))

#define RETURN_TO_MAIN_MENU     0
#define RETURN_TO_LAST_MEM      1
#define RETURN_TO_INT_MEM       2
#define RETURN_TO_DISABLE_MENU  3


typedef struct 
{
    int16   currentNumLatestSignal;                 // Текущий номер последнего сигнала в режиме ПАМЯТЬ - Последние
    int8    currentNumIntSignal;                    // Текущий номер сигнала, сохранённого в ППЗУ
    uint    alwaysShowMemIntSignal                      : 1;    // Если 1, то показывать всегда выбранный в режиме "Внутр. ЗУ" сигнал
    uint    runningFPGAbeforeSmallButtons   : 1;    // Здесь сохраняется информация о том, работала ли ПЛИС перед переходом в режим работы с памятью
    uint    exitFromIntToLast               : 1;    // Если 1, то выходить из страницы внутренней памяти нужно не стандартно, а в меню последних
    uint    exitFromModeSetNameTo           : 2;    // Куда возвращаться из окна установки имени при сохранении : 0 - в основное меню, 1 - в окно последних, 2 - в окно Внутр ЗУ, 3 - в основно окно в выключенным меню
    uint    needForSaveToFlashDrive         : 1;    // Если 1, то нужно сохранить после отрисовки на флешку.
} GMemory;

#define ALWAYS_SHOW_MEM_INT_SIGNAL (gMemory.alwaysShowMemIntSignal == 1)

extern GMemory gMemory;


typedef enum
{
    StateCalibration_None,
    StateCalibration_ADCinProgress,
    StateCalibration_RShiftAstart,
    StateCalibration_RShiftAinProgress,
    StateCalibration_RShiftBstart,
    StateCalibration_RShiftBinProgress,
    StateCalibration_ErrorCalibrationA,
    StateCalibration_ErrorCalibrationB
} StateCalibration;

typedef enum
{
    StateWorkFPGA_Stop,                 // СТОП - не занимается считыванием информации.
    StateWorkFPGA_Wait,                 // Ждёт поступления синхроимпульса.
    StateWorkFPGA_Work,                 // Идёт работа.
    StateWorkFPGA_Pause                 // Это состояние, когда временно приостановлен прибор, например, для чтения данных или для записи значений регистров.
} StateWorkFPGA;


typedef struct
{
    bool needCalibration;				// Установленное в true значение означает, что необходимо произвести калибровку.
    StateWorkFPGA stateWorkBeforeCalibration;
    StateCalibration stateCalibration;  // Текущее состояние калибровки. Используется в процессе калибровки.
} StateFPGA;

typedef struct
{
    uint timeMS;    // Время в миллисекундах от старта системы. Т.к. структура заполняется во время сохранения данных в хранилище, то timeMS == 0 означает, что полный сигнал в режиме поточеного вывода ещё не считан
    uint hours : 5;
    uint minutes : 6;
    uint seconds : 6;
    uint year : 7;
    uint month : 4;
    uint day : 5;
} PackedTime;

typedef struct
{
    uint16      rShift[2];
    uint16      trigLev[2];
    uint8*      addrData;               // Адрес данных во внешнем ОЗУ
    uint8       range[2];               // Масштаб по напряжению обоих каналов.
    int16       tShift;                 // Смещение по времени
    uint        tBase           : 5;    // Масштаб по времени
    uint        enableChB       : 1;    // Включен ли канал B
    uint        indexLength     : 3;    // Сколько байт в канале (при включённом пиковом детекторе байт в два раза больше, чем точек)
    uint        modeCoupleA     : 2;    // Режим канала по входу
    uint        modeCoupleB     : 2;
    uint        peackDet        : 2;    // Включен ли пиковый детектор
    uint        enableChA       : 1;    // Включён ли канал A
    uint        inverseChA      : 1;
    uint        inverseChB      : 1;
    uint        multiplierA     : 1;
    uint        multiplierB     : 1;
    PackedTime  time;
} DataSettings;

int NumBytesInChannel(DataSettings *ds);    // Возвращает количество байт на канал
int NumBytesInData(DataSettings *ds);       // Возвращает количество байт в обоих каналах
int NumPointsInChannel(DataSettings *ds);   // Возвращает количество точек на канал

// Возвращает 0, если канал выключен
uint8 *AddressChannel(DataSettings *ds, Channel ch);

extern const char *gStringForHint;          // Строка подсказки, которую надо выводить в случае включённого режима подсказок.
extern void* gItemHint;                     // Item, подсказку для которого нужно выводить в случае включённого режима подсказок.

void SetItemForHint(void *item);

extern StateFPGA gStateFPGA; 

extern uint8        *gDataA;            // Указатель на данные первого канала, который надо рисовать на экране
extern uint8        *gDataB;            // Указатель на данные второго канала, который надо рисовать на экране
extern DataSettings *gDSet;             // Указатель на параметры рисуемых сигналов

extern DataSettings *gDSmemInt;         //--
extern uint8        *gDataAmemInt;      // | Здесь данные из ППЗУ, которые должны выводиться на экран
extern uint8        *gDataBmemInt;      //-/

extern DataSettings *gDSmemLast;        //--
extern uint8        *gDataAmemLast;     // | Здесь данные из ОЗУ, которые должны выводиться на экран
extern uint8        *gDataBmemLast;     //-/

#define NUM_DATAS 999
extern DataSettings gDatas[NUM_DATAS];                  // Используется только в DataStorage
extern uint8 gDataAve[NumChannels][FPGA_MAX_POINTS];    // Используется только в DataStorage

extern int gAddNStop;

extern void *extraMEM;      // Это специальный указатель. Используется для выделения памяти переменным, которые не нужны всё время выполения программы,
                            // но нужны болеее чем в одной функции. Перед использованием с помощью вызова malloc() выделяется необходимое количество
                            // памяти, которая затем освобождается вызвом free()

#define MALLOC_EXTRAMEM(NameStruct, name)   extraMEM = malloc(sizeof(NameStruct));    \
                                            NameStruct *name = (NameStruct*)extraMEM
#define ACCESS_EXTRAMEM(NameStruct, name)   NameStruct *name = (NameStruct*)extraMEM
#define FREE_EXTRAMEM()                     free(extraMEM)
