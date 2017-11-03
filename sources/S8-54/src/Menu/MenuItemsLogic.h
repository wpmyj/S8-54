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



/// При открытом элементе переставляет курсор на следующую позицию.
void IPaddress_NextPosition(IPaddress *ip);
/// Изменяет значение в текущей позиции при открытом элементе.
void IPaddress_ChangeValue(IPaddress *ip, int delta);
/// Возвращает номер текущего байта (4 - номер порта) и номер текущей позиции в байте.
void IPaddress_GetNumPosIPvalue(int *numIP, int *selPos);

void MACaddress_ChangeValue(MACaddress *mac, int delta);

void Time_SetNewTime(Time *time);

void Time_SelectNextPosition(Time *time);

void Time_DecCurrentPosition(Time *time);
/// Изменить яркость цвета в governor.
void GovernorColor_ChangeValue(GovernorColor *governor, int delta);
/// Установить текущей данную страницу с мылыми кнопками.
void SBPage_SetCurrent(const Page *page);

/** @}  @}  @}
 */
