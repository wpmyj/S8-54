#pragma once


#include "Settings/Settings.h"


#define FUNC                (set.math.modeDraw)
#define FUNC_ENABLED        (FUNC != ModeDrawMath_Disable)
#define FUNC_SEPARATE       (FUNC == ModeDrawMath_Separate)
#define FUNC_TOGETHER       (FUNC == ModeDrawMath_Together)
#define FFT_ENABLED         (set.math.enableFFT)
#define MATH_ENABLED        (FUNC_ENABLED || FFT_ENABLED)
#define LANG                (set.common.lang)
#define LANG_RU             (LANG == Russian)
#define LANG_EN             (LANG == English)
#define TIME_SHOW_LEVELS    (set.display.timeShowLevels)

float   sService_MathGetFFTmaxDBabs(void);
