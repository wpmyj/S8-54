#pragma once
#include "Settings/DataSettings.h"
#include "Settings/SettingsTime.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

#define G_TSHIFT        (TSHIFT(DS))
#define G_TBASE         (TBASE(DS))
#define G_INVERSE(ch)   (INVERSE(DS, ch))
#define G_COUPLE(ch)    (COUPLE(DS, ch))
#define G_DIVIDER(ch)   ((Divider)((ch == A) ? DS->multiplierA : DS->multiplierB))
#define G_RANGE(ch)     ((Range)(DS->range[ch]))
#define G_ENABLED(ch)   (ENABLED(DS, ch))
#define G_RSHIFT(ch)    (DS->rShift[ch])
#define G_PEACKDET      (DS->peackDet)

#define G_TIME_DAY      (DS->time.day)
#define G_TIME_HOURS    (DS->time.hours)
#define G_TIME_MINUTES  (DS->time.minutes)
#define G_TIME_SECONDS  (DS->time.seconds)
#define G_TIME_MONTH    (DS->time.month)
#define G_TIME_YEAR     (DS->time.year)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_GetFromIntMemory(void);           // Считать из ППЗУ информацию о настройках и указатели на данные
void Data_GetAverageFromDataStorage(void);
void Data_Load(void);
