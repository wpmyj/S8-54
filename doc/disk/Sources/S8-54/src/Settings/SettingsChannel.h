#pragma once
#include "Data/DataSettings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsChannel Settings Channel
 *  @{
 */

#define SET_INVERSE(ch)         (set.chan_Inverse[ch])
#define SET_INVERSE_A           (SET_INVERSE(A))
#define SET_INVERSE_B           (SET_INVERSE(B))
#define SET_RSHIFT_MATH         (set.math_RShift)
#define SET_RANGE_MATH          (set.math_Range)

#define SET_RANGE(ch)           (set.chan_Range[ch])
#define SET_RANGE_A             (SET_RANGE(A))
#define SET_RANGE_B             (SET_RANGE(B))
#define SET_RANGE_2mV(ch)       (SET_RANGE(ch) == Range_2mV)

#define SET_RSHIFT(ch)          (set.chan_RShiftRel[ch])
#define SET_RSHIFT_A            (SET_RSHIFT(A))
#define SET_RSHIFT_B            (SET_RSHIFT(B))

#define SET_COUPLE(ch)          (set.chan_ModeCouple[ch])
#define SET_COUPLE_A            (SET_COUPLE(A))
#define SET_COUPLE_B            (SET_COUPLE(B))

#define SET_DIVIDER(ch)         (set.chan_Divider[ch])
#define SET_DIVIDER_A           (SET_DIVIDER(A))
#define SET_DIVIDER_B           (SET_DIVIDER(B))
#define SET_DIVIDER_10(ch)      (SET_DIVIDER(ch) == Divider_10)
#define SET_DIVIDER_1(ch)       (SET_DIVIDER(ch) == Divider_1)

#define SET_ENABLED(ch)         (set.chan_Enable[ch])
#define SET_ENABLED_A           (SET_ENABLED(A))
#define SET_ENABLED_B           (SET_ENABLED(B))

#define SET_BALANCE_ADC(ch)     (set.chan_BalanceShiftADC[ch])
#define SET_BALANCE_ADC_A       (SET_BALANCE_ADC(A))
#define SET_BALANCE_ADC_B       (SET_BALANCE_ADC(B))

#define SET_BANDWIDTH(ch)       (set.chan_Bandwidth[ch])
#define SET_BANDWIDTH_A         (SET_BANDWIDTH(A))
#define SET_BANDWIDTH_B         (SET_BANDWIDTH(B))
#define SET_BANDWIDTH_20MHz(ch) (SET_BANDWIDTH(ch) == Bandwidth_20MHz)

#define SET_RESISTANCE(ch)      (set.chan_Resistance[ch])
#define SET_RESISTANCE_A        (SET_RESISTANCE(A))
#define SET_RESISTANCE_B        (SET_RESISTANCE(B))

#define SET_CALIBR_MODE(ch)     (set.chan_CalibrationMode[ch])
#define SET_CALIBR_MODE_A       (SET_CALIBR_MODE(A))
#define CALIBR_MODE_B           (SET_CALIBR_MODE(B))


void sChannel_SetRange(Channel ch, Range range);           // Установить масштаб по напряжению канала ch.
int sChannel_MultiplierRel2Abs(Divider divider);
void sChannel_SetEnabled(Channel ch, bool enabled);

const char *sChannel_Range2String(Range range, Divider divider);
const char *sChannel_RShift2String(uint16 rShiftRel, Range range, Divider divider, char buffer[20]);

/** @}  @}
 */
