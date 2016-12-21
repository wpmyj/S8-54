// ������ - ���������� ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Utils/GlobalFunctions.h"
#include "Display/Symbols.h"
#include "Definition.h"
#include "Hardware/Sound.h"
#include "Utils/Math.h"

extern const Page mspCursFFT;
extern const Page mspServiceMathFunction;
extern const Page mspFFT;
extern const Page mspServiceMath;

void Reg_FFT_Cursors(int angle)
{
    set.math.posCur[set.math.currentCursor] += (uint8)angle;
    Sound_RegulatorShiftRotate();
}

void DrawSB_FFTcurs_Source(int x, int y)
{
    Painter_DrawText(x + 7, y + 5, set.math.currentCursor == 0 ? "1" : "2");
}

void PressSB_FFTcurs_Source(void)
{
    set.math.currentCursor = (set.math.currentCursor + 1) % 2;
}

static const SmallButton sbCursFFTSource =
{
    Item_SmallButton, &mspCursFFT,
    {
        "��������",
        "Source"
    },
    {
        "����� ��������� ��� ������� �������",
        "Source choice for calculation of a range"
    },
    EmptyFuncBV,
    PressSB_FFTcurs_Source,
    DrawSB_FFTcurs_Source
};

static void PressSB_MathFunction_Type(void)
{
    CircleIncreaseInt8((int8*)&set.math.mathFunc, 0, 1);
}

static void DrawSB_MathFunction_Sum(int x, int y)
{
    Painter_DrawHLine(y + 9, x + 4, x + 14);
    Painter_DrawVLine(x + 9, y + 4, y + 14);
}

static void DrawSB_MathFunction_Mul(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 4, y + 3, SYMBOL_MATH_FUNC_MUL);
    Painter_SetFont(TypeFont_8);
}

static void DrawSB_MathFunction_Type(int x, int y)
{
    const pFuncVII funcs[2] = {DrawSB_MathFunction_Sum, DrawSB_MathFunction_Mul};
    funcs[set.math.mathFunc](x, y);
}


static const SmallButton sbMathFunction =
{
    Item_SmallButton, &mspServiceMathFunction,
    {
        "���",
        "Type"
    },
    {
        "����� �������������� �������",
        "Choice of mathematical function"
    },
    EmptyFuncBV,
    PressSB_MathFunction_Type,
    DrawSB_MathFunction_Type,
    {
        { DrawSB_MathFunction_Sum,      "��������",     "Addition"       },
        { DrawSB_MathFunction_Mul,      "���������",    "Multiplication" }
    }
};

static void PressSB_MathFunction_ModeDraw(void)
{
    if (set.math.enableFFT)
    {
        Display_ShowWarning(ImpossibleEnableMathFunction);
    }
    else
    {
        CircleIncreaseInt8((int8*)&set.math.modeDraw, 0, 2);
    }
}

static void DrawSB_MathFunction_ModeDraw_Separate(int x, int y)
{
    Painter_DrawRectangle(x + 3, y + 5, 13, 9);
    Painter_DrawHLine(y + 9, x + 3, x + 16);
    Painter_DrawHLine(y + 10, x + 3, x + 16);
}

static void DrawSB_MathFunction_ModeDraw_Disable(int x, int y)
{
    Painter_DrawText(x + 2 + (LANG_EN ? 2 : 0), y + 5, LANG_RU ? "���" : "Dis");
}

static void DrawSB_MathFunction_ModeDraw_Together(int x, int y)
{
    Painter_DrawRectangle(x + 3, y + 5, 13, 9);
}

static void DrawSB_MathFunction_ModeDraw(int x, int y)
{
    const pFuncVII funcs[3] =
    {
        DrawSB_MathFunction_ModeDraw_Disable,
        DrawSB_MathFunction_ModeDraw_Separate,
        DrawSB_MathFunction_ModeDraw_Together
    };
    funcs[set.math.modeDraw](x, y);
}

static const SmallButton sbMathFunctionModeDraw =
{
    Item_SmallButton, &mspServiceMathFunction,
    {
        "�����",
        "Display"
    },
    {
        "�������� ����� ����������� ��������������� �������",
        "Chooses the mode of display of a mathematical signal"
    },
    EmptyFuncBV,
    PressSB_MathFunction_ModeDraw,
    DrawSB_MathFunction_ModeDraw,
    {
        {DrawSB_MathFunction_ModeDraw_Disable,  "����� �������������� ������� ��������",                        "The conclusion of mathematical function is disconnected"},
        {DrawSB_MathFunction_ModeDraw_Separate, "������� � �������������� ������� ��������� � ������ �����",    "Signals and mathematical function are removed in different windows"},
        {DrawSB_MathFunction_ModeDraw_Together, "������� � �������������� ������� ��������� � ����� ����",      "Signals and mathematical function are removed in one window"}
    }
};

