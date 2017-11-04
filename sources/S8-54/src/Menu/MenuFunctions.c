

#include "defines.h"
#include "MenuFunctions.h"
#include "Globals.h"
#include "Log.h"
#include "Display/Display.h"
#include "Display/DisplayTypes.h"
#include "Hardware/Sound.h"
#include "MenuItems.h"
#include "Menu.h"
#include "Panel/Controls.h"
#include "Panel/Panel.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Tables.h"
#include "Utils/Math.h"
#include "Utils/Measures.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mainPage;


static void *RetLastOpened(Page *_page, TypeItem *_type);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TypeItem TypeMenuItem(const void *address) 
{
    return address ? (*((TypeItem*)address)) : Item_None;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool    CurrentItemIsOpened(NamePage namePage)
{
    bool retValue = GetBit(MENU_POS_ACT_ITEM(namePage), 7) == 1;
    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int8    PosCurrentItem(const Page *page)
{
    return MENU_POS_ACT_ITEM(page->name) & 0x7f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void    SetCurrentItem(const void *item, bool active)
{
    if(item != 0)
    {
        Page *page = (Keeper(item));
        if(!active)
        {
            SetMenuPosActItem(page->name, 0x7f);
        }
        else
        {
            for(int i = 0; i < page->NumItems(); i++)
            {
                if(page->Item(i) == item)
                {
                    SetMenuPosActItem(page->name, (int8)i);
                    return;
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
TypeItem TypeOpenedItem(void)
{
    return TypeMenuItem(OpenedItem());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *OpenedItem(void)
{
    TypeItem type = Item_None;
    return RetLastOpened((Page *)&mainPage, &type);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *CurrentItem(void)
{
    TypeItem type = Item_None;
    void *lastOpened = RetLastOpened((Page *)&mainPage, &type);
    int8 pos = PosCurrentItem((const Page *)lastOpened);
    if(type == Item_Page && pos != 0x7f)
    {
        return ((const Page *)lastOpened)->Item(pos);
    }
    return lastOpened;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int HeightOpenedItem(void *item) 
{
    TypeItem type = TypeMenuItem(item);
    if(type == Item_Page)
    {
        int numItems = ((const Page *)item)->NumItems() - ((Page *)item)->NumCurrentSubPage() * MENU_ITEMS_ON_DISPLAY;
        LIMITATION(numItems, 0, MENU_ITEMS_ON_DISPLAY);
        return MP_TITLE_HEIGHT + MI_HEIGHT * numItems;
    } 
    else if(type == Item_Choice || type == Item_ChoiceReg)
    {
        return MOI_HEIGHT_TITLE + ((Choice *)item)->NumSubItems() * MOSI_HEIGHT - 1;
    }
    return MI_HEIGHT;
}



//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *TitleItem(void *item) 
{
    return ((Page *)item)->titleHint[LANG];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int PosItemOnTop(Page *page)
{
    return page->NumCurrentSubPage() * MENU_ITEMS_ON_DISPLAY;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool IsFunctionalButton(PanelButton button)
{
    return button >= B_F1 && button <= B_F5;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ChangeSubPage(const Page *page, int delta)
{
    if (page)
    {
        if (delta > 0 && MenuCurrentSubPage(page->name) < page->NumSubPages() - 1)
        {
            Sound_RegulatorSwitchRotate();
            SetMenuCurrentSubPage(page->name, MenuCurrentSubPage(page->name) + 1);
        }
        else if (delta < 0 && MenuCurrentSubPage(page->name) > 0)
        {
            Sound_RegulatorSwitchRotate();
            SetMenuCurrentSubPage(page->name, MenuCurrentSubPage(page->name) - 1);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *RetLastOpened(Page *page, TypeItem *type)
{
    if(CurrentItemIsOpened(page->GetNamePage()))
    {
        int8 posActItem = PosCurrentItem(page);
        void *item = page->Item(posActItem);
        TypeItem typeLocal = TypeMenuItem(page->Item(posActItem));
        if(typeLocal == Item_Page)
        {
            return RetLastOpened((Page *)item, type);
        }
        else
        {
            return item;
        }
    }
    *type = Item_Page;
    return page;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void CloseOpenedItem(void)
{
    void *item = OpenedItem();
    if(TypeOpenedItem() == Item_Page)
    {
        if (IsPageSB(item))
        {
            CallFuncOnPressButton(((Page *)item)->SmallButonFromPage(0));
        }
        NamePage name = Keeper(item)->name;
        SetMenuPosActItem(name, MENU_POS_ACT_ITEM(name) & 0x7f);
        if(item == &mainPage)
        {
            menu.Show(false);
        }
    }
    else
    {
        OpenItem(item, false);
    } 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OpenItem(const void *item, bool open)
{
    if(item)
    {
        Page *page = Keeper(item);
        SetMenuPosActItem(page->GetNamePage(), open ? (PosCurrentItem(page) | 0x80) : (PosCurrentItem(page) & 0x7f));
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool ItemIsOpened(const void *item)
{
    TypeItem type = TypeMenuItem(item);
    Page *page = Keeper(item);
    if(type == Item_Page)
    {
        return CurrentItemIsOpened(Keeper(item)->GetNamePage());
    }
    return (MENU_POS_ACT_ITEM(page->name) & 0x80) != 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Page *Keeper(const void *item)
{
    const Page *page = ((Page *)(item))->keeper;
    return (Page *)page;
}



//----------------------------------------------------------------------------------------------------------------------------------------------------
NamePage GetNameOpenedPage(void)
{
    return ((const Page *)OpenedItem())->GetNamePage();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool ItemIsAcitve(const void *item) 
{
    TypeItem type = TypeMenuItem(item);

    /// \todo Здесь оптимизировать через битовую маску

    if (type == Item_Choice || type == Item_Page || type == Item_Button || type == Item_Governor || type == Item_SmallButton || type == Item_ChoiceReg)
    {
        pFuncBV func = ((Page *)(item))->funcOfActive;

        return func ? func() : true;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *NameSubItem(Choice *choice, int i) 
{
    return choice->names[i][LANG];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool ChangeOpenedItem(void *item, int delta)
{
    if (delta < 2 && delta > -2)
    {
        return false;
    }

    TypeItem type = TypeMenuItem(item);

    if (type == Item_Page)
    {
        ChangeSubPage((const Page *)item, delta);
    }
    else if (type == Item_IP)
    {
        ((IPaddress *)item)->ChangeValue(delta);
    }
    else if (type == Item_MAC)
    {
        ((MACaddress *)item)->ChangeValue(delta);
    }
    else if (type == Item_ChoiceReg || type == Item_Choice)
    {
        ((Choice *)item)->ChangeIndex(MENU_IS_SHOWN ? delta : -delta);
    }
    else if (type == Item_Governor)
    {
        ((Governor *)item)->ChangeValue(delta);
    }
    
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ChangeItem(void *item, int delta)
{
    TypeItem type = TypeMenuItem(item);
    if (type == Item_Choice || type == Item_ChoiceReg)
    {
        ((Choice *)item)->StartChange(delta);
    }
    else if (type == Item_Governor)
    {
        Governor *governor = (Governor*)item;
        if (OpenedItem() != governor)
        {
            governor->StartChange(delta);
        }
        else
        {
            governor->ChangeValue(delta);
        }
    }
    else if (type == Item_GovernorColor)
    {
        ((GovernorColor *)item)->ChangeValue(delta);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPressOnPageItem(Page *page, int numItem)
{
    if (TypeMenuItem(page) == Item_Page)
    {
        CallFuncOnPressButton(page->items[numItem]);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Page *PagePointerFromName(NamePage namePage)
{
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPageSB(const void *item)
{
    if (TypeMenuItem(item) == Item_Page)
    {
        return ((Page *)item)->isPageSB;
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void CallFuncOnPressButton(void *button)
{
    if (button)
    {
        Button *btn = (Button*)button;
        if (btn->funcOnPress)
        {
            btn->funcOnPress();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void CallFuncOnDrawButton(Button *button, int x, int y)
{
    if (button->funcForDraw)
    {
        button->funcForDraw(x, y);
    }
}
