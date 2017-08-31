#pragma once

#include "defines.h"


#pragma once


#define SCREEN_WIDTH            320
#define SCREEN_WIDTH_IN_BYTES   (320 / 8)
#define SCREEN_HEIGHT           240
#define SCREEN_HEIGHT_IN_BYTES  (240 / 8)
#define SIZE_BUFFER_FOR_SCREEN  ((SCREEN_WIDTH) * (SCREEN_HEIGHT) / 8)

#define GRID_DELTA              20  /* Количество точек в клетке */
#define GRID_TOP                19
#define GRID_CELLS_IN_HEIGHT    10
#define GRID_HEIGHT             ((GRID_CELLS_IN_HEIGHT) * (GRID_DELTA))
#define GRID_BOTTOM             ((GRID_TOP) + (GRID_HEIGHT))
#define GRID_WIDTH              (280)

#define GRID_RIGHT              ((GridLeft()) + (GridWidth()))

#define CUR_WIDTH               6
#define CUR_HEIGHT              8

#define MP_Y                    ((GRID_TOP) + 1)
#define MP_TITLE_WIDTH          79
#define MP_X                    ((GRID_RIGHT) - (MP_TITLE_WIDTH) - 1)
#define MP_TITLE_HEIGHT         34
#define MI_HEIGHT               33
#define MI_WIDTH                ((MP_TITLE_WIDTH) + 1)
#define MI_HEIGHT_VALUE         13
#define MI_WIDTH_VALUE          ((MI_WIDTH) - 4)
#define MOSI_HEIGHT             14
#define MOI_HEIGHT_TITLE        19
#define MOI_WIDTH               MP_TITLE_WIDTH
#define MOI_WIDTH_D_IP          34      /* Увеличение ширины открытого ip-адреса в случае наличия порта */
#define WIDTH_SB                19
#define LEFT_SB                 299 

typedef enum
{
    DrawMode_Auto,
    DrawMode_Hand
} DrawMode;

typedef enum
{
    SB_Signals,
    SB_Send,
    SB_Intermediate
} SourceBuffer;

typedef enum
{
    TypeFont_5,
    TypeFont_8,
    TypeFont_UGO,
    TypeFont_UGO2,
    TypeFont_Number,
    TypeFont_None
} TypeFont;


typedef enum
{
    EMPTY_FUCTION,
    SET_BRIGHTNESS,         // 1
    SET_ORIENTATION,        // 2 Установить ориентацию экрана. 0 - нормальная, 1 - обратная
    SET_PALETTE_COLOR,      // 3 numColor:1 colorValue:2
    SET_COLOR,              // 4 numColor:1
    SET_FONT,               // 5 numFont:1
    DRAW_TEXT,              // 6
    DRAW_PIXEL,             // 7 x:2 y:1
    DRAW_LINE,              // 8 Нарисовать линию
    FILL_REGION,            // 9 x:2 y:1 width:2 height:1
    DRAW_MULTI_HPOINT_LINE, // 10 numLines:1 x:2 numPoints:1 dx:1 lines:numLines
    DRAW_MULTI_VPOINT_LINE, // 11 numLines:1 y:1 numPoints:1 dy:1 empty:1 lines:numLines*2
    DRAW_SIGNAL_LINES,      // 12 x:2 points:281
    DRAW_SIGNAL_POINTS,     // 13 x:2 points:281
    DRAW_VLINES_ARRAY,      // 14 x0:2 numLines:1 lines:numLines * 2
    INVALIDATE,             // 15
    RUN_BUFFER,             // 16
    GET_PIXEL,              // 17
    LOAD_IMAGE,             // 18
    DRAW_HLINE,             // 19 y:1 x0:2 x1:2
    DRAW_VLINE,             // 20 x:2 y0:1 y1:1
    SET_REINIT,             // 21

    LOAD_FONT,              // typeFont:1 height:4 symbols:9 * 256 . Описание symbols смотри в Font.h
    NUM_COMMANDS
} Command;
