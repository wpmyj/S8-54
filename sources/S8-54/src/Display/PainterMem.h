#pragma once
#include "defines.h"

/*
    Рисует в памяти 4-битным цветом
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PainterMem_SetBuffer(uint8 *address, int width, int height);
void PainterMem_FillRect(int x, int y, int width, int height, int color);
void PainterMem_SetPoint(int x, int y, int color);
void PainterMem_DrawVLine(int x, int y0, int y1, int color);
void PainterMem_DrawHLine(int y, int x0, int x1, int color);
void PainterMem_DrawRectangle(int x, int y, int widht, int height, int color);
