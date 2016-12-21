#pragma once


#include "SettingsTypes.h"
#include "defines.h"


#define TSHIFT  (set.time.tShiftRel)
#define TBASE   (set.time.tBase)
#define PEACKDET (set.time.peackDet)
#define PEACKDET_EN (set.time.peackDet == PeackDet_Enable)
#define PEACKDET_DIS (set.time.peackDet == PeackDet_Disable)


void    sTime_SetTBase(TBase tBase);        // Сохранить масштаб по времени.
void    sTime_SetTShift(int16 shift);       // Сохранить смещение по времени в относительных единицах.
int     sTime_TPosInBytes(void);            // Узнать привязку отсительно уровня синхронизации в байтах.
int16   sTime_TShiftMin(void);              // Минимальное смещение по времени, которое может быть записано в аппаратную часть.
int16   sTime_TShiftZero(void);             // Смещение по времени, соответствующее позиции TPos.
bool    sTime_RandomizeModeEnabled(void);   // Если true, значит, работает рандомизатор.
bool    sTime_P2PModeEnabled(void);         // Если true, то работает поточечный режим
int     sTime_TShiftInPoints(void);
int     sTime_TPosInPoints(void);
