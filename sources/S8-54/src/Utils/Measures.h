#pragma once


#include "defines.h"


// Виды измерений
typedef enum
{
    Measure_None,
    Measure_VoltageMax,
    Measure_VoltageMin,
    Measure_VoltagePic,
    Measure_VoltageMaxSteady,
    Measure_VoltageMinSteady,
    Measure_VoltageAmpl,
    Measure_VoltageAverage,
    Measure_VoltageRMS,
    Measure_VoltageVybrosPlus,
    Measure_VoltageVybrosMinus,
    Measure_Period,
    Measure_Freq,
    Measure_TimeNarastaniya,
    Measure_TimeSpada,
    Measure_DurationPlus,
    Measure_DurationMinus,
    Measure_SkvaznostPlus,
    Measure_SkvaznostMinus,
    Measure_DelayPlus,
    Measure_DelayMinus,
    Measure_PhazaPlus,
    Measure_PhazaMinus,
    Measure_NumMeasures
} Measure;


char        Measure_GetChar(Measure measure);
bool        Measure_IsActive(int row, int col); 
void        Measure_GetActive(int *row, int *col);
void        Measure_SetActive(int row, int col);
int         Measure_GetDY(void);
int         Measure_GetDX(void);
const char* Measure_Name(int row, int col);
Measure     Measure_Type(int row, int col);
int         Measure_NumRows(void);
int         Measure_NumCols(void);
int         Measure_GetTopTable(void);
int         Measure_GetDeltaGridLeft(void);
int         Measure_GetDeltaGridBottom(void);
void        Measure_ShortPressOnSmallButonMarker(void);
void        Measure_DrawPageChoice(void);                   // Нарисовать страницу выбора измерений.
