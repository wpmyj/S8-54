// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Log.h"
#include "Menu.h" 
#include "MenuDrawing.h"
#include "MenuFunctions.h"
#include "MenuItemsLogic.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Pages/Definition.h"
#include "Pages/PageCursors.h"
#include "Pages/PageDisplay.h"
#include "Pages/PageMemory.h"
#include "Panel/Panel.h"
#include "Utils/GlobalFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void OnChanged_ChanA_Input(bool active);
extern void OnChanged_ChanB_Input(bool active);

extern const Page pHelp;

/// Если произошло короткое нажатие кнопки, то здесь хранится имя этой кнопки до обработки  этого нажатия.
static PanelButton shortPressureButton = B_Empty;
/// Если произошло длинное нажатие кнопки, то здесь хранится имя этой кнопки до обработки этого нажатия.
static PanelButton longPressureButton = B_Empty;
/// При нажатии кнопки её имя записывается в эту переменную и хранится там до обратоки события нажатия кнопки.
static PanelButton pressButton = B_Empty;
/// При отпускании кнопки её имя записывается в эту переменную и хранится там до обработки события отпускания кнопки.
static PanelButton releaseButton = B_Empty;
static PanelRegulator pressRegulator = R_Empty;
/// Угол, на который нужно повернуть ручку УСТАНОВКА - величина означает количество щелчков, знак - направление - "-" - влево, "+" - вправо.
static int angleRegSet = 0;
///\brief  Здесь хранится адрес элемента меню, соответствующего функциональной клавише [1..5], если она находится в нижнем положении, и 0, если ни одна 
/// кнопка не нажата.
static void *itemUnderKey = 0;
/// Эта функция будет вызывана один раз после Menu_UpdateInput().
static pFuncVV funcAterUpdate = 0;
                                                
static void ProcessingShortPressureButton(void);            ///< Обработка короткого нажатия кнопки.
static void ProcessingLongPressureButton(void);             ///< Обработка длинного нажатия кнопки.
static void ProcessingPressButton(void);                    ///< Обработка опускания кнопки вниз.
static void ProcessingReleaseButton(void);                  ///< Обработка поднятия кнопки вверх.
static void ProcessingRegulatorSetRotate(void);             ///< Обработка поворота ручки УСТАНОВКА.
static void ProcessingRegulatorPress(void);                 ///< Обработка нажатия ручки.
static void OnTimerAutoHide(void);                          ///< Обработка события таймера автоматического сокрытия меню.
static void SwitchSetLED(void);                             ///< Включить/выключить светодиод ручки УСТАНОВКА, если необходимо.
static void ShortPress_Page(void *page);                    ///< Обработка короткого нажатия на элемент NamePage с адресом page.
static void ShortPress_Choice(void *choice);                ///< Обработка короткого нажатия на элемент Choice с адресом choice.
static void ShortPress_Time(void *time);
static void ShortPress_Button(void *button);                ///< Обработка короткого нажатия на элемент Button с адресом button.
static void ShortPress_Governor(void *governor);            ///< Обработка короткого нажатия на элемент Governor с адресом governor.
static void ShortPress_GovernorColor(void *governorColor);  ///< Обработка короткого нажатия на элемент GovernorColor с адресом governorColor.
static void FuncOnLongPressItem(void *item);                ///< Обработка длинного нажатия на элемент меню item.
static void FuncOnLongPressItemTime(void *item);
static void FuncOnLongPressItemButton(void *button);        ///< Обработка длинного нажатия на элемент Button с адресом button.

static  pFuncVpV    FuncForShortPressOnItem(void *item);    ///< Возвращает функцию обработки короткого нажатия на элемент меню item.
static  pFuncVpV    FuncForLongPressureOnItem(void *item);  ///< Возвращает функцию обработки длинного нажатия на элемент меню item.

