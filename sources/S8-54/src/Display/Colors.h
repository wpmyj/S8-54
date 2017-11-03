#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Color
{
public:
    static const uint8 BLACK                = 0;
    static const uint8 WHITE                = 1;
    static const uint8 GRID                 = 2;
    static const uint8 DATA_A               = 3;
    static const uint8 DATA_B               = 4;
    static const uint8 MENU_FIELD           = 5;
    static const uint8 MENU_TITLE           = 6;
    static const uint8 MENU_TITLE_DARK      = 7;
    static const uint8 MENU_TITLE_BRIGHT    = 8;
    static const uint8 MENU_ITEM            = 9;
    static const uint8 MENU_ITEM_DARK       = 10;
    static const uint8 MENU_ITEM_BRIGHT     = 11;
    static const uint8 DATA_WHITE_ACCUM_A   = 12;   ///< Используется как для отрисовки канала на белом фоне, так и для отрисовки накопленных сигналов
    static const uint8 DATA_WHITE_ACCUM_B   = 13;
    static const uint8 GRID_WHITE           = 14;
    static const uint8 EMPTY                = 15;
    static const uint8 NUMBER               = 16;
    static const uint8 FLASH_10             = 17;
    static const uint8 FLASH_01             = 18;
    static const uint8 INVERSE              = 19;   

    Color(uint8 val) : value(val) { };

    static Color Cursors(Channel ch);
    static Color Trig();
    static Color ChanAccum(Channel ch);     ///< Цвет канала в режиме накопления
    static Color MenuItem(bool shade);      ///< Цвет элемента меню.
    static Color MenuTitle(bool shade);     ///< Цвет заголовка страницы. inShade == true, если страница затенена
    static Color BorderMenu(bool shade);    ///< Цвет окантовки меню
    static Color LightShadingText();        ///< Светлый цвет в тени.
    static Color Contrast(Color color);     ///< Возвращает цвет, контрастный к color. Может быть белым или чёрным.
    
    uint8 value;
};

bool operator!=(const Color &left, const Color &right);
bool operator==(const Color &left, const Color &right);
bool operator>(const Color &left, const Color &right);

class ColorType
{
public:
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

    /// Если forced == false, то инициализация происходит только при первом вызове функции
    void Init(bool forced);
    void SetBrightness(float brightness = -1.0f);
    void BrightnessChange(int delta);
    void ComponentChange(int delta);
private:
    void CalcSteps();
    void SetColor();
};

extern Color gColorFill;
extern Color gColorBack;
extern Color gColorGrid;
extern Color gColorChan[4];

#define MAKE_COLOR(r, g, b) (((b) & 0x1f) + (((g) & 0x3f) << 5) + (((r) & 0x1f) << 11))
#define R_FROM_COLOR(color) (((uint16)(color) >> 11) & (uint16)0x1f)
#define G_FROM_COLOR(color) (((uint16)(color) >> 5) & (uint16)0x3f)
#define B_FROM_COLOR(color) ((uint16)(color) & 0x1f)
