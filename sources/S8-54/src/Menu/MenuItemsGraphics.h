#pragma once
#include "MenuItems.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GovernorColor_Draw(GovernorColor *govColor, int x, int y, bool opened);
void Governor_Draw(Governor *governor, int x, int y, bool opened);
void IPaddress_Draw(IPaddress *ip, int x, int y, bool opened);
void MACaddress_Draw(MACaddress *mac, int x, int y, bool opened);
void Formula_Draw(Formula *formula, int x, int y, bool opened);
void Choice_Draw(Choice *choice, int x, int y, bool opened);
void Button_Draw(Button *button, int x, int y);
void SmallButton_Draw(SmallButton *smallButton, int x, int y);
void Page_Draw(Page *page, int x, int y);
void Time_Draw(Time *time, int x, int y, bool opened);
