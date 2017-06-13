#pragma once
#include "defines.h"
#include "Panel/Controls.h"
#include "Settings/SettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define COMMON_BEGIN_SB_EXIT  0, {"Выход", "Exit", "Кнопка для выхода в предыдущее меню", "Button to return to the previous menu" }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Возвращает страницу меню, которая должна открываться по нажатию кнопки button.
const void* PageForButton(PanelButton button);
/// Возвращает true, если item - адрес главной страницы меню.
bool IsMainPage(const void *item);

void DrawSB_Exit(int x, int y);
