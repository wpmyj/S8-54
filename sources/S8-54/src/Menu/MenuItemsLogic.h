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



/// »змен€ет значение в текущей позиции при открытом элементе.
void IPaddress_ChangeValue(IPaddress *ip, int delta);
/// ¬озвращает номер текущего байта (4 - номер порта) и номер текущей позиции в байте.
void IPaddress_GetNumPosIPvalue(int *numIP, int *selPos);


/** @}  @}  @}
 */
