// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Display/DisplayTypes.h"
#include "font.h"


#include "font5display.inc"
#include "font8display.inc"
#include "fontUGOdisplay.inc"
#include "fontUGO2display.inc"


#include "font8.inc"
#include "font5.inc"
#include "fontUGO.inc"
#include "fontUGO2.inc"


const Font *fonts[TypeFont_Number] = {&font5, &font8, &fontUGO, &fontUGO2};
const Font *font = &font8;


int Font_GetSize(void)
{
    return font->height;
}

int Font_GetLengthText(const char *text)
{
    int retValue = 0;
    while (*text)
    {
        retValue += Font_GetLengthSymbol(*text);
        text++;
    }
    return retValue;
}

int Font_GetHeightSymbol(char symbol)
{
    return 9;
}

int Font_GetLengthSymbol(uchar symbol)
{
    return font->symbol[symbol].width + 1;
}
