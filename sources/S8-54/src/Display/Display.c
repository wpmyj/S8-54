// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Display.h"
#include "Grid.h"
#include "Log.h"
#include "PainterData.h"
#include "Symbols.h"
#include "Data/Data.h"
#include "Data/DataBuffer.h"
#include "Ethernet/TcpSocket.h"
#include "FlashDrive/FlashDrive.h"
#include "font/Font.h"
#include "FPGA/fpga.h"
#include "FPGA/fpgaExtensions.h"
#include "Hardware/FSMC.h"
#include "Hardware/RAM.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Menu/Menu.h"
#include "Menu/MenuDrawing.h"
#include "Menu/MenuFunctions.h"
#include "Utils/Debug.h"
#include "Utils/Dictionary.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Utils/ProcessingSignal.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    Warning      warning;
    bool         good;
    char * const message[2][3];
} StructWarning;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const StructWarning warns[Warning_Count] =
{
    {LimitChan1_Volts, false,           {{"ПРЕДЕЛ КАНАЛ 1 - ВОЛЬТ/ДЕЛ"},                                            {"LIMIT CHANNEL 1 - VOLTS/DIV"}}},
    {LimitChan2_Volts, false,           {{"ПРЕДЕЛ КАНАЛ 2 - ВОЛЬТ/ДЕЛ"},                                            {"LIMIT CHANNEL 2 - VOLTS/DIV"}}},
    {LimitSweep_Time, false,            {{"ПРЕДЕЛ РАЗВЕРТКА - ВРЕМЯ/ДЕЛ"},                                               {"LIMIT SWEEP - TIME/DIV"}}},
    {EnabledPeakDet, false,             {{"ВКЛЮЧЕН ПИКОВЫЙ ДЕТЕКТОР"},                                                       {"ENABLED PEAK. DET."}}},
    {LimitChan1_RShift, false,          {{"ПРЕДЕЛ КАНАЛ 1 - \x0d"},                                                      {"LIMIT CHANNEL 1 - \x0d"}}},
    {LimitChan2_RShift, false,          {{"ПРЕДЕЛ КАНАЛ 2 - \x0d"},                                                      {"LIMIT CHANNEL 2 - \x0d"}}},
    {LimitSweep_Level, false,           {{"ПРЕДЕЛ РАЗВЕРТКА - УРОВЕНЬ"},                                                    {"LIMIT SWEEP - LEVEL"}}},
    {LimitSweep_TShift, false,          {{"ПРЕДЕЛ РАЗВЕРТКА - \x97"},                                                        {"LIMIT SWEEP - \x97"}}},
    {TooSmallSweepForPeakDet, false,    {{"ПИК. ДЕТ. НЕ РАБОТАЕТ НА РАЗВЕРТКАХ МЕНЕЕ 0.5мкс/дел"},
                                                                                              {"PEAK. DET. NOT WORK ON SWEETS LESS THAN 0.5us/div"}}},
    {TooFastScanForRecorder, false,     {{"САМОПИСЕЦ НЕ МОЖЕТ РАБОТАТЬ НА БЫСТРЫХ РАЗВЁРТКАХ"},          {"SELF-RECORDER DOES NOT WORK FAST SCANS"}}},
    {FileIsSaved, true,                 {{"ФАЙЛ СОХРАНЕН"},                                                                       {"FILE IS SAVED"}}},
    {SignalIsSaved, true,               {{"СИГНАЛ СОХРАНЕН"},                                                                   {"SIGNAL IS SAVED"}}},
    {SignalIsDeleted, true,             {{"СИГНАЛ УДАЛЕН"},                                                                   {"SIGNAL IS DELETED"}}},
    {MenuDebugEnabled, true,            {{"МЕНЮ ОТЛАДКА ВКЛЮЧЕНО"},                                                       {"MENU DEBUG IS ENABLED"}}},
    {TimeNotSet, true,                  {{"ВРЕМЯ НЕ УСТАНОВЛЕНО. МОЖЕТЕ УСТАНОВИТЬ ЕГО СЕЙЧАС"},        {"TIME IS NOT SET. YOU CAN INSTALL IT NOW"}}},
    {SignalNotFound, true,              {{"СИГНАЛ НЕ НАЙДЕН"},                                                              {"SIGNAL IS NOT FOUND"}}},
    {SetTPosToLeft, true,               {{"НА РАЗВЕРТКАХ МЕДЛЕННЕЕ 10мс/дел ЖЕЛАТЕЛЬНО УСТАНАВ-",
                                          "ЛИВАТЬ \"РАЗВЕРТКА - \x7b\" В ПОЛОЖЕНИЕ \"Лево\" ДЛЯ УСКОРЕ-", "НИЯ ВЫВОДА СИГНАЛА"},
                             {"AT SCANNER SLOWLY 10ms/div DESIRABLY SET \"SCAN - \x7b\" IN", "SWITCH \"Left\" FOR TO ACCELERATE THE OUTPUT SIGNAL"}}},
    {NeedRebootDevice, true,            {{"Для вступления изменений в силу", "выключите прибор"},
                                                                              {"FOR THE INTRODUCTION OF CHANGES", "IN FORCE SWITCH OFF THE DEVICE"}}},
    {ImpossibleEnableMathFunction, true, {{"Отключите вычисление БПФ"},                                           {"DISCONNECT CALCULATION OF FFT"}}},
    {ImpossibleEnableFFT, true,         {{"Отключите математическую функцию"},                                 {"DISCONNECT MATHEMATICAL FUNCTION"}}},
    {WrongFileSystem, false,            {{"Не могу прочитать флешку. Убедитесь, что на ней FAT32"}, 
                                                                                      {"I can not mount a usb flash drive. Make sure to her FAT32"}}},
    {WrongModePeackDet, true,           {{"Чтобы изменить длину памяти, отключите пиковый детектор"},
                                                                                 {"To change the length of the memory, turn off the peak detector"}}},
    {DisableChannel2, true,             {{"Сначала выключите канал 2"},                                           {"First, turn off the channel 2"}}},
    {TooLongMemory, true,               {{"Второй канал рабоатает при длине памяти 16к и менее"},
                                                                                     {"The second channel runs at a length of memory 16k and less"}}},
    {NoPeakDet32k, true,                {{"Пиковый детектор не работает при длине памяти 32к"},
                                                                                  {"The peak detector does not work when the memory length of 32k"}}},
    {NoPeakDet16k, true,                {{"Для работы пикового детектора при длине памяти 16к", "отключите канал 2"},
                                                                         {"For the peak detector with a length of 16k memory", "disable channel 2"}}},
    {Warn50Ohms, false,                 {{"При величине входного сопротивления 50 Ом", "запрещается подавать на вход более 5 В"},
                                                              {"When the input impedance value of 50 ohms", "is prohibited to input more than 5 V"}}},
    {WarnNeedForFlashDrive, true,       {{"Сначала подключите флеш-диск"},                                        {"First connect the flash drive"}}},
    {FirmwareSaved, true,               {{"Прошивка сохранена"},                                                                 {"Firmware saved"}}},
    {FullyCompletedOTP, false,          {{"Память OTP полностью заполнена"},                                         {"OTP memory fully completed"}}}
};

