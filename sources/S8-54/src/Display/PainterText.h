#pragma once
#include "DisplayTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Painter
 *  @{
 *  @defgroup PainterText
 *  @brief Функции отрисовки текста
 *  @{
 */

void Painter_SetFont(TypeFont typeFont);
///  Загрузить шрифта в дисплей
void Painter_LoadFont(TypeFont typeFont);

int Painter_DrawChar(int x, int y, char symbol);

int Painter_DrawCharC(int x, int y, char symbol, Color color);

int Painter_DrawText(int x, int y, const char *text);
/// Выводит текст на прямоугольнике цвета colorBackgound
int Painter_DrawTextOnBackground(int x, int y, const char *text, Color colorBackground);

int Painter_DrawFormatText(int x, int y, char *format, ...);
/// Пишет строку в позиции x, y
int Painter_DrawFormText(int x, int y, Color color, char *text, ...);

int Painter_DrawTextC(int x, int y, const char *text, Color color);

int Painter_DrawTextWithLimitationC(int x, int y, const char* text, Color color, int limitX, int limitY, int limitWidth, int limitHeight);
/// Возвращает нижнюю координату прямоугольника
int Painter_DrawTextInBoundedRectWithTransfers(int x, int y, int width, const char *text, Color colorBackground, Color colorFill);

int Painter_DrawTextInRectWithTransfersC(int x, int y, int width, int height, const char *text, Color color);

int Painter_DrawStringInCenterRect(int x, int y, int width, int height, const char *text);

int Painter_DrawStringInCenterRectC(int x, int y, int width, int height, const char *text, Color color);
/// Пишет строку текста в центре области(x, y, width, height)цветом ColorText на прямоугольнике с шириной бордюра widthBorder цвета colorBackground
void Painter_DrawStringInCenterRectOnBackgroundC(int x, int y, int width, int height, const char *text, Color colorText, int widthBorder, Color colorBackground);

int Painter_DrawStringInCenterRectAndBoundItC(int x, int y, int width, int height, const char *text, Color colorBackground, Color colorFill);

void Painter_DrawHintsForSmallButton(int x, int y, int width, void *smallButton);

void Painter_DrawTextInRect(int x, int y, int width, char *text);

void Painter_DrawTextRelativelyRight(int xRight, int y, const char *text);

void Painter_Draw2SymbolsC(int x, int y, char symbol1, char symbol2, Color color1, Color color2);

void Painter_Draw4SymbolsInRect(int x, int y, char eChar);

void Painter_Draw10SymbolsInRect(int x, int y, char eChar);
/// Пишет текст с переносами
int Painter_DrawTextInRectWithTransfers(int x, int y, int width, int height, const char *text);

void Painter_DrawBigText(int x, int y, int size, const char *text);

/** @} @}
 */
