#pragma once


#include "Settings/Settings.h"


#define RECORDER_MODE       (set.service.recorder)

#define LANG                (set.common.lang)
#define LANG_RU             (LANG == Russian)
#define LANG_EN             (LANG == English)

#define TIME_SHOW_LEVELS    (set.display.timeShowLevels)

float   sService_MathGetFFTmaxDBabs(void);