#define  DELTA 5

#define MAX_NUM_STRINGS         35
#define SIZE_BUFFER_FOR_STRINGS 2000
static char                     *strings[MAX_NUM_STRINGS] = {0};
static char                     bufferForStrings[SIZE_BUFFER_FOR_STRINGS] = {0};
static int                      lastStringForPause = -1;


#define NUM_WARNINGS            10
static const char               *warnings[NUM_WARNINGS] = {0};      ///< Здесь предупреждающие сообщения.
static uint                     timeWarnings[NUM_WARNINGS] = {0};   ///< Здесь время, когда предупреждающее сообщение поступило на экран.


static pFuncVV funcOnHand       = 0;
static pFuncVV funcAdditionDraw = 0;

static bool showLevelRShiftA = false;   ///< Нужно ли рисовать горизонтальную линию уровня смещения первого канала.
static bool showLevelRShiftB = false;
static bool showLevelTrigLev = false;   ///< Нужно ли рисовать горизонтальную линию уровня смещения уровня синхронизации.

static bool trigEnable = false;
static bool drawRShiftMarkers = false;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
static void DrawSpectrumChannel(const float *spectrum, Color color);
static void WriteParametersFFT(Channel ch, float freq0, float density0, float freq1, float density1);

static void DrawCursors(void);                          ///< Нарисовать курсоры курсорных измерений.
static void DrawVerticalCursor(int x, int yTearing);    ///< \brief Нарисовать вертикальный курсор курсорных измерений. x - числовое значение курсора, 
                                                        ///< yTearing - координата места, в котором необходимо сделать разрыв для квадрата пересечения.
static void DrawHorizontalCursor(int y, int xTearing);  ///< Нарисовать горизонтальный курсор курсорных измерений. y - числовое значение курсора, 
                                                    ///< xTearing - координата места, в котором необходимо сделать разрыв для квадрата пересечения.

static void DrawCursorsWindow(void);                    ///< Нарисовать вспомогательные курсоры для смещения по напряжению.
static void DrawScaleLine(int x, bool forTrigLev);      ///< Нарисовать сетку для вспомогательных курсоров по напряжению.

static void DrawCursorTrigLevel(void);
static void WriteValueTrigLevel(void);                  ///< Вывести значение уровня синхронизации.
static void DrawCursorsRShift(void);
static void DrawCursorRShift(Channel ch);
static void DrawCursorTShift(void);

static void DrawHiPart(void);                           ///< Нарисовать то, что находится над основной сеткой.
static void WriteCursors(void);                         ///< Вывести значения курсоров курсорных измерений.
static void DrawHiRightPart(void);                      ///< Вывести то, что находится в правой части верхней строки.

static void DrawLowPart(void);                          ///< Нарисовать то, что находится под основной сеткой.
static void DrawTime(int x, int y);
static void WriteTextVoltage(Channel ch, int x, int y); ///< Написать в нижней строке параметры вертикального тракта.

