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
class Measures
{
public:
    char GetChar(Meas measure);

    bool IsActive(int row, int col);

    void GetActive(int *row, int *col);

    void SetActive(int row, int col);

    int GetDY(void);

    int GetDX(void);

    const char *Name(int row, int col);

    Meas Type(int row, int col);

    int NumRows(void);

    int NumCols(void);

    int GetTopTable(void);

    int GetDeltaGridLeft(void);

    int GetDeltaGridBottom(void);

    void ShortPressOnSmallButonMarker(void);
    /// Нарисовать страницу выбора измерений
    void DrawPageChoice(void);
};


extern Measures meas;


/** @}  @}
 */
