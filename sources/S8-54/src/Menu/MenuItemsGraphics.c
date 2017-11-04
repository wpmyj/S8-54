#include "defines.h"
#include "Menu/MenuItems.h"
#include "Menu/MenuFunctions.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Menu/MenuDrawing.h"
#include "Hardware/RTC.h"
#include <string.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawGovernorChoiceColorFormulaHiPart(void *item, int x, int y, bool pressed, bool shade, bool opened);
static void GovernorIpCommon_DrawOpened(void *item, int x, int y, int dWidth);
static void DrawValueWithSelectedPosition(int x, int y, int value, int numDigits, int selPos, bool hLine, bool fillNull);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GovernorColor::Draw(int x, int y, bool opened)
{
    if (opened)
    {
        DrawOpened(x, y);
    }
    else
    {
        DrawClosed(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void GovernorColor::DrawOpened(int x, int y)
{
    static const int delta = 43;
    x -= delta;
    ct->Init(false);
    painter.DrawRectangle(x - 1, y - 1, MI_WIDTH + delta + 2, MI_HEIGHT + 2, Color::BLACK);
    painter.DrawRectangle(x, y, MI_WIDTH + delta, MI_HEIGHT, Color::MenuTitle(false));
    painter.DrawVolumeButton(x + 1, y + 1, MI_WIDTH_VALUE + 2 + delta, MI_HEIGHT_VALUE + 3, 2, Color::MenuItem(false),
                             Color::MENU_ITEM_BRIGHT, Color::MENU_ITEM_DARK, IsPressed(this), IsShade(this));
    painter.DrawHLine(y + MI_HEIGHT / 2 + 2, x, x + MI_WIDTH + delta, Color::MenuTitle(false));
    painter.DrawStringInCenterRectC(x + (IsPressed(this) ? 2 : 1), y + (IsPressed(this) ? 2 : 1), MI_WIDTH + delta, MI_HEIGHT / 2 + 2, TitleItem(this),
                                    Color::WHITE);
    DrawValue(x + 1, y + 19, delta);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void GovernorColor::DrawClosed(int x, int y)
{
    ct->Init(false);
    DrawGovernorChoiceColorFormulaHiPart(this, x, y, IsPressed(this), IsShade(this) || !ItemIsAcitve(this), true);
    painter.FillRegion(x + 2, y + 20, MI_WIDTH_VALUE, MI_HEIGHT_VALUE - 1, ct->color);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void GovernorColor::DrawValue(int x, int y, int delta)
{
    char buffer[20];

    int8 field = ct->currentField;
    char *texts[4] = {"Яр", "Сн", "Зл", "Кр"};

    uint16 color = COLOR(ct->color.value);
    int red = R_FROM_COLOR(color);
    int green = G_FROM_COLOR(color);
    int blue = B_FROM_COLOR(color);
    ct->Init(false);
    int16 vals[4] = {(int16)(ct->brightness * 100.0f), (int16)blue, (int16)green, (int16)red};

    painter.FillRegion(x, y, MI_WIDTH + delta - 2, MI_HEIGHT / 2 - 3, Color::BLACK);
    x += 92;

    for (int i = 0; i < 4; i++)
    {
        Color colorBack = (field == i) ? Color::WHITE : Color::BLACK;
        Color colorDraw = (field == i) ? Color::BLACK : Color::WHITE;
        painter.FillRegion(x - 1, y + 1, 29, 10, colorBack);
        painter.DrawText(x, y + 2, texts[i], colorDraw);
        painter.DrawText(x + 14, y + 2, trans.Int2String(vals[i], false, 1, buffer));
        x -= 30;
    }

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::Draw(int x, int y, bool opened)
{
    if (funcBeforeDraw)
    {
        funcBeforeDraw();
    }
    if (opened)
    {
        DrawOpened(x, y);
    }
    else
    {
        DrawClosed(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::DrawOpened(int x, int y)
{
    GovernorIpCommon_DrawOpened(this, x, y, 0);
    DrawValue(x, y + 22);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::DrawClosed(int x, int y)
{
    bool pressed = IsPressed(this);
    bool shade = IsShade(this) || !ItemIsAcitve(this);
    DrawLowPart(x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(this, x, y, pressed, shade, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::DrawValue(int x, int y)
{
    char buffer[20];

    int startX = x + 40;
    int16 value = *cell;
    int signGovernor = *cell < 0 ? -1 : 1;
    if (signGovernor == -1)
    {
        value = -value;
    }
    painter.SetFont(TypeFont_5);
    bool sign = minValue < 0;
    painter.DrawText(x + 55, y - 5, trans.Int2String(maxValue, sign, 1, buffer), Color::WHITE);
    painter.DrawText(x + 55, y + 2, trans.Int2String(minValue, sign, 1, buffer));
    painter.SetFont(TypeFont_8);

    DrawValueWithSelectedPosition(startX, y, value, NumDigits(), gCurDigit, true, true);

    if (sign)
    {
        painter.DrawChar(startX - 30, y, signGovernor < 0 ? '\x9b' : '\x9a');
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::DrawLowPart(int x, int y, bool pressed, bool shade)
{
    char buffer[20];

    Color colorTextDown = Color::BLACK;

    painter.DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, Color::MENU_FIELD,
                             Color::MENU_ITEM_BRIGHT, Color::MENU_ITEM_DARK, true, shade);
    if (shade)
    {
        colorTextDown = Color::MenuItem(false);
    }

    x = painter.DrawText(x + 4, y + 21, "\x80", colorTextDown);
    if (OpenedItem() != this)
    {
        int delta = (int)Step();
        if (delta == 0)
        {
            x = painter.DrawText(x + 1, y + 21, trans.Int2String(*cell, false, 1, buffer));
        }
        else
        {
            int drawX = x + 1;
            int limX = x + 1;
            int limY = y + 19;
            int limWidth = MI_WIDTH_VALUE;
            int limHeight = MI_HEIGHT_VALUE - 1;
            if (delta > 0)
            {
                x = painter.DrawTextWithLimitationC(drawX, y + 21 - delta, trans.Int2String(*cell, false, 1, buffer),
                                                    Color::BLACK, limX, limY, limWidth, limHeight);
                painter.DrawTextWithLimitationC(drawX, y + 21 + 10 - delta, trans.Int2String(NextValue(), false, 1, buffer),
                                                Color::BLACK, limX, limY, limWidth, limHeight);
            }
            if (delta < 0)
            {
                x = painter.DrawTextWithLimitationC(drawX, y + 21 - delta, trans.Int2String(*cell, false, 1, buffer),
                                                    Color::BLACK, limX, limY, limWidth, limHeight);
                painter.DrawTextWithLimitationC(drawX, y + 21 - 10 - delta, trans.Int2String(PrevValue(), false, 1, buffer),
                                                Color::BLACK, limX, limY, limWidth, limHeight);
            }
        }
    }
    else
    {
        x = painter.DrawText(x + 1, y + 21, trans.Int2String(*cell, false, 1, buffer), Color::WHITE);
    }
    painter.DrawText(x + 1, y + 21, "\x81", colorTextDown);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void IPaddress::Draw(int x, int y, bool opened)
{
    if (opened)
    {
        DrawOpened(x - (port == 0 ? 0 : MOI_WIDTH_D_IP), y);
    }
    else
    {
        DrawClosed(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void IPaddress::DrawOpened(int x, int y)
{
    GovernorIpCommon_DrawOpened(this, x, y, port == 0 ? 0 : MOI_WIDTH_D_IP);
    DrawValue(x, y + 22);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void IPaddress::DrawClosed(int x, int y)
{
    bool pressed = IsPressed(this);
    bool shade = IsShade(this) || !ItemIsAcitve(this);
    DrawLowPart(x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(this, x, y, pressed, shade, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void IPaddress::DrawValue(int x, int y)
{
    if (gCurDigit > (port == 0 ? 11 : 16))
    {
        gCurDigit = 0;
    }

    uint8 *bytes = ip0;

    x += 15;

    y += 1;

    int numIP = 0;
    int selPos = 0;

    GetNumPosIPvalue(&numIP, &selPos);

    for (int i = 0; i < 4; i++)
    {
        DrawValueWithSelectedPosition(x, y, bytes[i], 3, numIP == i ? selPos : -1, false, true);
        if (i != 3)
        {
            painter.DrawChar(x + 5, y, '.', Color::WHITE);
        }
        x += 19;
    }

    if (port != 0)
    {
        painter.DrawChar(x - 13, y, ':', Color::WHITE);
        DrawValueWithSelectedPosition(x + 14, y, *port, 5, numIP == 4 ? selPos : -1, false, true);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void IPaddress::DrawLowPart(int x, int y, bool pressed, bool shade)
{
    const int SIZE = 20;
    char buffer[SIZE];

    Color colorTextDown = Color::BLACK;

    painter.DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, Color::MENU_FIELD,
                             Color::MENU_ITEM_BRIGHT, Color::MENU_ITEM_DARK, true, shade);
    if (shade)
    {
        colorTextDown = Color::MenuItem(false);
    }

    snprintf(buffer, SIZE, "%03d.%03d.%03d.%03d", *ip0, *ip1, *ip2, *ip3);

    if (OpenedItem() != this)
    {
        painter.DrawText(x + 4, y + 21, buffer, colorTextDown);
    }
    else
    {
        painter.DrawText(x + 4, y + 21, buffer, Color::WHITE);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void MACaddress::Draw(int x, int y, bool opened)
{
    if (opened)
    {
        DrawOpened(x, y);
    }
    else
    {
        DrawClosed(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void MACaddress::DrawOpened(int x, int y)
{
    GovernorIpCommon_DrawOpened(this, x, y, 0);
    DrawValue(x, y + 22);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void MACaddress::DrawClosed(int x, int y)
{
    bool pressed = IsPressed(this);
    bool shade = IsShade(this) || !ItemIsAcitve(this);
    DrawLowPart(x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(this, x, y, pressed, shade, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void MACaddress::DrawValue(int x, int y)
{
    if (gCurDigit > 5)
    {
        gCurDigit = 0;
    }
    uint8 *bytes = mac0;
    x += MOI_WIDTH - 14;
    y++;
    for (int num = 5; num >= 0; num--)
    {
        int value = (int)(*(bytes + num));
        if (gCurDigit == num)
        {
            painter.FillRegion(x - 1, y, 10, 8, Color::WHITE);
        }
        const int SIZE = 20;
        char buffer[SIZE];
        snprintf(buffer, SIZE, "%02X", value);
        painter.DrawText(x, y, buffer, gCurDigit == num ? Color::BLACK : Color::WHITE);
        x -= 12;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void MACaddress::DrawLowPart(int x, int y, bool pressed, bool shade)
{
    const int SIZE = 20;
    char buffer[SIZE];

    Color colorTextDown = Color::BLACK;

    painter.DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, Color::MENU_FIELD,
                             Color::MENU_ITEM_BRIGHT, Color::MENU_ITEM_DARK, true, shade);
    if (shade)
    {
        colorTextDown = Color::MenuItem(false);
    }

    snprintf(buffer, SIZE, "%02X.%02X.%02X.%02X.%02X.%02X", *mac0, *mac1, *mac2, *mac3, *mac4, *mac5);

    if (OpenedItem() != this)
    {

        painter.DrawText(x + 4, y + 21, buffer, colorTextDown);
    }
    else
    {
        painter.DrawText(x + 4, y + 21, buffer, Color::WHITE);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Formula::Draw(int x, int y, bool opened)
{
    if (opened)
    {

    }
    else
    {
        DrawClosed(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Formula::DrawClosed(int x, int y)
{
    bool pressed = IsPressed(this);
    bool shade = IsShade(this) || !ItemIsAcitve(this);
    DrawLowPart(x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(this, x, y, pressed, shade, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Formula::DrawLowPart(int x, int y, bool pressed, bool shade)
{
    Color colorTextDown = Color::BLACK;

    painter.DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, Color::MENU_FIELD,
                             Color::MENU_ITEM_BRIGHT, Color::MENU_ITEM_DARK, true, shade);
    if (shade)
    {
        colorTextDown = Color::MenuItem(false);
    }
    painter.SetColor(colorTextDown);
    WriteText(x + 6, y + 21, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Formula::WriteText(int x, int y, bool opened)
{
    Function function = (Function)(function);

    if (function != Function_Mul && function != Function_Sum)
    {
        return;
    }

    bool funcIsMul = function == Function_Mul;
    int8 koeff1 = funcIsMul ? *koeff1mul : *koeff1add;
    int8 koeff2 = funcIsMul ? *koeff2mul : *koeff2add;
    if (koeff1 != 0)
    {
        painter.DrawChar(x, y, koeff1 < 0 ? '-' : '+');
    }
    painter.DrawChar(x + 5, y, (char)(koeff1 + 0x30));
    painter.DrawChar(x + 10, y, '*');
    painter.DrawText(x + 14, y, "K1");
    painter.DrawChar(x + 27, y, funcIsMul ? '*' : '+');
    if (koeff2 != 0)
    {
        painter.DrawChar(x + 30, y, koeff2 < 0 ? '-' : '+');
    }
    painter.DrawChar(x + 39, y, (char)(koeff2 + 0x30));
    painter.DrawChar(x + 44, y, '*');
    painter.DrawText(x + 48, y, "K2");
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Choice::Draw(int x, int y, bool opened)
{
    if (opened)
    {
        DrawOpened(x, y);
    }
    else
    {
        DrawClosed(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Choice::DrawOpened(int x, int y)
{
    int height = HeightOpenedItem(this);

    painter.DrawRectangle(x - 2, y - 1, MP_TITLE_WIDTH + 3, height + 3, gColorBack);

    DrawGovernorChoiceColorFormulaHiPart(this, x - 1, y - 1, IsPressed(this), false, true);
    painter.DrawRectangle(x - 1, y, MP_TITLE_WIDTH + 1, height + 1, Color::MenuTitle(false));

    painter.DrawHLine(y + MOI_HEIGHT_TITLE - 1, x, x + MOI_WIDTH);
    painter.DrawVolumeButton(x, y + MOI_HEIGHT_TITLE, MOI_WIDTH - 1, height - MOI_HEIGHT_TITLE, 1, Color::BLACK, Color::MENU_TITLE_BRIGHT,
                             Color::MENU_TITLE_DARK, false, IsShade(this));
    int8 index = *cell;
    for (int i = 0; i < NumSubItems(); i++)
    {
        int yItem = y + MOI_HEIGHT_TITLE + i * MOSI_HEIGHT + 1;
        bool pressed = i == index;
        if (pressed)
        {
            painter.DrawVolumeButton(x + 1, yItem, MOI_WIDTH - 2, MOSI_HEIGHT - 2, 2, Color::MENU_FIELD, Color::MENU_TITLE_BRIGHT,
                                     Color::MENU_TITLE_DARK, pressed, IsShade(this));
        }
        painter.DrawText(x + 4, yItem + 2, NameSubItem(this, i), pressed ? Color::BLACK : Color::MENU_FIELD);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Choice::DrawClosed(int x, int y)
{
    bool pressed = IsPressed(this);
    bool shade = IsShade(this) || !ItemIsAcitve(this);

    painter.DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 1, Color::MENU_FIELD, Color::MENU_ITEM_BRIGHT,
                             Color::MENU_ITEM_DARK, true, shade);

    int deltaY = (int)Step();
    Color colorText = shade ? Color::MenuItem(true) : Color::BLACK;
    painter.SetColor(colorText);
    if (deltaY == 0)
    {
        painter.DrawText(x + 4, y + 21, NameCurrentSubItem());
    }
    else
    {
        painter.DrawTextWithLimitationC(x + 4, y + 21 - deltaY, NameCurrentSubItem(), colorText, x, y + 19, MI_WIDTH_VALUE, MI_HEIGHT_VALUE - 1);
        painter.DrawHLine(y + (deltaY > 0 ? 31 : 19) - deltaY, x + 3, x + MI_WIDTH_VALUE + 1, Color::BLACK);
        painter.DrawTextWithLimitationC(x + 4, y + (deltaY > 0 ? 33 : 9) - deltaY, deltaY > 0 ? NameNextSubItem() : NamePrevSubItem(),
                                        colorText, x, y + 19, MI_WIDTH_VALUE, MI_HEIGHT_VALUE - 1);
    }
    painter.DrawHLine(y + MI_HEIGHT + 1, x, x + MI_WIDTH, Color::BorderMenu(false));

    if (funcForDraw)
    {
        funcForDraw(x, y);
    }
    DrawGovernorChoiceColorFormulaHiPart(this, x, y, pressed, shade, false);
}













































































































//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawGovernorChoiceColorFormulaHiPart(void *item, int x, int y, bool pressed, bool shade, bool opened)
{
    int delta = pressed && !shade ? 1 : 0;
    int width = MI_WIDTH_VALUE;

    if (TypeMenuItem(item) == Item_IP && opened && ((IPaddress*)item)->port != 0)
    {
        width += MOI_WIDTH_D_IP;
    }

    Color color = shade ? Color::MENU_TITLE_DARK : (COLOR_SCHEME_IS_WHITE_LETTERS ? Color::WHITE :Color::BLACK);
    painter.DrawHLine(y + 1, x, x + width + 3, Color::BorderMenu(false));

    painter.DrawVolumeButton(x + 1, y + 2, width + 2, MI_HEIGHT_VALUE + 3, 1, Color::MenuItem(false), Color::MENU_ITEM_BRIGHT, Color::MENU_ITEM_DARK, 
        pressed, shade);

    painter.DrawText(x + 6 + delta, y + 6 + delta, TitleItem(item), color);
    
    TypeItem type = TypeMenuItem(item);

    if(CurrentItem() == item)
    {
        char symbol = 0;

        if (type == Item_Governor)
        {
            symbol = GetSymbolForGovernor(*((Governor*)item)->cell);
        }
        else if (type == Item_Governor || type == Item_ChoiceReg ||  (ItemIsOpened(item) && type == Item_Choice))
        {
            symbol = GetSymbolForGovernor(*((Choice*)item)->cell);
        }
        else if (type == Item_Time)
        {
            Time *time = (Time*)item;
            if ((OpenedItem() == item) && (*time->curField != iEXIT) && (*time->curField != iSET))
            {
                int8 values[7] =
                {
                    0,
                    *time->day,
                    *time->month,
                    *time->year,
                    *time->hours,
                    *time->minutes,
                    *time->seconds
                };
                symbol = GetSymbolForGovernor(values[*time->curField]);
            }
        }

        painter.Draw4SymbolsInRect(x + MI_WIDTH - 13, y + 5 + (ItemIsOpened(item) ? 0 : 15), symbol, shade ? color : Color::BLACK);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// Если selPos == -1, подсвечивать не нужно
static void DrawValueWithSelectedPosition(int x, int y, int value, int numDigits, int selPos, bool hLine, bool fillNull)
{
    int firstValue = value;
    int height = hLine ? 9 : 8;
    for (int i = 0; i < numDigits; i++)
    {
        int rest = value % 10;
        value /= 10;
        if (selPos == i)
        {
            painter.FillRegion(x - 1, y, 5, height, gColorFill);
        }
        if (!(rest == 0 && value == 0) || (firstValue == 0 && i == 0))
        {
            painter.DrawChar(x, y, (char)(rest + 48), selPos == i ? Color::BLACK : Color::WHITE);
        }
        else if (fillNull)
        {
            painter.DrawChar(x, y, '0', selPos == i ? Color::BLACK : Color::WHITE);
        }
        if (hLine)
        {
            painter.DrawLine(x, y + 9, x + 3, y + 9, Color::WHITE);
        }
        x -= 6;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Time_DrawOpened(Time *time, int x, int y)
{
    char buffer[20];
    
    int width = MI_WIDTH_VALUE + 3;
    int height = 61;
    painter.DrawRectangle(x - 1, y - 1, width + 2, height + 3, gColorBack);
    DrawGovernorChoiceColorFormulaHiPart(time, x - 1, y - 1, IsPressed(time), false, true);

    painter.DrawRectangle(x - 1, y, width + 1, height + 1, Color::MenuTitle(false));

    painter.DrawHLine(y + MOI_HEIGHT_TITLE - 1, x, x + MOI_WIDTH);
    painter.DrawVolumeButton(x, y + MOI_HEIGHT_TITLE, MOI_WIDTH - 1, height - MOI_HEIGHT_TITLE, 1, Color::BLACK, Color::MENU_TITLE_BRIGHT,
                             Color::MENU_TITLE_DARK, false, IsShade(time));

    int y0 = 21;
    int y1 = 31;

    typedef struct 
    {
        int x;
        int y;
        int width;
    } StructPaint;
    
    int y2 = 41;
    int y3 = 51;
    int dX = 13;
    int wS = 10;
    int x0 = 41;
    StructPaint strPaint[8] =
    {
        {3,             y3, 60},    // Не сохранять
        {x0,            y0, wS},    // день
        {x0 + dX,       y0, wS},    // месяц
        {x0 + 2 * dX,   y0, wS},    // год
        {x0,            y1, wS},    // часы
        {x0 + dX,       y1, wS},    // мин
        {x0 + 2 * dX,   y1, wS},    // сек
        {3,             y2, 46}     // Сохранить
    };

    char strI[8][20];
    strcpy(strI[iEXIT],     "Не сохранять");
    strcpy(strI[iDAY],      trans.Int2String(*time->day,      false, 2, buffer));
    strcpy(strI[iMONTH],    trans.Int2String(*time->month,    false, 2, buffer));
    strcpy(strI[iYEAR],     trans.Int2String(*time->year,     false, 2, buffer));
    strcpy(strI[iHOURS],    trans.Int2String(*time->hours,    false, 2, buffer));
    strcpy(strI[iMIN],      trans.Int2String(*time->minutes,  false, 2, buffer));
    strcpy(strI[iSEC],      trans.Int2String(*time->seconds,  false, 2, buffer));
    strcpy(strI[iSET],      "Сохранить");

    painter.DrawText(x + 3, y + y0, "д м г - ", Color::WHITE);
    painter.DrawText(x + 3, y + y1, "ч м с - ");

    for (int i = 0; i < 8; i++)
    {
        if (*time->curField == i)
        {
            painter.FillRegion(x + strPaint[i].x - 1, y + strPaint[i].y, strPaint[i].width, 8, Color::FLASH_10);
        }
        painter.DrawText(x + strPaint[i].x, y + strPaint[i].y, strI[i], *time->curField == i ? Color::FLASH_01 : Color::WHITE);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void GovernorIpCommon_DrawOpened(void *item, int x, int y, int dWidth)
{
    int height = 34;
    painter.DrawRectangle(x - 2, y - 1, MP_TITLE_WIDTH + 3 + dWidth, height + 3, gColorBack);
    painter.DrawRectangle(x - 1, y, MP_TITLE_WIDTH + 1 + dWidth, height + 1, Color::MenuTitle(false));
    painter.DrawHLine(y + MOI_HEIGHT_TITLE - 1, x, x + MOI_WIDTH + dWidth);
    DrawGovernorChoiceColorFormulaHiPart(item, x - 1, y - 1, IsPressed(item), false, true);
    painter.DrawVolumeButton(x, y + MOI_HEIGHT_TITLE, MOI_WIDTH - 1 + dWidth, height - MOI_HEIGHT_TITLE, 1, Color::BLACK, Color::MENU_TITLE_BRIGHT,
                             Color::MENU_TITLE_DARK, false, IsShade(item));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Time_DrawClosed(Time *item, int x, int y)
{
    char buffer[20];
    
    bool pressed = IsPressed(item);
    bool shade = IsShade(item);
    DrawGovernorChoiceColorFormulaHiPart(item, x, y, pressed, shade, false);

    painter.DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 1, Color::MENU_FIELD, Color::MENU_ITEM_BRIGHT, 
        Color::MENU_ITEM_DARK, true, shade);

    int deltaField = 10;
    int deltaSeparator = 2;
    int startX = 3;
    y += 21;
    PackedTime time = RTC_GetPackedTime();
    painter.DrawText(x + startX, y, trans.Int2String(time.hours, false, 2, buffer), shade ? Color::MenuItem(true) : Color::BLACK);
    painter.DrawText(x + startX + deltaField, y, ":");
    painter.DrawText(x + startX + deltaField + deltaSeparator, y, trans.Int2String(time.minutes, false, 2, buffer));
    painter.DrawText(x + startX + 2 * deltaField + deltaSeparator, y, ":");
    painter.DrawText(x + startX + 2 * deltaField + 2 * deltaSeparator, y, trans.Int2String(time.seconds, false, 2, buffer));

    startX = 44;
    painter.DrawText(x + startX, y, trans.Int2String(time.day, false, 2, buffer));
    painter.DrawText(x + startX + deltaField, y, ":");
    painter.DrawText(x + startX + deltaField + deltaSeparator, y, trans.Int2String(time.month, false, 2, buffer));
    painter.DrawText(x + startX + 2 * deltaField + deltaSeparator, y, ":");
    painter.DrawText(x + startX + 2 * deltaField + 2 * deltaSeparator, y, trans.Int2String(time.year, false, 2, buffer));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Time_Draw(Time *time, int x, int y, bool opened)
{
    if(opened)
    {
        Time_DrawOpened(time, x, y);
    }
    else
    {
        Time_DrawClosed(time, x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Button_Draw(Button *button, int x, int y)
{
    bool pressed = IsPressed(button);
    bool shade = IsShade(button) || !ItemIsAcitve(button);

    painter.DrawHLine(y + 1, x, x + MI_WIDTH, Color::MenuTitle(shade));
    Color color = shade ? Color::MenuItem(true) : Color::WHITE;
    painter.FillRegion(x + 1, y + 2, MI_WIDTH - 2, MI_HEIGHT - 2, Color::MenuItem(false));
    painter.DrawVolumeButton(x + 3, y + 4, MI_WIDTH - 6, MI_HEIGHT - 6, 2, Color::MenuItem(false), Color::MENU_ITEM_BRIGHT, 
                            Color::MENU_ITEM_DARK, pressed, shade);

    int delta = (pressed && (!shade)) ? 2 : 1;
    
    painter.DrawStringInCenterRectC(x + delta, y + delta, MI_WIDTH, MI_HEIGHT, TitleItem(button), color);

    button->CallFuncOnDraw(x, y);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void SmallButton_Draw(SButton *smallButton, int x, int y)
{
    if (ItemIsAcitve(smallButton))
    {
        if (IsPressed(smallButton))
        {
            painter.FillRegion(x, y, WIDTH_SB, WIDTH_SB, gColorFill);
            painter.SetColor(Color::BLACK);
        }
        else
        {
            painter.DrawRectangle(x, y, WIDTH_SB, WIDTH_SB, gColorFill);
        }
        if (smallButton->funcOnDraw)
        {
            smallButton->funcOnDraw(x, y);
        }
    }
    else
    {
        painter.DrawRectangle(x, y, WIDTH_SB, WIDTH_SB, gColorFill);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Page_Draw(Page *page, int x, int y)
{
    bool isShade = IsShade(page) || !ItemIsAcitve(page);
    bool isPressed = IsPressed(page);
    painter.DrawHLine(y + 1, x, x + MI_WIDTH, Color::BorderMenu(false));

    painter.DrawVolumeButton(x + 1, y + 2, MI_WIDTH - 2, MI_HEIGHT - 2, 1, Color::MenuItem(false), Color::MENU_ITEM_BRIGHT, Color::MENU_ITEM_DARK, 
        isPressed, isShade);

    Color colorText = isShade ? Color::MenuItem(true) : Color::BLACK;
    int delta = 0;
    if(isPressed && (!isShade))
    {
        colorText = Color::WHITE;
        delta = 1;
    }
    painter.DrawStringInCenterRectC(x + delta, y + delta, MI_WIDTH, MI_HEIGHT, TitleItem(page), colorText);
}
