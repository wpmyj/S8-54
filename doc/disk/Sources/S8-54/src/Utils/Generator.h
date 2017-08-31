#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Utils
 *  @{
 *  @defgroup Generator
 *  @{
 */

typedef enum
{
    Wave_Sinus,
    Wave_Meander
} TypeWave;

typedef struct
{
    void(*SetParametersWave)(Channel ch, TypeWave typeWave, float frequency, float startAngle, float amplWave, float amplNoise);    // numWave = 1, 2. Может быть до двух сигналов.
                                                                                                    // amplWave = 0.0f...1.0f, amplNoise = 0.0f...1.0f - относительно амплитуды сигнала.
    void(*StartNewWave)(Channel ch);
    uint8(*GetSampleWave)(Channel ch);
} SGenerator;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const SGenerator Generator;

/** @}  @}
 */
