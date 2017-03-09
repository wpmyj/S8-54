#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int     GridLeft(void);
int     GridRight(void);
int     GridWidth(void);
float   GridDeltaY(void);
float   GridDeltaX(void);

int     GridWidthInCells(void);

int     GridFullBottom(void);
int     GridFullHeight(void);
int     GridFullCenterHeight(void);

int     GridChannelBottom(void);
int     GridChannelHeight(void);
int     GridChannelCenterHeight(void);

int     GridMathHeight(void);
int     GridMathBottom(void);
int     GridMathTop(void);

int     BottomMessages(void);
void CoordTrigLevel(int *left, int *top, int width);
