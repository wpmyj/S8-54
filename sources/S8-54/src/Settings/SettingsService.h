#pragma once

#include "Settings/Settings.h"

#define MATH_NEED_DRAW  (set.math.modeDraw != ModeDrawMath_Disable)
#define MATH_ENABLED    (MATH_NEED_DRAW || set.math.enableFFT)
#define LANG            (set.common.lang)
#define LANG_RU         (LANG == Russian)
#define LANG_EN         (LANG == English)

float   sService_MathGetFFTmaxDBabs(void);
