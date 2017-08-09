#include "Globals.h"
#include "PainterDataNew.h"
#include "Display/Grid.h"
#include "FPGA/Data.h"
#include "FPGA/DataBuffer.h"
#include "FPGA/DataStorage.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"


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
/// Нарисовать данные из outA или outB c выключенным пиковым детектором.
static void DrawData_Out_Normal(Channel ch, uint8 data[281], int left, int bottom, float scaleY);
/// Нарисовать данные из outA или outB с включённым пиковым детектором.
static void DrawData_Out_PeakDet(Channel ch, uint8 data[281 * 2], int left, int bottom, float scaleY);
/// \brief Используется в режиме пикового детектора. В in хранятся два значения, соответствующие максимальному и минимальному. 
/// Они перемещаются в out в возрастающем порядке out[0] = min, out[1] = max. Возвращает false, если точка не считана - хотя бы одно значение == 0.
static bool CalcMinMax(uint8 in[2], uint8 out[2]);


/// Признак того, что на основном экране нужно рисовать бегущий сигнал поточечного вывода
#define NEED_DRAW_DYNAMIC_P2P (IN_P2P_MODE && FPGA_IsRunning())

/// Условие того, что мы находимся в ждущем режиме поточечном и на основном экране должно быть статичное изображение
#define STAND_P2P (START_MODE_WAIT && IN_P2P_MODE && DS_NumElementsWithCurrentSettings() > 1)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PainterDataNew_DrawData(void)
{
    // Нормальный режим
    if (MODE_WORK_DIR)                              // Установленный режим - непосредственный
    {
        if (ALWAYS_SHOW_ROM_SIGNAL)                 // Если нужно показывать сигнал из ППЗУ
        {
            DrawData_ModeROM();
        }
        DrawData_ModeDir();                         // Рисуем данные нормального режима
    }
    // ПАМЯТЬ - ПОСЛЕДНИЕ
    else if (MODE_WORK_RAM)
    {
        DrawData_ModeRAM();                         
    }
    // ПАМЯТЬ - ВНУТР ЗУ
    else
    {
        if (SHOW_IN_INT_BOTH || SHOW_IN_INT_SAVED)
        {
            DrawData_ModeROM();
        }
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
    }
    
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawData_ModeDir(void)
{
    if (START_MODE_WAIT && IN_P2P_MODE && DS_NumElementsWithCurrentSettings() > 1)
    {
        Data_ReadDataRAM(1);
    }
    else
    {
        Data_ReadDataRAM(0);
    }
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
    if (DS)
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

    Painter_SetColor(gColorChan[ch]);

    int pointFirst = 0;
    int pointLast = 0;
    sDisplay_PointsOnDisplay(&pointFirst, &pointLast);

    int left = GridLeft();
    int bottom = GridChannelBottom();
    int top = GRID_TOP;

    float scaleY = (bottom - top) / (float)(MAX_VALUE - MIN_VALUE);

    /// \todo Переделать на массив функций.
    if(G_PEACKDET)
    {
        DrawData_Out_PeakDet(ch, data + pointFirst * 2, left, bottom, scaleY);  
    }
    else
    {
        uint8 *pData = data;
        int pointer = 0;

        if (!STAND_P2P)
        {
            if (NEED_DRAW_DYNAMIC_P2P)
            {
                uint8 d[281] = {0};

                for (int i = 0; i < G_BYTES_IN_CHANNEL; i++)
                {
                    if (data[i])
                    {
                        d[pointer] = data[i];
                        pointer = (pointer + 1) % 281;
                    }
                }

                pData = d;
                pointFirst = 0;
            }
        }

        DrawData_Out_Normal(ch, pData + pointFirst, left, bottom, scaleY);

        if (!STAND_P2P)
        {
            if (NEED_DRAW_DYNAMIC_P2P)
            {
                DataSettings *ds = 0;
                uint8 *dA = 0;
                uint8 *dB = 0;
                if (DS_GetLastFrameP2P_RAM(&ds, &dA, &dB) < SET_POINTS_IN_CHANNEL)
                {
                    Painter_DrawVLineC(left + pointer - 1, bottom, GRID_TOP, gColorGrid);
                }
            }
        }
    }

    Painter_RunDisplay();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawData_Out_Normal(Channel ch, uint8 data[281], int left, int bottom, float scaleY)
{
    float k = bottom + MIN_VALUE * scaleY;

    for(int i = 0; i < 280; ++i)                        /// \todo Последня точка не рисуется.
    {
        uint8 val = data[i];

        if(val == 0)
        {
            continue;                                   // Если это значение отсутствует - переходим к следующей точке
        }

        LIMITATION(val, val, MIN_VALUE, MAX_VALUE);

        int y = k - val * scaleY;                       //int y = bottom - (val - MIN_VALUE) * scaleY;

        int x = left + i;

        if(MODE_DRAW_SIGNAL_POINTS)
        {
            Painter_SetPoint(x, y);
        }
        else
        {
            int yNext = k - data[i + 1] * scaleY;       //int yNext = bottom - (data[i + 1] - MIN_VALUE) * scaleY;

            if(yNext < y)
            {
                Painter_DrawVLine(x, y, yNext + 1);
            }
            else if(yNext > y)
            {
                Painter_DrawVLine(x, y, yNext - 1);
            }
            else
            {
                Painter_SetPoint(x, y);
            }

        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawData_Out_PeakDet(Channel ch, uint8 data[281 * 2], int left, int bottom, float scaleY)
{
    float k = bottom + MIN_VALUE * scaleY;

    for(int i = 0; i < 280; i++)
    {

#define Y_MIN in[0]
#define Y_MAX in[1]
#define Y_MIN_NEXT inNext[0]
#define Y_MAX_NEXT inNext[1]

        uint8 in[2];            // Здесь будут храниться отсортированные по возрастанию значения
        if(!CalcMinMax(data + i * 2, in))
        {
            continue;
        }
        
        uint8 inNext[2];
        if(!CalcMinMax(data + (i + 1) * 2, inNext))
        {
            Y_MIN_NEXT = Y_MIN;
            Y_MAX_NEXT = Y_MAX;
        }

        int min = k - Y_MAX * scaleY;
        int max = k - Y_MIN * scaleY;
        int minNext = k - Y_MAX_NEXT * scaleY;
        int maxNext = k - Y_MIN_NEXT * scaleY;

        int x = left + i;

        if(maxNext < min)
        {
            min = maxNext + 1;
        }
        if(minNext > max)
        {
            max = minNext - 1;
        }

        if(MODE_DRAW_SIGNAL_POINTS)
        {
            Painter_SetPoint(x, min);
            Painter_SetPoint(x, max);
        }
        else
        {
            if(min == max)
            {
                Painter_SetPoint(x, min);
            }
            else
            {
                Painter_DrawVLine(x, min, max);
            }
        }
    }

#undef Y_MIN
#undef Y_MAX
#undef Y_MIN_NEXT
#undef Y_MAX_NEXT
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool CalcMinMax(uint8 in[2], uint8 out[2])
{
    uint8 val1 = in[0];
    uint8 val2 = in[1];
    if(val1 == 0 || val2 == 0)
    {
        return false;
    }

    LIMITATION(val1, val1, MIN_VALUE, MAX_VALUE);
    LIMITATION(val2, val2, MIN_VALUE, MAX_VALUE);

    if(val1 < val2)
    {
        out[0] = val1;
        out[1] = val2;
    }
    else
    {
        out[0] = val2;
        out[1] = val1;
    }

    return true;
}
