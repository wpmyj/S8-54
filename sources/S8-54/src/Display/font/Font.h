#pragma once
#include "defines.h"
#include "Display/DisplayTypes.h"


typedef struct
{
    uchar width;
    uchar bytes[8];
} Symbol;


typedef struct
{
    int height;
    Symbol symbol[256];
} Font;


extern const Font *font;
extern const Font *fonts[TypeFont_Number];

extern const uchar font5display[3080];
extern const uchar font8display[3080];
extern const uchar fontUGOdisplay[3080];
extern const uchar fontUGO2display[3080];


int Font_GetSize(void);
int Font_GetLengthText(const char *text);
int Font_GetHeightSymbol(char symbol);
int Font_GetLengthSymbol(uchar symbol);
