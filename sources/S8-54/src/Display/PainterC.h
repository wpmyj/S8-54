#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Painter
 *  @{
 *  @defgroup PainterC
 *  @brief Макросы для добавления цвета в функции рисования
 *  @{
 */

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
