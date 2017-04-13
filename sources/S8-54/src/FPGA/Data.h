#pragma once
#include "Settings/DataSettings.h"
#include "Settings/SettingsTime.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup FPGA
 *  @{
 *  @defgroup Data
 *  @{
 */

#define DS              pDS                 // Указатель на настройки текущего рисуемого сигнала
#define DATA(ch)        (dataChan[ch])      // Указатель на данные отображаемого сигнала
#define DATA_A          (DATA(A))           // Указатель на данные отображаемого канала 1
#define DATA_B          (DATA(B))           // Указатель на данные отображаемого канала 2
#define DATA_INT(ch)    (dataChanInt[ch])
#define DATA_LAST(ch)   (dataChanLast[ch])

#ifndef _INCLUDE_DATA_
    #define EXTERN extern
#else
    #define EXTERN
#endif

EXTERN DataSettings *pDS;
EXTERN uint8* dataChan[2];
EXTERN uint8 *dataChanLast[2];
EXTERN uint8 *dataChanInt[2];

#undef EXTERN

// Настройки, действующие для отображаемого в данный момент сигнала
#define G_TSHIFT        (TSHIFT(DS))
#define G_TBASE         (TBASE(DS))
#define G_INVERSE(ch)   (INVERSE(DS, ch))
#define G_COUPLE(ch)    (COUPLE(DS, ch))
#define G_DIVIDER(ch)   (DIVIDER(DS, ch))
#define G_RANGE(ch)     (RANGE(DS, ch))
#define G_ENABLED(ch)   (ENABLED(DS, ch))
#define G_RSHIFT(ch)    (RSHIFT(DS, ch))
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_GetFromIntMemory(void);           // Считать из ППЗУ информацию о настройках и указатели на данные
void Data_GetAverageFromDataStorage(void);
void Data_Load(void);

/** @}  @}
 */
