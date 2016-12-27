#include "defines.h"
#include "Globals.h"
#include "Log.h"
#include "PainterMem.h"
#include "Symbols.h"
#include "Display.h"
#include "Grid.h"
#include "Painter.h"
#include "font/Font.h"
#include "Colors.h"
#include "PainterData.h"
#include "Ethernet/Ethernet.h"
#include "Ethernet/TcpSocket.h"
#include "FPGA/FPGA.h"
#include "FPGA/DataStorage.h"
#include "FPGA/FreqMeter.h"
#include "FlashDrive/FlashDrive.h"
#include "Hardware/Hardware.h"
#include "Hardware/FSMC.h"
#include "Hardware/FLASH.h" 
#include "Hardware/Sound.h"
#include "Hardware/RTC.h"
#include "Hardware/RAM.h"
#include "Hardware/Timer.h"
#include "Menu/Menu.h"
#include "Menu/MenuDrawing.h"
#include "Menu/MenuFunctions.h"
#include "Menu/Pages/PageCursors.h"
#include "Menu/Pages/PageMemory.h"
#include "Menu/Pages/PageHelp.h"
#include "Settings/Settings.h"
#include "Utils/Measures.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/ProcessingSignal.h"
#include "Utils/Debug.h"
#include "VCP/VCP.h"


#ifdef _MS_VS
#pragma warning(push)
#pragma warning(disable:4204)
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    Warning warning;
    bool good;
    char* message[2][3];
} StructWarning;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const StructWarning warns[Warning_NumWarnings] =
{
    {LimitChan1_Volts, false, {{"ПРЕДЕЛ КАНАЛ 1 - ВОЛЬТ/ДЕЛ"}, {"LIMIT CHANNEL 1 - VOLTS/DIV"}}},
    {LimitChan2_Volts, false, {{"ПРЕДЕЛ КАНАЛ 2 - ВОЛЬТ/ДЕЛ"}, {"LIMIT CHANNEL 2 - VOLTS/DIV"}}},
    {LimitSweep_Time, false, {{"ПРЕДЕЛ РАЗВЕРТКА - ВРЕМЯ/ДЕЛ"}, {"LIMIT SWEEP - TIME/DIV"}}},
    {EnabledPeakDet, false, {{"ВКЛЮЧЕН ПИКОВЫЙ ДЕТЕКТОР"}, {"ENABLED PEAK. DET."}}},
    {LimitChan1_RShift, false, {{"ПРЕДЕЛ КАНАЛ 1 - \x0d"}, {"LIMIT CHANNEL 1 - \x0d"}}},
    {LimitChan2_RShift, false, {{"ПРЕДЕЛ КАНАЛ 2 - \x0d"}, {"LIMIT CHANNEL 2 - \x0d"}}},
    {LimitSweep_Level, false, {{"ПРЕДЕЛ РАЗВЕРТКА - УРОВЕНЬ"}, {"LIMIT SWEEP - LEVEL"}}},
    {LimitSweep_TShift, false, {{"ПРЕДЕЛ РАЗВЕРТКА - \x97"}, {"LIMIT SWEEP - \x97"}}},
    {TooSmallSweepForPeakDet, false, {{"ПИК. ДЕТ. НЕ РАБОТАЕТ НА РАЗВЕРТКАХ МЕНЕЕ 0.5мкс/дел"}, {"PEAK. DET. NOT WORK ON SWEETS LESS THAN 0.5us/div"}}},
    {TooFastScanForRecorder, false, {{"САМОПИСЕЦ НЕ МОЖЕТ РАБОТАТЬ НА БЫСТРЫХ РАЗВЁРТКАХ"}, {"SELF-RECORDER DOES NOT WORK FAST SCANS"}}},
    {FileIsSaved, true, {{"ФАЙЛ СОХРАНЕН"}, {"FILE IS SAVED"}}},
    {SignalIsSaved, true, {{"СИГНАЛ СОХРАНЕН"}, {"SIGNAL IS SAVED"}}},
    {SignalIsDeleted, true, {{"СИГНЛА УДАЛЕН"}, {"SIGNAL IS DELETED"}}},
    {MenuDebugEnabled, true, {{"МЕНЮ ОТЛАДКА ВКЛЮЧЕНО"}, {"MENU DEBUG IS ENABLED"}}},
    {MovingData, true, {{"ПОДОЖДИТЕ НЕСКОЛЬКО СЕКУНД, ИДЕТ ПЕРЕМЕЩЕНИЕ ДАННЫХ"}, {"WAIT A FEW SECONDS, GOES A MOVEMENT DATA"}}},
    {TimeNotSet, true, {{"ВРЕМЯ НЕ УСТАНОВЛЕНО. МОЖЕТЕ УСТАНОВИТЬ ЕГО СЕЙЧАС"}, {"TIME IS NOT SET. YOU CAN INSTALL IT NOW"}}},
    {SignalNotFound, true, {{"СИГНАЛ НЕ НАЙДЕН"}, {"SIGNAL IS NOT FOUND"}}},
    {SetTPosToLeft, true,
    {{"НА РАЗВЕРТКАХ МЕДЛЕННЕЕ 10мс/дел ЖЕЛАТЕЛЬНО УСТАНАВ-", "ЛИВАТЬ \"РАЗВЕРТКА - \x7b\" В ПОЛОЖЕНИЕ \"Лево\" ДЛЯ УСКОРЕ-", "НИЯ ВЫВОДА СИГНАЛА"},
    {"AT SCANNER SLOWLY 10ms/div DESIRABLY SET \"SCAN - \x7b\" IN", "SWITCH \"Left\" FOR TO ACCELERATE THE OUTPUT SIGNAL"}}},
    {VerySmallMeasures, true, {{"ОЧЕНЬ МАЛО ИЗМЕРЕНИЙ"}, {"VERY SMALL MEASURES"}}},
    {NeedRebootDevice, true, {{"Для вступления изменений в силу", "выключите прибор"}, {"FOR THE INTRODUCTION OF CHANGES", "IN FORCE SWITCH OFF THE DEVICE"}}},
    {ImpossibleEnableMathFunction, true, {"Отключите вычисление БПФ", "DISCONNECT CALCULATION OF FFT"}},
    {ImpossibleEnableFFT, true, {{"Отключите математическую функцию"}, {"DISCONNECT MATHEMATICAL FUNCTION"}}},
    {WrongFileSystem, false, {{"Не могу примонтировать флешку. Убедитесь, что на ней FAT32"}, {"I can not mount a usb flash drive. Make sure to her FAT32"}}},
    {WrongModePeackDet, true, {{"Чтобы изменить длину памяти, отключите пиковый детектор"}, {"To change the length of the memory, turn off the peak detector"}}},
    {DisableChannel2, true, {{"Сначала выключите канал 2"}, {"First, turn off the channel 2"}}},
    {TooLongMemory, true, {{"Второй канал рабоатает при длине памяти 16к и менее"}, {"The second channel runs at a length of memory 16k and less"}}},
    {NoPeakDet32k, true, {{"Пиковый детектор не работает при длине памяти 32к"}, {"The peak detector does not work when the memory length of 32k"}}},
    {NoPeakDet16k, true, {{"Для работы пикового детектора при длине памяти 16к", "отключите канал 2"}, {"For the peak detector with a length of 16k memory", "disable channel 2"}}},
    {Warn50Ohms, false, {{"При величине входного сопротивления 50 Ом", "запрещается подавать на вход более 5 В"}, {"When the input impedance value of 50 ohms", "is prohibited to input more than 5 V"}}},
    {WarnNeedForFlashDrive, true, {{"Сначала подключите флеш-диск"}, {"First connect the flash drive"}}}
};

#define  DELTA 5

#define MAX_NUM_STRINGS         35
#define SIZE_BUFFER_FOR_STRINGS 2000
static char                     *strings[MAX_NUM_STRINGS] = {0};
static char                     bufferForStrings[SIZE_BUFFER_FOR_STRINGS] = {0};
static int                      lastStringForPause = -1;


#define NUM_WARNINGS            10
static const char               *warnings[NUM_WARNINGS] = {0};      // Здесь предупреждающие сообщения.
static uint                     timeWarnings[NUM_WARNINGS] = {0};   // Здесь время, когда предупреждающее сообщение поступило на экран.


static pFuncVV funcOnHand       = 0;
static pFuncVV funcAdditionDraw = 0;

static bool showLevelRShiftA = false;   // Нужно ли рисовать горизонтальную линию уровня смещения первого канала
static bool showLevelRShiftB = false;
static bool showLevelTrigLev = false;   // Нужно ли рисовать горизонтальную линию уровня смещения уровня синхронизации

static bool trigEnable = false;
static bool drawRShiftMarkers = false;
static bool needFinishDraw = true;      // Если true, то дисплей нуждается в перерисовке
static uint numDrawingSignals = 0;      // Число нарисованных сигналов для режима накопления


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void InitHardware(void);
static void SetOrientation(void);

static bool NeedForClearScreen(void);

