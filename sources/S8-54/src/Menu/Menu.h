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
class Menu
{
public:
    /// Инициализация
    void Init(void);
    /// Функция должна вызываться в главном цикле.
    void UpdateInput(void);
    /// Функция обработки короткого нажатия кнопки (менее 0.5 сек.).
    void ShortPressureButton(PanelButton button);
    /// Функция обработки длинного нажатия кнопки (более 0.5 сек.).
    void LongPressureButton(PanelButton button);
    /// Функция вызывается, когда кнопка переходит из отжатого в нажатое положение.
    void PressButton(PanelButton button);
    /// Функция вызывается, когда кнопка переходит из нажатого в отжатое положение.
    void ReleaseButton(PanelButton button);
    /// Функция вызывается в момент нажатия гувернёра.
    void PressReg(PanelRegulator reg);
    /// Функция обработки поворота ручки УСТАНОВКА вправо.
    void RotateRegSetRight(void);
    /// Функция обработки поворота ручки УСТАНОВКА влево.
    void RotateRegSetLeft(void);
    /// Установить время автоматического сокрытия меню в соответствии с установками.
    void SetAutoHide(bool active);
    /// Возвращает адрес пункта меню, находящегося под нажатой в данный момент кнопкой.
    void *ItemUnderKey(void);
    /// Возвращает путь к текущему пункту меню в текстовом виде, готовом к выводу на экран.
    char *StringNavigation(char buffer[100]);

    void Show(bool show);

    void OpenItemTime(void);
    /// Функция временно включает отображение строки навигации меню, если задано настройками.
    void TemporaryEnableStrNavi(void);
    /// Ввыполнить эту функцию после Menu_UpdateInput().
    void RunAfterUpdate(pFuncVV func);
};


extern Menu menu;


/** @}
 */
