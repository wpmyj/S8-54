﻿#pragma once
#include "MenuItems.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Menu
 *  @{
 *  @addtogroup MenuItems
 *  @{
 *  @defgroup MenuItemsGraphics Menu Items Graphics
 *  @{
 */


void MACaddress_Draw(MACaddress *mac, int x, int y, bool opened);

void Formula_Draw(Formula *formula, int x, int y, bool opened);

void Choice_Draw(Choice *choice, int x, int y, bool opened);

void Button_Draw(Button *button, int x, int y);

void SmallButton_Draw(SButton *smallButton, int x, int y);

void Page_Draw(Page *page, int x, int y);

void Time_Draw(Time *time, int x, int y, bool opened);

/** @}  @}  @}
 */
