#pragma once


#include "Settings.h"


#define TRIGLEV(ch)         (set.trig.levelRel[ch])

#define TRIG_MODE_FIND      (set.trig.modeFind)
#define TRIG_MODE_FIND_HAND (TRIG_MODE_FIND == TrigModeFind_Hand)
#define TRIG_MODE_FIND_AUTO (TRIG_MODE_FIND == TrigModeFind_Auto)

#define TRIGSOURCE          (set.trig.source)
#define TRIGSOURCE_A        (TRIGSOURCE == TrigSource_A)
#define TRIGSOURCE_B        (TRIGSOURCE == TrigSource_B)
#define TRIGSOURCE_EXT      (TRIGSOURCE == TrigSource_Ext)

#define START_MODE          (set.trig.startMode)
#define START_MODE_AUTO     (START_MODE == StartMode_Auto)
#define START_MODE_WAIT     (START_MODE == StartMode_Wait)
#define START_MODE_SINGLE   (START_MODE == StartMode_Single)

#define TRIG_POLARITY       (set.trig.polarity)
#define TRIG_POLARITY_FRONT (TRIG_POLARITY == TrigPolarity_Front)

#define TRIG_INPUT          (set.trig.input)
#define TRIG_INPUT_AC       (TRIG_INPUT == TrigInput_AC)
#define TRIG_INPUT_LPF      (TRIG_INPUT == TrigInput_LPF)
#define TRIG_INPUT_FULL     (TRIG_INPUT == TrigInput_Full)
