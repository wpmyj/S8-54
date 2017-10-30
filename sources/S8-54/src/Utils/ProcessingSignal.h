#pragma once
#include "Measures.h"
#include "defines.h"
#include "Data/DataStorage.h"
#include "FPGA/FPGAtypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Utils
 *  @{
 *  @defgroup ProcessingSignal
 *  @{
 */

#define MARKER_VOLTAGE(ch, num)  (markerVoltage[ch][num] - MIN_VALUE)
#define MARKER_TIME(ch, num)    (markerTime[ch][num])

extern int markerVoltage[NumChannels][2];
extern int markerTime[NumChannels][2];


/// \brief Установить сигнал для обработки. Данные берутся из DS, inA, inB.
/// Выходные данные, соответствующие текущим настройками set, рассчитываются сразу и записываются в outA, outB.
void Processing_SetData(bool needSmoothing);
/// Получить позицию курсора напряжения, соответствующю заданной позиции курсора posCurT
float Processing_GetCursU(Channel ch, float posCurT);
/// Получить позицию курсора времени, соответствующую заданной позиции курсора напряжения posCurU
float Processing_GetCursT(Channel ch, float posCurU, int numCur);
/// Аппроксимировать единичное измерение режима рандомизатора функцией sinX/X
void Processing_InterpolationSinX_X(uint8 *data, int numPoints, TBase tBase);
/// Возвращает строку автоматического измерения
char* Processing_GetStringMeasure(Measure measure, Channel ch, char *buffer, int lenBuf);
/// Расчитать все измерения
void Processing_CalculateMeasures(void);


/** @}  @}
 */
