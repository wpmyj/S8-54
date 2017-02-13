#pragma once


#include "defines.h"

/*
    Структура используется для отрисовки прогресс-бара во время автоматического
    поиска сигнала
*/
typedef struct
{
    uint8 progress; // Относительная величина прогресса
    int8 sign;      // Направление изменения прогресса
} StrForAutoFind;
