#pragma once
#include "Settings/Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Hardware
 *  @{
 *  @defgroup Sound
 *  @brief Работа со звуковым динамиком
 *  @{
 */


typedef enum
{
    TypeWave_Sine,
    TypeWave_Meandr,
    TypeWave_Triangle
} TypeWave;


void Sound_Init(void);
/// Звук нажатия на кнопку
void Sound_ButtonPress(void);
/// Функция вызовет звук отпускаемой кнопки только если перед этим проигрывался звук нажатия кнопки
void Sound_ButtonRelease(void);

void Sound_GovernorChangedValue(void);

void Sound_RegulatorShiftRotate(void);

void Sound_RegulatorSwitchRotate(void);

void Sound_WarnBeepBad(void);

void Sound_WarnBeepGood(void);

/** @}  @}
 */
