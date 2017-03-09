#pragma once
#include "Panel/Controls.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_IS_SHOWN (set.menu.isShown)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void    Menu_Init(void);
void    Menu_UpdateInput(void);                             // Функция должна вызываться в главном цикле.

void    Menu_ShortPressureButton(PanelButton button);       // Функция обработки короткого нажатия кнопки (менее 0.5 сек.).
void    Menu_LongPressureButton(PanelButton button);        // Функция обработки длинного нажатия кнопки (более 0.5 сек.).
void    Menu_PressButton(PanelButton button);               // Функция вызывается, когда кнопка переходит из отжатого в нажатое положение.
void    Menu_ReleaseButton(PanelButton button);             // Функция вызывается, когда кнопка переходит из нажатого в отжатое положение.
void    Menu_PressReg(Regulator reg);                       // Функция вызывается в момент нажатия гувернёра

void    Menu_RotateRegSetRight(void);                       // Функция обработки поворота ручки УСТАНОВКА вправо.
void    Menu_RotateRegSetLeft(void);                        // Функция обработки поворота ручки УСТАНОВКА влево.
void    Menu_SetAutoHide(bool active);                      // Установить время автоматического сокрытия меню в соответствии с установками.
void*   Menu_ItemUnderKey(void);                            // Возвращает адрес пункта меню, находящегося под нажатой в данный момент кнопкой.
char*   Menu_StringNavigation(char buffer[100]);            // Возвращает путь к текущему пункту меню в текстовом виде, готовом к выводу на экран.

void    Menu_Show(bool show);
void    Menu_OpenItemTime(void);
