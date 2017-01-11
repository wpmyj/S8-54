#pragma once

#include "defines.h"
#include "Hardware/FLASH.h"


typedef struct
{
    uint start;
    int end;
} Segment;          // Структура для описания пустого промежутка между заполеннными участками памяти


void Segments_Set(Segment segments[MAX_NUM_SAVED_WAVES + 2]);   // Установить массив сегментов, с которым будем работать
void Segments_Cut(uint start, uint end);                        // Вырезать сегмент с этими границами
int  Segments_Num(void);
