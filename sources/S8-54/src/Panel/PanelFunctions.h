// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Globals.h"
#include "FPGA/FPGA.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Menu/Menu.h"
#include "Menu/MenuFunctions.h"
#include "Settings/Settings.h"
#include "Settings/SettingsDebug.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const uint MIN_TIME = 500;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Long_Help(void)
{
    HINT_MODE_ENABLE = !HINT_MODE_ENABLE;
    gStringForHint = 0;
    gItemHint = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Long_ChannelA(void)
{
    Menu_LongPressureButton(B_Channel1);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Long_ChannelB(void)
{
    Menu_LongPressureButton(B_Channel2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Long_Time(void)
{
    Menu_LongPressureButton(B_Time);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Set_Press(void)
{
    Menu_PressReg(R_Set);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncLongRegRShift(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Func_Start(int key)                    // B_Start
{
    if (key == 1)
    {
        if (MODE_PAUSE_CONSOLE)             // Если кнопка ПУСК/СТОП управляет выводом на консоль
        {
            CONSOLE_IN_PAUSE = CONSOLE_IN_PAUSE ? 0 : 1;
        }
        if (MODE_WORK_DIR)                  // Если кнопка ПУСК/СТОП выполняет стнадартную функцию
        {
            Menu_PressButton(B_Start);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Func_Memory(void)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Long_Start(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EFB(int delta)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Func_Power(int key)                        // B_Power
{
    SET_FLAG(NEED_DISABLE_POWER);
    if (IsPageSB(OpenedItem()))     // Если открата страница малых кнопок,
    {
        CloseOpenedItem();          // то закрываем её
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Long_Menu(void)
{
    Menu_LongPressureButton(B_Menu);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void F1_Long(void)
{
    Menu_LongPressureButton(B_F1);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void F2_Long(void)
{
    Menu_LongPressureButton(B_F2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void F3_Long(void)
{
    Menu_LongPressureButton(B_F3);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void F4_Long(void)
{
    Menu_LongPressureButton(B_F4);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void F5_Long(void)
{
    Menu_LongPressureButton(B_F5);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int CalculateCount(int *prevTime)
{
    uint time = gTimeMS;
    uint delta = time - *prevTime;
    *prevTime = time;

    if (delta > 75)
    {
        return 1;
    }
    else if (delta > 50)
    {
        return 2;
    }
    else if (delta > 25)
    {
        return 3;
    }
    return 4;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool CanChangeTShift(int16 tShift)
{
    static uint time = 0;
    if (tShift == 0)
    {
        time = gTimeMS;
        return true;
    }
    else if (time == 0)
    {
        return true;
    }
    else if (gTimeMS - time > MIN_TIME)
    {
        time = 0;
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool CanChangeRShiftOrTrigLev(TrigSource channel, uint16 rShift)
{
    static uint time[3] = {0, 0, 0};
    if (rShift == RShiftZero)
    {
        time[channel] = gTimeMS;
        return true;
    }
    else if (time[channel] == 0)
    {
        return true;
    }
    else if (gTimeMS - time[channel] > MIN_TIME)
    {
        time[channel] = 0;
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ChangeRShift(int *prevTime, void(*f)(Channel, uint16), Channel ch, int relStep)
{
    if (ENUM_ACCUM == ENumAccum_1)
    {
        FPGA_TemporaryPause();
    }
    int count = CalculateCount(prevTime);
    int rShiftOld = SET_RSHIFT(ch);
    int rShift = SET_RSHIFT(ch) + relStep * count;
    if ((rShiftOld > RShiftZero && rShift < RShiftZero) || (rShiftOld < RShiftZero && rShift > RShiftZero))
    {
        rShift = RShiftZero;
    }

    if (rShift < RShiftMin)
    {
        rShift = RShiftMin - 1;
    }

    if (CanChangeRShiftOrTrigLev((TrigSource)ch, (uint16)rShift))
    {
        Sound_RegulatorShiftRotate();
        f(ch, (uint16)rShift);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ChangeTrigLev(int *prevTime, void(*f)(TrigSource, uint16), TrigSource trigSource, int16 relStep)
{
    int count = CalculateCount(prevTime);
    int trigLevOld = SET_TRIGLEV(trigSource);
    int trigLev = SET_TRIGLEV(trigSource) + relStep * count;
    if ((trigLevOld > TrigLevZero && trigLev < TrigLevZero) || (trigLevOld < TrigLevZero && trigLev > TrigLevZero))
    {
        trigLev = TrigLevZero;
    }

    if (trigLev < TrigLevMin)
    {
        trigLev = TrigLevMin - 1;
    }

    if (CanChangeRShiftOrTrigLev(trigSource, (uint16)trigLev))
    {
        Sound_RegulatorShiftRotate();
        f(trigSource, (uint16)trigLev);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ChangeTShift(int *prevTime, void(*f)(int), int16 relStep)
{
    int count = CalculateCount(prevTime);
    int tShiftOld = SET_TSHIFT;
    int step = relStep * count;
    if (step < 0)
    {
        if (step > -1)
        {
            step = -1;
        }
    }
    else
    {
        if (step < 1)
        {
            step = 1;
        }
    }

    int16 tShift = SET_TSHIFT + (int16)step;
    if (((tShiftOld > 0) && (tShift < 0)) || (tShiftOld < 0 && tShift > 0))
    {
        tShift = 0;
    }
    if (CanChangeTShift(tShift))
    {
        Sound_RegulatorShiftRotate();
        f(tShift);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ChangeShiftScreen(int *prevTime, void(*f)(int), int16 relStep)
{
    int count = CalculateCount(prevTime);
    int step = relStep * count;
    if (step < 0)
    {
        if (step > -1)
        {
            step = -1;
        }
    }
    else if (step < 1)
    {
        step = 1;
    }
    f(step);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncRShiftA(int delta)
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, FPGA_SetRShift, A, delta * STEP_RSHIFT);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncRShiftB(int delta)
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, FPGA_SetRShift, B, delta * STEP_RSHIFT);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncTrigLev(int delta)
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, FPGA_SetTrigLev, TRIGSOURCE, (int16)(delta * STEP_RSHIFT));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void XShift(int delta)
{
    static int prevTime = 0;
    if (!FPGA_IS_RUNNING || TIME_DIVXPOS == FunctionTime_ShiftInMemory)
    {
        ChangeShiftScreen(&prevTime, Display_ShiftScreen, (int16)(2 * delta));
    }
    else
    {
        ChangeTShift(&prevTime, FPGA_SetTShift, (int16)delta);
        FPGA_ChangePostValue(delta);                           /// \todo temp for С8_54
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void XShiftLeft(void)
{
    XShift(-1);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void XShiftRigth(void)
{
    XShift(1);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncTShift(int delta)
{
    XShift(delta);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncTBase(int delta)
{
    Sound_RegulatorSwitchRotate();

    delta == 1 ? FPGA_TBaseDecrease() : FPGA_TBaseIncrease();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ChangeRange(Channel ch, int delta)    // delta == -1 - уменьшаем. delta == 1 - увеличиваем
{
    Sound_RegulatorSwitchRotate();
    if (delta > 0)
    {
        FPGA_RangeIncrease(ch);
    }
    else
    {
        FPGA_RangeDecrease(ch);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncRangeA(int delta)
{
    LAST_AFFECTED_CH = A;
    ChangeRange(A, -delta);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncRangeB(int delta)
{
    LAST_AFFECTED_CH = B;
    ChangeRange(B, -delta);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncRegSet(int delta)
{
    delta == -1 ? Menu_RotateRegSetLeft() : Menu_RotateRegSetRight();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncBtnRegChannelA(int key)
{
    if (key == 1)
    {
        Long_ChannelA();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncBtnRegChannelB(int key)
{
    if (key == 1)
    {
        Long_ChannelB();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncBtnRegTime(int key)
{
    if (key == 1)
    {
        Long_Time();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncBtnRegTrig(int key)
{
    if (key == 1)
    {
        Menu_LongPressureButton(B_Trig);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Long_Trig(void)
{
    FuncBtnRegTrig(1);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncBtnRegSet(int key)
{
    if (key == 1)
    {
        Set_Press();
    }
}
