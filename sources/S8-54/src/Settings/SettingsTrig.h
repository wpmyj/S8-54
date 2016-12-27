#pragma once


#include "Settings.h"


#define TRIG_MODE_FIND      (set.trig.modeFind)
#define TRIG_MODE_FIND_HAND (TRIG_MODE_FIND == TrigModeFind_Hand)
#define TRIG_MODE_FIND_AUTO (TRIG_MODE_FIND == TrigModeFind_Auto)

#define TRIGLEV(ch)         (set.trig.levelRel[ch])

#define TRIGSOURCE          (set.trig.source)
#define TRIGSOURCE_A        (TRIGSOURCE == TrigSource_A)
#define TRIGSOURCE_B        (TRIGSOURCE == TrigSource_B)
#define TRIGSOURCE_EXT      (TRIGSOURCE == TrigSource_Ext)
