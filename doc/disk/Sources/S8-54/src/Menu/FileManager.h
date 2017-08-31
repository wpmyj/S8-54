#pragma once 
#include "defines.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Menu
 *  @{
 *  @defgroup FileManager File Manager
 *  @{
 */

/// Вызывается один раз при обнаружении новой флешки
void FM_Init(void);
/// Нарисовать файловый менеджер
void FM_Draw(void);

void PressSB_FM_LevelUp(void);

void PressSB_FM_LevelDown(void);

void FM_RotateRegSet(int angle);

bool FM_GetNameForNewFile(char name[255]);

/** @}
 */
