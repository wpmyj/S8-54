#pragma once


#include "defines.h"


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

int Font_GetSize(void);
int Font_GetLengthText(const char *text);
int Font_GetHeightSymbol(char symbol);
int Font_GetLengthSymbol(uchar symbol);
