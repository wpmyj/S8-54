#pragma once
#include "Data/DataSettings.h"
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


/** @defgroup Globals
 *  @brief Глобальые переменые
 *  @{
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RETURN_TO_MAIN_MENU     0
#define RETURN_TO_LAST_MEM      1
#define RETURN_TO_INT_MEM       2
#define RETURN_TO_DISABLE_MENU  3

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern USBH_HandleTypeDef handleUSBH;
extern HCD_HandleTypeDef handleHCD;
extern SPI_HandleTypeDef handleSPI;
extern DMA_HandleTypeDef handleDMA_RAM;
extern ADC_HandleTypeDef handleADC;
extern DAC_HandleTypeDef handleDAC;

#define NEED_DISABLE_POWER      (gBF.disablePower)
#define FPGA_NEED_AUTO_FIND     (gBF.FPGAneedAutoFind)

#define FPGA_IN_PROCESS_OF_READ (gBF.FPGAinProcessingOfRead)
#define FPGA_FIRST_AFTER_WRITE  (gBF.FPGAfirstAfterWrite)

#define FM_CURSOR_IN_DIRS       (gBF.cursorInDirs)
#define FM_NEED_REDRAW          (gBF.needRedrawFileManager)
#define FM_REDRAW_FULL          1
#define FM_REDRAW_FOLDERS       2
#define FM_REDRAW_FILES         3

#define NUM_DRAWING_SIGNALS     (gBF.numDrawingSignals)
/// В этом при нажатии на кнопки вместо выполнения её функции выводится информация о её назначении
#define HINT_MODE_ENABLE        (gBF.showHelpHints)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint numDrawingSignals          : 9;    ///< \brief Количество нарисованных сигналов. Используется для того, чтобы определить, нужно ли стирать 
                                            ///<        дисплей в режиме накопления
    uint FPGAneedAutoFind           : 1;    ///< Если 1, то нужно найти сигнал.
    uint FPGAinProcessingOfRead     : 1;
    uint FPGAfirstAfterWrite        : 1;    ///< \brief Используется в режиме рандомизатора. После записи любого параметра в альтеру нужно не 
                                            ///<        использовать первое считанное данное с АЦП, потому что оно завышено и портит ворота.
    uint cursorInDirs               : 1;    ///< Если 1, то ручка УСТАНОВКА перемещает по каталогам.
    uint needRedrawFileManager      : 2;    ///< @brief Если 1, то файл-менеджер нуждается в полной перерисовке.
                                            ///< Если 2, то перерисовать только каталоги.
                                            ///< Если 3, то перерисовать только файлы.
    uint alreadyLoadSettings        : 1;    ///< @brief Эта переменная нужна для того, чтобы исключить ложную запись пустых настроек из-за 
                                            ///< неправильного включения прибора (при исключённом из схемы программном включении иногда сигнал от 
                                            ///< кнопки отключения питания приходит быстрее, чем программа успевает настроить настройки).
    uint showHelpHints              : 1;    ///< Если 1, то при нажатии кнопки вместо выполнения её фунции выводится подсказка о её назначении.
    uint showDebugMenu              : 1;
    uint tuneTime                   : 1;    ///< Если 1, то после загрузки настроек нужно вызвать страницу установки текущего времени.
    int topMeasures                 : 9;    ///< \brief Верх таблицы вывода измерений. Это значение нужно для нормального вывода сообщений на экран - 
                                            ///<        чтобы они ничего не перекрывали.
    uint disablePower               : 1;    ///< Если 1, то нужно выключить питание.
    uint temporaryShowStrNavi       : 1;    ///< Признак того, что нужно временно показывать строку навигации меню.
    uint needSetOrientation         : 1;    ///< Для установки ориентации дисплея.
    uint cableEthIsConnected        : 1;    ///< 1, если подключён кабель LAN.
    uint panelControlReceive        : 1;    ///< Если 1, то панель прислала команду.
    uint needToMountFlash           : 1;    ///< Установленное в 1 значение означает, что подсоединена флешка. Надо её монтировать.
    uint consoleInPause             : 1;    ///< Если 1, то консоль находится в режиме паузы.
    uint needStopAfterReadFrameP2P  : 1;    ///< Если 1, то после считывания очередного фрейма нужно остановить вывод на экран.
    uint needFinishDraw             : 1;    ///< Если 1, то нужно немедленно завершить отрисовку и вывести на экран то, что уже нарисовано.
} BitField;

extern volatile BitField gBF;   ///< @brief Структура сделана volatile, потому что иначе при вклюённой оптимизации зависает во время выключения. 
                                ///< Вероятно, это связано с переменной soundIsBeep (перед стиранием сектора в цикле происходит ожидание, когда эта 
                                ///< переменная изменит своё состояние (каковое изменение происходит из прерывания, ясен перец)).

#define NEED_FINISH_DRAW        (gBF.needFinishDraw)

#define NUM_RAM_SIGNAL          (gMemory.currentNumRAMSignal)
#define NUM_ROM_SIGNAL          (gMemory.currentNumROMSignal)
#define ALWAYS_SHOW_ROM_SIGNAL  (gMemory.alwaysShowROMSignal)
#define RUN_FPGA_BEFORE_SB      (gMemory.runningFPGAbeforeSmallButtons)
#define EXIT_FROM_ROM_TO_RAM    (gMemory.exitFromROMtoRAM)
#define EXIT_FROM_SETNAME_TO    (gMemory.exitFromModeSetNameTo)
#define NEED_SAVE_TO_FLASHDRIVE (gMemory.needForSaveToFlashDrive)

#define RECORDING_TO_RAM        (gMemory.recordSaveToRAM == 1)  ///< 
#define RECORDING_TO_EXT        (gMemory.recordSaveToRAM == 0)
#define SET_RECORDING_TO_RAM    (gMemory.recordSaveToRAM = 1)
#define SET_RECORDING_TO_EXT    (gMemory.recordSaveToRAM = 0)

typedef struct 
{
    int16 currentNumRAMSignal;                  ///< Текущий номер последнего сигнала в режиме ПАМЯТЬ - Последние.
    int8 currentNumROMSignal;                   ///< Текущий номер сигнала, сохранённого в ППЗУ.
    uint alwaysShowROMSignal            : 1;    ///< Если 1, то показывать всегда выбранный в режиме "Внутр. ЗУ" сигнал.
    uint runningFPGAbeforeSmallButtons  : 1;    ///< Здесь сохраняется информация о том, работала ли ПЛИС перед переходом в режим работы с памятью.
    uint exitFromROMtoRAM               : 1;    ///< Если 1, то выходить из страницы внутренней памяти нужно не стандартно, а в меню последних.
    uint exitFromModeSetNameTo          : 2;    ///< \brief Куда возвращаться из окна установки имени при сохранении : 0 - в основное меню, 1 - 
                                                ///< в окно последних, 2 - в окно Внутр ЗУ, 3 - в основно окно в выключенным меню.
    uint needForSaveToFlashDrive        : 1;    ///< Если 1, то нужно сохранить после отрисовки на флешку.
    uint recordSaveToRAM                : 1;    ///< Если 1, то запись данных регистратора происходит в RAM, иначе - на флешку
} GMemory;



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
    StateWorkFPGA_Stop,     ///< СТОП - не занимается считыванием информации.
    StateWorkFPGA_Wait,     ///< Ждёт поступления синхроимпульса.
    StateWorkFPGA_Work,     ///< Идёт работа.
    StateWorkFPGA_Pause     ///< Это состояние, когда временно приостановлен прибор, например, для чтения данных или для записи значений регистров.
} StateWorkFPGA;


typedef struct
{
    bool needCalibration;                       ///< Установленное в true значение означает, что необходимо произвести калибровку.
    StateWorkFPGA stateWorkBeforeCalibration;
    StateCalibration stateCalibration;          ///< Текущее состояние калибровки. Используется в процессе калибровки.
} StateFPGA;

extern const char *gStringForHint;              ///< Строка подсказки, которую надо выводить в случае включённого режима подсказок.
extern void *gItemHint;                         ///< Item, подсказку для которого нужно выводить в случае включённого режима подсказок.

void SetItemForHint(void *item);

extern StateFPGA gStateFPGA; 

extern GMemory gMemory;

#define MALLOC_EXTRAMEM(NameStruct, name)   extraMEM = malloc(sizeof(NameStruct));    \
                                            NameStruct *name = (NameStruct*)extraMEM
#define ACCESS_EXTRAMEM(NameStruct, name)   NameStruct *name = (NameStruct*)extraMEM
#define FREE_EXTRAMEM()                     free(extraMEM);

/** @}
 */
