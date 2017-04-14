#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGAtypes.h"
#include "FPGA/DataStorage.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Utils
 *  @{
 *  @defgroup ProcessingSignal
 *  @{
 */


/// Установить сигнал для обработки
void Processing_SetSignal(uint8 *dataA, uint8 *dataB, DataSettings *ds, int firstPoint, int lastPoint);
/// Получить данные ранее установленного сигнала
void Processing_GetData(uint8 **dataA, uint8 **dataB, DataSettings **ds);
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
/// Возвращает значение горизонтального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно
int Processing_GetMarkerHorizontal(Channel ch, int numMarker);
/// Возвращает значение вертикального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно
int Processing_GetMarkerVertical(Channel ch, int numMarker);

/** @}  @}
 */
