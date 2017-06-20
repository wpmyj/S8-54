#include "Globals.h"
#include "PainterDataNew.h"
#include "Display/Grid.h"
#include "FPGA/Data.h"
#include "FPGA/DataBuffer.h"
#include "Settings/Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Нарисовать данные, которые рисовались бы, если б был установлен режим ModeWork_Dir.
static void DrawData_ModeDir(void);
/// Нарисовать данные, которые рисовались бы, если б был установлен режим ModeWork_RAM.
static void DrawData_ModeRAM(void);
/// Нарисовать данные, которые рисовались бы, если б был установлен режим ModeWork_ROM.
static void DrawData_ModeROM(void);
/// Нарисовать данные из outA, outB.
static void DrawData_OutAB(void);
/// Нарисовать данные из outA или outB.
static void DrawData_Out(Channel ch, uint8 *data);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PainterDataNew_DrawData(void)
{
    if (MODE_WORK_DIR)                              // Установленный режим - непосредственный
    {
        if (ALWAYS_SHOW_ROM_SIGNAL)                 // Если нужно показывать сигнал из ППЗУ
        {
            DrawData_ModeROM();
        }
        DrawData_ModeDir();                         // Рисуем данные нормального режима
    }
    else if (MODE_WORK_RAM)                         // Установленный режим индикации - ПАМЯТЬ - ПОСЛЕДНИЕ
    {
        DrawData_ModeRAM();                         
    }
    else                                            // Установленный режим индикации - ПАМЯТЬ - ВНУТР ЗУ
    {
        if (SHOW_IN_INT_BOTH || SHOW_IN_INT_DIRECT) // Если нужно показывать не только сохранённый сигнал
        {
            if (EXIT_FROM_ROM_TO_RAM)               // и мы перешли на страницу "ПАМЯТЬ-ВНУТР ЗУ" со страницы "ПАМЯТЬ-ПОСЛЕДНИЕ"
            {
                DrawData_ModeRAM();
            }
            else                                    // А если перешли из нормального режим
            {
                DrawData_ModeDir();                 // То нарисуем сигнал нормального режима
            }
        }
        if (SHOW_IN_INT_BOTH || SHOW_IN_INT_SAVED)
        {
            DrawData_ModeROM();
        }
    }
    
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawData_ModeDir(void)
{
    Data_ReadDataRAM(0);
    DrawData_OutAB();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawData_ModeRAM(void)
{
    Data_ReadDataRAM(NUM_RAM_SIGNAL);
    DrawData_OutAB();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawData_ModeROM(void)
{
    Data_ReadDataROM();
    DrawData_OutAB();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawData_OutAB(void)
{
    if (LAST_AFFECTED_CH_IS_A)
    {
        DrawData_Out(B, outB);
        DrawData_Out(A, outA);
    }
    else
    {
        DrawData_Out(A, outA);
        DrawData_Out(B, outB);
    }

    Painter_DrawRectangleC(GridLeft(), GRID_TOP, GridWidth(), GridFullHeight(), gColorFill);                                                                                                                         
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawData_Out(Channel ch, uint8 *data)
{
    if (!G_ENABLED(ch))
    {
        return;
    }

    //Painter_EndScene();

    Painter_SetColor(gColorChan[ch]);

    int pointFirst = 0;
    int pointLast = 0;
    sDisplay_PointsOnDisplay(&pointFirst, &pointLast);

    int left = GridLeft();
    int bottom = GridChannelBottom();
    int top = GRID_TOP;

    float scaleY = (bottom - top) / (float)(MAX_VALUE - MIN_VALUE);

    uint8 *in = data + pointFirst;

    for (int i = 0; i < 280; ++i)
    {
        int x = left + i;

        uint8 val = in[i];

        if (val == 0) { continue; }                     // Если это значение отсутствует - переходим к следующей точке

        if (val < MIN_VALUE)  { val = MIN_VALUE; }      // Вписываем сигнал

        if (val > MAX_VALUE)  { val = MAX_VALUE; }      // в допустимый диапазон

        int y = bottom - (val - MIN_VALUE) * scaleY;    // Нашли положение нашей точки по оси Y

        if (MODE_DRAW_SIGNAL_POINTS)
        {
            Painter_SetPoint(x, y);
        }
        else
        {
            int yNext = bottom - (in[i + 1] - MIN_VALUE) * scaleY;

            Painter_DrawLine(x, y, x, yNext);
        }
    }

    RunDisplay();
}