static void PressSB_MathFunction_RangeA(void)
{
    set.math.range = set.chan[A].range;
    set.math.divider = set.chan[A].divider;
}

static void DrawSB_MathFunction_RangeA(int x, int y)
{
    Painter_DrawChar(x + 8, y + 5, '1');
}

static const SmallButton sbMathFunctionRangeA =
{
    Item_SmallButton, &mspServiceMathFunction,
    {
        "������� 1-�� ������",
        "Scale of the 1st channel"
    },
    {
        "���� ������� ��� ��������������� ������� �� ������� ������",
        "Takes scale for a mathematical signal from the first channel"
    },
    EmptyFuncBV,
    PressSB_MathFunction_RangeA,
    DrawSB_MathFunction_RangeA
};

static void PressSB_MathFunction_RangeB(void)
{
    set.math.range = set.chan[B].range;
    set.math.divider = set.chan[B].divider;
}

static void DrawSB_MathFunction_RangeB(int x, int y)
{
    Painter_DrawChar(x + 8, y + 5, '2');
}

static const SmallButton sbMathFunctionRangeB =
{
    Item_SmallButton, &mspServiceMathFunction,
    {
        "������� 2-�� ������",
        "Scale of the 2nd channel"
    },
    {
        "���� ������� ��� ��������������� ������� �� ������� ������",
        "Takes scale for a mathematical signal from the second channel"
    },
    EmptyFuncBV,
    PressSB_MathFunction_RangeB,
    DrawSB_MathFunction_RangeB
};

static void PressSB_MathFunction_ModeRegSet(void)
{
    CircleIncreaseInt8((int8*)&set.math.modeRegSet, 0, 1);
}

static void DrawSB_MathFunction_ModeRegSet_Range(int x, int y)
{
    Painter_DrawChar(x + 7, y + 5, LANG_RU ? 'M' : 'S');
}

static void DrawSB_MathFunction_ModeRegSet_RShift(int x, int y)
{
    Painter_DrawText(x + 5 - (LANG_EN ? 3 : 0), y + 5, LANG_RU ? "��" : "Shif");
}

static void DrawSB_MathFunction_ModeRegSet(int x, int y)
{
    const pFuncVII funcs[2] = {DrawSB_MathFunction_ModeRegSet_Range, DrawSB_MathFunction_ModeRegSet_RShift};
    funcs[set.math.modeRegSet](x, y);
}

static const SmallButton sbMathFunctionModeRegSet =
{
    Item_SmallButton, &mspServiceMathFunction,
    {
        "����� ����� ���������",
        "Mode regulator SET"
    },
    {
        "����� ������ ����� ��������� - ���������� ��������� ��� ���������",
        "Choice mode regulcator ��������� - management of scale or shift"
    },
    EmptyFuncBV,
    PressSB_MathFunction_ModeRegSet,
    DrawSB_MathFunction_ModeRegSet,
    {
        {DrawSB_MathFunction_ModeRegSet_Range,  "���������� ���������", "Management of scale"},
        {DrawSB_MathFunction_ModeRegSet_RShift, "���������� ���������", "Management of shift"}
    }
};
    
// ������ -> ���������� -> ������� -> �������
static bool ActiveF_MathFormula(void)
{
    return set.math.mathFunc == Function_Mul || set.math.mathFunc == Function_Sum;
}

static void ChangeF_MathFormula(void)
{

}
                                    
static int8 curDigit = 0;
const Formula mfMathFormula =
{
    Item_Formula, &mspServiceMathFunction,
    {
        "�������", "Formulf"
    },
    {
        "����� �������� ������������ � ����� � �������������� �������",
        "Here you can set the coefficients and signs in a mathematical formula"
    },
    ActiveF_MathFormula,
    (int8*)&set.math.mathFunc, (int8*)&set.math.koeff1add, (int8*)&set.math.koeff2add, (int8*)&set.math.koeff1mul, (int8*)&set.math.koeff2mul, &curDigit, ChangeF_MathFormula
};