static void OnTimerStrNaviAutoHide(void);                   ///< Функция, которая отключит вывод строки навигации меню.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SIZE_BUFFER_FOR_BUTTONS 10
static PanelButton bufferForButtons[SIZE_BUFFER_FOR_BUTTONS] = {B_Empty};
static const PanelButton sampleBufferForButtons[SIZE_BUFFER_FOR_BUTTONS] = {B_F5, B_F5, B_F4, B_F4, B_F3, B_F3, B_F2, B_F2, B_F1, B_F1};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Menu_UpdateInput(void)
{
    ProcessingShortPressureButton();
    ProcessingLongPressureButton();
    ProcessingRegulatorSetRotate();
    ProcessingPressButton();
    ProcessingReleaseButton();
    ProcessingRegulatorPress();
    SwitchSetLED();

    if (funcAterUpdate)
    {
        funcAterUpdate();
        funcAterUpdate = 0;
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_ShortPressureButton(PanelButton button)
{
    if (!HINT_MODE_ENABLE)
    {
        if (button == B_Memory && FDRIVE_IS_CONNECTED && MODE_BTN_MEMORY_SAVE)
        {
            NEED_SAVE_TO_FLASHDRIVE = 1;
        }

        shortPressureButton = button;
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_LongPressureButton(PanelButton button)
{
    if (!HINT_MODE_ENABLE)
    {
        longPressureButton = button;
        NEED_FINISH_DRAW = 1;
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ProcessButtonForHint(PanelButton button)
{
    if (button == B_Menu)
    {
        gStringForHint = LANG_RU ?
            "Кнопка МЕНЮ выполняет следующие функции:\n"
            "1. При закрытом меню нажатие либо нажатие с удержанием в течение 0.5с открывает меню.\n"
            "2. При открытом меню удержание кнопки в течение 0.5с закрывает меню.\n"
            "3. При настройке \"СЕРВИС\x99Режим кн МЕНЮ\x99Закрывать\" текущей становится страница предыдущего уровня меню. Если текущей является "
            "корневая страница, меню закрывается.\n"
            "4. При настройке \"СЕРВИС\x99Режим кн МЕНЮ\x99Переключать\" текущей становится страница текущего уровня меню. Если текущая страница "
            "последняя в текущем уровне, происходит переход на предыдущий уровень меню.\n"
            "5. Если меню находится в режиме малых кнопок, то нажатие закрывает страницу."
            :
        "MENU button performs the following functions:\n"
            "1. At the closed menu pressing or pressing with deduction during 0.5s opens the menu.\n"
            "2. At the open menu deduction of the button during 0.5s closes the menu.\n"
#pragma push
#pragma diag_suppress 192
            "3. At control \"SERVICE\x99Mode btn MENU\x99\Close\" current becomes the page of the previous level of the menu. If the root page is "
            "current, the menu is closed.\n"
#pragma pop
            "4. At control \"SERVICE\x99Mode btn MENU\x99Toggle\" current becomes the page of the current level of the menu. If the current page the "
            "last in the current level, happens transition to the previous level of the menu.\n"
            "5. If the menu is in the mode of small buttons, pressing closes the page.";

    } else if (button == B_Cursors)
    {
        gStringForHint = LANG_RU ? 
            "Кнопка КУРСОРЫ открывает меню курсорных измерений."
            :
            "КУРСОРЫ button to open the menu cursor measurements.";
    }
    else if (button == B_Display)
    {
        gStringForHint = LANG_RU ?
            "Кнопка ДИСПЛЕЙ открывает меню настроек дисплея."
            :
            "DISPLAY button opens the display settings menu.";
    }
    else if (button == B_Memory)
    {
        gStringForHint = LANG_RU ?
            "1. При настройке \"ПАМЯТЬ\x99ВНЕШН ЗУ\x99Реж кн ПАМЯТЬ\x99Меню\" открывает меню работы с памятью.\n"
            "2. При настройке \"ПАМЯТь\x99ВНЕШН ЗУ\x99Реж кн ПАМЯТЬ\x99Сохранение\" сохраняет сигнал на флеш-диск."
            :
            "1. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Menu\" opens a menu of memory\n"
            "2. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Save\" saves the signal to the flash drive";
    }
    else if (button == B_Measures)
    {
        gStringForHint = LANG_RU ?
            "Кнопка ИЗМЕР открывает меню автоматических измерений."
            :
            "ИЗМЕР button opens a menu of automatic measurements.";
    }
    else if (button == B_Help)
    {
        gStringForHint = LANG_RU ?
            "1. Кнопка ПОМОЩЬ открывает меню помощи.\n"
            "2. Нажатие и удержание кнопки ПОМОЩЬ в течение 0.5с включает и отключает режим вывода подсказок."
            :
            "1. ПОМОЩЬ button opens the help menu.\n"
            "2. Pressing and holding the ПОМОЩЬ for 0.5s enables and disables output mode hints.";
    }
    else if (button == B_Service)
    {
        gStringForHint = LANG_RU ?
            "Кнопка СЕРВИС открывает меню сервисных возможностей."
            :
            "СЕРВИС button opens a menu of service options.";
    }
    else if (button == B_Start)
    {
        gStringForHint = LANG_RU ?
            "Кнопка ПУСК/СTOП запускает и останавливает процесс сбора информации."
            :
            "ПУСК/СTOП button starts and stops the process of gathering information.";
    }
    else if (button == B_Channel1)
    {
        gStringForHint = LANG_RU ?
            "1. Кнопка КАНАЛ1 открывает меню настроек канала 1.\n"
            "2. Нажатие и удержание кнопки КАНАЛ1 в течение 0.5с устанавливает смещение канала 1 по вертикали 0В."
            :
            "1. КАНАЛ1 button opens the settings menu of the channel 1.\n"
            "2. Pressing and holding the button КАНАЛ1 for 0.5c for the offset of the vertical channel 1 0V.";
    }
    else if (button == B_Channel2)
    {
        gStringForHint = LANG_RU ?
            "1. Кнопка КАНАЛ2 открывает меню настроек канала 2.\n"
            "2. Нажатие и удержание кнопки КАНАЛ2 в течение 0.5с устанавливает смещение канала 2 по вертикали 0В."
            :
            "1. КАНАЛ2 button opens the settings menu of the channel 2.\n"
            "2. Pressing and holding the button КАНАЛ2 for 0.5c for the offset of the vertical channel 2 0V.";
    }
    else if (button == B_Time)
    {
        gStringForHint = LANG_RU ?
            "1. Кнопка РАЗВ открывает меню настроек развертки.\n"
            "2. Нажатие и удержание кнопки РАЗВ в течение 0.5с устанавливает смещение по горизонтали 0с."
            :
            "1. РАЗВ button open the settings menu sweep.\n"
            "2. Pressing and holding the button РАЗВ for 0.5s Is the offset horizontal 0s.";
    }
    else if (button == B_Trig)
    {
        gStringForHint = LANG_RU ?
            "1. Кнопка СИНХР открывает меню настроек синхронизации.\n"
            "2. Нажатие и удержание в течение 0.5с кнопки СИНХР при настройке \"СЕРВИС\x99Реж длит СИНХР\x99Автоуровень\" производит автоматическую "
            "настройку уровня синхронизации.\n"
            "3. Нажатие и удержание в течение 0.5с кнопки СИНХР при настройке \"СЕРВИС\x99Реж длит СИНХР\x99Сброс уровня\" устанавливает уровень "
            "синхронизации 0В."
            :
            "1. СИНХР button opens a menu settings synchronization.\n"
#pragma push
#pragma diag_suppress 192
            "2. Pressing and holding the button СИНХР for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99\Autolevel\" automatically adjust the "
            "trigger level.\n"
#pragma pop
            "3. Pressing and holding the button СИНХР for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99SReset trig level\" sets the trigger "
            "level 0V.";
    }
    else
    {
        shortPressureButton = button;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_PressButton(PanelButton button)
{
    Sound_ButtonPress();
    if (HINT_MODE_ENABLE)
    {
        ProcessButtonForHint(button);
        return;
    }

    if (!MENU_IS_SHOWN)
    {
        for (int i = SIZE_BUFFER_FOR_BUTTONS - 1; i > 0; i--)
        {
            bufferForButtons[i] = bufferForButtons[i - 1];
        }
        bufferForButtons[0] = button;
      
        if (memcmp(bufferForButtons, sampleBufferForButtons, SIZE_BUFFER_FOR_BUTTONS) == 0) //-V512
        {
            SHOW_DEBUG_MENU = 1;
            Display_ShowWarning(MenuDebugEnabled);
        }
    }
    pressButton = button;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_ReleaseButton(PanelButton button)
{
    Sound_ButtonRelease();
    if (!HINT_MODE_ENABLE)
    {
        releaseButton = button;
    }
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_PressReg(PanelRegulator reg)
{
    if (!HINT_MODE_ENABLE)
    {
        pressRegulator = reg;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_RotateRegSetRight(void)
{   
    if (!HINT_MODE_ENABLE)
    {
        angleRegSet++;
        NEED_FINISH_DRAW = 1;
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_RotateRegSetLeft(void)
{
    if (!HINT_MODE_ENABLE)
    {
        angleRegSet--;
        NEED_FINISH_DRAW = 1;
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *Menu_ItemUnderKey(void)
{
    return itemUnderKey;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_SetAutoHide(bool active)
{
    if(!MENU_IS_SHOWN)
    {
        return;
    }
    if(sDisplay_TimeMenuAutoHide() == 0)
    {
        Timer_Disable(kMenuAutoHide);
    }
    else
    {
        Timer_SetAndStartOnce(kMenuAutoHide, OnTimerAutoHide, sDisplay_TimeMenuAutoHide());
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Menu_StringNavigation(char buffer[100])
{
    buffer[0] = 0;
    const char * titles[10] = {0};
    int numTitle = 0;
    void *item = OpenedItem();
    if(IsMainPage(item))
    {
        return 0;
    }
    while(!IsMainPage(item))
    {
        titles[numTitle++] = TitleItem(item);
        item = Keeper(item);
    }
    for(int i = 9; i >= 0; i--)
    {
        if(titles[i])
        {
            strcat(buffer, titles[i]);
            if(i != 0)
            {
                strcat(buffer, " - ");
            }
        }
    }
    return buffer;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnTimerAutoHide(void)
{
    Menu_Show(false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ProcessingShortPressureButton(void)
{
    if(shortPressureButton != B_Empty)
    {
        if (shortPressureButton == B_Memory && MODE_BTN_MEMORY_SAVE && FDRIVE_IS_CONNECTED)
        {
            EXIT_FROM_SETNAME_TO = MENU_IS_SHOWN ? RETURN_TO_MAIN_MENU : RETURN_TO_DISABLE_MENU;
            Memory_SaveSignalToFlashDrive();
            shortPressureButton = B_Empty;
            return;
        }
        NEED_FINISH_DRAW = 1;
        Menu_SetAutoHide(true);

        PanelButton button = shortPressureButton;

        do
        {
            if (button == B_Help)
            {
                Long_Help();
            }
            else if(button == B_Menu)                                   // Если нажата кнопка МЕНЮ и мы не находимся в режме настройки измерений.
            {
                if(!MENU_IS_SHOWN)
                {
                    Menu_Show(true);
                }
                else
                {
                    if (TypeOpenedItem() == Item_Page)
                    {
                        Menu_TemporaryEnableStrNavi();
                    }
                    CloseOpenedItem();
                }
            }
            else if (MENU_IS_SHOWN && IsFunctionalButton(button))       // Если меню показано и нажата функциональная клавиша
            {
                void *item = ItemUnderButton(button);
                if (HINT_MODE_ENABLE)
                {
                    SetItemForHint(item);
                }
                else
                {
                    FuncForShortPressOnItem(item)(item);
                }
            }
            else                                                        // Если меню не показано.
            {
                NamePage name = GetNamePage(OpenedItem());
                if(button == B_Channel1 && name == Page_ChannelA && MENU_IS_SHOWN)
                {
                    SET_ENABLED_A = !SET_ENABLED_A;
                    OnChanged_ChanA_Input(true);
                    break;
                }
                if(button == B_Channel2 && name == Page_ChannelB && MENU_IS_SHOWN)
                {
                    SET_ENABLED_B = !SET_ENABLED_B;
                    OnChanged_ChanB_Input(true);
                    break;
                }

                const void *page = PageForButton(button);
                if(page && page != &pHelp)
                {
                    SetCurrentItem(page, true);
                    OpenItem(page, true);
                    Menu_TemporaryEnableStrNavi();
                    Menu_Show(true);
                }
            }
        } while(false);

        shortPressureButton = B_Empty;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessingLongPressureButton(void)
{
    PanelButton button = longPressureButton;

    if(button != B_Empty)
    {
        Sound_ButtonRelease();
        NEED_FINISH_DRAW = 1;
        Menu_SetAutoHide(true);

        if(button == B_Time)
        {
            FPGA_SetTShift(0);
        }
        else if(button == B_Trig)
        {
            FPGA_SetTrigLev(TRIGSOURCE, TrigLevZero);
        }
        else if(button == B_Channel1)
        {
            FPGA_SetRShift(A, RShiftZero);
        }
        else if(button == B_Channel2)
        {
            FPGA_SetRShift(B, RShiftZero);
        }
        else if(button == B_Menu)
        {
            if (IsPageSB(OpenedItem()))
            {
                CloseOpenedItem();
            }
            else
            {
                Menu_Show(!MENU_IS_SHOWN);
                if (TypeOpenedItem() != Item_Page)
                {
                    Menu_TemporaryEnableStrNavi();
                }
            }
        }
        else if(MENU_IS_SHOWN && IsFunctionalButton(button))
        {
            void *item = ItemUnderButton(button);
            FuncForLongPressureOnItem(item)(item);
            if (TypeOpenedItem() != Item_Page)
            {
                Menu_TemporaryEnableStrNavi();
            }
        }
        longPressureButton = B_Empty;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessingRegulatorPress(void)
{
    if (pressRegulator != R_Empty)
    {
        Menu_SetAutoHide(true);
        if (pressRegulator == R_Set)
        {
            Menu_Show(!MENU_IS_SHOWN);
            if (TypeOpenedItem() != Item_Page)
            {
                Menu_TemporaryEnableStrNavi();
            }
        }

        pressRegulator = R_Empty;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessingRegulatorSetRotate(void)
{
    if (angleRegSet == 0)
    {
        return;
    }

    if (MENU_IS_SHOWN || TypeOpenedItem() != Item_Page)
    {
        void *item = CurrentItem();
        TypeItem type = TypeMenuItem(item);
        static const int step = 2;
        if (TypeOpenedItem() == Item_Page && (type == Item_ChoiceReg || type == Item_Governor || type == Item_IP || type == Item_MAC))
        {
            if (angleRegSet > step || angleRegSet < -step)
            {
                ChangeItem(item, angleRegSet);
                angleRegSet = 0;
            }
            return;
        }
        else
        {
            item = OpenedItem();
            type = TypeMenuItem(item);
            if (MenuIsMinimize())
            {
                CurrentPageSBregSet(angleRegSet);
            }
            else if (type == Item_Page || type == Item_IP || type == Item_MAC || type == Item_Choice || type == Item_ChoiceReg || type == Item_Governor)
            {
                if (ChangeOpenedItem(item, angleRegSet))
                {
                    angleRegSet = 0;
                }
                return;
            }
            else if (type == Item_GovernorColor)
            {
                ChangeItem(item, angleRegSet);
            }
            else if (type == Item_Time)
            {
                angleRegSet > 0 ? Time_IncCurrentPosition(item) : Time_DecCurrentPosition(item);
            }
        }
    }

    angleRegSet = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessingPressButton(void)
{
    if((pressButton >= B_F1 && pressButton <= B_F5) || pressButton == B_Menu)
    {
        if (pressButton != B_Menu)
        {
            itemUnderKey = ItemUnderButton(pressButton);
        }
    }
    if (pressButton == B_Start && !MODE_WORK_RAM)
    {
        FPGA_OnPressStartStop();
    }
    pressButton = B_Empty;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessingReleaseButton(void)
{
    if(releaseButton >= B_F1 && releaseButton <= B_F5 || pressButton == B_Menu)
    {
        itemUnderKey = 0;
        releaseButton = B_Empty;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPress_Page(void *item)
{
    Page *page = item;
    if (page->funcOnPress)
    {
        page->funcOnPress();
    }

    SBPage_SetCurrent(page);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_TemporaryEnableStrNavi(void)
{
    if (SHOW_STRING_NAVI_TEMP)
    {
        SHOW_STRING_NAVIGATION = 1;                                            // Устанавливаем признак того, что надо выводить строку навигации меню
        Timer_SetAndStartOnce(kStrNaviAutoHide, OnTimerStrNaviAutoHide, 3000); // и запускаем таймер, который его отключит
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void OnTimerStrNaviAutoHide(void)
{
    SHOW_STRING_NAVIGATION = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPress_Choice(void *choice_)
{
    Choice *choice = (Choice*)choice_;

    if (!ItemIsAcitve(choice))
    {
        CHOICE_RUN_FUNC_CHANGED(choice, false);
    }
    else if (!ItemIsOpened(choice))
    {
        SetCurrentItem(choice, CurrentItem() != choice);
        Choice_StartChange(choice, 1);
    }
    else
    {
        Choice_ChangeIndex(choice, -1);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPress_ChoiceReg(void *choice_)
{
    Choice *choice = (Choice*)choice_;

    if(!ItemIsAcitve(choice)) 
    {
        CHOICE_RUN_FUNC_CHANGED(choice, false);
    } 
    else if(OpenedItem() != choice) 
    {
        SetCurrentItem(choice, CurrentItem() != choice);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncOnLongPressItemButton(void *button)
{
    ShortPress_Button(button);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPress_Button(void *button)
{
    if(!ItemIsAcitve(button))
    {
        return;
    }
    SetCurrentItem(button, true);
    CallFuncOnPressButton(button);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncOnLongPressItem(void *item)
{
    if (CurrentItem() != item)
    {
        SetCurrentItem(item, true);
    }
    OpenItem(item, !ItemIsOpened(item));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncOnLongPressItemTime(void *time)
{
    if (CurrentItem() != time)
    {
        SetCurrentItem(time, true);
    }
    if(ItemIsOpened(time) && *((Time*)time)->curField == iSET)
    {
        Time_SetNewTime(time);
    }
    OpenItem(time, !ItemIsOpened(time));
    Time_SetOpened(time);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPress_Time(void *time)
{
    if(!ItemIsOpened(time))
    {
        SetCurrentItem(time, true);
        Time_SetOpened(time);
        OpenItem(time, true);
    }
    else
    {
        Time_SelectNextPosition(time);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPress_Governor(void *governor)
{
    Governor *gov = (Governor*)governor;
    if(!ItemIsAcitve(governor))
    {
        return;
    }
    if(OpenedItem() == gov)
    {
        Governor_NextPosition(gov);
    }
    else
    {
        SetCurrentItem(gov, CurrentItem() != gov);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPress_IP(void *item)
{
    if (OpenedItem() == item)
    {
        IPaddress_NextPosition((IPaddress*)item);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPress_MAC(void *item)
{
    if (OpenedItem() == item)
    {
        CircleIncreaseInt8(&gCurDigit, 0, 5);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ShortPress_GovernorColor(void *governorColor)
{
    if(!ItemIsAcitve(governorColor))
    {
        return;
    }
    GovernorColor *governor = (GovernorColor*)governorColor;
    if(OpenedItem() == governor)
    {
        CircleIncreaseInt8(&(governor->colorType->currentField), 0, 3);
    }
    else
    {
        FuncOnLongPressItem(governorColor);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ShortPress_SmallButton(void *smallButton)
{
    SButton *sb = smallButton;
    if (sb)
    {
        pFuncVV func = sb->funcOnPress;
        if (func)
        {
            func();
            itemUnderKey = smallButton;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
pFuncVpV FuncForShortPressOnItem(void *item)
{
    static const pFuncVpV shortFunction[Item_NumberItems] =
    {
        EmptyFuncVpV,               // Item_None
        ShortPress_Choice,          // Item_Choice
        ShortPress_Button,          // Item_Button
        ShortPress_Page,            // Item_Page
        ShortPress_Governor,        // Item_Governor
        ShortPress_Time,            // Item_Time
        ShortPress_IP,              // Item_IP
        ShortPress_GovernorColor,   // Item_GovernorColor
        EmptyFuncVpV,               // Item_Formula
        ShortPress_MAC,             // Item_MAC
        ShortPress_ChoiceReg,       // Item_ChoiceReg
        ShortPress_SmallButton      // Item_SmallButton
    };

    return shortFunction[TypeMenuItem(item)];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
pFuncVpV FuncForLongPressureOnItem(void *item)
{
    static const pFuncVpV longFunction[Item_NumberItems] =
    {
        EmptyFuncVpV,               // Item_None
        FuncOnLongPressItem,        // Item_Choice
        FuncOnLongPressItemButton,  // Item_Button
        FuncOnLongPressItem,        // Item_Page
        FuncOnLongPressItem,        // Item_Governor
        FuncOnLongPressItemTime,    // Item_Time
        FuncOnLongPressItem,        // Item_IP
        FuncOnLongPressItem,        // Item_GovernorColor
        EmptyFuncVpV,               // Item_Formula
        FuncOnLongPressItem,        // Item_MAC
        FuncOnLongPressItem,        // Item_ChoiceReg
        ShortPress_SmallButton      // Item_SmallButton
    };

    return ItemIsAcitve(item) ? longFunction[TypeMenuItem(item)] : EmptyFuncVpV;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ChangeStateFlashDrive(void)
{
    if(!FDRIVE_IS_CONNECTED)
    {
        if(GetNameOpenedPage() == PageSB_Memory_Drive_Manager)
        {
            ShortPressOnPageItem(OpenedItem(), 0);
        }
    }
    else if(FLASH_AUTO_CONNECT)
    {
        OnPress_Drive_Manager();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_OpenItemTime(void)
{
    Display_ShowWarning(TimeNotSet);
    Menu_ShortPressureButton(B_Service);
    Menu_UpdateInput();
    Display_Update();
    for (int i = 0; i < 2; i++)
    {
        Menu_RotateRegSetRight();
        Menu_UpdateInput();
        Display_Update();
    }
    Menu_ShortPressureButton(B_F4);
    Menu_UpdateInput();
    Display_Update();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
/// Возвращает true, если лампочка УСТАНОВКА должна гореть
static bool NeedForFireSetLED(void)
{
    if (!MENU_IS_SHOWN)
    {
        TypeItem type = TypeOpenedItem();
        return (type == Item_ChoiceReg) || (type == Item_Choice) || (type == Item_Governor);
    }

    NamePage name = GetNameOpenedPage();
    if (
            name == PageSB_Debug_SerialNumber   ||
            name == PageSB_Service_FFT_Cursors  || 
            name == PageSB_Measures_Tune        || 
            name == PageSB_Memory_Last          || 
            name == PageSB_Memory_Internal      ||
            IsRegSetActiveOnCursors()           ||
            (name == PageSB_Service_Function && FUNC_ENABLED)
        )
    {
        return true;
    }
    
    TypeItem type = TypeMenuItem(CurrentItem());
    if (type == Item_Governor    ||
        type == Item_ChoiceReg   ||
        type == Item_GovernorColor)
    {
        return true;
    }

    type = TypeOpenedItem();
    if (type == Item_Choice       ||
        (type == Item_Page && NumSubPages(OpenedItem()) > 1)
        )
    {
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void SwitchSetLED(void)
{
    static bool first = true;
    static bool prevState = false;  // true - горит, false - не горит

    bool state = NeedForFireSetLED();

    if (first)
    {
        first = false;
        Panel_EnableLEDRegSet(state);
        prevState = state;
    }
    else if (prevState != state)
    {
        Panel_EnableLEDRegSet(state);
        prevState = state;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_Show(bool show)
{
    set.menu_IsShown = show;
    if (show)
    {
        Menu_TemporaryEnableStrNavi();
    }
    Menu_SetAutoHide(true);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_Init(void)
{
    PageDisplay_Init();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_RunAfterUpdate(pFuncVV func)
{
    funcAterUpdate = func;
}
