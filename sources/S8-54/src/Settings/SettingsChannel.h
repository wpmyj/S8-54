#pragma once
#include "DataSettings.h"


#define SET_INVERSE(ch)     (set.chan[ch].inverse)
#define SET_INVERSE_A       (SET_INVERSE(A))
#define SET_INVERSE_B       (SET_INVERSE(B))
#define RSHIFT_MATH         (set.math.rShift)
#define RANGE_MATH          (set.math.range)

#define SET_RANGE(ch)       (set.chan[ch].range)
#define SET_RANGE_A         (SET_RANGE(A))
#define SET_RANGE_B         (SET_RANGE(B))
#define SET_RANGE_2mV(ch)   (SET_RANGE(ch) == Range_2mV)

#define SET_RSHIFT(ch)      (set.chan[ch].rShiftRel)
#define SET_RSHIFT_A        (SET_RSHIFT(A))
#define SET_RSHIFT_B        (SET_RSHIFT(B))

#define SET_COUPLE(ch)      (set.chan[ch].modeCouple)
#define SET_COUPLE_A        (SET_COUPLE(A))
#define SET_COUPLE_B        (SET_COUPLE(B))

#define SET_DIVIDER(ch)     (set.chan[ch].divider)
#define SET_DIVIDER_A       (SET_DIVIDER(A))
#define SET_DIVIDER_B       (SET_DIVIDER(B))
#define SET_DIVIDER_10(ch)  (SET_DIVIDER(ch) == Divider_10)
#define SET_DIVIDER_1(ch)   (SET_DIVIDER(ch) == Divider_1)

#define SET_ENABLED(ch)     (set.chan[ch].enable)
#define SET_ENABLED_A       (SET_ENABLED(A))
#define SET_ENABLED_B       (SET_ENABLED(B))

#define BALANCE_ADC(ch)     (set.chan[ch].balanceShiftADC)
#define BALANCE_ADC_A       (BALANCE_ADC(A))
#define BALANCE_ADC_B       (BALANCE_ADC(B))

#define BANDWIDTH(ch)       (set.chan[ch].bandwidth)
#define BANDWIDTH_A         (BANDWIDTH(A))
#define BANDWIDTH_B         (BANDWIDTH(B))
#define BANDWIDTH_20MHz(ch) (BANDWIDTH(ch) == Bandwidth_20MHz)

#define RESISTANCE(ch)      (set.chan[ch].resistance)
#define RESISTANCE_A        (RESISTANCE(A))
#define RESISTANCE_B        (RESISTANCE(B))

#define CALIBR_MODE(ch)     (set.chan[ch].calibrationMode)
#define CALIBR_MODE_A       (CALIBR_MODE(A))
#define CALIBR_MODE_B       (CALIBR_MODE(B))


void    sChannel_SetRange(Channel ch, Range range);           // Установить масштаб по напряжению канала ch.
int     sChannel_MultiplierRel2Abs(Divider divider);
bool    sChannel_Enabled(Channel ch);                         // Возвращает true, если канал ch включён.
bool    sChannel_NeedForDraw(const uint8 *data, Channel ch, DataSettings *ds);

const char *sChannel_Range2String(Range range, Divider divider);
const char *sChannel_RShift2String(uint16 rShiftRel, Range range, Divider divider, char buffer[20]);
const char *sChannel_TShift2String(int16 tShiftRel, TBase tBase);