/// ������ -> ���������� -> ������� -> �������
static bool ActiveF_Math_Scale(void)
{
    return set.math.mathFunc == Function_Mul || set.math.mathFunc == Function_Sum;
}

// ������ -> ���������� -> ������ -> �����������
const Choice mcFFTenable =
{
    Item_Choice, &mspFFT,   {"�����������", "Display"},
    {
        "�������� � ��������� ����������� �������",
        "Enables or disables the display of the spectrum"
    },
    EmptyFuncBV,
    {                           {DISABLE_RU,    DISABLE_EN},
                                {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.math.enableFFT, EmptyFuncVB, EmptyFuncVII
};

// ������ -> ���������� -> ������ -> �����
const Choice mcFFTscale =
{
    Item_Choice, &mspFFT,   {"�����",       "Scale"},
    {
        "����� ������� ������ ������� - �������� ��� ���������������",
        "Sets the scale of the output spectrum - linear or logarithmic"
    },
    EmptyFuncBV,
    {                           {"��������",    "Log"},
                                {"��������",    "Linear"}
    },
    (int8*)&set.math.scaleFFT, EmptyFuncVB, EmptyFuncVII
};

// ������ -> ���������� -> ������ -> ��������
const Choice mcFFTsource =
{
    Item_Choice, &mspFFT,   {"��������",    "Source"},
    {
        "����� ��������� ��� ������� �������",
        "Selecting the source for the calculation of the spectrum"
    },
    EmptyFuncBV,
    {                           {"����� 1",     "Channel 1"},
                                {"����� 2",     "Channel 2"},
                                {"����� 1 + 2", "Channel 1 + 2"}
    },
    (int8*)&set.math.sourceFFT, EmptyFuncVB, EmptyFuncVII
};

// ������ -> ���������� -> ������ -> ����
const Choice mcFFTwindow =
{
    Item_Choice, &mspFFT,   {"����",        "Window"},
    {
        "����� ���� ��� ������� �������",
        "Sets the window to calculate the spectrum"
    },
    EmptyFuncBV,
    {                           {"�����������", "Rectangle"},
                                {"��������",    "Hamming"},
                                {"��������",    "Blackman"},
                                {"�����",       "Hann"}
    },
    (int8*)&set.math.windowFFT, EmptyFuncVB, EmptyFuncVII
};

// ������ -> ���������� -> ������ -> ������
bool ActiveC_FFT_Limit(void)
{
    return set.math.scaleFFT == ScaleFFT_Log;
}

const Choice mcFFTrange =
{
    Item_Choice, &mspFFT,   {"��������",  "Range"},
    {
        "����� ����� ������ ������ ���������� �� ��������� �������",
        "Here you can set the limit of monitoring the power spectrum"
    },
    ActiveC_FFT_Limit,
    {                           {"-40��",   "-40dB"},
                                {"-60��",   "-60dB"},
                                {"-80��",   "-80dB"}
    },
    (int8*)&set.math.fftMaxDB, EmptyFuncVB, EmptyFuncVII
};

// ������ -> ���������� -> ������ -> �������
bool ActiveP_FFT_Cursors(void)
{
    return set.math.enableFFT;
}

static const SmallButton sbExitMathFunction =
{
    Item_SmallButton, &mspServiceMathFunction,
    {
        "�����",
        "Exit"
    },
    {
        "������ ��� ������ � ���������� ����",
        "Button to return to the previous menu"
    },
    EmptyFuncBV,
    EmptyFuncVV,
    DrawSB_Exit
};

static void OnRegSetMathFunction(int delta)
{
    if (!MATH_NEED_DRAW)
    {
        return;
    }

    if (set.math.modeRegSet == ModeRegSet_RShift)
    {
        uint16 prevRShift = set.math.rShift;
        uint16 rShift = prevRShift;
        if (delta > 0)
        {
            if (rShift < RShiftMax)
            {
                rShift += 4 * STEP_RSHIFT;
                LIMIT_ABOVE(rShift, RShiftMax);
                if (prevRShift < RShiftZero && rShift > RShiftZero)
                {
                    rShift = RShiftZero;
                }
                Sound_RegulatorShiftRotate();
                set.math.rShift = rShift;
            }
        }
        else if (delta < 0)
        {
            if (rShift > RShiftMin)
            {
                rShift -= 4 * STEP_RSHIFT;
                LIMIT_BELOW(rShift, RShiftMin);
                if (prevRShift > RShiftZero && rShift < RShiftZero)
                {
                    rShift = RShiftZero;
                }
                Sound_RegulatorShiftRotate();
                set.math.rShift = rShift;
            }
        }
    }
    if (set.math.modeRegSet == ModeRegSet_Range)
    {
        static int sum = 0;
        sum -= delta;

        float rShiftAbs = RSHIFT_2_ABS(set.math.rShift, set.math.range);

        if (sum > 2)
        {
            if (set.math.range < RangeSize - 1)
            {
                set.math.range++;
                set.math.rShift = (int16)Math_RShift2Rel(rShiftAbs, set.math.range);
                Sound_RegulatorSwitchRotate();
            }
            sum = 0;
        }
        else if (sum < -2)
        {
            if (set.math.range > 0)
            {
                set.math.range--;
                set.math.rShift = (int16)Math_RShift2Rel(rShiftAbs, set.math.range);
                Sound_RegulatorSwitchRotate();
            }
            sum = 0;
        }
    }
}

static bool FuncOfActiveServiceMathFunction(void)
{
    return !set.math.enableFFT;
}

static void FuncOfPressServiceMathFunction(void)
{
    if (set.math.enableFFT)
    {
        Display_ShowWarning(ImpossibleEnableMathFunction);
    }
}

// ������ - ���������� - ������� /////////////////////////////////////////////////////////////////////////////
const Page mspServiceMathFunction =
{
    Item_Page, &mspServiceMath,
    {
        "�������", "FUNCTION"
    },
    {
        "��������� � ����� �������������� ������� - �������� ��� ���������",
        "Installation and selection of mathematical functions - addition or multiplication"
    },
    FuncOfActiveServiceMathFunction, Page_SB_MathFunction,
    {
        (void*)&sbExitMathFunction,
        (void*)&sbMathFunctionModeDraw,
        (void*)&sbMathFunction,
        (void*)&sbMathFunctionModeRegSet,
        (void*)&sbMathFunctionRangeA,
        (void*)&sbMathFunctionRangeB
    },
    FuncOfPressServiceMathFunction,
    EmptyFuncVV,
    OnRegSetMathFunction
};

static void PressSB_CursFFT_Exit(void)
{
    Display_RemoveAddDrawFunction();
}

const SmallButton sbExitCursFFT =
{
    Item_SmallButton, &mspCursFFT,
    COMMON_BEGIN_SB_EXIT
    PressSB_CursFFT_Exit,
    DrawSB_Exit
};

///////////////////////////////////////////////////////////////////////////////////
const Page mspCursFFT =
{
    Item_Page, &mspFFT,
    {
        "�������", "CURSORS"
    },
    {
        "�������� ������� ��� ��������� ���������� �������",
        "Includes cursors to measure the parameters of the spectrum"
    },
    ActiveP_FFT_Cursors, Page_SB_MathCursorsFFT,
    {
        (void*)&sbExitCursFFT,
        (void*)&sbCursFFTSource,
        (void*)0,
        (void*)0,
        (void*)0,
        (void*)0
    },
    EmptyFuncVV, EmptyFuncVV, Reg_FFT_Cursors
};

static bool FuncOfActiveFFT(void)
{
    return !MATH_NEED_DRAW;
}

static void FuncOfPressFFT(void)
{
    if (!FuncOfActiveFFT())
    {
        Display_ShowWarning(ImpossibleEnableFFT);
    }
}

// ������ - ���������� - ������� /////////////////////////////////////////////////////////////////////////////
const Page mspFFT =
{
    Item_Page, &mspServiceMath,
    {
        "������", "SPECTRUM"
    },
    {
        "����������� ������� �������� �������",
        "Mapping the input signal spectrum"
    },
    FuncOfActiveFFT, Page_MathFFT,
    {
        (void*)&mcFFTenable,
        (void*)&mcFFTscale,
        (void*)&mcFFTsource,
        (void*)&mcFFTwindow,
        (void*)&mspCursFFT,
        (void*)&mcFFTrange
    },
    FuncOfPressFFT
};


// ������ - ���������� /////////////////////////////////////////////////////////////////////////////
const Page mspServiceMath =
{
    Item_Page, &mpService,
    {
        "����������", "MATH"
    },
    {
        "�������������� ������� � ���",
        "Mathematical functions and FFT"
    },
    EmptyFuncBV, Page_Math,
    {
        (void*)&mspServiceMathFunction,
        (void*)&mspFFT
    }
};
