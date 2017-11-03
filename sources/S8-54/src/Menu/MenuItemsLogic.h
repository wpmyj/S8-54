#pragma once
#include "defines.h"
#include "MenuItems.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern int8 gCurDigit;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Menu
 *  @{
 *  @addtogroup MenuItems
 *  @{
 *  @defgroup MenuItmesLogic Menu Items Logic
 *  @{
 */


/// Рассчитывате следующий кадр анимации.
float Governor_Step(Governor *governor);
/// Изменяет значение в текущей позиции при раскрытом элементе.
void Governor_ChangeValue(Governor *governor, int delta);
/// Возвращает следующее большее значение, которое может принять governor.
int16 Governor_NextValue(Governor *governor);
/// Возвращает следующее меньшее значение, которое может принять governor.
int16 Governor_PrevValue(Governor *governor);
/// При открытом элементе переставляет курсор на следующую позицию.
void Governor_NextPosition(Governor *governor);
/// Возвращает число знакомест в поле для ввода элемента governor. Зависит от максимального значения, которое может принимать governor.
int  Governor_NumDigits(Governor *governor);
/// При открытом элементе переставляет курсор на следующую позицию.
void IPaddress_NextPosition(IPaddress *ip);
/// Изменяет значение в текущей позиции при открытом элементе.
void IPaddress_ChangeValue(IPaddress *ip, int delta);
/// Возвращает номер текущего байта (4 - номер порта) и номер текущей позиции в байте.
void IPaddress_GetNumPosIPvalue(int *numIP, int *selPos);

void MACaddress_ChangeValue(MACaddress *mac, int delta);

void Time_SetOpened(Time *time);

void Time_SetNewTime(Time *time);

void Time_SelectNextPosition(Time *time);

void Time_IncCurrentPosition(Time *time);

void Time_DecCurrentPosition(Time *time);
/// Изменить яркость цвета в governor.
void GovernorColor_ChangeValue(GovernorColor *governor, int delta);
/// Установить текущей данную страницу с мылыми кнопками.
void SBPage_SetCurrent(const Page *page);

/** @}  @}  @}
 */
