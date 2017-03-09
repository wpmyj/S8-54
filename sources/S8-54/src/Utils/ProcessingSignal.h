#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGAtypes.h"
#include "FPGA/DataStorage.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void    Processing_SetSignal(uint8 *dataA, uint8 *dataB, DataSettings *ds, int firstPoint, int lastPoint);         // Установить сигнал для обработки.
void    Processing_GetData(uint8 **dataA, uint8 **dataB, DataSettings **ds);            // Получить данные ранее установленного сигнала.
float   Processing_GetCursU(Channel ch, float posCurT);                                 // Получить позицию курсора напряжения, соответствующю заданной позиции курсора posCurT.
float   Processing_GetCursT(Channel ch, float posCurU, int numCur);                     // Получить позицию курсора времени, соответствующую заданной позиции курсора напряжения posCurU.
void    Processing_InterpolationSinX_X(uint8 *data, int numPoints, TBase tBase);        // Аппроксимировать единичное измерение режима рандомизатора функцией sinX/X.
char*   Processing_GetStringMeasure(Measure measure, Channel ch, char *buffer, int lenBuf); // Возвращает строку автоматического измерения.
void    Processing_CalculateMeasures(void);                                             // Расчитать все измерения.
int     Processing_GetMarkerHorizontal(Channel ch, int numMarker);                      // Возвращает значение горизонтального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
int     Processing_GetMarkerVertical(Channel ch, int numMarker);                        // Возвращает значение вертикального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
