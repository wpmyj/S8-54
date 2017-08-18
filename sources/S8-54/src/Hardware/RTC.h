#pragma once
#include "defines.h"
#include "Data/DataSettings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Hardware
 *  @{
 *  @defgroup RTC
 *  @brief Работа с часами реального времени
 *  @{
 */

/// Инициализация
void RTC_Init(void);

bool RTC_SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds);

PackedTime RTC_GetPackedTime(void);

void RTC_SetCorrection(int8 correction);

/** @}  @}
 */
