#pragma once
#include "defines.h"
#include "Panel/Controls.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup Menu
 *  @{
 *  @defgroup Pages
 *  @{
 *  @}
 */

#define MENU_IS_SHOWN (set.menu_IsShown)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Инициализация
void Menu_Init(void);
/// Функция должна вызываться в главном цикле.
void Menu_UpdateInput(void);
/// Функция обработки короткого нажатия кнопки (менее 0.5 сек.).
void Menu_ShortPressureButton(PanelButton button);
/// Функция обработки длинного нажатия кнопки (более 0.5 сек.).
void Menu_LongPressureButton(PanelButton button);
/// Функция вызывается, когда кнопка переходит из отжатого в нажатое положение.
void Menu_PressButton(PanelButton button);
/// Функция вызывается, когда кнопка переходит из нажатого в отжатое положение.
void Menu_ReleaseButton(PanelButton button);
/// Функция вызывается в момент нажатия гувернёра.
void Menu_PressReg(PanelRegulator reg);
/// Функция обработки поворота ручки УСТАНОВКА вправо.
void Menu_RotateRegSetRight(void);
/// Функция обработки поворота ручки УСТАНОВКА влево.
void Menu_RotateRegSetLeft(void);
/// Установить время автоматического сокрытия меню в соответствии с установками.
void Menu_SetAutoHide(bool active);
/// Возвращает адрес пункта меню, находящегося под нажатой в данный момент кнопкой.
void *Menu_ItemUnderKey(void);
/// Возвращает путь к текущему пункту меню в текстовом виде, готовом к выводу на экран.
char *Menu_StringNavigation(char buffer[100]);

void Menu_Show(bool show);
     
void Menu_OpenItemTime(void);
/// Функция временно включает отображение строки навигации меню, если задано настройками.
void Menu_TemporaryEnableStrNavi(void);
/// Ввыполнить эту функцию после Menu_UpdateInput().
void Menu_RunAfterUpdate(pFuncVV func);

/** @}
 */
