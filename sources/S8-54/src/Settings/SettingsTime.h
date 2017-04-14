#pragma once
#include "SettingsTypes.h"
#include "defines.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsTime Settings Time
 *  @{
 */

#define SET_TSHIFT          (set.time.tShiftRel)
#define SET_TBASE           (set.time.tBase)
#define TIME_DIVXPOS        (set.time.timeDivXPos)

#define SET_PEACKDET        (set.time.peackDet)
#define SET_PEACKDET_EN     (SET_PEACKDET == PeackDet_Enable)
#define SET_PEACKDET_DIS    (SET_PEACKDET == PeackDet_Disable)

#define IN_RANDOM_MODE      (SET_TBASE < TBase_50ns)
#define IN_P2P_MODE         (SET_TBASE >= MIN_TBASE_P2P)

#define TPOS                (set.time.tPos)
#define TPOS_RIGHT          (TPOS == TPos_Right)
#define TPOS_LEFT           (TPOS == TPos_Left)
#define TPOS_CENTER         (TPOS == TPos_Center)

#define SAMPLE              (set.time.sampleType)
#define SAMPLE_REAL         (SAMPLE == SampleType_Real)
#define SAMPLE_OLD          (set.time.sampleTypeOld)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Сохранить масштаб по времени
void sTime_SetTBase(TBase tBase);
/// Сохранить смещение по времени в относительных единицах
void sTime_SetTShift(int16 shift);
/// Узнать привязку отсительно уровня синхронизации в байтах
int sTime_TPosInBytes(void);
/// Минимальное смещение по времени, которое может быть записано в аппаратную часть
int16 sTime_TShiftMin(void);
/// Смещение по времени, соответствующее позиции TPos
int16 sTime_TShiftZero(void);

int sTime_TShiftInPoints(void);

int sTime_TPosInPoints(void);

/** @}  @}
 */