static void DrawMeasures(void);                         ///< Вывести значения автоматических измерений.
static void DrawStringNavigation(void);                 ///< Вывести строку навигации по меню.
static void DrawRandStat(void);                         ///< Нарисовать график статистики рандомизатора.
static void DrawWarnings(void);                         ///< Вывести предупреждающие сообщения.
static void DrawTimeForFrame(uint timeTicks);
static void DisableShowLevelRShiftA(void);              ///< Отключить вспомогательную линию маркера смещения по напряжению первого канала.
static void DisableShowLevelRShiftB(void);              ///< Отключить вспомогательную линию маркера смещения по напряжению второго канала.
static void DisableShowLevelTrigLev(void);
static void OnRShiftMarkersAutoHide(void);
static int  FirstEmptyString(void);
static void DeleteFirstString(void);
static void AddString(const char *string);
static void ShowWarn(const char *message);
static void WriteStringAndNumber(const char *text, int16 x, int16 y, int number);
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

    Painter_LoadFont(TypeFont_5);
    Painter_LoadFont(TypeFont_8);
    Painter_LoadFont(TypeFont_UGO);
    Painter_LoadFont(TypeFont_UGO2);
    Painter_SetFont(TypeFont_8);

    Painter_LoadPalette();

    SetOrientation();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_Update(void)
{
    Data_Clear();

    uint timeStart = gTimerTics;

    if (funcOnHand)
    {
        funcOnHand();
        return;
    }

    ModeFSMC mode = FSMC_GetMode();

    bool needClear = NeedForClearScreen();

    if(needClear)
    {
        Painter_BeginScene(gColorBack);
        DrawFullGrid();
    }

    PainterData_DrawData();

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

    if(SHOW_RAND_STAT)
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

    Display_DrawConsole();

    if(needClear)
    {
        WriteValueTrigLevel();
    }

    DrawTimeForFrame(gTimerTics - timeStart);

    Painter_EndScene();

    if(NEED_SAVE_TO_FLASHDRIVE)
    {
        if(Painter_SaveScreenToFlashDrive())
        {
            Display_ShowWarning(FileIsSaved);
        }
        NEED_SAVE_TO_FLASHDRIVE = 0;
    }

    FSMC_SetMode(mode);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_RotateRShift(Channel ch)
{
    LAST_AFFECTED_CH = ch;
    if(TIME_SHOW_LEVELS)
    {
        (ch == A) ? (showLevelRShiftA = true) : (showLevelRShiftB = true);
        Timer_SetAndStartOnce((ch == A) ? kShowLevelRShiftA : kShowLevelRShiftB, (ch == A) ? DisableShowLevelRShiftA : DisableShowLevelRShiftB, 
                              TIME_SHOW_LEVELS  * 1000);
    };
    NEED_FINISH_DRAW = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_RotateTrigLev(void)
{
    if(TIME_SHOW_LEVELS && TRIG_MODE_FIND_HAND)
    {
        showLevelTrigLev = true;
        Timer_SetAndStartOnce(kShowLevelTrigLev, DisableShowLevelTrigLev, TIME_SHOW_LEVELS * 1000);
    }
    NEED_FINISH_DRAW = 1;
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
    funcOnHand = func;
    if (mode == DrawMode_Hand)
    {
        Timer_SetAndEnable(kTimerDisplay, funcOnHand, 40);
    }
    else
    {
        Timer_Disable(kTimerDisplay);
    }
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
    if(PEAKDET_DS)
    {
        delta *= 2;
    }
    int16 shift = SHIFT_IN_MEMORY;
    shift += (int16)delta;
    int16 max = (int16)BYTES_IN_CHANNEL(DS) - 282 * (PEAKDET_DS ? 2 : 1);
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
    drawRShiftMarkers = !ALT_MARKERS_HIDE;
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
    if(!CONSOLE_IN_PAUSE)
    {
    }
    else if(lastStringForPause > CONSOLE_NUM_STRINGS - 1)
    {
        lastStringForPause--;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_OneStringDown(void)
{
    if(!CONSOLE_IN_PAUSE)
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
    DISPLAY_ORIENTATION = orientation;
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
static bool NeedForClearScreen(void)
{
    static bool first = true;
    if (first)
    {
        first = false;
        return true;
    }

    if(IN_RANDOM_MODE || NUM_ACCUM == 1 || MODE_ACCUM_NO_RESET || RECORDER_MODE)
    {
        return true;
    }
    if(NEED_FINISH_DRAW)
    {
        NEED_FINISH_DRAW = 0;
        return true;
    }
    if(!ENUM_ACCUM_INF && MODE_ACCUM_RESET && NUM_DRAWING_SIGNALS >= NUM_ACCUM)
    {
        NUM_DRAWING_SIGNALS = 0;
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SetOrientation(void)
{
    if(gBF.needSetOrientation == 1)
    {
        uint8 command[4] ={SET_ORIENTATION, DISPLAY_ORIENTATION, 0, 0};
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

    if(MODE_WORK_DIR)
    {
        int numPoints = BYTES_IN_CHANNEL(DS);
        if(numPoints < 512)
        {
            numPoints = 256;
        }

        if(SOURCE_FFT_A)
        {
            DRAW_SPECTRUM(OUT_A, numPoints, A);
        }
        else if(SOURCE_FFT_B)
        {
            DRAW_SPECTRUM(OUT_B, numPoints, B);
        }
        else
        {
            if(LAST_AFFECTED_CH_IS_A)
            {
                DRAW_SPECTRUM(OUT_B, numPoints, B);
                DRAW_SPECTRUM(OUT_A, numPoints, A);
            }
            else
            {
                DRAW_SPECTRUM(OUT_A, numPoints, A);
                DRAW_SPECTRUM(OUT_B, numPoints, B);
            }
        }

    }

    Painter_DrawHLineC(GridChannelBottom(), GridLeft(), GridRight(), gColorFill);
    Painter_DrawHLine(GridMathBottom(), GridLeft(), GridRight());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursors(void)
{
    Channel source = CURS_SOURCE;
    Painter_SetColor(ColorCursors(source));
    if(sCursors_NecessaryDrawCursors())
    {

        bool bothCursors = CURsT_ENABLED && CURsU_ENABLED;  // Признак того, что включены и вертикальные и горизонтальные курсоры - надо нарисовать 
                                                            // квадраты в местах пересечения

        int x0 = -1;
        int x1 = -1;
        int y0 = -1;
        int y1 = -1;

        if(bothCursors)
        {
            x0 = GridLeft() + (int)CURsT_POS(source, 0);
            x1 = GridLeft() + (int)CURsT_POS(source, 1);
            y0 = GRID_TOP + (int)sCursors_GetCursPosU(source, 0);
            y1 = GRID_TOP + (int)sCursors_GetCursPosU(source, 1);

            Painter_DrawRectangle(x0 - 2, y0 - 2, 4, 4);
            Painter_DrawRectangle(x1 - 2, y1 - 2, 4, 4);
        }

        if(CURsT_ENABLED)
        {
            DrawVerticalCursor((int)CURsT_POS(source, 0), y0);
            DrawVerticalCursor((int)CURsT_POS(source, 1), y1);
        }
        if(CURsU_ENABLED)
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
    const int SIZE = 100;
    char buffer[SIZE] ={0};

    TBase tBase = SET_TBASE;
    int16 tShift = SET_TSHIFT;

    if(MODE_WORK_ROM && SHOW_IN_INT_DIRECT)
    { 
    }
    else if(!MODE_WORK_DIR)
    {
        if(DS)
        {
            tBase = TBASE_DS;
            tShift = TSHIFT_DS;
        }
    }
    snprintf(buffer, SIZE, "р\xa5%s", Tables_GetTBaseString(tBase));
    Painter_DrawText(x, y0, buffer);

    buffer[0] = 0;
    char bufForVal[20];
    snprintf(buffer, SIZE, "\xa5%s", FPGA_GetTShiftString(tShift, bufForVal));
    Painter_DrawText(x + 35, y0, buffer);

    buffer[0] = 0;
    const char * const source[3] ={"1", "2", "\x82"};
    if(MODE_WORK_DIR)
    {
        snprintf(buffer, 100, "с\xa5\x10%s", source[TRIGSOURCE]);
    }

    Painter_DrawTextC(x, y1, buffer, ColorTrig());

    buffer[0] = 0;
    static const char * const couple[] =
    {
        "\x92",
        "\x91",
        "\x92",
        "\x92"
    };
    static const char * const polar[] =
    {
        "\xa7",
        "\xa6"
    };
    static const char * const filtr[] =
    {
        "\xb5\xb6",
        "\xb5\xb6",
        "\xb3\xb4",
        "\xb1\xb2"
    };
    if(MODE_WORK_DIR)
    {
        snprintf(buffer, SIZE, "\xa5\x10%s\x10\xa5\x10%s\x10\xa5\x10", couple[TRIG_INPUT], polar[TRIG_POLARITY]);
        Painter_DrawText(x + 18, y1, buffer);
        Painter_DrawChar(x + 45, y1, filtr[TRIG_INPUT][0]);
        Painter_DrawChar(x + 53, y1, filtr[TRIG_INPUT][1]);
    }

    buffer[0] = '\0';
    const char mode[] =
    {
        '\xb7',
        '\xa0',
        '\xb0'
    };
    if(MODE_WORK_DIR)
    {
        snprintf(buffer, 100, "\xa5\x10%c", mode[START_MODE]);
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

    if(MODE_WORK_DIR)
    {
        WriteStringAndNumber("накопл", (int16)x, (int16)y0, NUM_ACCUM);
        WriteStringAndNumber("усредн", (int16)x, (int16)y1, NUM_AVE);
        WriteStringAndNumber("мн\x93мкс", (int16)x, (int16)y2, NUM_MIN_MAX);
    }

    x += 42;
    Painter_DrawVLine(x, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2);

    Painter_SetFont(TypeFont_8);

    if(MODE_WORK_DIR)
    {
        char mesFreq[20] = "\x7c=";
        char buffer[20];
        float freq = FreqMeter_GetFreq();
        if(freq == -1.0f) //-V550
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
    if((gEthIsConnected || gBF.cableEthIsConnected) && gTimeMS > 2000)
    {
        Painter_Draw4SymbolsInRectC(x + 87, GRID_BOTTOM + 2, SYMBOL_ETHERNET, gEthIsConnected ? COLOR_WHITE : COLOR_FLASH_01);
    }

    if(gConnectToHost || gCableVCPisConnected)
    {
        Painter_Draw4SymbolsInRectC(x + 72, GRID_BOTTOM + 2, SYMBOL_USB, gConnectToHost ? COLOR_WHITE : COLOR_FLASH_01);
    }

    Painter_SetColor(gColorFill);
    // Пиковый детектор
    if(!SET_PEAKDET_DIS)
    {
        Painter_DrawChar(x + 38, GRID_BOTTOM + 11, '\x12');
        Painter_DrawChar(x + 46, GRID_BOTTOM + 11, '\x13');
    }

    if(MODE_WORK_DIR)
    {
        Painter_SetFont(TypeFont_5);
        WriteStringAndNumber("СГЛАЖ.:", (int16)(x + 57), (int16)(GRID_BOTTOM + 10), (int)ENUM_SMOOTHING + 1);
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
    int trigLev = SET_TRIGLEV(ch) + (TRIGSOURCE_EXT ? 0 : SET_RSHIFT(ch) - RShiftZero);
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
        int shiftFull = SET_TRIGLEV(TRIGSOURCE) + (TRIGSOURCE_EXT ? 0 : SET_RSHIFT(ch));
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
    if(!SHOW_MEASURES)
    {
        gBF.topMeasures = GRID_BOTTOM;
        return;
    }

    gBF.topMeasures = GRID_BOTTOM;

    Processing_CalculateMeasures();

    if(MEAS_ZONE_HAND)
    {
        int x0 = POS_MEAS_CUR_T_0 - SHIFT_IN_MEMORY + GridLeft();
        int y0 = POS_MEAS_CUR_U_0 + GRID_TOP;
        int x1 = POS_MEAS_CUR_T_1 - SHIFT_IN_MEMORY + GridLeft();
        int y1 = POS_MEAS_CUR_U_1 + GRID_TOP;
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
            bool active = Measure_IsActive(str, elem) && GetNameOpenedPage() == PageSB_Measures_Tune;
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
                if(meas == MARKED_MEAS)
                {
                    Painter_FillRegionC(x + 1, y + 1, dX - 2, 9, active ? gColorBack : gColorFill);
                    Painter_DrawTextC(x + 4, y + 2, Measure_Name(str, elem), active ? gColorFill : gColorBack);
                }
                if(SOURCE_MEASURE_A && SET_ENABLED_A)
                {
                    Painter_DrawTextC(x + 2, y + 11, Processing_GetStringMeasure(meas, A, buffer, SIZE_BUFFER), gColorChan[A]);
                }
                else if(SOURCE_MEASURE_B && SET_ENABLED_B)
                {
                    Painter_DrawTextC(x + 2, y + 11, Processing_GetStringMeasure(meas, B, buffer, SIZE_BUFFER), gColorChan[B]);
                }
                else
                {
                    Painter_DrawTextC(x + 2, y + 11, Processing_GetStringMeasure(meas, A, buffer, SIZE_BUFFER), gColorChan[A]);
                    Painter_DrawTextC(x + 2, y + (SET_ENABLED_A ? 20 : 11), Processing_GetStringMeasure(meas, B, buffer, SIZE_BUFFER), gColorChan[B]);
                }
#undef SIZE_BUFFER
            }
        }
    }

    if(GetNameOpenedPage() == PageSB_Measures_Tune)
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
#define FIRST_POINT (points.word0)
#define LAST_POINT  (points.word1)
    
    BitSet64 points = sDisplay_PointsOnDisplay();
    LAST_POINT = FIRST_POINT + 281;

    // Рисуем TPos
    int shiftTPos = TPOS_IN_POINTS - SHIFT_IN_MEMORY_IN_POINTS;

    float scale = (float)(LAST_POINT - FIRST_POINT) / GridWidth();
    int gridLeft = GridLeft();
    int x = gridLeft + (int)(shiftTPos * scale) - 3;
    if(IntInRange(x + 3, gridLeft, GridRight() + 1))
    {
        Painter_Draw2SymbolsC(x, GRID_TOP - 1, SYMBOL_TPOS_2, SYMBOL_TPOS_1, gColorBack, gColorFill);
    };

    // Рисуем tShift
    int shiftTShift = TPOS_IN_POINTS - TSHIFT_IN_POINTS;
    if(IntInRange(shiftTShift, FIRST_POINT, LAST_POINT))
    {
        int x = gridLeft + shiftTShift - FIRST_POINT - 3;
        Painter_Draw2SymbolsC(x, GRID_TOP - 1, SYMBOL_TSHIFT_NORM_1, SYMBOL_TSHIFT_NORM_2, gColorBack, gColorFill);
    }
    else if(shiftTShift < FIRST_POINT)
    {
        Painter_Draw2SymbolsC(gridLeft + 1, GRID_TOP, SYMBOL_TSHIFT_LEFT_1, SYMBOL_TSHIFT_LEFT_2, gColorBack, gColorFill);
        Painter_DrawLineC(GridLeft() + 9, GRID_TOP + 1, GridLeft() + 9, GRID_TOP + 7, gColorBack);
    }
    else if(shiftTShift > LAST_POINT)
    {
        Painter_Draw2SymbolsC(GridRight() - 8, GRID_TOP, SYMBOL_TSHIFT_RIGHT_1, SYMBOL_TSHIFT_RIGHT_2, gColorBack, gColorFill);
        Painter_DrawLineC(GridRight() - 9, GRID_TOP + 1, GridRight() - 9, GRID_TOP + 7, gColorBack);
    }
    
#undef FIRST_POINT
#undef LAST_POINT
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
void Display_DrawConsole(void)
{
    int count = 0;
    Painter_SetFont(CONSOLE_SIZE_FONT == 5 ? TypeFont_5 : TypeFont_8);
    int height = Font_GetSize();

    int lastString = FirstEmptyString() - 1;
    int numString = CONSOLE_NUM_STRINGS;
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
    if(showLevelTrigLev && MODE_WORK_DIR)
    {
        TrigSource trigSource = TRIGSOURCE;
        float trigLev = RSHIFT_2_ABS(SET_TRIGLEV(trigSource), TRIGSOURCE_EXT ? Range_500mV : SET_RANGE(trigSource));
        if(TRIG_INPUT_AC && !TRIGSOURCE_EXT)
        {
            uint16 rShift = SET_RSHIFT(trigSource);
            float rShiftAbs = RSHIFT_2_ABS(rShift, SET_RANGE(trigSource));
            trigLev += rShiftAbs;
        }

        char buffer[20];
        strcpy(buffer, DICT(DTrigLev));
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
    if(!SHOW_STAT)
    {
        return;
    }
    const int SIZE = 20;
    static char buffer[SIZE];
    static bool first = true;
    static uint timeMSstartCalculation = 0;
    static int numFrames = 0;
    static float numMS = 0.0f;
    if(first)
    {
        timeMSstartCalculation = gTimeMS;
        first = false;
    }
    numMS += timeTicks / 120000.0f;
    numFrames++;

    if((gTimeMS - timeMSstartCalculation) >= 500)
    {
        snprintf(buffer, SIZE, "%.1fms/%d", numMS / numFrames, numFrames * 2);
        timeMSstartCalculation = gTimeMS;
        numMS = 0.0f;
        numFrames = 0;
    }

    Painter_DrawRectangleC(GridLeft(), GridFullBottom() - 10, 84, 10, gColorFill);
    Painter_FillRegionC(GridLeft() + 1, GridFullBottom() - 9, 82, 8, gColorBack);
    Painter_DrawTextC(GridLeft() + 2, GridFullBottom() - 9, buffer, gColorFill);

    char message[SIZE] ={0};
    snprintf(message, SIZE, "%d", DS_NumElementsWithSameSettings());
    strcat(message, "/");
    char numAvail[10] ={0};
    snprintf(numAvail, SIZE, "%d", DS_NumberAvailableEntries());
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
    if(CONSOLE_IN_PAUSE)
    {
        return;
    }
    static int num = 0;
    const int SIZE = 100;
    char buffer[SIZE];
    snprintf(buffer, SIZE, "%d\x11", num++);
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
        strings[FirstEmptyString()] = address; //-V557
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
            timeWarnings[i] = gTimeMS;
            alreadyStored = true;
        }
        else if(warnings[i] == message)
        {
            timeWarnings[i] = gTimeMS;
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
    if(SCALE_FFT_LOG)
    {
        static const int nums[] ={4, 6, 8};
        static const char * const strs[] ={"0", "-10", "-20", "-30", "-40", "-50", "-60", "-70"};
        int numParts = nums[MAX_DB_FFT];
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
    else if(SCALE_FFT_LIN)
    {
        static const char * const strs[] ={"1.0", "0.8", "0.6", "0.4", "0.2"};
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
    if(!SET_ENABLED(ch))
    {
        return;
    }

    float dataR[numPoints];
    float spectrum[numPoints];

    float freq0 = 0.0f;
    float freq1 = 0.0f;
    float density0 = 0.0f;
    float density1 = 0.0f;
    int y0 = 0;
    int y1 = 0;
    int s = 2;

    uint8 *data = malloc(numPoints);

    RAM_MemCpy16((void *)dataIn, data, numPoints);

    Math_PointsRelToVoltage(data, numPoints, RANGE_DS(ch), RSHIFT_DS(ch), dataR);
    Math_CalculateFFT(dataR, numPoints, spectrum, &freq0, &density0, &freq1, &density1, &y0, &y1);
    DrawSpectrumChannel(spectrum, gColorChan[ch]);
    if(!MENU_IS_SHOWN || MenuIsMinimize())
    {
        Color color = gColorFill;
        WriteParametersFFT(ch, freq0, density0, freq1, density1);
        Painter_DrawRectangleC(POS_MATH_CUR_0 + GridLeft() - s, y0 - s, s * 2, s * 2, color);
        Painter_DrawRectangle(POS_MATH_CUR_1 + GridLeft() - s, y1 - s, s * 2, s * 2);

        Painter_DrawVLine(GridLeft() + POS_MATH_CUR_0, GridMathBottom(), y0 + s);
        Painter_DrawVLine(GridLeft() + POS_MATH_CUR_1, GridMathBottom(), y1 + s);
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
    if(MODE_WORK_DIR)
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
        Channel source = CURS_SOURCE;
        Color colorText = gColorChan[source];
        if (CURsU_ENABLED)
        {
            Painter_DrawTextC(x, y1, "1:", colorText);
            Painter_DrawText(x, y2, "2:");
            x += 7;
            Painter_DrawText(x, y1, sCursors_GetCursVoltage(source, 0, buffer));
            Painter_DrawText(x, y2, sCursors_GetCursVoltage(source, 1, buffer));
            x = startX + 49;
            float pos0 = Math_VoltageCursor(sCursors_GetCursPosU(source, 0), SET_RANGE(source), SET_RSHIFT(source));
            float pos1 = Math_VoltageCursor(sCursors_GetCursPosU(source, 1), SET_RANGE(source), SET_RSHIFT(source));
            float delta = fabsf(pos1 - pos0);
            if(SET_DIVIDER_10(source))
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
        if(CURsT_ENABLED)
        {
            Painter_SetColor(colorText);
            Painter_DrawText(x, y1, "1:");
            Painter_DrawText(x, y2, "2:");
            x += 7;
            Painter_DrawText(x, y1, sCursors_GetCursorTime(source, 0, buffer));
            Painter_DrawText(x, y2, sCursors_GetCursorTime(source, 1, buffer));
            x = startX + 153;

            /// \todo Дичь.

            /*
            float p0 = 0.0f;
            memcpy(&p0, &CURsT_POS(source, 0), sizeof(float));
            float p1 = 0.0f;
            memcpy(&p1, &CURsT_POS(source, 1), sizeof(float));
            */

            float pos0 = Math_TimeCursor(CURsT_POS(source, 0), SET_TBASE);
            float pos1 = Math_TimeCursor(CURsT_POS(source, 1), SET_TBASE);
            float delta = fabsf(pos1 - pos0);
            Painter_DrawText(x, y1, ":dT=");
            char buffer[20];
            Painter_DrawText(x + 17, y1, Time2String(delta, false, buffer));
            Painter_DrawText(x, y2, ":");
            Painter_DrawText(x + 8, y2, sCursors_GetCursorPercentsT(source, buffer));

            if(CURSORS_SHOW_FREQ)
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

    if(!MODE_WORK_RAM)
    {
        Painter_DrawVLineC(x, 1, GRID_TOP - 2, gColorFill);

        x += 2;

        if(trigEnable)
        {
            Painter_FillRegionC(x, 1 + y, GRID_TOP - 3, GRID_TOP - 7, gColorFill);
            Painter_DrawTextC(x + 3, 3 + y, DICT(DTrig), gColorBack);
        }
    }

    // Режим работы
    static const char * const strings[][2] =
    {
        {"ИЗМ", "MEAS"},
        {"ПОСЛ", "LAST"},
        {"ВНТР", "INT"}
    };

    if(!MODE_WORK_DIR)
    {
        x += 18;
        Painter_DrawVLineC(x, 1, GRID_TOP - 2, gColorFill);
        x += 2;
        Painter_DrawText(LANG_RU ? x : x + 3, -1, DICT(DMode));
        Painter_DrawStringInCenterRect(x + 1, 9, 25, 8, strings[MODE_WORK][LANG]);
    }
    else
    {
        x -= 9;
    }

    if(!MODE_WORK_RAM)
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
    if(!SET_ENABLED(ch))
    {
        return;
    }

    static const char * const couple[] = {"\x92", "\x91", "\x90" };

    Color color = gColorChan[ch];

    bool inverse = SET_INVERSE(ch);
    Divider divider = SET_DIVIDER(ch);
    Range range = SET_RANGE(ch);

    const int widthField = 91;
    const int heightField = 8;
    Color colorDraw = inverse ? COLOR_WHITE : color;
    if(inverse)
    {
        Painter_FillRegionC(x, y, widthField, heightField, color);
    }
    const int SIZE = 100;
    char buffer[SIZE];
    snprintf(buffer, SIZE, "%s\xa5%s\xa5%s", (ch == A) ? DICT(D1ch) : DICT(D2ch), couple[SET_COUPLE(ch)], sChannel_Range2String(range, divider));
    Painter_DrawTextC(x + 1, y, buffer, colorDraw);
    char bufferTemp[SIZE];
    snprintf(bufferTemp, SIZE, "\xa5%s", sChannel_RShift2String((int16)SET_RSHIFT(ch), range, divider, buffer));
    Painter_DrawText(x + 46, y, bufferTemp);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteStringAndNumber(const char *text, int16 x, int16 y, int number)
{
    const int SIZE = 100;
    char buffer[SIZE];
    Painter_DrawTextC(x, y, text, gColorFill);
    if(number == 0)
    {
        snprintf(buffer, SIZE, "-");
    }
    else
    {
        snprintf(buffer, SIZE, "%d", number);
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

    if(MODE_WORK_ROM || MODE_WORK_RAM)
    {
        if(DS)
        {
            y -= 9;
            time.day = TIME_DAY_DS;
            time.hours = TIME_HOURS_DS;
            time.minutes = TIME_MINUTES_DS;
            time.seconds = TIME_SECONDS_DS;
            time.month = TIME_MONTH_DS;
            time.year = TIME_YEAR_DS;
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
    if(ALT_MARKERS_HIDE)
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

    static const Channel order[NumChannels][2] = { {B, A}, {A, B} };

    DrawCursorRShift(order[LAST_AFFECTED_CH][0]);
    DrawCursorRShift(order[LAST_AFFECTED_CH][1]);
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
    uint time = gTimeMS;
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
    Painter_SetColor(gColorGrid);
    Painter_DrawVPointLine(centerX, top + stepX, bottom - stepX, (float)stepX);
    uint16 masX[6] ={(uint16)(left + 1), (uint16)(left + 2), (uint16)(centerX - 1), (uint16)(centerX + 1), (uint16)(right - 2), (uint16)(right - 1)};
    Painter_DrawMultiVPointLine(6, top + deltaY, masX, deltaY, (bottom - top) / deltaY, gColorGrid);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSpectrumChannel(const float *spectrum, Color color)
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
    Painter_DrawText(x, y, SCALE_FFT_LOG ? Float2Db(density0, 4, buffer) : Float2String(density0, false, 7, buffer));
    y += dY;
    Painter_DrawText(x, y, SCALE_FFT_LOG ? Float2Db(density1, 4, buffer) : Float2String(density1, false, 7, buffer));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursorRShift(Channel ch)
{
    int x = GridRight() - GridWidth() - Measure_GetDeltaGridLeft();

    if(ch == Math)
    {
        int yCenter = (GridMathTop() + GridMathBottom()) / 2;
//        int y = yCenter - Math_RShift2Pixels(SET_RSHIFT_MATH, GridMathHeight());
        float scaleFull = (float)GridMathHeight() / (RShiftMax - RShiftMin);
        int yFull = yCenter - (int)(scaleFull * (SET_RSHIFT_MATH - RShiftZero));
        Painter_DrawCharC(x - 9, yFull - 4, SYMBOL_RSHIFT_NORMAL, gColorFill);
        Painter_DrawCharC(x - 8, yFull - 5, 'm', gColorBack);
        return;
    }
    if(!SET_ENABLED(ch))
    {
        return;
    }

    int rShift = SET_RSHIFT(ch);

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
        if(((ch == A) ? showLevelRShiftA : showLevelRShiftB) && MODE_WORK_DIR)
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
    if(SHOW_MEASURES && MEAS_COMPRESS_GRID)
    {
        if(NUM_MEASURES_1_5)
        {
            return SOURCE_MEASURE_A_B ? 42 : 44;
        }
        if(NUM_MEASURES_2_5)
        {
            return SOURCE_MEASURE_A_B ? 69 : 39;
        }
        if(NUM_MEASURES_3_5)
        {
            return SOURCE_MEASURE_A_B ? 54 : 68;
        }
    }

    return 49;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int CalculateCountH(void)
{
    if(MEAS_COMPRESS_GRID)
    {
        if(NUM_MEASURES_6_1)
        {
            return 73;
        }
        if(NUM_MEASURES_6_2)
        {
            return 83;
        }
    }
    return 69;
}

static uint timeStart = 0;
static const char *textWait = 0;
static bool clearBackground = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncOnWait(void)
{
    if (clearBackground)
    {
        Painter_BeginScene(gColorBack);
    }

    uint time = ((gTimeMS - timeStart) / 50) % 100;

    if (time > 50)
    {
        time = (100 - time);
    }

    int width = 200;
    int height = 80;
    int x = 160 - width / 2;
    int y = 120 - height / 2;

    Painter_FillRegionC(x, y, width, height, gColorBack);
    Painter_DrawRectangleC(x, y, width, height, gColorFill);
    Painter_DrawStringInCenterRect(x, y, width, height - 20, textWait);
    char buffer[100];
    buffer[0] = 0;
    for (uint i = 0; i < time; i++)
    {
        strcat(buffer, ".");
    }
    Painter_DrawStringInCenterRect(x, y + 20, width, height - 20, buffer);
    Painter_EndScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_FuncOnWaitStart(const char *text, bool eraseBackground)
{
    timeStart = gTimeMS;
    textWait = text;
    clearBackground = eraseBackground;
    Display_SetDrawMode(DrawMode_Hand, FuncOnWait);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display_FuncOnWaitStop(void)
{
    Display_SetDrawMode(DrawMode_Auto, 0);
}
