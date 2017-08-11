#pragma once
#include "Settings/DataSettings.h"


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

#define G_TSHIFT            (TSHIFT(DS))
#define G_TBASE             (TBASE(DS))
#define G_INVERSE(ch)       (INVERSE(DS, ch))
#define G_COUPLE(ch)        (COUPLE(DS, ch))
#define G_DIVIDER(ch)       (DIVIDER(DS, ch))
#define G_RANGE(ch)         (RANGE(DS, ch))
#define G_RANGE_A           (RANGE(DS, A))
#define G_RANGE_B           (RANGE(DS, B))
#define G_ENABLED(ch)       (ENABLED(DS, ch))
#define G_ENABLED_A         (ENABLED(DS, A))
#define G_ENABLED_B         (ENABLED(DS, B))
#define G_RSHIFT(ch)        (RSHIFT(DS, ch))
#define G_RSHIFT_A          (RSHIFT(DS, A))
#define G_RSHIFT_B          (RSHIFT(DS, B))
#define G_PEACKDET          (PEACKDET(DS))
#define G_TRIGLEV(ch)       (TRIGLEV(DS))
#define G_ENUM_BYTES        (ENUM_BYTES(DS))
#define G_BYTES_IN_CHANNEL  (BYTES_IN_CHANNEL(DS))

#define G_TIME_TIME         (TIME_TIME(DS))
#define G_TIME_DAY          (TIME_DAY(DS))
#define G_TIME_HOURS        (TIME_HOURS(DS))
#define G_TIME_MINUTES      (TIME_MINUTES(DS))
#define G_TIME_SECONDS      (TIME_SECONDS(DS))
#define G_TIME_MONTH        (TIME_MONTH(DS))
#define G_TIME_YEAR         (TIME_YEAR(DS))

/** @}
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_Clear(void);

/// Читает данные из ОЗУ, fromEnd c конца (fromEnd == 0 - последний считанный сигнал) и заполняет данными inA(B), outA(B), DS.
void Data_ReadDataRAM(int fromEnd);
/// Читает данные из EPROM. Номер сигнала - глобвльнй NUM_ROM_SIGNAL и заполняет данными inA(B), outA(B), DS.
void Data_ReadDataROM(void);


/** @}  @}  @}
 */