static void DrawFullGrid(void);
static void DrawGrid(int left, int top, int width, int height);
static void DrawGridType1(int left, int top, int right, int bottom, float centerX, float centerY, float deltaX, float deltaY, float stepX, float stepY);
static void DrawGridType2(int left, int top, int right, int bottom, int deltaX, int deltaY, int stepX, int stepY);
static void DrawGridType3(int left, int top, int right, int bottom, int centerX, int centerY, int deltaX, int deltaY, int stepX);
static void DrawGridSpectrum(void);

static void DrawSpectrum(void);
static void DRAW_SPECTRUM(const uint8 *dataIn, int numPoints, Channel ch);
static void DrawSpectrumChannel(float *spectrum, Color color);
static void WriteParametersFFT(Channel ch, float freq0, float density0, float freq1, float density1);

static void DrawCursors(void);                          // Нарисовать курсоры курсорных измерений
static void DrawVerticalCursor(int x, int yTearing);    // Нарисовать вертикальный курсор курсорных измерений. x - числовое значение курсора, yTearing - координата места, в котором необходимо сделать разрыв для квадрата пересечения.
static void DrawHorizontalCursor(int y, int xTearing);  // Нарисовать горизонтальный курсор курсорных измерений. y - числовое значение курсора, xTearing - координата места, в котором необходимо сделать разрыв для квадрата пересечения.

static void DrawCursorsWindow(void);                    // Нарисовать вспомогательные курсоры для смещения по напряжению
static void DrawScaleLine(int x, bool forTrigLev);      // Нарисовать сетку для вспомогательных курсоров по напряжению

static void DrawCursorTrigLevel(void);
static void WriteValueTrigLevel(void);                  // Вывести значение уровня синхронизации
static void DrawCursorsRShift(void);
static void DrawCursorRShift(Channel ch);
static void DrawCursorTShift(void);

static void DrawHiPart(void);                           // Нарисовать то, что находится над основной сеткой
static void WriteCursors(void);                         // Вывести значения курсоров курсорных измерений
static void DrawHiRightPart(void);                      // Вывести то, что находится в правой части верхней строки

static void DrawLowPart(void);                          // Нарисовать то, что находится под основной сеткой
static void DrawTime(int x, int y);
static void WriteTextVoltage(Channel ch, int x, int y); // Написать в нижней строке параметры вертикального тракта

