#pragma once
#include "defines.h"
#include "MenuItems.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern int8 gCurDigit;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void    Choice_StartChange(Choice *choice, int delta);
float   Choice_Step(Choice *choice);                            // Рассчитывает следующий кадр анимации.
void    Choice_ChangeIndex(Choice *choice, int delta);          // Изменяет значение choice в зависимости от величины и знака delta.

void    Governor_StartChange(Governor *governor, int detla);    // Запускает процессс анимации инкремента или декремента элемента меню типа Governor (в зависимости от знака delta)
float   Governor_Step(Governor *governor);                      // Рассчитывате следующий кадр анимации.
void    Governor_ChangeValue(Governor *governor, int delta);    // Изменяет значение в текущей позиции при раскрытом элементе
int16   Governor_NextValue(Governor *governor);                 // Возвращает следующее большее значение, которое может принять governor.
int16   Governor_PrevValue(Governor *governor);                 // Возвращает следующее меньшее значение, которое может принять governor.
void    Governor_NextPosition(Governor *governor);              // При открытом элементе переставляет курсор на следующую позицию
int     Governor_NumDigits(Governor *governor);                 // Возвращает число знакомест в поле для ввода элемента governor. Зависит от максимального значения, которое может принимать governor.

void    IPaddress_NextPosition(IPaddress *ip);                  // При открытом элементе переставляет курсор на следующую позицию
void    IPaddress_ChangeValue(IPaddress *ip, int delta);        // Изменяет значение в текущей позиции при открытом элементе
void    IPaddress_GetNumPosIPvalue(int *numIP, int *selPos);        // Возвращает номер текущего байта (4 - номер порта) и номер текущей позиции в байте.

void    MACaddress_ChangeValue(MACaddress *mac, int delta);

void    Time_SetOpened(Time *time);
void    Time_SetNewTime(Time *time);
void    Time_SelectNextPosition(Time *time);
void    Time_IncCurrentPosition(Time *time);
void    Time_DecCurrentPosition(Time *time);

void    GovernorColor_ChangeValue(GovernorColor *governor, int delta);  // Изменить яркость цвета в governor
