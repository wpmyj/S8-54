#include "Display.h"
#include "Grid.h"
#include "Log.h"
#include "PainterData.h"
#include "Symbols.h"
#include "Data/Data.h"
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
Display display;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NEED_SET_ORIENTATION    (bf.needSetOrientation)

static struct BitFieldDisplay
{
    uint needSetOrientation : 1;
} bf = {0};

typedef struct
{
    Warning         warning;
    bool            good;
    char * const    message[2][3];
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
static void SendOrientationToDisplay(void);

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
void Display::Init(void)
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

    painter.ResetFlash();

    painter.LoadFont(TypeFont_5);
    painter.LoadFont(TypeFont_8);
    painter.LoadFont(TypeFont_UGO);
    painter.LoadFont(TypeFont_UGO2);
    painter.SetFont(TypeFont_8);

    painter.LoadPalette();

    SendOrientationToDisplay();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::Update(void)
{
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
        painter.BeginScene(gColorBack);
        DrawFullGrid();
    }

    painterData.DrawData();

    if(needClear)
    {
        painterData.DrawMath();
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

    FreqMeter_Draw(grid.Left(), GRID_TOP);

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

    Display::DrawConsole();

    if(needClear)
    {
        WriteValueTrigLevel();
    }

    DrawTimeForFrame(gTimerTics - timeStart);

    painter.EndScene();

    if(NEED_SAVE_TO_FLASHDRIVE)
    {
        if(painter.SaveScreenToFlashDrive())
        {
            Display::ShowWarning(FileIsSaved);
        }
        NEED_SAVE_TO_FLASHDRIVE = 0;
    }

    FSMC_SetMode(mode);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::RotateRShift(Channel ch)
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
void Display::RotateTrigLev(void)
{
    if(TIME_SHOW_LEVELS && TRIG_MODE_FIND_HAND)
    {
        showLevelTrigLev = true;
        Timer_SetAndStartOnce(kShowLevelTrigLev, DisableShowLevelTrigLev, TIME_SHOW_LEVELS * 1000);
    }
    NEED_FINISH_DRAW = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::EnableTrigLabel(bool enable)
{
    trigEnable = enable;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ClearFromWarnings(void)
{
    Timer_Disable(kShowMessages);
    for(int i = 0; i < NUM_WARNINGS; i++)
    {
        warnings[i] = 0;
        timeWarnings[i] = 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::SetDrawMode(DrawMode mode, pFuncVV func)
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
void Display::SetAddDrawFunction(pFuncVV func)
{
    funcAdditionDraw = func;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
pFuncVV Display::GetAddDrawFunction(void)
{
    return funcAdditionDraw;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::RemoveAddDrawFunction(void)
{
    funcAdditionDraw = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::Clear(void)
{
    painter.FillRegion(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 2, gColorBack);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ShiftScreen(int delta)
{
    if(PEAKDET_DS)
    {
        delta *= 2;
    }
    int16 shift = SHIFT_IN_MEMORY;
    shift += (int16)delta;
    int16 max = (int16)BytesInChannel(DS) - 282 * (PEAKDET_DS ? 2 : 1);
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
void Display::ChangedRShiftMarkers(bool active)
{
    drawRShiftMarkers = !ALT_MARKERS_HIDE;
    Timer_SetAndStartOnce(kRShiftMarkersAutoHide, OnRShiftMarkersAutoHide, 5000);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::AddStringToIndicating(const char *string)
{
    if(FirstEmptyString() == MAX_NUM_STRINGS)
    {
        DeleteFirstString();
    }

    AddString(string);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::OneStringUp(void)
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
void Display::OneStringDown(void)
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
void Display::SetPauseForConsole(bool pause)
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
void Display::SetOrientation(DisplayOrientation orientation)
{
    DISPLAY_ORIENTATION = orientation;
    NEED_SET_ORIENTATION = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ShowWarning(Warning warning)
{
    painter.ResetFlash();
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
static void SendOrientationToDisplay(void)
{
    if(NEED_SET_ORIENTATION)
    {
        uint8 command[4] ={SET_ORIENTATION, DISPLAY_ORIENTATION, 0, 0};
        painter.SendToDisplay(command, 4);
        painter.SendToInterfaces(command, 2);
        NEED_SET_ORIENTATION = 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawFullGrid(void)
{
    if(sDisplay_IsSeparate())
    {
        DrawGrid(grid.Left(), GRID_TOP, grid.Width(), grid.FullHeight() / 2);
        if(FFT_ENABLED)
        {
            DrawGridSpectrum();
        }
        if(FUNC_ENABLED)
        {
            DrawGrid(grid.Left(), GRID_TOP + grid.FullHeight() / 2, grid.Width(), grid.FullHeight() / 2);
        }
        painter.DrawHLine(GRID_TOP + grid.FullHeight() / 2, grid.Left(), grid.Left() + grid.Width(), gColorFill);
    }
    else
    {
        DrawGrid(grid.Left(), GRID_TOP, grid.Width(), grid.FullHeight());
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSpectrum(void)
{
    if(!FFT_ENABLED)
    {
        return;
    }

    painter.DrawVLine(grid.Right(), grid.ChannelBottom() + 1, grid.MathBottom() - 1, gColorBack);

    if(MODE_WORK_DIR)
    {
        int numPoints = BytesInChannel(DS);

        if (numPoints > 4096)       /// \todo Пока 8к и более не хочет считать
        {
            numPoints = 4096;
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

    painter.DrawHLine(grid.ChannelBottom(), grid.Left(), grid.Right(), gColorFill);
    painter.DrawHLine(grid.MathBottom(), grid.Left(), grid.Right());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursors(void)
{
    Channel source = CURS_SOURCE;
    painter.SetColor(ColorCursors(source));
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
            x0 = grid.Left() + (int)CURsT_POS(source, 0);
            x1 = grid.Left() + (int)CURsT_POS(source, 1);
            y0 = GRID_TOP + (int)sCursors_GetCursPosU(source, 0);
            y1 = GRID_TOP + (int)sCursors_GetCursPosU(source, 1);

            painter.DrawRectangle(x0 - 2, y0 - 2, 4, 4);
            painter.DrawRectangle(x1 - 2, y1 - 2, 4, 4);
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

    painter.DrawHLine(grid.ChannelBottom(), 1, grid.Left() - meas.GetDeltaGridLeft() - 2, gColorFill);
    painter.DrawHLine(grid.FullBottom(), 1, grid.Left() - meas.GetDeltaGridLeft() - 2);
    WriteTextVoltage(A, x + 2, y0);
    WriteTextVoltage(B, x + 2, y1);
    painter.DrawVLine(x + 95, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2, gColorFill);

    x += 98;
    const int SIZE = 100;
    char buffer[SIZE] ={0};

    TBase tBase = SET_TBASE;
    int16 tShift = SET_TSHIFT;

    snprintf(buffer, SIZE, "р\xa5%s", Tables_GetTBaseString(tBase));
    painter.DrawText(x, y0, buffer);

    buffer[0] = 0;
    char bufForVal[20];
    snprintf(buffer, SIZE, "\xa5%s", FPGA_GetTShiftString(tShift, bufForVal));
    painter.DrawText(x + 35, y0, buffer);

    buffer[0] = 0;
    const char * const source[3] ={"1", "2", "\x82"};
    if(MODE_WORK_DIR)
    {
        snprintf(buffer, 100, "с\xa5\x10%s", source[TRIGSOURCE]);
    }

    painter.DrawText(x, y1, buffer, ColorTrig());

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
        painter.DrawText(x + 18, y1, buffer);
        painter.DrawChar(x + 45, y1, filtr[TRIG_INPUT][0]);
        painter.DrawChar(x + 53, y1, filtr[TRIG_INPUT][1]);
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
        painter.DrawText(x + 63, y1, buffer);
    }

    painter.DrawVLine(x + 79, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2, gColorFill);

    painter.DrawHLine(GRID_BOTTOM, GRID_RIGHT + 2, SCREEN_WIDTH - 2);
    painter.DrawHLine(grid.ChannelBottom(), GRID_RIGHT + 2, SCREEN_WIDTH - 2);

    x += 82;
    y0 = y0 - 3;
    y1 = y1 - 6;
    int y2 = y1 + 6;
    painter.SetFont(TypeFont_5);

    if(MODE_WORK_DIR)
    {
        WriteStringAndNumber(LANG_RU ? "накопл" : "accum", (int16)x, (int16)y0, NUM_ACCUM);
        WriteStringAndNumber(LANG_RU ? "усредн" : "ave", (int16)x, (int16)y1, NUM_AVE);
        WriteStringAndNumber(LANG_RU ? "мн\x93мкс" : "mn\x93max", (int16)x, (int16)y2, NUM_MIN_MAX);
    }

    x += 42;
    painter.DrawVLine(x, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2);

    painter.SetFont(TypeFont_8);

    if(MODE_WORK_DIR)
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
            strcat(mesFreq, trans.Freq2String(freq, false, buffer));
        }
        painter.DrawText(x + 3, GRID_BOTTOM + 2, mesFreq);
    }

    DrawTime(x + 3, GRID_BOTTOM + 11);

    painter.DrawVLine(x + 55, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2);

    painter.SetFont(TypeFont_UGO2);

    // Флешка
    if(FDRIVE_IS_CONNECTED)
    {
        painter.Draw4SymbolsInRect(x + 57, GRID_BOTTOM + 2, SYMBOL_FLASH_DRIVE);
    }

    // Ethernet
    if((gEthIsConnected || CABLE_LAN_IS_CONNECTED) && gTimeMS > 2000)
    {
        painter.Draw4SymbolsInRect(x + 87, GRID_BOTTOM + 2, SYMBOL_ETHERNET, gEthIsConnected ? COLOR_WHITE : COLOR_FLASH_01);
    }

    if(CONNECTED_TO_USB || CABLE_USB_IS_CONNECTED)
    {
        painter.Draw4SymbolsInRect(x + 72, GRID_BOTTOM + 2, SYMBOL_USB, CONNECTED_TO_USB ? COLOR_WHITE : COLOR_FLASH_01);
    }

    painter.SetColor(gColorFill);
    // Пиковый детектор
    if(!SET_PEAKDET_DIS)
    {
        painter.DrawChar(x + 38, GRID_BOTTOM + 11, '\x12');
        painter.DrawChar(x + 46, GRID_BOTTOM + 11, '\x13');
    }

    if(MODE_WORK_DIR)
    {
        painter.SetFont(TypeFont_5);
        WriteStringAndNumber("СГЛАЖ.:", (int16)(x + 57), (int16)(GRID_BOTTOM + 10), (int)ENUM_SMOOTHING + 1);
        painter.SetFont(TypeFont_8);
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
    float scale = 1.0f / ((TrigLevMax - TrigLevMin) / 2.4f / grid.ChannelHeight());
    int y0 = (GRID_TOP + grid.ChannelBottom()) / 2 + (int)(scale * (TrigLevZero - TrigLevMin));
    int y = y0 - (int)(scale * (trigLev - TrigLevMin));

    if(!TRIGSOURCE_EXT)
    {
        y = (y - grid.ChannelCenterHeight()) + grid.ChannelCenterHeight();
    }

    int x = grid.Right();
    painter.SetColor(ColorTrig());
    if(y > grid.ChannelBottom())
    {
        painter.DrawChar(x + 3, grid.ChannelBottom() - 11, SYMBOL_TRIG_LEV_LOWER);
        painter.SetPoint(x + 5, grid.ChannelBottom() - 2);
        y = grid.ChannelBottom() - 7;
        x--;
    }
    else if(y < GRID_TOP)
    {
        painter.DrawChar(x + 3, GRID_TOP + 2, SYMBOL_TRIG_LEV_ABOVE);
        painter.SetPoint(x + 5, GRID_TOP + 2);
        y = GRID_TOP + 7;
        x--;
    }
    else
    {
        painter.DrawChar(x + 1, y - 4, SYMBOL_TRIG_LEV_NORMAL);
    }
    painter.SetFont(TypeFont_5);

    const char simbols[3] ={'1', '2', 'В'};
    int dY = 0;

    painter.DrawChar(x + 5, y - 9 + dY, simbols[TRIGSOURCE], gColorBack);
    painter.SetFont(TypeFont_8);

    if(drawRShiftMarkers && !MenuIsMinimize())
    {
        DrawScaleLine(SCREEN_WIDTH - 11, true);
        int left = grid.Right() + 9;
        int height = grid.ChannelHeight() - 2 * DELTA;
        int shiftFullMin = RShiftMin + TrigLevMin;
        int shiftFullMax = RShiftMax + TrigLevMax;
        scale = (float)height / (shiftFullMax - shiftFullMin);
        int shiftFull = SET_TRIGLEV(TRIGSOURCE) + (TRIGSOURCE_EXT ? 0 : SET_RSHIFT(ch));
        int yFull = GRID_TOP + DELTA + height - (int)(scale * (shiftFull - RShiftMin - TrigLevMin) + 4);
        painter.FillRegion(left + 2, yFull + 1, 4, 6, ColorTrig());
        painter.SetFont(TypeFont_5);
        painter.DrawChar(left + 3, yFull - 5 + dY, simbols[TRIGSOURCE], gColorBack);
        painter.SetFont(TypeFont_8);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawMeasures(void)
{
    TOP_MEASURES = GRID_BOTTOM;

    if(!SHOW_MEASURES)
    {
        return;
    }

    processing.CalculateMeasures();

    if(MEAS_ZONE_HAND)
    {
        int x0 = POS_MEAS_CUR_T_0 - SHIFT_IN_MEMORY + grid.Left();
        int y0 = POS_MEAS_CUR_U_0 + GRID_TOP;
        int x1 = POS_MEAS_CUR_T_1 - SHIFT_IN_MEMORY + grid.Left();
        int y1 = POS_MEAS_CUR_U_1 + GRID_TOP;
        SortInt(&x0, &x1);
        SortInt(&y0, &y1);
        painter.DrawRectangle(x0, y0, x1 - x0, y1 - y0, gColorFill);
    }

    int x0 = grid.Left() - meas.GetDeltaGridLeft();
    int dX = meas.GetDX();
    int dY = meas.GetDY();
    int y0 = meas.GetTopTable();

    int numRows = meas.NumRows();
    int numCols = meas.NumCols();

    for(int str = 0; str < numRows; str++)
    {
        for(int elem = 0; elem < numCols; elem++)
        {
            int x = x0 + dX * elem;
            int y = y0 + str * dY;
            bool active = meas.IsActive(str, elem) && GetNameOpenedPage() == PageSB_Measures_Tune;
            Color color = active ? gColorBack : gColorFill;
            Meas measure = meas.Type(str, elem);
            if(measure != Meas_None)
            {
                painter.FillRegion(x, y, dX, dY, gColorBack);
                painter.DrawRectangle(x, y, dX, dY, gColorFill);
                TOP_MEASURES = math.MinInt(TOP_MEASURES, y);
            }
            if(active)
            {
                painter.FillRegion(x + 2, y + 2, dX - 4, dY - 4, gColorFill);
            }
            if(measure != Meas_None)
            {
#define SIZE_BUFFER 20
                char buffer[SIZE_BUFFER];

                painter.DrawText(x + 4, y + 2, meas.Name(str, elem), color);
                if(measure == MARKED_MEAS)
                {
                    painter.FillRegion(x + 1, y + 1, dX - 2, 9, active ? gColorBack : gColorFill);
                    painter.DrawText(x + 4, y + 2, meas.Name(str, elem), active ? gColorFill : gColorBack);
                }
                if(SOURCE_MEASURE_A && SET_ENABLED_A)
                {
                    painter.DrawText(x + 2, y + 11, processing.GetStringMeasure(measure, A, buffer, SIZE_BUFFER), gColorChan[A]);
                }
                else if(SOURCE_MEASURE_B && SET_ENABLED_B)
                {
                    painter.DrawText(x + 2, y + 11, processing.GetStringMeasure(measure, B, buffer, SIZE_BUFFER), gColorChan[B]);
                }
                else
                {
                    painter.DrawText(x + 2, y + 11, processing.GetStringMeasure(measure, A, buffer, SIZE_BUFFER), gColorChan[A]);
                    painter.DrawText(x + 2, y + (SET_ENABLED_A ? 20 : 11), processing.GetStringMeasure(measure, B, buffer, SIZE_BUFFER), gColorChan[B]);
                }
#undef SIZE_BUFFER
            }
        }
    }

    if(GetNameOpenedPage() == PageSB_Measures_Tune)
    {
        meas.DrawPageChoice();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawStringNavigation(void)
{
    if((SHOW_STRING_NAVIGATION || SHOW_STRING_NAVI_ALL) && (MENU_IS_SHOWN || (TypeOpenedItem()) != Item_Page))
    {
        char buffer[100];
        char *string = menu.StringNavigation(buffer);
        if(string)
        {
            int length = Font_GetLengthText(string);
            int height = 10;
            painter.DrawRectangle(grid.Left(), GRID_TOP, length + 2, height, gColorFill);
            painter.FillRegion(grid.Left() + 1, GRID_TOP + 1, length, height - 2, gColorBack);
            painter.DrawText(grid.Left() + 2, GRID_TOP + 1, string, gColorFill);
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

    float scale = (float)(LAST_POINT - FIRST_POINT) / grid.Width();

    int gridLeft = grid.Left();
    int gridRight = grid.Right();

    int x = gridLeft + (int)(shiftTPos * scale) - 3;
    if(IntInRange(x + 3, gridLeft, gridRight + 1))
    {
        painter.Draw2SymbolsC(x, GRID_TOP - 1, SYMBOL_TPOS_2, SYMBOL_TPOS_1, gColorBack, gColorFill);
    };

    // Рисуем tShift
    int shiftTShift = TPOS_IN_POINTS - TSHIFT_IN_POINTS;
    if(IntInRange(shiftTShift, FIRST_POINT, LAST_POINT))
    {
        int x = gridLeft + shiftTShift - FIRST_POINT - 3;
        painter.Draw2SymbolsC(x, GRID_TOP - 1, SYMBOL_TSHIFT_NORM_1, SYMBOL_TSHIFT_NORM_2, gColorBack, gColorFill);
    }
    else if(shiftTShift < FIRST_POINT)
    {
        painter.Draw2SymbolsC(gridLeft + 1, GRID_TOP, SYMBOL_TSHIFT_LEFT_1, SYMBOL_TSHIFT_LEFT_2, gColorBack, gColorFill);
        painter.DrawLine(gridLeft + 9, GRID_TOP + 1, gridLeft + 9, GRID_TOP + 7, gColorBack);
    }
    else if(shiftTShift > LAST_POINT)
    {
        painter.Draw2SymbolsC(gridRight - 8, GRID_TOP, SYMBOL_TSHIFT_RIGHT_1, SYMBOL_TSHIFT_RIGHT_2, gColorBack, gColorFill);
        painter.DrawLine(gridRight - 9, GRID_TOP + 1, gridRight - 9, GRID_TOP + 7, gColorBack);
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

    float scale = (float)grid.FullHeight() / (float)max;

    painter.SetColor(COLOR_WHITE);

    for(int i = 0; i < 281; i++)
    {
        painter.DrawVLine(grid.Left() + i, grid.FullBottom() - (int)(scale * gRandStat[i]), grid.FullBottom());
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// Написать предупреждения
static void DrawWarnings(void)
{
    int delta = 12;
    int y = grid.BottomMessages();
    for(int i = 0; i < 10; i++)
    {
        if(warnings[i] != 0)
        {
            DrawStringInRectangle(grid.Left(), y, warnings[i]);
            y -= delta;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawConsole(void)
{
    int count = 0;
    painter.SetFont(CONSOLE_SIZE_FONT == 5 ? TypeFont_5 : TypeFont_8);
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
        painter.FillRegion(grid.Left() + 1, GRID_TOP + 1 + count * (height + 1) + delta, width, height + 1, gColorBack);
        int y = GRID_TOP + 5 + count * (height + 1) - 4;
        if(Font_GetSize() == 5)
        {
            y -= 3;
        }
        painter.DrawText(grid.Left() + 2, y + dY + delta, strings[numString], gColorFill);
        count++;
    }

    painter.SetFont(TypeFont_8);
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
        strcat(buffer, trans.Voltage2String(trigLev, true, bufForVolt));
        int width = 96;

        // Рассчитаем координаты вывода строки уровня синхронизации.

        int x = 0;
        int y = 0;
        grid.CoordTrigLevel(&x, &y, width);

        painter.DrawRectangle(x, y, width, 10, gColorFill);
        painter.FillRegion(x + 1, y + 1, width - 2, 8, gColorBack);
        painter.DrawText(x + 2, y + 1, buffer, gColorFill);
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

    painter.DrawRectangle(grid.Left(), grid.FullBottom() - 10, 84, 10, gColorFill);
    painter.FillRegion(grid.Left() + 1, grid.FullBottom() - 9, 82, 8, gColorBack);
    painter.DrawText(grid.Left() + 2, grid.FullBottom() - 9, buffer, gColorFill);

    char message[SIZE] ={0};
    snprintf(message, SIZE, "%d", dS.NumElementsWithSameSettings());
    strcat(message, "/");
    char numAvail[10] ={0};
    snprintf(numAvail, SIZE, "%d", dS.NumberAvailableEntries());
    strcat(message, numAvail);
    painter.DrawText(grid.Left() + 50, grid.FullBottom() - 9, message);
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

    painter.SetColor(gColorFill);

    if(top == GRID_TOP)
    {
        painter.DrawHLine(top, 1, left - 2);
        painter.DrawHLine(top, right + 2, SCREEN_WIDTH - 2);

        if(!MenuIsMinimize() || !MENU_IS_SHOWN)
        {
            painter.DrawVLine(1, top + 2, bottom - 2);
            painter.DrawVLine(318, top + 2, bottom - 2);
        }
    }

    float deltaX = grid.DeltaX() * (float)width / width;
    float deltaY = grid.DeltaY() * (float)height / height;
    float stepX = deltaX / 5;
    float stepY = deltaY / 5;

    float centerX = (float)(left + width / 2);
    float centerY = (float)(top + height / 2);

    painter.SetColor(gColorGrid);
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
        float scale = (float)grid.MathHeight() / numParts;
        for(int i = 1; i < numParts; i++)
        {
            int y = grid.MathTop() + (int)(i * scale);
            painter.DrawHLine(y, grid.Left(), grid.Left() + 256, gColorGrid);
            if(!MenuIsMinimize())
            {
                painter.SetColor(gColorFill);
                painter.DrawText(3, y - 4, strs[i]);
            }
        }
        if(!MenuIsMinimize())
        {
            painter.SetColor(gColorFill);
            painter.DrawText(5, grid.MathTop() + 1, "дБ");
        }
    }
    else if(SCALE_FFT_LIN)
    {
        static const char * const strs[] ={"1.0", "0.8", "0.6", "0.4", "0.2"};
        float scale = (float)grid.MathHeight() / 5;
        for(int i = 1; i < 5; i++)
        {
            int y = grid.MathTop() + (int)(i * scale);
            painter.DrawHLine(y, grid.Left(), grid.Left() + 256, gColorGrid);
            if(!MenuIsMinimize())
            {
                painter.DrawText(5, y - 4, strs[i], gColorFill);
            }
        }
    }
    painter.DrawVLine(grid.Left() + 256, grid.MathTop(), grid.MathBottom(), gColorFill);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DRAW_SPECTRUM(const uint8 *dataIn, int numPoints, Channel ch)
{
    if(!SET_ENABLED(ch))
    {
        return;
    }
    
#define SIZE_BUFFER (1024 * 8)
    
    if(numPoints > SIZE_BUFFER)
    {
        numPoints = SIZE_BUFFER;
    }

    float dataR[SIZE_BUFFER];
    float spectrum[SIZE_BUFFER];

    float freq0 = 0.0f;
    float freq1 = 0.0f;
    float density0 = 0.0f;
    float density1 = 0.0f;
    int y0 = 0;
    int y1 = 0;
    int s = 2;

    uint8 *data = (uint8 *)malloc(numPoints);

    RAM_MemCpy16((void *)dataIn, data, numPoints);

    math.PointsRelToVoltage(data, numPoints, RANGE_DS(ch), RSHIFT_DS(ch), dataR);
    math.CalculateFFT(dataR, numPoints, spectrum, &freq0, &density0, &freq1, &density1, &y0, &y1);
    DrawSpectrumChannel(spectrum, gColorChan[ch]);
    if(!MENU_IS_SHOWN || MenuIsMinimize())
    {
        Color color = gColorFill;
        WriteParametersFFT(ch, freq0, density0, freq1, density1);
        painter.DrawRectangle(POS_MATH_CUR_0 + grid.Left() - s, y0 - s, s * 2, s * 2, color);
        painter.DrawRectangle(POS_MATH_CUR_1 + grid.Left() - s, y1 - s, s * 2, s * 2);

        painter.DrawVLine(grid.Left() + POS_MATH_CUR_0, grid.MathBottom(), y0 + s);
        painter.DrawVLine(grid.Left() + POS_MATH_CUR_1, grid.MathBottom(), y1 + s);
    }

    free(data);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawVerticalCursor(int x, int yTearing)
{
    x += grid.Left();
    if(yTearing == -1)
    {
        painter.DrawDashedVLine(x, GRID_TOP + 2, grid.ChannelBottom() - 1, 1, 1, 0);
    }
    else
    {
        painter.DrawDashedVLine(x, GRID_TOP + 2, yTearing - 2, 1, 1, 0);
        painter.DrawDashedVLine(x, yTearing + 2, grid.ChannelBottom() - 1, 1, 1, 0);
    }
    painter.DrawRectangle(x - 1, GRID_TOP - 1, 2, 2);
    painter.DrawRectangle(x - 1, grid.ChannelBottom() - 1, 2, 2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawHorizontalCursor(int y, int xTearing)
{
    y += GRID_TOP;
    if(xTearing == -1)
    {
        painter.DrawDashedHLine(y, grid.Left() + 2, grid.Right() - 1, 1, 1, 0);
    }
    else
    {
        painter.DrawDashedHLine(y, grid.Left() + 2, xTearing - 2, 1, 1, 0);
        painter.DrawDashedHLine(y, xTearing + 2, grid.Right() - 1, 1, 1, 0);
    }
    painter.DrawRectangle(grid.Left() - 1, y - 1, 2, 2);
    painter.DrawRectangle(grid.Right() - 1, y - 1, 2, 2);
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
        painter.DrawVLine(x, 1, GRID_TOP - 2, gColorFill);
        x += 3;
        Channel source = CURS_SOURCE;
        Color colorText = gColorChan[source];
        if (CURsU_ENABLED)
        {
            painter.DrawText(x, y1, "1:", colorText);
            painter.DrawText(x, y2, "2:");
            x += 7;
            painter.DrawText(x, y1, sCursors_GetCursVoltage(source, 0, buffer));
            painter.DrawText(x, y2, sCursors_GetCursVoltage(source, 1, buffer));
            x = startX + 49;
            float pos0 = math.VoltageCursor(sCursors_GetCursPosU(source, 0), SET_RANGE(source), SET_RSHIFT(source));
            float pos1 = math.VoltageCursor(sCursors_GetCursPosU(source, 1), SET_RANGE(source), SET_RSHIFT(source));
            float delta = fabsf(pos1 - pos0);
            if(SET_DIVIDER_10(source))
            {
                delta *= 10;
            }
            painter.DrawText(x, y1, ":dU=");
            painter.DrawText(x + 17, y1, trans.Voltage2String(delta, false, buffer));
            painter.DrawText(x, y2, ":");
            painter.DrawText(x + 10, y2, sCursors_GetCursorPercentsU(source, buffer));
        }

        x = startX + 101;
        painter.DrawVLine(x, 1, GRID_TOP - 2, gColorFill);
        x += 3;
        if(CURsT_ENABLED)
        {
            painter.SetColor(colorText);
            painter.DrawText(x, y1, "1:");
            painter.DrawText(x, y2, "2:");
            x += 7;
            painter.DrawText(x, y1, sCursors_GetCursorTime(source, 0, buffer));
            painter.DrawText(x, y2, sCursors_GetCursorTime(source, 1, buffer));
            x = startX + 153;

            /// \todo Дичь.

            /*
            float p0 = 0.0f;
            memcpy(&p0, &CURsT_POS(source, 0), sizeof(float));
            float p1 = 0.0f;
            memcpy(&p1, &CURsT_POS(source, 1), sizeof(float));
            */

            float pos0 = math.TimeCursor(CURsT_POS(source, 0), SET_TBASE);
            float pos1 = math.TimeCursor(CURsT_POS(source, 1), SET_TBASE);
            float delta = fabsf(pos1 - pos0);
            painter.DrawText(x, y1, ":dT=");
            char buffer[20];
            painter.DrawText(x + 17, y1, trans.Time2String(delta, false, buffer));
            painter.DrawText(x, y2, ":");
            painter.DrawText(x + 8, y2, sCursors_GetCursorPercentsT(source, buffer));

            if(CURSORS_SHOW_FREQ)
            {
                int width = 65;
                int x = grid.Right() - width;
                painter.DrawRectangle(x, GRID_TOP, width, 12, gColorFill);
                painter.FillRegion(x + 1, GRID_TOP + 1, width - 2, 10, gColorBack);
                painter.DrawText(x + 1, GRID_TOP + 2, "1/dT=", colorText);
                char buffer[20];
                painter.DrawText(x + 25, GRID_TOP + 2, trans.Freq2String(1.0f / delta, false, buffer));
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
        painter.DrawVLine(x, 1, GRID_TOP - 2, gColorFill);

        x += 2;

        if(trigEnable)
        {
            painter.FillRegion(x, 1 + y, GRID_TOP - 3, GRID_TOP - 7, gColorFill);
            painter.DrawText(x + 3, 3 + y, DICT(DTrig), gColorBack);
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
        painter.DrawVLine(x, 1, GRID_TOP - 2, gColorFill);
        x += 2;
        painter.DrawText(LANG_RU ? x : x + 3, -1, DICT(DMode));
        painter.DrawStringInCenterRect(x + 1, 9, 25, 8, strings[MODE_WORK][LANG]);
    }
    else
    {
        x -= 9;
    }

    if(!MODE_WORK_RAM)
    {
        x += 27;
        painter.DrawVLine(x, 1, GRID_TOP - 2, gColorFill);

        x += 2;
        y = 1;
        if(FPGA_IN_STATE_WORK)       // Рабочий режим
        {
            painter.Draw4SymbolsInRect(x, 1, SYMBOL_PLAY);
        }
        else if(FPGA_IN_STATE_STOP)  // Режим остановки
        {
            painter.FillRegion(x + 3, y + 3, 10, 10);
        }
        else if(FPGA_IN_STATE_WAIT)  // Режим ожидания сигнала
        {
            int w = 4;
            int h = 14;
            int delta = 4;
            x = x + 2;
            painter.FillRegion(x, y + 1, w, h);
            painter.FillRegion(x + w + delta, y + 1, w, h);
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
        painter.FillRegion(x, y, widthField, heightField, color);
    }
    const int SIZE = 100;
    char buffer[SIZE];
    snprintf(buffer, SIZE, "%s\xa5%s\xa5%s", (ch == A) ? DICT(D1ch) : DICT(D2ch), couple[SET_COUPLE(ch)], sChannel_Range2String(range, divider));
    painter.DrawText(x + 1, y, buffer, colorDraw);
    char bufferTemp[SIZE];
    snprintf(bufferTemp, SIZE, "\xa5%s", sChannel_RShift2String((int16)SET_RSHIFT(ch), range, divider, buffer));
    painter.DrawText(x + 46, y, bufferTemp);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteStringAndNumber(const char *text, int16 x, int16 y, int number)
{
    const int SIZE = 100;
    char buffer[SIZE];
    painter.DrawText(x, y, text, gColorFill);
    if(number == 0)
    {
        snprintf(buffer, SIZE, "-");
    }
    else
    {
        snprintf(buffer, SIZE, "%d", number);
    }
    painter.DrawTextRelativelyRight(x + 41, y, buffer);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawTime(int x, int y)
{
    int dField = 10;
    int dSeparator = 2;

    PackedTime time = RTC_GetPackedTime();

    char buffer[20];

    painter.SetColor(gColorFill);

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
            painter.DrawText(x, y, trans.Int2String(time.day, false, 2, buffer));
            painter.DrawText(x + dField, y, ":");
            painter.DrawText(x + dField + dSeparator, y, trans.Int2String(time.month, false, 2, buffer));
            painter.DrawText(x + 2 * dField + dSeparator, y, ":");
            painter.DrawText(x + 2 * dField + 2 * dSeparator, y, trans.Int2String(time.year + 2000, false, 4, buffer));
            y += 9;
        }
        else
        {
            return;
        }
    }

    painter.DrawText(x, y, trans.Int2String(time.hours, false, 2, buffer));
    painter.DrawText(x + dField, y, ":");
    painter.DrawText(x + dField + dSeparator, y, trans.Int2String(time.minutes, false, 2, buffer));
    painter.DrawText(x + 2 * dField + dSeparator, y, ":");
    painter.DrawText(x + 2 * dField + 2 * dSeparator, y, trans.Int2String(time.seconds, false, 2, buffer));
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
    int bottomY = grid.ChannelBottom() - DELTA;
    int centerY = (grid.ChannelBottom() + GRID_TOP) / 2;
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
        painter.DrawLine(x + 1, levels[i], x2 - 1, levels[i], gColorFill);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursorsRShift(void)
{
    if(FUNC_ENABLED)
    {
        DrawCursorRShift(MathCh);
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
    painter.DrawRectangle(x, y, width + 4, height + 4, gColorFill);
    painter.DrawRectangle(x + 1, y + 1, width + 2, height + 2, gColorBack);
    painter.FillRegion(x + 2, y + 2, width, height, COLOR_FLASH_10);
    painter.DrawText(x + 3, y + 2, text, COLOR_FLASH_01);
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

    painter.DrawMultiVPointLine(17, top + (int)stepY, masX, (int)stepY, CalculateCountV(), gColorGrid);

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

    painter.DrawMultiHPointLine(13, left + (int)stepX, mas, (int)stepX, CalculateCountH(), gColorGrid);
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
    painter.DrawMultiVPointLine(15, top + stepY, masX, stepY, CalculateCountV(), gColorGrid);

    uint8 mas[11];
    mas[0] = (uint8)(top + 1);
    for(int i = 1; i < 10; i++)
    {
        mas[i] = (uint8)(top + (int)(deltaY * i));
    }
    mas[10] = (uint8)(bottom - 1);
    painter.DrawMultiHPointLine(11, left + stepX, mas, stepX, CalculateCountH(), gColorGrid);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawGridType3(int left, int top, int right, int bottom, int centerX, int centerY, int deltaX, int deltaY, int stepX)
{
    painter.DrawHPointLine(centerY, left + stepX, right, (float)stepX);
    uint8 masY[6] ={(uint8)(top + 1), (uint8)(top + 2), (uint8)(centerY - 1), (uint8)(centerY + 1), (uint8)(bottom - 2), (uint8)(bottom - 1)};
    painter.DrawMultiHPointLine(6, left + deltaX, masY, deltaX, (right - top) / deltaX, gColorGrid);
    painter.SetColor(gColorGrid);
    painter.DrawVPointLine(centerX, top + stepX, bottom - stepX, (float)stepX);
    uint16 masX[6] ={(uint16)(left + 1), (uint16)(left + 2), (uint16)(centerX - 1), (uint16)(centerX + 1), (uint16)(right - 2), (uint16)(right - 1)};
    painter.DrawMultiVPointLine(6, top + deltaY, masX, deltaY, (bottom - top) / deltaY, gColorGrid);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSpectrumChannel(const float *spectrum, Color color)
{
    painter.SetColor(color);
    int gridLeft = grid.Left();
    int gridBottom = grid.MathBottom();
    int gridHeight = grid.MathHeight();
    for(int i = 0; i < 256; i++)
    {
        painter.DrawVLine(gridLeft + i, gridBottom, gridBottom - (int)(gridHeight * spectrum[i]));
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteParametersFFT(Channel ch, float freq0, float density0, float freq1, float density1)
{
    int x = grid.Left() + 259;
    int y = grid.ChannelBottom() + 5;
    int dY = 10;

    char buffer[20];
    painter.SetColor(gColorFill);
    painter.DrawText(x, y, trans.Freq2String(freq0, false, buffer));
    y += dY;
    painter.DrawText(x, y, trans.Freq2String(freq1, false, buffer));
    if(ch == A)
    {
        y += dY + 2;
    }
    else
    {
        y += dY * 3 + 4;
    }
    painter.SetColor(gColorChan[ch]);
    painter.DrawText(x, y, SCALE_FFT_LOG ? trans.Float2Db(density0, 4, buffer) : trans.Float2String(density0, false, 7, buffer));
    y += dY;
    painter.DrawText(x, y, SCALE_FFT_LOG ? trans.Float2Db(density1, 4, buffer) : trans.Float2String(density1, false, 7, buffer));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCursorRShift(Channel ch)
{
    int x = grid.Right() - grid.Width() - meas.GetDeltaGridLeft();

    if(ch == MathCh)
    {
        int yCenter = (grid.MathTop() + grid.MathBottom()) / 2;
        float scaleFull = (float)grid.MathHeight() / (RShiftMax - RShiftMin);
        int yFull = yCenter - (int)(scaleFull * (SET_RSHIFT_MATH - RShiftZero));
        painter.DrawChar(x - 9, yFull - 4, SYMBOL_RSHIFT_NORMAL, gColorFill);
        painter.DrawChar(x - 8, yFull - 5, 'm', gColorBack);
        return;
    }
    if(!SET_ENABLED(ch))
    {
        return;
    }

    int rShift = SET_RSHIFT(ch);

    int y = grid.ChannelCenterHeight() - math.RShift2Pixels((uint16)rShift, grid.ChannelHeight());

    float scaleFull = (float)grid.ChannelHeight() / (RShiftMax - RShiftMin) * (MATH_ENABLED ? 0.9f : 0.91f);
    int yFull = grid.ChannelCenterHeight() - (int)(scaleFull * (rShift - RShiftZero));

    if(y > grid.ChannelBottom())
    {
        painter.DrawChar(x - 7, grid.ChannelBottom() - 11, SYMBOL_RSHIFT_LOWER, gColorChan[ch]);
        painter.SetPoint(x - 5, grid.ChannelBottom() - 2);
        y = grid.ChannelBottom() - 7;
        x++;
    }
    else if(y < GRID_TOP)
    {
        painter.DrawChar(x - 7, GRID_TOP + 2, SYMBOL_RSHIFT_ABOVE, gColorChan[ch]);
        painter.SetPoint(x - 5, GRID_TOP + 2);
        y = GRID_TOP + 7;
        x++;
    }
    else
    {
        painter.DrawChar(x - 8, y - 4, SYMBOL_RSHIFT_NORMAL, gColorChan[ch]);
        if(((ch == A) ? showLevelRShiftA : showLevelRShiftB) && MODE_WORK_DIR)
        {
            painter.DrawDashedHLine(y, grid.Left(), grid.Right(), 7, 3, 0);
        }
    }

    painter.SetFont(TypeFont_5);
    int dY = 0;

    if((!MenuIsMinimize() || !MENU_IS_SHOWN) && drawRShiftMarkers)
    {
        painter.FillRegion(4, yFull - 3, 4, 6, gColorChan[ch]);
        painter.DrawChar(5, yFull - 9 + dY, ch == A ? '1' : '2', gColorBack);
    }
    painter.DrawChar(x - 7, y - 9 + dY, ch == A ? '1' : '2', gColorBack);
    painter.SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int CalculateCountV(void)
{
    if(SHOW_MEASURES && MEAS_COMPRESS_GRID)
    {
        if(NUM_MEASURES_1_5)
        {
            return SOURCE_MEASURE_A_B ? 55 : 59;
        }
        if(NUM_MEASURES_2_5)
        {
            return SOURCE_MEASURE_A_B ? 69 : 51;
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
        painter.BeginScene(gColorBack);
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

    painter.FillRegion(x, y, width, height, gColorBack);
    painter.DrawRectangle(x, y, width, height, gColorFill);
    painter.DrawStringInCenterRect(x, y, width, height - 20, textWait);
    char buffer[100];
    buffer[0] = 0;
    for (uint i = 0; i < time; i++)
    {
        strcat(buffer, ".");
    }
    painter.DrawStringInCenterRect(x, y + 20, width, height - 20, buffer);
    painter.EndScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::FuncOnWaitStart(const char *text, bool eraseBackground)
{
    timeStart = gTimeMS;
    textWait = text;
    clearBackground = eraseBackground;
    Display::SetDrawMode(DrawMode_Hand, FuncOnWait);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::FuncOnWaitStop(void)
{
    Display::SetDrawMode(DrawMode_Auto, 0);
}
