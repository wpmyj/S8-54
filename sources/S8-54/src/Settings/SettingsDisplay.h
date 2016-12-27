#pragma once
#include "defines.h"
#include "SettingsTypes.h"
#include "Display/Colors.h"


#define DISPLAY_NUM_ACCUM       (1 << (int)set.display.numAccumulation) /* Возвращает число накоплений */
#define DISPLAY_NUM_MIN_MAX     (1 << (int)set.display.numMinMax)       /* Возвращает количество измерений сигналов для расчёта минимумов и максимумов. */
#define SHOW_STRING_NAVI        (set.display.showStringNavigation)
#define SHOW_STRING_NAVI_TEMP   (set.display.showStringNavigation == ShowStrNavi_Temp)
#define SHOW_STRING_NAVI_ALL    (set.display.showStringNavigation == ShowStrNavi_All)
#define SHIFT_IN_MEMORY         (set.display.shiftInMemory)
#define TIME_MESSAGES           (set.display.timeMessages)
#define COLOR(x)                (set.display.colors[x])


int     sDisplay_NumSignalsInS(void);                       // Возвращает ограничение частоты кадров.
void    sDisplay_SetNumSignalsInS(int maxFPS);              // Устанавливает ограничение частоты кадров.
int     sDisplay_NumPointSmoothing(void);                   // Возвращает число точек сглаживающего фильтра (режим ДИСПЛЕЙ - Сглаживание).
void    sDisplay_PointsOnDisplay(int *firstPoint, int *lastPoint); // Возвращает адрес первой и последней точки на экране.
int     sDisplay_TimeMenuAutoHide(void);                    // Возвращает время, через которое меню автоматически скрывается, если не было больше нажатий.
bool    sDisplay_IsSeparate(void);                          // Если экран разделён на две части и основной сигнал выводится сверху - например, в режиме вывода спектра
void    sDisplay_SetBrightness(int16 brightness);           // brightness = 1..100
ModeAveraging sDisplay_GetModeAveraging(void);              // Возвращает режим усреднения
int     sDisplay_NumAverage(void);
int     sDisplay_ShiftInMemoryInPoints(void);               // Это смещение экрана по памяти в режиме пикового детектора оно будет в два раза меньше, чем байт, потому что каждая точка представлена двумя байтами

