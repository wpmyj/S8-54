#pragma once


#include "defines.h"
#include "Panel/Controls.h"


#define COMMON_BEGIN_SB_EXIT  {"Выход", "Exit"}, {"Кнопка для выхода в предыдущее меню", "Button to return to the previous menu" }, EmptyFuncBV


const void* PageForButton(PanelButton button);          // Возвращает страницу меню, которая должна открываться по нажатию кнопки button.
bool IsMainPage(void *item);                            // Возвращает true, если item - адрес главной страницы меню.


void DrawMenuCursVoltage(int x, int y, bool top, bool bottom);
void CalculateConditions(int16 pos0, int16 pos1, CursCntrl cursCntrl, bool *cond0, bool *cond1);    // Рассчитывает условия отрисовки УГО малых кнопок управления выбором курсорами.
void DrawMenuCursTime(int x, int y, bool left, bool right);
void DrawSB_Exit(int x, int y);
