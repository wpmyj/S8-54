#pragma once
#include "Data/DataSettings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup FPGA
 *  @{
 *  @defgroup Data
 *  @{
 *  @defgroup Interface Интерфейс
 *  @{
 */

#ifndef _INCLUDE_DATA_
#define EXTERN extern
#else
#define EXTERN
#endif

#define DS          pDS             ///< Указатель на настройки текущего рисуемого сигнала.

EXTERN DataSettings *pDS;           ///< Указатель на настройки текущего рисуемого сигнала. Обращаться к нему следует через макрос DS.

#undef EXTERN
/** @defgroup GlobalData Global Data
 *  @brief Настройки, действующие для отображаемого в данный момент сигнала
 *  @{
 */

#define TSHIFT_DS           (TSHIFT(DS))
#define TBASE_DS            (TBASE(DS))
#define INVERSE_DS(ch)      (INVERSE(DS, ch))
#define COUPLE_DS(ch)       (COUPLE(DS, ch))
#define DIVIDER_DS(ch)      (DIVIDER(DS, ch))
#define RANGE_DS(ch)        (RANGE(DS, ch))
#define RANGE_DS_A          (RANGE(DS, A))
#define RANGE_DS_B          (RANGE(DS, B))
#define ENABLED_DS(ch)      (ENABLED(DS, ch))
#define ENABLED_DS_A        (ENABLED(DS, A))
#define ENABLED_DS_B        (ENABLED(DS, B))
#define RSHIFT_DS(ch)       (RSHIFT(DS, ch))
#define RSHIFT_DS_A         (RSHIFT(DS, A))
#define RSHIFT_DS_B         (RSHIFT(DS, B))
#define PEAKDET_DS         (PEAKDET(DS))
#define TRIGLEV_DS(ch)      (TRIGLEV(DS))
#define ENUM_BYTES_DS       (ENUM_BYTES(DS))
#define BYTES_IN_CHANNEL_DS (BYTES_IN_CHANNEL(DS))

#define TIME_TIME_DS        (TIME_TIME(DS))
#define TIME_DAY_DS         (TIME_DAY(DS))
#define TIME_HOURS_DS       (TIME_HOURS(DS))
#define TIME_MINUTES_DS     (TIME_MINUTES(DS))
#define TIME_SECONDS_DS     (TIME_SECONDS(DS))
#define TIME_MONTH_DS       (TIME_MONTH(DS))
#define TIME_YEAR_DS        (TIME_YEAR(DS))

/** @}
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// В этой структуре будут храниться точки, подготовленные для вывода на экран
typedef struct
{
    uint8   data[NumChannels][281 * 2];     ///< Данные обоих каналов. Точек в два раза больше, чем на экране, для пикового детектора
    bool    needDraw[NumChannels];          ///< Если true, то канал 1 надо рисовать
} StructDataDrawing;

/// Вызывается в начале цикла
void Data_Clear(void);
/// Читает данные из ОЗУ, fromEnd c конца (fromEnd == 0 - последний считанный сигнал) и заполняет данными inA(B), outA(B), DS.
void Data_ReadFromRAM(int fromEnd, StructDataDrawing *dataStruct);
/// Читает данные из EPROM. Номер сигнала - глобвльнй NUM_ROM_SIGNAL и заполняет данными inA(B), outA(B), DS.
void Data_ReadFromROM(StructDataDrawing *dataStruct);


/** @}  @}  @}
 */
