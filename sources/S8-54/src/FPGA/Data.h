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

#define DS              pDS             ///< Указатель на настройки текущего рисуемого сигнала.
#define DATA(ch)        (dataChan[ch])  ///< Указатель на данные отображаемого сигнала.
#define DATA_A          (DATA(A))       ///< Указатель на данные отображаемого канала 1.
#define DATA_B          (DATA(B))       ///< Указатель на данные отображаемого канала 2.

EXTERN DataSettings *pDS;               ///< Указатель на настройки текущего рисуемого сигнала. Обращаться к нему следует через макрос DS.
EXTERN uint8 *dataChan[2];              ///< Указатель на данные отображаемого сигнала. Обращаться к нему следует через макрос DATA(ch).

#undef EXTERN

/** @defgroup GlobalData Global Data
 *  @brief Настройки, действующие для отображаемого в данный момент сигнала
 *  @{
 */

#define G_TSHIFT        (TSHIFT(DS))
#define G_TBASE         (TBASE(DS))
#define G_INVERSE(ch)   (INVERSE(DS, ch))
#define G_COUPLE(ch)    (COUPLE(DS, ch))
#define G_DIVIDER(ch)   (DIVIDER(DS, ch))
#define G_RANGE(ch)     (RANGE(DS, ch))
#define G_RANGE_A       (RANGE(DS, A))
#define G_RANGE_B       (RANGE(DS, B))
#define G_ENABLED(ch)   (ENABLED(DS, ch))
#define G_ENABLED_A     (ENABLED(DS, A))
#define G_ENABLED_B     (ENABLED(DS, B))
#define G_RSHIFT(ch)    (RSHIFT(DS, ch))
#define G_RSHIFT_A      (RSHIFT(DS, A))
#define G_RSHIFT_B      (RSHIFT(DS, B))
#define G_PEACKDET      (PEACKDET(DS))
#define G_TRIGLEV(ch)   (TRIGLEV(DS))
#define G_INDEXLENGHT   (INDEXLENGTH(DS))

#define G_TIME_TIME     (TIME_TIME(DS))
#define G_TIME_DAY      (TIME_DAY(DS))
#define G_TIME_HOURS    (TIME_HOURS(DS))
#define G_TIME_MINUTES  (TIME_MINUTES(DS))
#define G_TIME_SECONDS  (TIME_SECONDS(DS))
#define G_TIME_MONTH    (TIME_MONTH(DS))
#define G_TIME_YEAR     (TIME_YEAR(DS))

/** @}
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Считать из ППЗУ информацию о настройках и указатели на данные.
void Data_GetFromIntMemory(void);

void Data_GetAverageFromDataStorage(void);
/// Здесь заполняем указатели на данные и их настройки в соответствии с текущими режимами отображения.
void Data_Load(void);
/// Подготовить глобальные указатели на данные и их настройки для вывода данных на экран.
void Data_PrepareToUse(ModeWork mode);
/// \todoВозвращает режим, на который сейчас настроены указатели. Нужно для того, что если находимся в реальном режиме, выводить каналы в нужной 
/// последовательности в зависимости от того, какой канал был задействован последним.
ModeWork Data_GetUsedModeWork(void);

/** @}  @}  @}
 */
