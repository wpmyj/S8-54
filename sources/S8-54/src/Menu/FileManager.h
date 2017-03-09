#pragma once 
#include "defines.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FM_Init(void);                     // Вызывается один раз при обнаружении новой флешки
void FM_Draw(void);
void PressSB_FM_LevelUp(void);
void PressSB_FM_LevelDown(void);
void FM_RotateRegSet(int angle);
bool FM_GetNameForNewFile(char name[255]);
