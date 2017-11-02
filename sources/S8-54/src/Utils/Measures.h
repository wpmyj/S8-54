#pragma once
#include "defines.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Utils
 *  @{
 *  @defgroup Measures
 *  @{
 */

// Виды измерений
typedef enum
{
    Meas_None,
    Meas_VoltageMax,
    Meas_VoltageMin,
    Meas_VoltagePic,
    Meas_VoltageMaxSteady,
    Meas_VoltageMinSteady,
    Meas_VoltageAmpl,
    Meas_VoltageAverage,
    Meas_VoltageRMS,
    Meas_VoltageVybrosPlus,
    Meas_VoltageVybrosMinus,
    Meas_Period,
    Meas_Freq,
    Meas_TimeNarastaniya,
    Meas_TimeSpada,
    Meas_DurationPlus,
    Meas_DurationMinus,
    Meas_SkvaznostPlus,
    Meas_SkvaznostMinus,
    Meas_DelayPlus,
    Meas_DelayMinus,
    Meas_PhazaPlus,
    Meas_PhazaMinus,
    Meas_NumMeasures
} Meas;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char Measure_GetChar(Meas measure);

bool Measure_IsActive(int row, int col); 

void Measure_GetActive(int *row, int *col);

void Measure_SetActive(int row, int col);

int Measure_GetDY(void);

int Measure_GetDX(void);

const char *Measure_Name(int row, int col);

Meas Measure_Type(int row, int col);

int Measure_NumRows(void);

int Measure_NumCols(void);

int Measure_GetTopTable(void);

int Measure_GetDeltaGridLeft(void);

int Measure_GetDeltaGridBottom(void);

void Measure_ShortPressOnSmallButonMarker(void);
/// Нарисовать страницу выбора измерений
void Measure_DrawPageChoice(void);

/** @}  @}
 */
