#include "defines.h"
#include "Menu/MenuItems.h"
#include "Menu/MenuFunctions.h"
#include "Menu/MenuItemsGraphics.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Display/Grid.h"
#include "Menu/Menu.h"
#include "Menu/MenuFunctions.h"
#include "Settings/Settings.h"
#include "Globals.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawOpenedPage(Page *page, int layer, int yTop);
static void DrawTitlePage(Page *page, int layer, int yTop);
static void DrawItemsPage(Page *page, int layer, int yTop);
static void DrawPagesUGO(Page *page, int right, int bottom);
static void DrawNestingPage(Page *page, int left, int bottom);
static int CalculateX(int layer);
static void ResetItemsUnderButton(void);
static int ItemOpenedPosY(void *item);


static void *itemUnderButton[B_NumButtons] = {0};

extern Page mainPage;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PanelButton GetFuncButtonFromY(int _y)
{
    int y = GRID_TOP + GRID_HEIGHT / 12;
    int step = GRID_HEIGHT / 6;
    PanelButton button = B_Menu;
    for(int i = 0; i < 6; i++)
    {
        if(_y < y)
        {
            return button;
        }
        button = (PanelButton)((int)button + 1);    // button++;
        y += step;
    }
    return  B_F5;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawHintItem(int x, int y, int width)
{
    if (!gItemHint)
    {
        return;
    }

    const char * const names[Item_NumberItems][2] =
    {
        {"",            ""},        // Item_None
        {"",            ""},        // Item_Choice
        {"Кнопка",      "Button"},  // Item_Button
        {"Страница",    "Page"},    // Item_Page
        {"",            ""},        // Item_Governor
        {"",            ""},        // Item_Time
        {"",            ""},        // Item_IP
        {"",            ""},        // Item_GovernorColor
        {"",            ""},        // Item_Formula
        {"",            ""},        // Item_MAC
        {"",            ""},        // Item_ChoiceReg
        {"Кнопка",      "Button"}   // Item_SmallButton
    };
    TypeItem type = TypeMenuItem(gItemHint);
    Language lang = LANG;
    Page *item = (Page *)gItemHint;

    const int SIZE = 100;
    char title[SIZE];
    snprintf(title, SIZE, "%s \"%s\"", names[type][lang], item->titleHint[lang]);

    if (item->type == Item_SmallButton)
    {
        y -= 9;
    }
    painter.DrawStringInCenterRectAndBoundItC(x, y, width, 15, title, gColorBack, gColorFill);
    y = painter.DrawTextInBoundedRectWithTransfers(x, y + 15, width, item->titleHint[2 + lang], gColorBack, gColorFill);
    if (item->type == Item_SmallButton)
    {
        painter.DrawHintsForSmallButton(x, y, width, (SButton*)item);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_Draw(void)
{
    if(MENU_IS_SHOWN || TypeOpenedItem() != Item_Page)
    {
        ResetItemsUnderButton();
        void *item = OpenedItem();
        if(MENU_IS_SHOWN)
        {
            DrawOpenedPage(TypeMenuItem(item) == Item_Page ? (Page *)item : Keeper(item), 0, GRID_TOP);
        }
        else
        {
            if(TypeMenuItem(item) == Item_Choice || TypeMenuItem(item) == Item_ChoiceReg)
            {
                Choice_Draw((Choice *)item, CalculateX(0), GRID_TOP, false);
                painter.DrawVLine(CalculateX(0), GRID_TOP + 1, GRID_TOP + 34, Color::BorderMenu(false));
                painter.DrawVLine(CalculateX(0) + 1, GRID_TOP + 1, GRID_TOP + 34);
                painter.DrawVLine(GRID_RIGHT, GRID_TOP + 30, GRID_TOP + 40, gColorFill);
                painter.DrawVLine(CalculateX(0) - 1, GRID_TOP + 1, GRID_TOP + 35, gColorBack);
                painter.DrawHLine(GRID_TOP + 35, CalculateX(0) - 1, GRID_RIGHT - 1);
            }
            else if(TypeMenuItem(item) == Item_Governor)
            {
                Governor_Draw((Governor *)item, CalculateX(0), GRID_TOP, true);
                painter.DrawHLine(GRID_TOP, CalculateX(0) - 2, GRID_RIGHT, gColorFill);
                painter.DrawVLine(GRID_RIGHT, GRID_TOP, GRID_TOP + 40);
            }
        }
    }

    if (HINT_MODE_ENABLE)
    {
        int x = 1;
        int y = 0;
        int width = 318;
        if (MENU_IS_SHOWN)
        {
            width = MenuIsMinimize() ? 289 : 220;
        }
        painter.DrawTextInBoundedRectWithTransfers(x, y, width,
            LANG_RU ?    "Включён режим подсказок. В этом режиме при нажатии на кнопку на экран выводится информация о её назначении. "
                                                "Чтобы выключить этот режим, нажмите кнопку ПОМОЩЬ и удерживайте её в течение 0.5с." : 
                                                "Mode is activated hints. In this mode, pressing the button displays the information on its purpose. "
                                                "To disable this mode, press the button HELP and hold it for 0.5s.",
                                                gColorBack, gColorFill);
        y += LANG_RU ? 49 : 40;
        if (gStringForHint)
        {
            painter.DrawTextInBoundedRectWithTransfers(x, y, width, gStringForHint, gColorBack, Color::WHITE);
        }
        else if (gItemHint)
        {
            DrawHintItem(x, y, width);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawTitlePage(Page *page, int layer, int yTop)
{
    int x = CalculateX(layer);
    if (IsPageSB(page))
    {
        SmallButton_Draw(SmallButonFromPage(page, 0), LEFT_SB, yTop + 3);
        return;
    }
    int height = HeightOpenedItem(page);
    bool shade = CurrentItemIsOpened(GetNamePage(page));
    painter.FillRegion(x - 1, yTop, MP_TITLE_WIDTH + 2, height + 2, gColorBack);
    painter.DrawRectangle(x, yTop, MP_TITLE_WIDTH + 1, height + 1, Color::BorderMenu(shade));

    if (shade)
    {
        painter.FillRegion(x + 1, yTop + 1, MP_TITLE_WIDTH - 1, MP_TITLE_HEIGHT - 1, Color::MENU_TITLE_DARK);
        painter.FillRegion(x + 4, yTop + 4, MP_TITLE_WIDTH - 7, MP_TITLE_HEIGHT - 7, Color::MENU_TITLE_DARK);
    }
    else
    {
        painter.DrawVolumeButton(x + 1, yTop + 1, MP_TITLE_WIDTH - 1, MP_TITLE_HEIGHT - 1, 2, Color::MenuTitle(false), Color::MENU_TITLE_BRIGHT, Color::MENU_TITLE_DARK, shade, shade);
    }
    
    painter.DrawVLine(x, yTop, yTop + HeightOpenedItem(page), Color::BorderMenu(false));
    bool condDrawRSet = page->NumSubPages() > 1 && TypeMenuItem(CurrentItem()) != Item_ChoiceReg && TypeMenuItem(CurrentItem()) != Item_Governor && TypeOpenedItem() == Item_Page;
    int delta = condDrawRSet ? -10 : 0;
    Color colorText = shade ? Color::LightShadingText() : Color::BLACK;
    x = painter.DrawStringInCenterRectC(x, yTop, MP_TITLE_WIDTH + 2 + delta, MP_TITLE_HEIGHT, TitleItem(page), colorText);
    if(condDrawRSet)
    {
        painter.Draw4SymbolsInRect(x + 4, yTop + 11, GetSymbolForGovernor(NumCurrentSubPage(page)), colorText);
    }

    itemUnderButton[GetFuncButtonFromY(yTop)] = page;

    delta = 0;
    
    painter.SetColor(colorText);
    DrawPagesUGO(page, CalculateX(layer) + MP_TITLE_WIDTH - 3 + delta, yTop + MP_TITLE_HEIGHT - 2 + delta);
    DrawNestingPage(page, CalculateX(layer) + 5, yTop + MP_TITLE_HEIGHT - 8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawPagesUGO(Page *page, int right, int bottom)
{
    int size = 4;
    int delta = 2;
    
    int allPages = (page->NumItemsInPage() - 1) / MENU_ITEMS_ON_DISPLAY + 1;
    int currentPage = NumCurrentSubPage(page);

    int left = right - (size + 1) * allPages - delta + (3 - allPages);
    int top = bottom - size - delta;

    for(int p = 0; p < allPages; p++)
    {
        int x = left + p * (size + 2);
        if(p == currentPage)
        {
            painter.FillRegion(x, top, size, size);
        }
        else
        {
            painter.DrawRectangle(x, top, size, size);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawNestingPage(Page *page, int left, int bottom)
{
    if (page != &mainPage)
    {
        int nesting = 0;

        Page *parent = Keeper(page);

        while (parent != &mainPage)
        {
            page = parent;
            parent = Keeper(page);
            nesting++;
        }

        int size = 4;
        int delta = 2;

        for (int i = 0; i <= nesting; i++)
        {
            int x = left + i * (size + delta);
            painter.DrawRectangle(x, bottom, size, size);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawGovernor(void *item, int x, int y)
{
    Governor_Draw((Governor *)item, x, y, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawIPaddress(void *item, int x, int y)
{
    IPaddress_Draw((IPaddress *)item, x, y, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawMACaddress(void *item, int x, int y)
{
    MACaddress_Draw((MACaddress *)item, x, y, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawFormula(void *item, int x, int y)
{
    Formula_Draw((Formula *)item, x, y, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawChoice(void *item, int x, int y)
{
    Choice_Draw((Choice *)item, x, y, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSmallButton(void *item, int x, int y)
{
    SmallButton_Draw((SButton *)item, LEFT_SB, y + 7);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawTime(void *item, int x, int y)
{
    Time_Draw((Time *)item, x, y, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawGovernorColor(void *item, int x, int y)
{
    GovernorColor_Draw((GovernorColor *)item, x, y, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawButton(void *item, int x, int y)
{
    Button_Draw((Button *)item, x, y);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawPage(void *item, int x, int y)
{
    Page_Draw((Page *)item, x, y);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawItemsPage(Page *page, int layer, int yTop)
{
    void (*funcOfDraw[Item_NumberItems])(void *, int, int) = 
    {  
        EmptyFuncpVII,      // Item_None
        DrawChoice,         // Item_Choice
        DrawButton,         // Item_Button
        DrawPage,           // Item_Page
        DrawGovernor,       // Item_Governor
        DrawTime,           // Item_Time
        DrawIPaddress,      // Item_IP
        DrawGovernorColor,  // Item_GovernorColor
        DrawFormula,        // Item_Formula
        DrawMACaddress,     // TypeItem_Mac
        DrawChoice,         // Item_ChoiceReg
        DrawSmallButton     // Item_SmallButton
    };
    int posFirstItem = PosItemOnTop(page);
    int posLastItem = posFirstItem + MENU_ITEMS_ON_DISPLAY - 1;
    LIMITATION(posLastItem, 0, page->NumItemsInPage() - 1);
    int count = 0;
    for(int posItem = posFirstItem; posItem <= posLastItem; posItem++)
    {
        void *item = Item(page, posItem);
        TypeItem type = TypeMenuItem(item);
        int top = yTop + MI_HEIGHT * count;
        funcOfDraw[type](item, CalculateX(layer), top);
        count++;
        itemUnderButton[GetFuncButtonFromY(top)] = item;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawOpenedPage(Page *page, int layer, int yTop)
{
    DrawTitlePage(page, layer, yTop);
    DrawItemsPage(page, layer, yTop + MP_TITLE_HEIGHT);
    if (CurrentItemIsOpened(GetNamePage(page)))
    {
        int8 posCurItem = PosCurrentItem(page);
        void *item = Item(page, posCurItem);
        for (int i = 0; i < 5; i++)
        {
            if (itemUnderButton[i + B_F1] != item)
            {
                itemUnderButton[i + B_F1] = 0;
            }
        }
        TypeItem type = TypeMenuItem(item);
        if (type == Item_Choice || type == Item_ChoiceReg)
        {
            Choice_Draw((Choice *)item, CalculateX(1), ItemOpenedPosY(item), true);
        }
        else if (type == Item_Governor)
        {
            Governor_Draw((Governor *)item, CalculateX(1), ItemOpenedPosY(item), true);
        }
        else if (type == Item_GovernorColor)
        {
            GovernorColor_Draw((GovernorColor *)item, CalculateX(1), ItemOpenedPosY(item), true);
        }
        else if (type == Item_Time)
        {
            Time_Draw((Time *)item, CalculateX(1), ItemOpenedPosY(item), true);
        }
        else if (type == Item_IP)
        {
            IPaddress_Draw((IPaddress *)item, CalculateX(1), ItemOpenedPosY(item), true);
        }
        else if (type == Item_MAC)
        {
            MACaddress_Draw((MACaddress *)item, CalculateX(1), ItemOpenedPosY(item), true);
        }
    }

    if (page->funcOnDraw)
    {
        page->funcOnDraw();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int CalculateX(int layer)
{
    return MP_X - layer * GRID_DELTA / 4;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool IsShade(void *item)
{
    return CurrentItemIsOpened(GetNamePage(Keeper(item))) && (item != OpenedItem());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPressed(void *item)
{
    return item == menu.ItemUnderKey();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *ItemUnderButton(PanelButton button)
{
    return itemUnderButton[button];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ResetItemsUnderButton(void)
{
    for(int i = 0; i < B_NumButtons; i++)
    {
        itemUnderButton[i] = 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int ItemOpenedPosY(void *item)
{
    Page *page = Keeper(item);
    int8 posCurItem = PosCurrentItem(page);
    int y = GRID_TOP + (posCurItem % MENU_ITEMS_ON_DISPLAY) * MI_HEIGHT + MP_TITLE_HEIGHT;
    if(y + HeightOpenedItem(item) > GRID_BOTTOM)
    {
        y = GRID_BOTTOM - HeightOpenedItem(item) - 2;
    }
    return y + 1;
}
