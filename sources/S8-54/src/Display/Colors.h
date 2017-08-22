#pragma once
#include "Settings/SettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Painter
 *  @{
 *  @defgroup Colors
 *  @brief Параметры цветов
 *  @{
 */

Color ColorCursors(Channel cnan);
Color ColorTrig(void);                  ///< Возвращает цвет синхронизации.
Color ColorMenuTitle(bool inShade);     ///< Цвет заголовка страницы. inShade == true, если страница затенена
Color ColorMenuItem(bool inShade);      ///< Цвет элемента меню.
Color ColorBorderMenu(bool inShade);    ///< Цвет окантовки меню.
Color ColorContrast(Color color);       ///< Возвращает цвет, контрастный к color. Может быть белым или чёрным.
Color LightShadingTextColor(void);      ///< Светлый цвет в тени.


/// Чуть менее светлый цвет, чем цвет элемента меню. Используется для создания эффекта объёма.
#define ColorMenuItemLessBright()   COLOR_MENU_ITEM_DARK
/// Чуть более светлый цвет, чем цвет заголовка страницы. Используется для создания эффекта объёма.
#define ColorMenuTitleBrighter()    COLOR_MENU_TITLE_BRIGHT
/// Чуть менее светлый цвет, чем цвет заголовка страницы. Используется для создания эффекта объёма.
#define ColorMenuTitleLessBright()  COLOR_MENU_TITLE_DARK
/// Чуть более светлый цвет, чем цвет элемента меню. Используется для создания эффекта объёма.
#define ColorMenuItemBrighter()     COLOR_MENU_ITEM_BRIGHT
/// Тёмный цвет в тени.
#define DarkShadingTextColor()      COLOR_MENU_TITLE_DARK

typedef struct
{
    float   red;
    float   green;
    float   blue;
    float   stepRed;
    float   stepGreen;
    float   stepBlue;
    float   brightness;
    Color   color;
    int8    currentField;
    bool    alreadyUsed;
} ColorType;

void Color_Log(Color color);           // Вывести в лог значение цвета.
void Color_BrightnessChange(ColorType *colorType, int delta);
void Color_SetBrightness(ColorType *colorType, float brightness);
void Color_Init(ColorType *colorType, bool forced);                 // Если forced == false, то инициализация происходит только при первом вызове функции
void Color_ComponentChange(ColorType * const colorType, int delta);

const char *NameColorFromValue(uint16 colorValue);
const char *NameColor(Color color);
#define MAKE_COLOR(r, g, b) (((b) & 0x1f) + (((g) & 0x3f) << 5) + (((r) & 0x1f) << 11))
#define R_FROM_COLOR(color) (((uint16)(color) >> 11) & (uint16)0x1f)
#define G_FROM_COLOR(color) (((uint16)(color) >> 5) & (uint16)0x3f)
#define B_FROM_COLOR(color) ((uint16)(color) & 0x1f)

extern Color gColorFill;
extern Color gColorBack;
extern Color gColorGrid;
extern Color gColorChan[4];

/** @}
 */
