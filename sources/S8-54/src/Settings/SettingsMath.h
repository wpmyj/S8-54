#pragma once


#define WINDOW_FFT              (set.math.windowFFT)
#define MAX_DB_FFT              (set.math.fftMaxDB)
#define FFT_ENABLED             (set.math.enableFFT)
#define MATH_ENABLED            (FUNC_ENABLED || FFT_ENABLED)

#define SCALE_FFT               (set.math.scaleFFT)
#define SCALE_FFT_LOG           (SCALE_FFT == ScaleFFT_Log)
#define SCALE_FFT_LIN           (SCALE_FFT == ScaleFFT_Linear)

#define MATH_FUNC               (set.math.mathFunc)
#define MATH_FUNC_MUL           (MATH_FUNC == Function_Mul)
#define MATH_FUNC_SUM           (MATH_FUNC == Function_Sum)

#define FUNC_MODE_DRAW          (set.math.modeDraw)
#define FUNC_ENABLED            (FUNC_MODE_DRAW != ModeDrawMath_Disable)
#define FUNC_MODE_DRAW_SEPARATE (FUNC_MODE_DRAW == ModeDrawMath_Separate)
#define FUNC_MODE_DRAW_TOGETHER (FUNC_MODE_DRAW == ModeDrawMath_Together)
