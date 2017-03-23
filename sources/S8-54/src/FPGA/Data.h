#pragma once
#include "Settings/DataSettings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DS              pDS             
#define DATA(ch)        (dataChan[ch])  

#define DS_INT          pDSInt
#define DATA_INT(ch)    (dataChanInt[ch])

#define DS_LAST         pDSLast
#define DATA_LAST(ch)   (dataChanLast[ch])

#ifndef _INCLUDE_DATA_
    #define EXTERN extern
#else
    #define EXTERN
#endif

EXTERN DataSettings *pDS;
EXTERN uint8* dataChan[2];

EXTERN DataSettings *pDSLast;
EXTERN uint8 *dataChanLast[2];

EXTERN DataSettings *pDSInt;
EXTERN uint8 *dataChanInt[2];

#undef EXTERN


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_GetFromIntMemory(void);       // Считать из ППЗУ информацию о настройках и указатели на данные
void Data_GetAverageFromDataStorage(void);
