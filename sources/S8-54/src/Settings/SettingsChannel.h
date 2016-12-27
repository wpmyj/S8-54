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
#define TRIGLEV(ch)         (set.trig.levelRel[ch])
#define TRIGSOURCE          (set.trig.source)
#define TRIGSOURCE_A        (TRIGSOURCE == TrigSource_A)
#define TRIGSOURCE_B        (TRIGSOURCE == TrigSource_B)
#define TRIGSOURCE_EXT      (TRIGSOURCE == TrigSource_Ext)
#define RSHIFT_MATH         (set.math.rShift)
#define RANGE_MATH          (set.math.range)
#define START_MODE          (set.trig.startMode)
#define START_MODE_AUTO     (START_MODE == StartMode_Auto)
#define START_MODE_WAIT     (START_MODE == StartMode_Wait)
#define START_MODE_SINGLE   (START_MODE == StartMode_Single)


void    sChannel_SetRange(Channel ch, Range range);           // Установить масштаб по напряжению канала ch.
int     sChannel_MultiplierRel2Abs(Divider divider);
bool    sChannel_Enabled(Channel ch);                         // Возвращает true, если канал ch включён.
bool    sChannel_NeedForDraw(const uint8 *data, Channel ch, DataSettings *ds);

const char *sChannel_Range2String(Range range, Divider divider);
const char *sChannel_RShift2String(uint16 rShiftRel, Range range, Divider divider, char buffer[20]);
const char *sChannel_TShift2String(int16 tShiftRel, TBase tBase);