static void DrawMeasures(void);                         // Вывести значения автоматических измерений
static void DrawStringNavigation(void);                 // Вывести строку навигации по меню
static void DrawRandStat(void);                         // Нарисовать график статистики рандомизатора
static void DrawWarnings(void);                         // Вывести предупреждающие сообщения
static void DrawConsole(void);                          // Вывести содержимое консоли
static void DrawTimeForFrame(uint timeTicks);
static void DisableShowLevelRShiftA(void);              // Отключить вспомогательную линию маркера смещения по напряжению первого канала
static void DisableShowLevelRShiftB(void);              // Отключить вспомогательную линию маркера смещения по напряжению второго канала
static void DisableShowLevelTrigLev(void);
static void OnRShiftMarkersAutoHide(void);
static int  FirstEmptyString(void);
static void DeleteFirstString(void);
static void AddString(const char *string);
static void ShowWarn(const char *message);
static void WriteStringAndNumber(char *text, int16 x, int16 y, int number);
static void DrawStringInRectangle(int x, int y, char const *text);
static int  CalculateFreeSize(void);
static void OnTimerShowWarning(void);
static int  CalculateCountV(void);
static int  CalculateCountH(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Display_Init(void)
{
    /*
    Проверка функций рисования 4-х битным цветом в памяти

    static uint8 buffer[100 * 100 / 2];

    PainterMem_SetBuffer(buffer, 100, 100);
    PainterMem_FillRect(0, 0, 99, 99, ColorChannel(A));
    PainterMem_DrawRectangle(10, 10, 10, 30, COLOR_FILL);
    PainterMem_DrawHLine(90, 10, 90, ColorChannel(B));
    PainterMem_DrawVLine(90, 10, 90, COLOR_BACK);
    PainterMem_DrawRectangle(0, 0, 99, 99, COLOR_FILL);
    */

    Painter_ResetFlash();

    InitHardware();

    Painter_LoadPalette(0);
    Painter_LoadPalette(1);
    Painter_LoadPalette(2);

    Painter_LoadFont(TypeFont_5);
    Painter_LoadFont(TypeFont_8);
    Painter_LoadFont(TypeFont_UGO);
    Painter_LoadFont(TypeFont_UGO2);
    Painter_SetFont(TypeFont_8);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_Update(void)
{
    static uint prevLoadPalette = 0;
    bool needReloadPalette = false;

    if(gTimerMS - prevLoadPalette > 1000)
    {
        needReloadPalette = true;
        prevLoadPalette = gTimerMS;
    }

    if(needReloadPalette)
    {
        Painter_LoadPalette(2);
    }

    uint timeStart = gTimerTics;

    if(funcOnHand != 0)
    {
        funcOnHand();
        return;
    }

    ModeFSMC mode = FSMC_GetMode();

    bool needClear = NeedForClearScreen();

    if(needClear)
    {
        Painter_BeginScene(gColorBack);
        SetOrientation();
        PainterData_DrawMemoryWindow();
        DrawFullGrid();
    }

    PainterData_DrawData();

    if(needReloadPalette)
    {
        Painter_LoadPalette(1);
    }

    if(needClear)
    {
        PainterData_DrawMath();
        DrawSpectrum();
        DrawCursors();
        DrawHiPart();
        DrawLowPart();
        DrawCursorsWindow();
        DrawCursorTrigLevel();
        DrawCursorsRShift();
        DrawMeasures();
        DrawStringNavigation();
        DrawCursorTShift();
    }

    FreqMeter_Draw(GridLeft(), GRID_TOP);

    Menu_Draw();

    if(set.debug.showRandStat)
    {
        DrawRandStat();
    }

    if(needClear)
    {
        DrawWarnings();

        if(funcAdditionDraw)
        {
            funcAdditionDraw();
        }
    }

    DrawConsole();

    if(needClear)
    {
        WriteValueTrigLevel();
    }

    DrawTimeForFrame(gTimerTics - timeStart);


    Painter_EndScene();

    if(gMemory.needForSaveToFlashDrive == 1)
    {
        if(Painter_SaveScreenToFlashDrive())
        {
            Display_ShowWarning(FileIsSaved);
        }
        gMemory.needForSaveToFlashDrive = 0;
    }

    if(needReloadPalette)
    {
        Painter_LoadPalette(0);
    }

    FSMC_SetMode(mode);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_RotateRShift(Channel ch)
{
    set.display.lastAffectedChannel = ch;
    if(TIME_SHOW_LEVELS)
    {
        (ch == A) ? (showLevelRShiftA = true) : (showLevelRShiftB = true);
        Timer_SetAndStartOnce((ch == A) ? kShowLevelRShiftA : kShowLevelRShiftB, (ch == A) ? DisableShowLevelRShiftA : DisableShowLevelRShiftB, TIME_SHOW_LEVELS  * 1000);
    };
    Display_Redraw();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_RotateTrigLev(void)
{
    if(TIME_SHOW_LEVELS && TRIG_MODE_FIND_HAND)
    {
        showLevelTrigLev = true;
        Timer_SetAndStartOnce(kShowLevelTrigLev, DisableShowLevelTrigLev, TIME_SHOW_LEVELS * 1000);
    }
    Display_Redraw();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_Redraw(void)
{
    needFinishDraw = true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_EnableTrigLabel(bool enable)
{
    trigEnable = enable;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_ClearFromWarnings(void)
{
    Timer_Disable(kShowMessages);
    for(int i = 0; i < NUM_WARNINGS; i++)
    {
        warnings[i] = 0;
        timeWarnings[i] = 0;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_SetDrawMode(DrawMode mode, pFuncVV func)
{
    funcOnHand = mode == DrawMode_Auto ? 0 : func;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_SetAddDrawFunction(pFuncVV func)
{
    funcAdditionDraw = func;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
pFuncVV Display_GetAddDrawFunction(void)
{
    return funcAdditionDraw;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_RemoveAddDrawFunction(void)
{
    funcAdditionDraw = 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_Clear(void)
{
    Painter_FillRegionC(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 2, gColorBack);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_ShiftScreen(int delta)
{
    if(PEACKDET_EN)
    {
        delta *= 2;
    }
    int16 shift = SHIFT_IN_MEMORY;
    shift += (int16)delta;
    int16 max = (int16)sMemory_NumBytesInChannel(false) - 282 * (PEACKDET_EN ? 2 : 1);
    if(shift < 0)
    {
        shift = 0;
    }
    else if(shift > max)
    {
        shift = max;
    }
    SHIFT_IN_MEMORY = shift;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_ChangedRShiftMarkers(bool active)
{
    drawRShiftMarkers = set.display.altMarkers != AM_Hide;
    Timer_SetAndStartOnce(kRShiftMarkersAutoHide, OnRShiftMarkersAutoHide, 5000);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_AddStringToIndicating(const char *string)
{
    if(FirstEmptyString() == MAX_NUM_STRINGS)
    {
        DeleteFirstString();
    }

    AddString(string);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_OneStringUp(void)
{
    if(!set.debug.consoleInPause)
    {
    }
    else if(lastStringForPause > set.debug.numStrings - 1)
    {
        lastStringForPause--;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_OneStringDown(void)
{
    if(!set.debug.consoleInPause)
    {
    }
    else if(lastStringForPause < FirstEmptyString() - 1)
    {
        lastStringForPause++;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_SetPauseForConsole(bool pause)
{
    if(pause)
    {
        lastStringForPause = FirstEmptyString() - 1;
    }
    else
    {
        lastStringForPause = -1;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_SetOrientation(DisplayOrientation orientation)
{
    set.debug.orientation = orientation;
    gBF.needSetOrientation = 1;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_ShowWarning(Warning warning)
{
    Painter_ResetFlash();
    for(int i = 2; i >= 0; i--)
    {
        char *message = warns[warning].message[LANG][i];
        if(message)
        {
            ShowWarn(message);
        }
    }
    if(warns[warning].good)
    {
        Sound_WarnBeepGood();
    }
    else
    {
        Sound_WarnBeepBad();
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void InitHardware(void)
{
    GPIO_InitTypeDef isGPIO_ =
    {
        GPIO_PIN_11,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SPEED_HIGH,
        GPIO_AF0_MCO,
    };
    // Сигнал готовности дисплея  к приёму команды
    HAL_GPIO_Init(GPIOG, &isGPIO_);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool NeedForClearScreen(void)
{
    int numAccum = DISPLAY_NUM_ACCUM;
    if(IN_RANDOM_MODE || numAccum == 1 || MODE_ACCUM_NO_RESET || set.service.recorder)
    {
        return true;
    }
    if(needFinishDraw)
    {
        needFinishDraw = false;
        return true;
    }
    if(MODE_ACCUM_RESET && numDrawingSignals >= (uint)numAccum)
    {
        numDrawingSignals = 0;
        return true;
    }
    return false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SetOrientation(void)
{
    if(gBF.needSetOrientation == 1)
    {
        uint8 command[4] ={SET_ORIENTATION, set.debug.orientation, 0, 0};
        Painter_SendToDisplay(command, 4);
        Painter_SendToInterfaces(command, 2);
        gBF.needSetOrientation = 0;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawFullGrid(void)
{
    if(sDisplay_IsSeparate())
    {
        DrawGrid(GridLeft(), GRID_TOP, GridWidth(), GridFullHeight() / 2);
        if(FFT_ENABLED)
        {
            DrawGridSpectrum();
        }
        if(FUNC_ENABLED)
        {
            DrawGrid(GridLeft(), GRID_TOP + GridFullHeight() / 2, GridWidth(), GridFullHeight() / 2);
        }
        Painter_DrawHLineC(GRID_TOP + GridFullHeight() / 2, GridLeft(), GridLeft() + GridWidth(), gColorFill);
    }
    else
    {
        DrawGrid(GridLeft(), GRID_TOP, GridWidth(), GridFullHeight());
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSpectrum(void)
{
    if(!FFT_ENABLED)
    {
        return;
    }

    Painter_DrawVLineC(GridRight(), GridChannelBottom() + 1, GridMathBottom() - 1, gColorBack);

    if(WORK_DIRECT)
    {
        int numPoints = sMemory_NumBytesInChannel(false);
        if(numPoints < 512)
        {
            numPoints = 256;
        }

        SourceFFT source = set.math.sourceFFT;

        if(source == SourceFFT_Channel0)
        {
            DRAW_SPECTRUM(gDataA, numPoints, A);
        }
        else if(source == SourceFFT_Channel1)
        {
            DRAW_SPECTRUM(gDataB, numPoints, B);
        }
        else
        {
            if(set.display.lastAffectedChannel == A)
            {
                DRAW_SPECTRUM(gDataB, numPoints, B);
                DRAW_SPECTRUM(gDataA, numPoints, A);
            }
            else
            {
                DRAW_SPECTRUM(gDataA, numPoints, A);
                DRAW_SPECTRUM(gDataB, numPoints, B);
            }
        }

    }

    Painter_DrawHLineC(GridChannelBottom(), GridLeft(), GridRight(), gColorFill);
    Painter_DrawHLine(GridMathBottom(), GridLeft(), GridRight());
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursors(void)
{
    Channel source = set.cursors.source;
    Painter_SetColor(ColorCursors(source));
    if(sCursors_NecessaryDrawCursors())
    {

        bool bothCursors = set.cursors.cntrlT[source] != CursCntrl_Disable && set.cursors.cntrlU[source] != CursCntrl_Disable;  // Признак того, что включены и вертикальные и горизонтальные курсоры - надо нарисовать квадраты в местах пересечения

        int x0 = -1;
        int x1 = -1;
        int y0 = -1;
        int y1 = -1;

        if(bothCursors)
        {
            x0 = GridLeft() + (int)set.cursors.posCurT[source][0];
            x1 = GridLeft() + (int)set.cursors.posCurT[source][1];
            y0 = GRID_TOP + (int)sCursors_GetCursPosU(source, 0);
            y1 = GRID_TOP + (int)sCursors_GetCursPosU(source, 1);

            Painter_DrawRectangle(x0 - 2, y0 - 2, 4, 4);
            Painter_DrawRectangle(x1 - 2, y1 - 2, 4, 4);
        }

        CursCntrl cntrl = set.cursors.cntrlT[source];
        if(cntrl != CursCntrl_Disable)
        {
            DrawVerticalCursor((int)set.cursors.posCurT[source][0], y0);
            DrawVerticalCursor((int)set.cursors.posCurT[source][1], y1);
        }
        cntrl = set.cursors.cntrlU[source];
        if(cntrl != CursCntrl_Disable)
        {
            DrawHorizontalCursor((int)sCursors_GetCursPosU(source, 0), x0);
            DrawHorizontalCursor((int)sCursors_GetCursPosU(source, 1), x1);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawHiPart(void)
{
    WriteCursors();
    DrawHiRightPart();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawLowPart(void)
{
    int y0 = SCREEN_HEIGHT - 19;
    int y1 = SCREEN_HEIGHT - 10;
    int x = -1;

    Painter_DrawHLineC(GridChannelBottom(), 1, GridLeft() - Measure_GetDeltaGridLeft() - 2, gColorFill);
    Painter_DrawHLine(GridFullBottom(), 1, GridLeft() - Measure_GetDeltaGridLeft() - 2);

    WriteTextVoltage(A, x + 2, y0);

    WriteTextVoltage(B, x + 2, y1);

    Painter_DrawVLineC(x + 95, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2, gColorFill);

    x += 98;
    char buffer[100] ={0};

    TBase tBase = TBASE;
    int16 tShift = TSHIFT;

    if(!WORK_DIRECT)
    {
        DataSettings *ds = WORK_LAST ? gDSmemLast : gDSmemInt;
        if(ds != 0)
        {
            tBase = (TBase)ds->tBase;
            tShift = ds->tShift;
        }
    }
    sprintf(buffer, "р\xa5%s", Tables_GetTBaseString(tBase));
    Painter_DrawText(x, y0, buffer);

    buffer[0] = 0;
    char bufForVal[20];
    sprintf(buffer, "\xa5%s", FPGA_GetTShiftString(tShift, bufForVal));
    Painter_DrawText(x + 35, y0, buffer);

    buffer[0] = 0;
    const char *source[3] ={"1", "2", "\x82"};
    if(WORK_DIRECT)
    {
        sprintf(buffer, "с\xa5\x10%s", source[TRIGSOURCE]);
    }

    Painter_DrawTextC(x, y1, buffer, ColorTrig());

    buffer[0] = 0;
    static const char *couple[] =
    {
        "\x92",
        "\x91",
        "\x92",
        "\x92"
    };
    static const char *polar[] =
    {
        "\xa7",
        "\xa6"
    };
    static const char *filtr[] =
    {
        "\xb5\xb6",
        "\xb5\xb6",
        "\xb3\xb4",
        "\xb1\xb2"
    };
    if(WORK_DIRECT)
    {
        sprintf(buffer, "\xa5\x10%s\x10\xa5\x10%s\x10\xa5\x10", couple[set.trig.input], polar[set.trig.polarity]);
        Painter_DrawText(x + 18, y1, buffer);
        Painter_DrawChar(x + 45, y1, filtr[set.trig.input][0]);
        Painter_DrawChar(x + 53, y1, filtr[set.trig.input][1]);
    }

    buffer[0] = '\0';
    const char mode[] =
    {
        '\xb7',
        '\xa0',
        '\xb0'
    };
    if(WORK_DIRECT)
    {
        sprintf(buffer, "\xa5\x10%c", mode[set.trig.startMode]);
        Painter_DrawText(x + 63, y1, buffer);
    }

    Painter_DrawVLineC(x + 79, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2, gColorFill);

    Painter_DrawHLine(GRID_BOTTOM, GRID_RIGHT + 2, SCREEN_WIDTH - 2);
    Painter_DrawHLine(GridChannelBottom(), GRID_RIGHT + 2, SCREEN_WIDTH - 2);

    x += 82;
    y0 = y0 - 3;
    y1 = y1 - 6;
    int y2 = y1 + 6;
    Painter_SetFont(TypeFont_5);

    if(WORK_DIRECT)
    {
        WriteStringAndNumber("накопл", (int16)x, (int16)y0, DISPLAY_NUM_ACCUM);
        WriteStringAndNumber("усредн", (int16)x, (int16)y1, DISPLAY_NUM_AVE);
        WriteStringAndNumber("мн\x93мкс", (int16)x, (int16)y2, DISPLAY_NUM_MIN_MAX);
    }

    x += 42;
    Painter_DrawVLine(x, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2);

    Painter_SetFont(TypeFont_8);

    if(WORK_DIRECT)
    {
        char mesFreq[20] = "\x7c=";
        char buffer[20];
        float freq = FreqMeter_GetFreq();
        if(freq == -1.0f)
        {
            strcat(mesFreq, "******");
        }
        else
        {
            strcat(mesFreq, Freq2String(freq, false, buffer));
        }
        Painter_DrawText(x + 3, GRID_BOTTOM + 2, mesFreq);
    }

    DrawTime(x + 3, GRID_BOTTOM + 11);

    Painter_DrawVLine(x + 55, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2);

    Painter_SetFont(TypeFont_UGO2);

    // Флешка
    if(gFlashDriveIsConnected)
    {
        Painter_Draw4SymbolsInRect(x + 57, GRID_BOTTOM + 2, SYMBOL_FLASH_DRIVE);
    }

    // Ethernet
    if(gEthIsConnected || gBF.cableEthIsConnected)
    {
        Painter_Draw4SymbolsInRectC(x + 87, GRID_BOTTOM + 2, SYMBOL_ETHERNET, gEthIsConnected ? COLOR_WHITE : COLOR_FLASH_01);
    }

    if(gConnectToHost || gCableVCPisConnected)
    {
        Painter_Draw4SymbolsInRectC(x + 72, GRID_BOTTOM + 2, SYMBOL_USB, gConnectToHost ? COLOR_WHITE : COLOR_FLASH_01);
    }

    Painter_SetColor(gColorFill);
    // Пиковый детектор
    if(!PEACKDET_DIS)
    {
        Painter_DrawChar(x + 38, GRID_BOTTOM + 11, '\x12');
        Painter_DrawChar(x + 46, GRID_BOTTOM + 11, '\x13');
    }

    if(WORK_DIRECT)
    {
        Painter_SetFont(TypeFont_5);
        WriteStringAndNumber("СГЛАЖ.:", (int16)(x + 57), (int16)(GRID_BOTTOM + 10), (int)set.display.smoothing + 1);
        Painter_SetFont(TypeFont_8);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursorsWindow(void)
{
    if((!MenuIsMinimize() || !MENU_IS_SHOWN) && drawRShiftMarkers)
    {
        DrawScaleLine(2, false);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursorTrigLevel(void)
{
    if(TRIGSOURCE_EXT)
    {
        return;
    }
    TrigSource ch = TRIGSOURCE;
    int trigLev = TRIGLEV(ch) + (TRIGSOURCE_EXT ? 0 : RSHIFT(ch) - RShiftZero);
    float scale = 1.0f / ((TrigLevMax - TrigLevMin) / 2.4f / GridChannelHeight());
    int y0 = (GRID_TOP + GridChannelBottom()) / 2 + (int)(scale * (TrigLevZero - TrigLevMin));
    int y = y0 - (int)(scale * (trigLev - TrigLevMin));

    if(!TRIGSOURCE_EXT)
    {
        y = (y - GridChannelCenterHeight()) + GridChannelCenterHeight();
    }

    int x = GridRight();
    Painter_SetColor(ColorTrig());
    if(y > GridChannelBottom())
    {
        Painter_DrawChar(x + 3, GridChannelBottom() - 11, SYMBOL_TRIG_LEV_LOWER);
        Painter_SetPoint(x + 5, GridChannelBottom() - 2);
        y = GridChannelBottom() - 7;
        x--;
    }
    else if(y < GRID_TOP)
    {
        Painter_DrawChar(x + 3, GRID_TOP + 2, SYMBOL_TRIG_LEV_ABOVE);
        Painter_SetPoint(x + 5, GRID_TOP + 2);
        y = GRID_TOP + 7;
        x--;
    }
    else
    {
        Painter_DrawChar(x + 1, y - 4, SYMBOL_TRIG_LEV_NORMAL);
    }
    Painter_SetFont(TypeFont_5);

    const char simbols[3] ={'1', '2', 'В'};
    int dY = 0;

    Painter_DrawCharC(x + 5, y - 9 + dY, simbols[TRIGSOURCE], gColorBack);
    Painter_SetFont(TypeFont_8);

    if(drawRShiftMarkers && !MenuIsMinimize())
    {
        DrawScaleLine(SCREEN_WIDTH - 11, true);
        int left = GridRight() + 9;
        int height = GridChannelHeight() - 2 * DELTA;
        int shiftFullMin = RShiftMin + TrigLevMin;
        int shiftFullMax = RShiftMax + TrigLevMax;
        scale = (float)height / (shiftFullMax - shiftFullMin);
        int shiftFull = TRIGLEV(TRIGSOURCE) + (TRIGSOURCE_EXT ? 0 : RSHIFT(ch));
        int yFull = GRID_TOP + DELTA + height - (int)(scale * (shiftFull - RShiftMin - TrigLevMin) + 4);
        Painter_FillRegionC(left + 2, yFull + 1, 4, 6, ColorTrig());
        Painter_SetFont(TypeFont_5);
        Painter_DrawCharC(left + 3, yFull - 5 + dY, simbols[TRIGSOURCE], gColorBack);
        Painter_SetFont(TypeFont_8);
    }
}





//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawMeasures(void)
{
    if(!set.measures.show)
    {
        gBF.topMeasures = GRID_BOTTOM;
        return;
    }

    gBF.topMeasures = GRID_BOTTOM;

    Processing_CalculateMeasures();

    if(set.measures.zone == MeasuresZone_Hand)
    {
        int x0 = set.measures.posCurT[0] - SHIFT_IN_MEMORY + GridLeft();
        int y0 = set.measures.posCurU[0] + GRID_TOP;
        int x1 = set.measures.posCurT[1] - SHIFT_IN_MEMORY + GridLeft();
        int y1 = set.measures.posCurU[1] + GRID_TOP;
        SortInt(&x0, &x1);
        SortInt(&y0, &y1);
        Painter_DrawRectangleC(x0, y0, x1 - x0, y1 - y0, gColorFill);
    }

    int x0 = GridLeft() - Measure_GetDeltaGridLeft();
    int dX = Measure_GetDX();
    int dY = Measure_GetDY();
    int y0 = Measure_GetTopTable();

    int numRows = Measure_NumRows();
    int numCols = Measure_NumCols();

    for(int str = 0; str < numRows; str++)
    {
        for(int elem = 0; elem < numCols; elem++)
        {
            int x = x0 + dX * elem;
            int y = y0 + str * dY;
            bool active = Measure_IsActive(str, elem) && GetNameOpenedPage() == Page_SB_MeasTuneMeas;
            Color color = active ? gColorBack : gColorFill;
            Measure meas = Measure_Type(str, elem);
            if(meas != Measure_None)
            {
                Painter_FillRegionC(x, y, dX, dY, gColorBack);
                Painter_DrawRectangleC(x, y, dX, dY, gColorFill);
                gBF.topMeasures = Math_MinInt(gBF.topMeasures, y);
            }
            if(active)
            {
                Painter_FillRegionC(x + 2, y + 2, dX - 4, dY - 4, gColorFill);
            }
            if(meas != Measure_None)
            {
#define SIZE_BUFFER 20
                char buffer[SIZE_BUFFER];

                Painter_DrawTextC(x + 4, y + 2, Measure_Name(str, elem), color);
                if(meas == set.measures.markedMeasure)
                {
                    Painter_FillRegionC(x + 1, y + 1, dX - 2, 9, active ? gColorBack : gColorFill);
                    Painter_DrawTextC(x + 4, y + 2, Measure_Name(str, elem), active ? gColorFill : gColorBack);
                }
                if(set.measures.source == A)
                {
                    Painter_DrawTextC(x + 2, y + 11, Processing_GetStringMeasure(meas, A, buffer, SIZE_BUFFER), gColorChan[A]);
                }
                else if(set.measures.source == B)
                {
                    Painter_DrawTextC(x + 2, y + 11, Processing_GetStringMeasure(meas, B, buffer, SIZE_BUFFER), gColorChan[B]);
                }
                else
                {
                    Painter_DrawTextC(x + 2, y + 11, Processing_GetStringMeasure(meas, A, buffer, SIZE_BUFFER), gColorChan[A]);
                    Painter_DrawTextC(x + 2, y + 20, Processing_GetStringMeasure(meas, B, buffer, SIZE_BUFFER), gColorChan[B]);
                }
#undef SIZE_BUFFER
            }
        }
    }

    if(GetNameOpenedPage() == Page_SB_MeasTuneMeas)
    {
        Measure_DrawPageChoice();
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawStringNavigation(void)
{
    if((gBF.temporaryShowStrNavi || SHOW_STRING_NAVI_ALL) && (MENU_IS_SHOWN || (TypeOpenedItem()) != Item_Page))
    {
        char buffer[100];
        char *string = Menu_StringNavigation(buffer);
        if(string)
        {
            int length = Font_GetLengthText(string);
            int height = 10;
            Painter_DrawRectangleC(GridLeft(), GRID_TOP, length + 2, height, gColorFill);
            Painter_FillRegionC(GridLeft() + 1, GRID_TOP + 1, length, height - 2, gColorBack);
            Painter_DrawTextC(GridLeft() + 2, GRID_TOP + 1, string, gColorFill);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursorTShift(void)
{
    int firstPoint = 0;
    int lastPoint = 0;
    sDisplay_PointsOnDisplay(&firstPoint, &lastPoint);
    lastPoint = firstPoint + 281;

    // Рисуем TPos
    int shiftTPos = sTime_TPosInPoints() - sDisplay_ShiftInMemoryInPoints();

    float scale = (float)(lastPoint - firstPoint) / GridWidth();
    int gridLeft = GridLeft();
    int x = gridLeft + (int)(shiftTPos * scale) - 3;
    if(IntInRange(x + 3, gridLeft, GridRight() + 1))
    {
        Painter_Draw2SymbolsC(x, GRID_TOP - 1, SYMBOL_TPOS_2, SYMBOL_TPOS_3, gColorBack, gColorFill);
    };

    // Рисуем tShift
    int shiftTShift = sTime_TPosInPoints() - sTime_TShiftInPoints();
    if(IntInRange(shiftTShift, firstPoint, lastPoint))
    {
        int x = gridLeft + shiftTShift - firstPoint - 3;
        Painter_Draw2SymbolsC(x, GRID_TOP - 1, SYMBOL_TSHIFT_NORM_1, SYMBOL_TSHIFT_NORM_2, gColorBack, gColorFill);
    }
    else if(shiftTShift < firstPoint)
    {
        Painter_Draw2SymbolsC(gridLeft + 1, GRID_TOP, SYMBOL_TSHIFT_LEFT_1, SYMBOL_TSHIFT_LEFT_2, gColorBack, gColorFill);
        Painter_DrawLineC(GridLeft() + 9, GRID_TOP + 1, GridLeft() + 9, GRID_TOP + 7, gColorBack);
    }
    else if(shiftTShift > lastPoint)
    {
        Painter_Draw2SymbolsC(GridRight() - 8, GRID_TOP, SYMBOL_TSHIFT_RIGHT_1, SYMBOL_TSHIFT_RIGHT_2, gColorBack, gColorFill);
        Painter_DrawLineC(GridRight() - 9, GRID_TOP + 1, GridRight() - 9, GRID_TOP + 7, gColorBack);
    }
}


//-------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawRandStat(void)
{
    extern int gRandStat[281];

    int max = 0;

    for(int i = 0; i < 281; i++)
    {
        if(gRandStat[i] > max)
        {
            max = gRandStat[i];
        }
    }

    float scale = (float)GridFullHeight() / (float)max;

    Painter_SetColor(COLOR_WHITE);

    for(int i = 0; i < 281; i++)
    {
        Painter_DrawVLine(GridLeft() + i, GridFullBottom() - (int)(scale * gRandStat[i]), GridFullBottom());
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Написать предупреждения
static void DrawWarnings(void)
{
    int delta = 12;
    int y = BottomMessages();
    for(int i = 0; i < 10; i++)
    {
        if(warnings[i] != 0)
        {
            DrawStringInRectangle(GridLeft(), y, warnings[i]);
            y -= delta;
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Написать сообщения отладочной консоли
static void DrawConsole(void)
{
    int count = 0;
    Painter_SetFont(sDebug_GetSizeFontForConsole() == 5 ? TypeFont_5 : TypeFont_8);
    int height = Font_GetSize();

    int lastString = FirstEmptyString() - 1;
    int numString = set.debug.numStrings;
    if(height == 8 && numString > 22)
    {
        numString = 22;
    }
    int delta = 0;
    if(SHOW_STRING_NAVI_ALL)
    {
        numString -= ((height == 8) ? 1 : 2);
        delta = 10;
    }
    int firstString = lastString - numString + 1;
    if(firstString < 0)
    {
        firstString = 0;
    }

    int dY = 0;

    for(int numString = firstString; numString <= lastString; numString++)
    {
        int width = Font_GetLengthText(strings[numString]);
        Painter_FillRegionC(GridLeft() + 1, GRID_TOP + 1 + count * (height + 1) + delta, width, height + 1, gColorBack);
        int y = GRID_TOP + 5 + count * (height + 1) - 4;
        if(Font_GetSize() == 5)
        {
            y -= 3;
        }
        Painter_DrawTextC(GridLeft() + 2, y + dY + delta, strings[numString], gColorFill);
        count++;
    }

    Painter_SetFont(TypeFont_8);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Вывести значение уровян синхронизации
static void WriteValueTrigLevel(void)
{
    if(showLevelTrigLev && WORK_DIRECT)
    {
        TrigSource trigSource = TRIGSOURCE;
        float trigLev = RSHIFT_2_ABS(TRIGLEV(trigSource), TRIGSOURCE_EXT ? Range_500mV : RANGE(trigSource));
        if(set.trig.input == TrigInput_AC && !TRIGSOURCE_EXT)
        {
            uint16 rShift = RSHIFT(trigSource);
            float rShiftAbs = RSHIFT_2_ABS(rShift, RANGE(trigSource));
            trigLev += rShiftAbs;
        }

        char buffer[20];
        strcpy(buffer, LANG_RU ? "Ур синхр = " : "Trig lvl = ");
        char bufForVolt[20];
        strcat(buffer, Voltage2String(trigLev, true, bufForVolt));
        int width = 96;

        // Рассчитаем координаты вывода строки уровня синхронизации.

        int x = 0;
        int y = 0;
        CoordTrigLevel(&x, &y, width);

        Painter_DrawRectangleC(x, y, width, 10, gColorFill);
        Painter_FillRegionC(x + 1, y + 1, width - 2, 8, gColorBack);
        Painter_DrawTextC(x + 2, y + 1, buffer, gColorFill);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawTimeForFrame(uint timeTicks)
{
    if(!set.debug.showStats)
    {
        return;
    }
    static char buffer[10];
    static bool first = true;
    static uint timeMSstartCalculation = 0;
    static int numFrames = 0;
    static float numMS = 0.0f;
    if(first)
    {
        timeMSstartCalculation = gTimerMS;
        first = false;
    }
    numMS += timeTicks / 120000.0f;
    numFrames++;

    if((gTimerMS - timeMSstartCalculation) >= 500)
    {
        sprintf(buffer, "%.1fms/%d", numMS / numFrames, numFrames * 2);
        timeMSstartCalculation = gTimerMS;
        numMS = 0.0f;
        numFrames = 0;
    }

    Painter_DrawRectangleC(GridLeft(), GridFullBottom() - 10, 84, 10, gColorFill);
    Painter_FillRegionC(GridLeft() + 1, GridFullBottom() - 9, 82, 8, gColorBack);
    Painter_DrawTextC(GridLeft() + 2, GridFullBottom() - 9, buffer, gColorFill);

    char message[20] ={0};
    sprintf(message, "%d", DS_NumElementsWithSameSettings());
    strcat(message, "/");
    char numAvail[10] ={0};
    sprintf(numAvail, "%d", DS_NumberAvailableEntries());
    strcat(message, numAvail);
    Painter_DrawText(GridLeft() + 50, GridFullBottom() - 9, message);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DisableShowLevelRShiftA(void)
{
    showLevelRShiftA = false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DisableShowLevelRShiftB(void)
{
    showLevelRShiftB = false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DisableShowLevelTrigLev(void)
{
    showLevelTrigLev = false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnRShiftMarkersAutoHide(void)
{
    drawRShiftMarkers = false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int FirstEmptyString(void)
{
    for(int i = 0; i < MAX_NUM_STRINGS; i++)
    {
        if(strings[i] == 0)
        {
            return i;
        }
    }
    return MAX_NUM_STRINGS;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DeleteFirstString(void)
{
    if(FirstEmptyString() < 2)
    {
        return;
    }
    int delta = strlen(strings[0]) + 1;
    int numStrings = FirstEmptyString();
    for(int i = 1; i < numStrings; i++)
    {
        strings[i - 1] = strings[i] - delta;
    }
    for(int i = numStrings - 1; i < MAX_NUM_STRINGS; i++)
    {
        strings[i] = 0;
    }
    for(int i = 0; i < SIZE_BUFFER_FOR_STRINGS - delta; i++)
    {
        bufferForStrings[i] = bufferForStrings[i + delta];
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void AddString(const char *string)
{
    if(set.debug.consoleInPause)
    {
        return;
    }
    static int num = 0;
    char buffer[100];
    sprintf(buffer, "%d\x11", num++);
    strcat(buffer, string);
    int size = strlen(buffer) + 1;
    while(CalculateFreeSize() < size)
    {
        DeleteFirstString();
    }
    if(!strings[0])
    {
        strings[0] = bufferForStrings;
        strcpy(strings[0], buffer);
    }
    else
    {
        char *addressLastString = strings[FirstEmptyString() - 1];
        char *address = addressLastString + strlen(addressLastString) + 1;
        strings[FirstEmptyString()] = address;
        strcpy(address, buffer);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ShowWarn(const char *message)
{
    if(warnings[0] == 0)
    {
        Timer_SetAndEnable(kShowMessages, OnTimerShowWarning, 100);
    }
    bool alreadyStored = false;
    for(int i = 0; i < NUM_WARNINGS; i++)
    {
        if(warnings[i] == 0 && !alreadyStored)
        {
            warnings[i] = message;
            timeWarnings[i] = gTimerMS;
            alreadyStored = true;
        }
        else if(warnings[i] == message)
        {
            timeWarnings[i] = gTimerMS;
            return;
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawGrid(int left, int top, int width, int height)
{
    int right = left + width;
    int bottom = top + height;

    Painter_SetColor(gColorFill);

    if(top == GRID_TOP)
    {
        Painter_DrawHLine(top, 1, left - 2);
        Painter_DrawHLine(top, right + 2, SCREEN_WIDTH - 2);

        if(!MenuIsMinimize() || !MENU_IS_SHOWN)
        {
            Painter_DrawVLine(1, top + 2, bottom - 2);
            Painter_DrawVLine(318, top + 2, bottom - 2);
        }
    }

    float deltaX = GridDeltaX() *(float)width / width;
    float deltaY = GridDeltaY() * (float)height / height;
    float stepX = deltaX / 5;
    float stepY = deltaY / 5;

    float centerX = (float)(left + width / 2);
    float centerY = (float)(top + height / 2);

    Painter_SetColor(gColorGrid);
    if(TYPE_GRID_1)
    {
        DrawGridType1(left, top, right, bottom, centerX, centerY, deltaX, deltaY, stepX, stepY);
    }
    else if(TYPE_GRID_2)
    {
        DrawGridType2(left, top, right, bottom, (int)deltaX, (int)deltaY, (int)stepX, (int)stepY);
    }
    else if(TYPE_GRID_3)
    {
        DrawGridType3(left, top, right, bottom, (int)centerX, (int)centerY, (int)deltaX, (int)deltaY, (int)stepX);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawGridSpectrum(void)
{
    if(set.math.scaleFFT == ScaleFFT_Log)
    {
        static const int nums[] ={4, 6, 8};
        static const char *strs[] ={"0", "-10", "-20", "-30", "-40", "-50", "-60", "-70"};
        int numParts = nums[set.math.fftMaxDB];
        float scale = (float)GridMathHeight() / numParts;
        for(int i = 1; i < numParts; i++)
        {
            int y = GridMathTop() + (int)(i * scale);
            Painter_DrawHLineC(y, GridLeft(), GridLeft() + 256, gColorGrid);
            if(!MenuIsMinimize())
            {
                Painter_SetColor(gColorFill);
                Painter_DrawText(3, y - 4, strs[i]);
            }
        }
        if(!MenuIsMinimize())
        {
            Painter_SetColor(gColorFill);
            Painter_DrawText(5, GridMathTop() + 1, "дБ");
        }
    }
    else if(set.math.scaleFFT == ScaleFFT_Linear)
    {
        static const char *strs[] ={"1.0", "0.8", "0.6", "0.4", "0.2"};
        float scale = (float)GridMathHeight() / 5;
        for(int i = 1; i < 5; i++)
        {
            int y = GridMathTop() + (int)(i * scale);
            Painter_DrawHLineC(y, GridLeft(), GridLeft() + 256, gColorGrid);
            if(!MenuIsMinimize())
            {
                Painter_DrawTextC(5, y - 4, strs[i], gColorFill);
            }
        }
    }
    Painter_DrawVLineC(GridLeft() + 256, GridMathTop(), GridMathBottom(), gColorFill);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DRAW_SPECTRUM(const uint8 *dataIn, int numPoints, Channel ch)
{
    if(!sChannel_Enabled(ch))
    {
        return;
    }

#ifdef _MS_VS

    float dataR[10];
    float spectrum[10];

#else

    float dataR[numPoints];
    float spectrum[numPoints];

#endif

    float freq0 = 0.0f;
    float freq1 = 0.0f;
    float density0 = 0.0f;
    float density1 = 0.0f;
    int y0 = 0;
    int y1 = 0;
    int s = 2;

    uint8 *data = malloc(numPoints);

    RAM_MemCpy16((void*)dataIn, data, numPoints);

    Math_PointsRelToVoltage(data, numPoints, (Range)gDSet->range[ch], gDSet->rShift[ch], dataR);
    Math_CalculateFFT(dataR, numPoints, spectrum, &freq0, &density0, &freq1, &density1, &y0, &y1);
    DrawSpectrumChannel(spectrum, gColorChan[ch]);
    if(!MENU_IS_SHOWN || MenuIsMinimize())
    {
        Color color = gColorFill;
        WriteParametersFFT(ch, freq0, density0, freq1, density1);
        Painter_DrawRectangleC(set.math.posCur[0] + GridLeft() - s, y0 - s, s * 2, s * 2, color);
        Painter_DrawRectangle(set.math.posCur[1] + GridLeft() - s, y1 - s, s * 2, s * 2);

        Painter_DrawVLine(GridLeft() + set.math.posCur[0], GridMathBottom(), y0 + s);
        Painter_DrawVLine(GridLeft() + set.math.posCur[1], GridMathBottom(), y1 + s);
    }

    free(data);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawVerticalCursor(int x, int yTearing)
{
    x += GridLeft();
    if(yTearing == -1)
    {
        Painter_DrawDashedVLine(x, GRID_TOP + 2, GridChannelBottom() - 1, 1, 1, 0);
    }
    else
    {
        Painter_DrawDashedVLine(x, GRID_TOP + 2, yTearing - 2, 1, 1, 0);
        Painter_DrawDashedVLine(x, yTearing + 2, GridChannelBottom() - 1, 1, 1, 0);
    }
    Painter_DrawRectangle(x - 1, GRID_TOP - 1, 2, 2);
    Painter_DrawRectangle(x - 1, GridChannelBottom() - 1, 2, 2);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawHorizontalCursor(int y, int xTearing)
{
    y += GRID_TOP;
    if(xTearing == -1)
    {
        Painter_DrawDashedHLine(y, GridLeft() + 2, GridRight() - 1, 1, 1, 0);
    }
    else
    {
        Painter_DrawDashedHLine(y, GridLeft() + 2, xTearing - 2, 1, 1, 0);
        Painter_DrawDashedHLine(y, xTearing + 2, GridRight() - 1, 1, 1, 0);
    }
    Painter_DrawRectangle(GridLeft() - 1, y - 1, 2, 2);
    Painter_DrawRectangle(GridRight() - 1, y - 1, 2, 2);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteCursors(void)
{
    char buffer[20];
    int startX = 43;
    if(WORK_DIRECT)
    {
        startX += 29;
    }
    int x = startX;
    int y1 = 0;
    int y2 = 9;
    if(sCursors_NecessaryDrawCursors())
    {
        Painter_DrawVLineC(x, 1, GRID_TOP - 2, gColorFill);
        x += 3;
        Channel source = set.cursors.source;
        Color colorText = gColorChan[source];
        if(set.cursors.cntrlU[source] != CursCntrl_Disable)
        {
            Painter_DrawTextC(x, y1, "1:", colorText);
            Painter_DrawText(x, y2, "2:");
            x += 7;
            Painter_DrawText(x, y1, sCursors_GetCursVoltage(source, 0, buffer));
            Painter_DrawText(x, y2, sCursors_GetCursVoltage(source, 1, buffer));
            x = startX + 49;
            float pos0 = Math_VoltageCursor(sCursors_GetCursPosU(source, 0), RANGE(source), RSHIFT(source));
            float pos1 = Math_VoltageCursor(sCursors_GetCursPosU(source, 1), RANGE(source), RSHIFT(source));
            float delta = fabsf(pos1 - pos0);
            if(set.chan[source].divider == Multiplier_10)
            {
                delta *= 10;
            }
            Painter_DrawText(x, y1, ":dU=");
            Painter_DrawText(x + 17, y1, Voltage2String(delta, false, buffer));
            Painter_DrawText(x, y2, ":");
            Painter_DrawText(x + 10, y2, sCursors_GetCursorPercentsU(source, buffer));
        }

        x = startX + 101;
        Painter_DrawVLineC(x, 1, GRID_TOP - 2, gColorFill);
        x += 3;
        if(set.cursors.cntrlT[source] != CursCntrl_Disable)
        {
            Painter_SetColor(colorText);
            Painter_DrawText(x, y1, "1:");
            Painter_DrawText(x, y2, "2:");
            x += 7;
            Painter_DrawText(x, y1, sCursors_GetCursorTime(source, 0, buffer));
            Painter_DrawText(x, y2, sCursors_GetCursorTime(source, 1, buffer));
            x = startX + 153;
            float pos0 = Math_TimeCursor(set.cursors.posCurT[source][0], TBASE);
            float pos1 = Math_TimeCursor(set.cursors.posCurT[source][1], TBASE);
            float delta = fabsf(pos1 - pos0);
            Painter_DrawText(x, y1, ":dT=");
            char buffer[20];
            Painter_DrawText(x + 17, y1, Time2String(delta, false, buffer));
            Painter_DrawText(x, y2, ":");
            Painter_DrawText(x + 8, y2, sCursors_GetCursorPercentsT(source, buffer));

            if(set.cursors.showFreq)
            {
                int width = 65;
                int x = GridRight() - width;
                Painter_DrawRectangleC(x, GRID_TOP, width, 12, gColorFill);
                Painter_FillRegionC(x + 1, GRID_TOP + 1, width - 2, 10, gColorBack);
                Painter_DrawTextC(x + 1, GRID_TOP + 2, "1/dT=", colorText);
                char buffer[20];
                Painter_DrawText(x + 25, GRID_TOP + 2, Freq2String(1.0f / delta, false, buffer));
            }
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawHiRightPart(void)
{
    // Синхроимпульс
    int y = 2;

    static const int xses[3] ={280, 271, 251};
    int x = xses[MODE_WORK];

    if(!WORK_LAST)
    {
        Painter_DrawVLineC(x, 1, GRID_TOP - 2, gColorFill);

        x += 2;

        if(trigEnable)
        {
            Painter_FillRegionC(x, 1 + y, GRID_TOP - 3, GRID_TOP - 7, gColorFill);
            Painter_DrawTextC(x + 3, 3 + y, LANG_RU ? "СИ" : "Tr", gColorBack);
        }
    }

    // Режим работы
    static const char *strings[][2] =
    {
        {"ИЗМ", "MEAS"},
        {"ПОСЛ", "LAST"},
        {"ВНТР", "INT"}
    };

    if(!WORK_DIRECT)
    {
        x += 18;
        Painter_DrawVLineC(x, 1, GRID_TOP - 2, gColorFill);
        x += 2;
        Painter_DrawText(LANG_RU ? x : x + 3, -1, LANG_RU ? "режим" : "mode");
        Painter_DrawStringInCenterRect(x + 1, 9, 25, 8, strings[MODE_WORK][LANG]);
    }
    else
    {
        x -= 9;
    }

    if(!WORK_LAST)
    {
        x += 27;
        Painter_DrawVLineC(x, 1, GRID_TOP - 2, gColorFill);

        x += 2;
        y = 1;
        if(fpgaStateWork == StateWorkFPGA_Work)       // Рабочий режим
        {
            Painter_Draw4SymbolsInRect(x, 1, SYMBOL_PLAY);
        }
        else if(fpgaStateWork == StateWorkFPGA_Stop)  // Режим остановки
        {
            Painter_FillRegion(x + 3, y + 3, 10, 10);
        }
        else if(fpgaStateWork == StateWorkFPGA_Wait)  // Режим ожидания сигнала
        {
            int w = 4;
            int h = 14;
            int delta = 4;
            x = x + 2;
            Painter_FillRegion(x, y + 1, w, h);
            Painter_FillRegion(x + w + delta, y + 1, w, h);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteTextVoltage(Channel ch, int x, int y)
{
    static const char *couple[] =
    {
        "\x92",
        "\x91",
        "\x90"
    };
    Color color = gColorChan[ch];

    bool inverse = INVERSE(ch);
    ModeCouple modeCouple = COUPLE(ch);
    Divider divider = set.chan[ch].divider;
    Range range = RANGE(ch);
    uint rShift = RSHIFT(ch);
    bool enable = sChannel_Enabled(ch);

    if(!WORK_DIRECT)
    {
        DataSettings *ds = WORK_DIRECT ? gDSet : gDSmemInt;
        if(ds != 0)
        {
            inverse = (ch == A) ? (bool)ds->inverseChA : (bool)ds->inverseChB;
            modeCouple = (ch == A) ? (ModeCouple)ds->modeCoupleA : (ModeCouple)ds->modeCoupleB;
            divider = (ch == A) ? (Divider)ds->multiplierA : (Divider)ds->multiplierB;
            range = (Range)ds->range[ch];
            rShift = ds->rShift[ch];
            enable = (ch == A) ? (bool)ds->enableChA : (bool)ds->enableChB;
        }
    }

    if(enable)
    {
        const int widthField = 91;
        const int heightField = 8;

        Color colorDraw = inverse ? COLOR_WHITE : color;
        if(inverse)
        {
            Painter_FillRegionC(x, y, widthField, heightField, color);
        }

        char buffer[100] ={0};
        sprintf(buffer, "%s\xa5%s\xa5%s", (ch == A) ? (LANG_RU ? "1к" : "1c") : (LANG_RU ? "2к" : "2c"), couple[modeCouple],
                sChannel_Range2String(range, divider));

        Painter_DrawTextC(x + 1, y, buffer, colorDraw);

        char bufferTemp[20];
        sprintf(buffer, "\xa5%s", sChannel_RShift2String((int16)rShift, range, divider, bufferTemp));
        Painter_DrawText(x + 46, y, buffer);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteStringAndNumber(char *text, int16 x, int16 y, int number)
{
    char buffer[100];
    Painter_DrawTextC(x, y, text, gColorFill);
    if(number == 0)
    {
        sprintf(buffer, "-");
    }
    else
    {
        sprintf(buffer, "%d", number);
    }
    Painter_DrawTextRelativelyRight(x + 41, y, buffer);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawTime(int x, int y)
{
    int dField = 10;
    int dSeparator = 2;

    PackedTime time = RTC_GetPackedTime();

    char buffer[20];

    Painter_SetColor(gColorFill);

    if(WORK_INT || WORK_LAST)
    {
        DataSettings *ds = WORK_INT ? gDSmemInt : gDSmemLast;

        if(ds != 0)
        {
            y -= 9;
            time.day = ds->time.day;
            time.hours = ds->time.hours;
            time.minutes = ds->time.minutes;
            time.seconds = ds->time.seconds;
            time.month = ds->time.month;
            time.year = ds->time.year;
            Painter_DrawText(x, y, Int2String(time.day, false, 2, buffer));
            Painter_DrawText(x + dField, y, ":");
            Painter_DrawText(x + dField + dSeparator, y, Int2String(time.month, false, 2, buffer));
            Painter_DrawText(x + 2 * dField + dSeparator, y, ":");
            Painter_DrawText(x + 2 * dField + 2 * dSeparator, y, Int2String(time.year + 2000, false, 4, buffer));
            y += 9;
        }
        else
        {
            return;
        }
    }


    Painter_DrawText(x, y, Int2String(time.hours, false, 2, buffer));
    Painter_DrawText(x + dField, y, ":");
    Painter_DrawText(x + dField + dSeparator, y, Int2String(time.minutes, false, 2, buffer));
    Painter_DrawText(x + 2 * dField + dSeparator, y, ":");
    Painter_DrawText(x + 2 * dField + 2 * dSeparator, y, Int2String(time.seconds, false, 2, buffer));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawScaleLine(int x, bool forTrigLev)
{
    if(set.display.altMarkers == AM_Hide)
    {
        return;
    }
    int width = 6;
    int topY = GRID_TOP + DELTA;
    int x2 = width + x + 2;
    int bottomY = GridChannelBottom() - DELTA;
    int centerY = (GridChannelBottom() + GRID_TOP) / 2;
    int levels[] =
    {
        topY,
        bottomY,
        centerY,
        centerY - (bottomY - topY) / (forTrigLev ? 8 : 4),
        centerY + (bottomY - topY) / (forTrigLev ? 8 : 4)
    };
    for(int i = 0; i < 5; i++)
    {
        Painter_DrawLineC(x + 1, levels[i], x2 - 1, levels[i], gColorFill);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursorsRShift(void)
{
    if(FUNC_ENABLED)
    {
        DrawCursorRShift(Math);
    }
    if(set.display.lastAffectedChannel == B)
    {
        DrawCursorRShift(A);
        DrawCursorRShift(B);
    }
    else
    {
        DrawCursorRShift(B);
        DrawCursorRShift(A);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawStringInRectangle(int x, int y, char const *text)
{
    int width = Font_GetLengthText(text);
    int height = 8;
    Painter_DrawRectangleC(x, y, width + 4, height + 4, gColorFill);
    Painter_DrawRectangleC(x + 1, y + 1, width + 2, height + 2, gColorBack);
    Painter_FillRegionC(x + 2, y + 2, width, height, COLOR_FLASH_10);
    Painter_DrawTextC(x + 3, y + 2, text, COLOR_FLASH_01);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int CalculateFreeSize(void)
{
    int firstEmptyString = FirstEmptyString();
    if(firstEmptyString == 0)
    {
        return SIZE_BUFFER_FOR_STRINGS;
    }
    return SIZE_BUFFER_FOR_STRINGS - (strings[firstEmptyString - 1] - bufferForStrings) - strlen(strings[firstEmptyString - 1]) - 1;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnTimerShowWarning(void)
{
    uint time = gTimerMS;
    for(int i = 0; i < NUM_WARNINGS; i++)
    {
        if(time - timeWarnings[i] >(uint)TIME_MESSAGES * 1000)
        {
            timeWarnings[i] = 0;
            warnings[i] = 0;
        }
    }

    int pointer = 0;
    for(int i = 0; i < NUM_WARNINGS; i++)
    {
        if(warnings[i] != 0)
        {
            warnings[pointer] = warnings[i];
            timeWarnings[pointer] = timeWarnings[i];
            if(pointer != i)
            {
                timeWarnings[i] = 0;
                warnings[i] = 0;
            }
            pointer++;
        }
    }

    if(pointer == 0)
    {
        Timer_Disable(kShowMessages);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawGridType1(int left, int top, int right, int bottom, float centerX, float centerY, float deltaX, float deltaY, float stepX, float stepY)
{
    uint16 masX[17];
    masX[0] = (uint16)(left + 1);
    for(int i = 1; i < 7; i++)
    {
        masX[i] = (uint16)(left + (int)(deltaX * i));
    }
    for(int i = 7; i < 10; i++)
    {
        masX[i] = (uint16)((int)centerX - 8 + i);
    }
    for(int i = 10; i < 16; i++)
    {
        masX[i] = (uint16)(centerX + deltaX * (i - 9));
    }
    masX[16] = (uint16)(right - 1);

    Painter_DrawMultiVPointLine(17, top + (int)stepY, masX, (int)stepY, CalculateCountV(), gColorGrid);

    uint8 mas[13];
    mas[0] = (uint8)(top + 1);
    for(int i = 1; i < 5; i++)
    {
        mas[i] = (uint8)(top + (int)(deltaY * i));
    }
    for(int i = 5; i < 8; i++)
    {
        mas[i] = (uint8)((int)(centerY)-6 + i);
    }
    for(int i = 8; i < 12; i++)
    {
        mas[i] = (uint8)((int)centerY + (int)(deltaY * (i - 7)));
    }
    mas[12] = (uint8)(bottom - 1);

    Painter_DrawMultiHPointLine(13, left + (int)stepX, mas, (int)stepX, CalculateCountH(), gColorGrid);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawGridType2(int left, int top, int right, int bottom, int deltaX, int deltaY, int stepX, int stepY)
{
    uint16 masX[15];
    masX[0] = (uint16)(left + 1);
    for(int i = 1; i < 14; i++)
    {
        masX[i] = (uint16)(left + (int)(deltaX * i));
    }
    masX[14] = (uint16)(right - 1);
    Painter_DrawMultiVPointLine(15, top + stepY, masX, stepY, CalculateCountV(), gColorGrid);

    uint8 mas[11];
    mas[0] = (uint8)(top + 1);
    for(int i = 1; i < 10; i++)
    {
        mas[i] = (uint8)(top + (int)(deltaY * i));
    }
    mas[10] = (uint8)(bottom - 1);
    Painter_DrawMultiHPointLine(11, left + stepX, mas, stepX, CalculateCountH(), gColorGrid);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawGridType3(int left, int top, int right, int bottom, int centerX, int centerY, int deltaX, int deltaY, int stepX)
{
    Painter_DrawHPointLine(centerY, left + stepX, right, (float)stepX);
    uint8 masY[6] ={(uint8)(top + 1), (uint8)(top + 2), (uint8)(centerY - 1), (uint8)(centerY + 1), (uint8)(bottom - 2), (uint8)(bottom - 1)};
    Painter_DrawMultiHPointLine(6, left + deltaX, masY, deltaX, (right - top) / deltaX, gColorGrid);
    //Painter_DrawVPointLine(centerX, top + stepY, bottom - 10, (float)stepY, gColorGrid);
    uint16 masX[6] ={(uint16)(left + 1), (uint16)(left + 2), (uint16)(centerX - 1), (uint16)(centerX + 1), (uint16)(right - 2), (uint16)(right - 1)};
    Painter_DrawMultiVPointLine(6, top + deltaY, masX, deltaY, (bottom - top) / deltaY, gColorGrid);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSpectrumChannel(float *spectrum, Color color)
{
    Painter_SetColor(color);
    int gridLeft = GridLeft();
    int gridBottom = GridMathBottom();
    int gridHeight = GridMathHeight();
    for(int i = 0; i < 256; i++)
    {
        Painter_DrawVLine(gridLeft + i, gridBottom, gridBottom - (int)(gridHeight * spectrum[i]));
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteParametersFFT(Channel ch, float freq0, float density0, float freq1, float density1)
{
    int x = GridLeft() + 259;
    int y = GridChannelBottom() + 5;
    int dY = 10;

    char buffer[20];
    Painter_SetColor(gColorFill);
    Painter_DrawText(x, y, Freq2String(freq0, false, buffer));
    y += dY;
    Painter_DrawText(x, y, Freq2String(freq1, false, buffer));
    if(ch == A)
    {
        y += dY + 2;
    }
    else
    {
        y += dY * 3 + 4;
    }
    Painter_SetColor(gColorChan[ch]);
    Painter_DrawText(x, y, set.math.scaleFFT == ScaleFFT_Log ? Float2Db(density0, 4, buffer) : Float2String(density0, false, 7, buffer));
    y += dY;
    Painter_DrawText(x, y, set.math.scaleFFT == ScaleFFT_Log ? Float2Db(density1, 4, buffer) : Float2String(density1, false, 7, buffer));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursorRShift(Channel ch)
{
    int x = GridRight() - GridWidth() - Measure_GetDeltaGridLeft();

    if(ch == Math)
    {
        int yCenter = (GridMathTop() + GridMathBottom()) / 2;
        int y = yCenter - Math_RShift2Pixels(RSHIFT_MATH, GridMathHeight());
        float scaleFull = (float)GridMathHeight() / (RShiftMax - RShiftMin);
        int yFull = yCenter - (int)(scaleFull * (RSHIFT_MATH - RShiftZero));
        Painter_DrawCharC(x - 9, yFull - 4, SYMBOL_RSHIFT_NORMAL, gColorFill);
        Painter_DrawCharC(x - 8, yFull - 5, 'm', gColorBack);
        return;
    }
    if(!sChannel_Enabled(ch))
    {
        return;
    }

    int rShift = RSHIFT(ch);

    int y = GridChannelCenterHeight() - Math_RShift2Pixels((uint16)rShift, GridChannelHeight());

    float scaleFull = (float)GridChannelHeight() / (RShiftMax - RShiftMin) * (MATH_ENABLED ? 0.9f : 0.91f);
    int yFull = GridChannelCenterHeight() - (int)(scaleFull * (rShift - RShiftZero));

    if(y > GridChannelBottom())
    {
        Painter_DrawCharC(x - 7, GridChannelBottom() - 11, SYMBOL_RSHIFT_LOWER, gColorChan[ch]);
        Painter_SetPoint(x - 5, GridChannelBottom() - 2);
        y = GridChannelBottom() - 7;
        x++;
    }
    else if(y < GRID_TOP)
    {
        Painter_DrawCharC(x - 7, GRID_TOP + 2, SYMBOL_RSHIFT_ABOVE, gColorChan[ch]);
        Painter_SetPoint(x - 5, GRID_TOP + 2);
        y = GRID_TOP + 7;
        x++;
    }
    else
    {
        Painter_DrawCharC(x - 8, y - 4, SYMBOL_RSHIFT_NORMAL, gColorChan[ch]);
        if(((ch == A) ? showLevelRShiftA : showLevelRShiftB) && WORK_DIRECT)
        {
            Painter_DrawDashedHLine(y, GridLeft(), GridRight(), 7, 3, 0);
        }
    }

    Painter_SetFont(TypeFont_5);
    int dY = 0;

    if((!MenuIsMinimize() || !MENU_IS_SHOWN) && drawRShiftMarkers)
    {
        Painter_FillRegionC(4, yFull - 3, 4, 6, gColorChan[ch]);
        Painter_DrawCharC(5, yFull - 9 + dY, ch == A ? '1' : '2', gColorBack);
    }
    Painter_DrawCharC(x - 7, y - 9 + dY, ch == A ? '1' : '2', gColorBack);
    Painter_SetFont(TypeFont_8);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int CalculateCountV(void)
{
    if(set.measures.show && set.measures.modeViewSignals == ModeViewSignals_Compress)
    {
        if(set.measures.number == MN_1_5)
        {
            return set.measures.source == A_B ? 42 : 44;
        }
        if(set.measures.number == MN_2_5)
        {
            return set.measures.source == A_B ? 69 : 39;
        }
        if(set.measures.number == MN_3_5)
        {
            return set.measures.source == A_B ? 54 : 68;
        }
    }

    return 49;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int CalculateCountH(void)
{
    if(set.measures.modeViewSignals == ModeViewSignals_Compress)
    {
        if(set.measures.number == MN_6_1)
        {
            return 73;
        }
        if(set.measures.number == MN_6_2)
        {
            return 83;
        }
    }
    return 69;
}


#ifdef _MS_VS
#pragma warning(pop)
#endif
