#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Painter
 *  @{
 *  @defgroup PainterC
 *  @brief Макросы для добавления цвета в функции рисования
 *  @{
 */

/*
#define painter_SetPointC(x, y, color)                                      \
    painter.SetColor(color);                                                \
    painter.SetPoint(x, y);
    
#define painter_DrawDashedHLineC(y, x0, x1, dFill, dEmpty, dStart, color);  \
    painter.SetColor(color);                                                \
    painter.DrawDashedHLine(y, x0, x1, dFill, dEmpty, dStart);
    
#define painter_DrawDashedVLineC(x, y0, y1, dFill, dEmpty, dStart, color);  \
    painter.SetColor(color);                                                \
    painter.DrawDashedVLine(x, y0, y1, dFill, dEmpty, dStart);
    
#define painter_DrawTextInRectC(x, y, width, text, color)                   \
    painter.SetColor(color);                                                \
    painter.DrawTextInRect(x, y, width, text);
*/

#define painter_DrawVLineC(x, y0, y1, color)                                \
    painter.SetColor(color);                                                \
    painter.DrawVLine(x, y0, y1);

#define painter_DrawHLineC(y, x0, x1, color)                                \
    painter.SetColor(color);                                                \
    painter.DrawHLine(y, x0, x1);

#define painter_FillRegionC(x, y, width, height, color)                     \
    painter.SetColor(color);                                                \
    painter.FillRegion(x, y, width, height);

#define painter_DrawRectangleC(x, y, width, height, color)                  \
    painter.SetColor(color);                                                \
    painter.DrawRectangle(x, y, width, height);

#define painter_DrawLineC(x0, y0, x1, y1, color)                            \
    painter.SetColor(color);                                                \
    painter.DrawLine(x0, y0, x1, y1);

#define painter_Draw4SymbolsInRectC(x, y, eChar, color)                     \
    painter.SetColor(color);                                                \
    painter.Draw4SymbolsInRect(x, y, eChar);

#define painter_DrawTextRelativelyRightC(x, y, text, color)                 \
    painter.SetColor(color);                                                \
    painter.DrawTextRelativelyRight(x, y, text);

/** @}  @}
 */
