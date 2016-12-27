#pragma once


#include "SettingsTypes.h"


#define INVERSE(ch)         (set.chan[ch].inverse)

#define RANGE(ch)           (set.chan[ch].range)
#define RANGE_A             (set.chan[A].range)
#define RANGE_B             (set.chan[B].range)

#define RSHIFT(ch)          (set.chan[ch].rShiftRel)
#define RSHIFT_A            (set.chan[A].rShiftRel)
#define RSHIFT_B            (set.chan[B].rShiftRel)

#define COUPLE(ch)          (set.chan[ch].modeCouple)
#define COUPLE_A            (set.chan[A].modeCouple)
#define COUPLE_B            (set.chan[B].modeCouple)

#define RSHIFT_MATH         (set.math.rShift)
#define RANGE_MATH          (set.math.range)

#define START_MODE          (set.trig.startMode)
#define START_MODE_AUTO     (START_MODE == StartMode_Auto)
#define START_MODE_WAIT     (START_MODE == StartMode_Wait)
#define START_MODE_SINGLE   (START_MODE == StartMode_Single)

#define DIVIDER(ch)         (set.chan[ch].divider)
#define DIVIDER_A           (DIVIDER(A))
#define DIVIDER_B           (DIVIDER(B))
#define DIVIDER_10(ch)      (DIVIDER(ch) == Multiplier_10)
#define DIVIDER_1(ch)       (DIVIDER(ch) == Multiplier_1)

#define ENABLE(ch)          (set.chan[ch].enable)
#define ENABLE_A            (ENABLE(A))
#define ENABLE_B            (ENABLE(B))

#define BALANCE_ADC(ch)     (set.chan[ch].balanceShiftADC)
#define BALANCE_ADC_A       (BALANCE_ADC(A))
#define BALANCE_ADC_B       (BALANCE_ADC(B))


void    sChannel_SetRange(Channel ch, Range range);           // Установить масштаб по напряжению канала ch.
int     sChannel_MultiplierRel2Abs(Divider divider);
bool    sChannel_Enabled(Channel ch);                         // Возвращает true, если канал ch включён.
bool    sChannel_NeedForDraw(const uint8 *data, Channel ch, DataSettings *ds);

const char *sChannel_Range2String(Range range, Divider divider);
const char *sChannel_RShift2String(uint16 rShiftRel, Range range, Divider divider, char buffer[20]);
const char *sChannel_TShift2String(int16 tShiftRel, TBase tBase);
