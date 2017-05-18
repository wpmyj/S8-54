// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "PainterData.h"
#include "DisplayTypes.h"
#include "Globals.h"
#include "Settings/SettingsDisplay.h"
#include "FPGA/Data.h"
#include "FPGA/DataStorage.h"
#include "Settings/Settings.h"
#include "Grid.h"
#include "Utils/ProcessingSignal.h"
#include "Utils/Math.h"
#include "Utils/Debug.h"
#include "Hardware/RAM.h"
#include "Hardware/FSMC.h"
#include "Hardware/Timer.h"
#include "Symbols.h"
#include "Log.h"
#include "Settings/SettingsDisplay.h"
#include "Utils/GlobalFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CONVERT_DATA_TO_DISPLAY(out, inVal)                     \
    int in = inVal;                                             \
    if(in < MIN_VALUE) { in = MIN_VALUE; }                      \
    if(in > MAX_VALUE) { in = MAX_VALUE; }                      \
    out = (uint8)(maxY - (int)(((in) - MIN_VALUE) * scaleY));   \
    if(out < (uint8)minY)   { out = (uint8)minY; }              \
    if(out > (uint8)maxY)   { out = (uint8)maxY; };


// Размещает два значения по возрастанию : val1 - меньшее, val2 - большее
#define PLACE_2_ASCENDING(v1, v2) if((v1) > (v2)) { int qwerty = v1; v1 = v2; v2 = qwerty; }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int xP2P = 0;                // Здесь хранится значение для отрисовки вертикальной линии
static Channel curCh = A;           // Текущий ресуемый сигнал


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void  DrawDataInModeEEPROM(void);
static void  DrawDataInModeDirect(void);
static void  DrawDataInModeLatest(void);
static void  DrawDataMinMax(void);
static void  DrawDataChannel(uint8 *dataIn, int minY, int maxY);
static void  DrawDataInRect(int x, uint width, const uint8 *data, int numElems, bool peackDet);
static void  DrawTPos(int leftX, int rightX);
static void  DrawTShift(int leftX, int rightX, int numPoints);
static void  DrawBothChannels(uint8 *dataA, uint8 *dataB);                              // Нарисовать оба канала. Если data == 0, то данные берутся из Processing_GetData
static int   FillDataP2P(uint8 *data, DataSettings **ds);
static void  DrawMarkersForMeasure(float scale);
static bool  DataBeyondTheBorders(const uint8 *data, int firstPoint, int lastPoint);    // Возвращает true, если изогражение сигнала выходит за пределы экрана
static void  DrawSignalLined(const uint8 *data, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX, bool calculateFiltr);
static void  DrawSignalPointed(const uint8 *data, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX);
static int   Ordinate(uint8 x, float scale);                                            // Возвращает точку в экранной координате. Если точка не считана (NONE_VALUE), возвращает -1
static int   FillDataP2PforRecorder(int numPoints, int numPointsDS, int pointsInScreen, uint8 *src, uint8 *dest);
static int   FillDataP2PforNormal(int numPoints, int numPointsDS, int pointsInScreen, uint8 *src, uint8 *dest);
static void  DrawLimitLabel(int delta);     // Выоводит сообщение на экране о выходе сигнала за границы экрана. 
                                            // delta - расстояние от края сетки, на котором находится сообщение. Если delta < 0 - выводится внизу сетки
static void  SendToDisplayDataInRect(int x, const int *min, const int *max, uint width);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PainterData_DrawData(void)
{
    xP2P = 0;

	if (!IN_P2P_MODE && (DS_NumElementsInStorage() < 2))    // WARN Это сделано для того, чтобы не было артефактов при включении
                                                            // Но не факт, что причина в этом. И в поточечном режиме однозначно долго ждать
	{
        Painter_DrawRectangleC(GridLeft(), GRID_TOP, GridWidth(), GridFullHeight(), gColorFill);
		return;
	}

	// Режим просмотра сигналов, записанных в ППЗУ
	if (WORK_EEPROM)
	{
        if (SHOW_IN_INT_DIRECT || SHOW_IN_INT_BOTH)
        {
            //Data_PreparePointersToUse(ModeWork_Direct);
            DrawDataInModeDirect();
        }
        if (SHOW_IN_INT_SAVED || SHOW_IN_INT_BOTH)
        {
            //Data_PreparePointersToUse(ModeWork_EEPROM);
            DrawDataInModeEEPROM();
        }
	}
	// Режим просмотра сигналов ОЗУ
	else if (WORK_LAST)
	{
		DrawDataInModeLatest();
	}
	// Нормальный режим
	else
	{
		if (ALWAYS_SHOW_MEM_INT_SIGNAL)                 // Если нужно показывать сигннал из ППЗУ
		{
            Data_PreparePointersToUse(ModeWork_EEPROM); // то показываем
			DrawDataInModeEEPROM();
		}

        Data_PreparePointersToUse(ModeWork_Direct);     // И рисуем последний сигнал
		DrawDataInModeDirect();    
	}

	if (DISPLAY_NUM_MIN_MAX != 1)
	{
		DrawDataMinMax();
	}

    if (xP2P)
    {
        Painter_DrawVLineC(xP2P, GRID_TOP, GridFullBottom(), gColorGrid);
    }

    Painter_DrawRectangleC(GridLeft(), GRID_TOP, GridWidth(), GridFullHeight(), gColorFill);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
void PainterData_DrawMath(void)
{
    if (!FUNC_ENABLED || DS_GetData_RAM(A, 0) == 0 || DS_GetData_RAM(B, 0) == 0)
    {
        return;
    }

    uint8 *dataRel0 = 0;
    uint8 *dataRel1 = 0;
    Processing_GetData(&dataRel0, &dataRel1, &DS);

    float *dataAbsA = (float*)RAM(DRAW_MATH_DATA_REL_A);
    float *dataAbsB = (float*)RAM(DRAW_MATH_DATA_REL_B);

    int numPoints = NumBytesInChannel(DS);

    Math_PointsRelToVoltage(dataRel0, numPoints, G_RANGE_A, G_RSHIFT_A, dataAbsA);
    Math_PointsRelToVoltage(dataRel1, numPoints, G_RANGE_B, G_RSHIFT_B, dataAbsB);

    Math_CalculateMathFunction(dataAbsA, dataAbsB, numPoints);

    uint8 points[FPGA_MAX_POINTS];

    Math_PointsVoltageToRel(dataAbsA, numPoints, SET_RANGE_MATH, SET_RSHIFT_MATH, points);

    curCh = Math;
    DrawDataChannel(points, GridMathTop(), GridMathBottom());

    static const int WIDTH = 71;
    static const int HEIGHT = 10;
    int delta = (SHOW_STRING_NAVI_ALL && FUNC_MODE_DRAW_TOGETHER) ? 10 : 0;
    Painter_DrawRectangleC(GridLeft(), GridMathTop() + delta, WIDTH, HEIGHT, gColorFill);
    Painter_FillRegionC(GridLeft() + 1, GridMathTop() + 1 + delta, WIDTH - 2, HEIGHT - 2, gColorBack);
    Divider divider = set.math.divider;
    Painter_DrawTextC(GridLeft() + 2, GridMathTop() + 1 + delta, sChannel_Range2String(SET_RANGE_MATH, divider), gColorFill);
    Painter_DrawText(GridLeft() + 25, GridMathTop() + 1 + delta, ":");
    char buffer[20];
    Painter_DrawText(GridLeft() + 27, GridMathTop() + 1 + delta, sChannel_RShift2String(SET_RSHIFT_MATH, SET_RANGE_MATH, divider, buffer));
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
// Нарисовать окно памяти
void PainterData_DrawMemoryWindow(void)
{
    uint8 *datA = DATA_INT(A);
    uint8 *datB = DATA_INT(B);

    if (IN_P2P_MODE && !DS_GetLastFrameP2P_RAM(&DS, &datA, &datB))      // Страхуемся от глюков
    {
        return;
    }

    uint8 *dA = 0;  // Сюда считаем данные каналов из RAM
    uint8 *dB = 0;

    if (WORK_DIRECT || WORK_LAST)
    {
        datA = DATA(A);
        datB = DATA(B);

        if (TBASE(DS_DataSettingsFromEnd(0)) >= MIN_TBASE_P2P)          // Если находимся в режиме поточечного вывода
        {
            DS_GetLastFrameP2P_RAM(&DS, &datA, &datB);
        }

        // Нужно переписать из внешнего ОЗУ в стек, потому чт
        dA = AllocMemForChannelFromHeap(A, DS);
        dB = AllocMemForChannelFromHeap(B, DS);

        int numBytes = NumBytesInChannel(DS);

        RAM_MemCpy16(datA, dA, numBytes);
        RAM_MemCpy16(datB, dB, numBytes);

        datA = dA;
        datB = dB;
    }

    int leftX = 3;
    int top = 0;
    int height = GRID_TOP - 3;
    int bottom = top + height; //-V2007

    static const int rightXses[3] = {276, 285, 247};
    int rightX = rightXses[MODE_WORK];
    if (sCursors_NecessaryDrawCursors())
    {
        rightX = 68;
    }

    float scaleX = (float)(rightX - leftX + 1) / sMemory_NumPointsInChannel();
    int width = (int)((rightX - leftX) * (282.0f / sMemory_NumPointsInChannel()));

    int16 shiftInMemory = (int16)sDisplay_ShiftInMemoryInPoints();

    const int xVert0 = leftX + (int)(shiftInMemory * scaleX);

    Channel lastAffectedChannel = LAST_AFFECTED_CH;
    if (((uint)NumPoints_2_FPGA_NUM_POINTS(sMemory_NumBytesInChannel(false)) == G_INDEXLENGHT) && (DATA(A) || DATA(B)))
    {
        Channel chanFirst = lastAffectedChannel == A ? B : A;
        Channel chanSecond = lastAffectedChannel == A ? A : B;
        const uint8 *dataFirst = lastAffectedChannel == A ? datB : datA;
        const uint8 *dataSecond = lastAffectedChannel == A ? datA : datB;

        bool peackDet = G_PEACKDET != PeackDet_Disable;

        if (sChannel_NeedForDraw(dataFirst, chanFirst, DS))
        {
            curCh = chanFirst;
            DrawDataInRect(1, rightX + 3, dataFirst, sMemory_NumBytesInChannel(false), peackDet);
        }
        if (sChannel_NeedForDraw(dataSecond, chanSecond, DS))
        {
            curCh = chanSecond;
            DrawDataInRect(1, rightX + 3, dataSecond, sMemory_NumBytesInChannel(false), peackDet);
        }
    }

    Painter_DrawRectangleC(xVert0, top, width - (FPGA_NUM_POINTS_8k ? 1 : 0), bottom - top + 1, gColorFill); //-V2007

    DrawTPos(leftX, rightX);

    DrawTShift(leftX, rightX, sMemory_NumBytesInChannel(false));

    free(dA);
    free(dB);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawDataInModeEEPROM(void)
{
    if (DS)
    {
        curCh = A;
        DrawDataChannel(DATA_INT(A), GRID_TOP, GridChannelBottom());
        curCh = B;
        DrawDataChannel(DATA_INT(B), GRID_TOP, GridChannelBottom());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawDataInModeDirect(void)
{
    uint index = G_INDEXLENGHT;
    uint numBytesInChannel = sMemory_NumBytesInChannel(false);
    uint numPoints_2_FPGA_NUM_POINTS = (uint)NumPoints_2_FPGA_NUM_POINTS(numBytesInChannel);

    if(numPoints_2_FPGA_NUM_POINTS != index)    // Если количество точек в данных не соответствует установленному в настройках - просто выходим
    {
        //return;                     // WARN Это временно. По хорошему нужно преобразовывать так же, как мы преобразуем tShift, rShift, Range, TBase
    }

    int16 numSignals = (int16)DS_NumElementsWithSameSettings();
    LIMITATION(numSignals, numSignals, 1, DISPLAY_NUM_ACCUM);
    if (numSignals == 1 ||      // В хранилище только один сигнал с текущими настройками
        NUM_ACCUM_INF ||        // или бесконечное накопление
        MODE_ACCUM_RESET ||     // или автоматическая очистка экрана для накопления
        IN_RANDOM_MODE)         // или в режиме рандомизатора
    {
        DrawBothChannels(0, 0);                 // когда 0, просто рисуем последний сигнал
    }
    else
    {
        for (int i = 0; i < numSignals; i++)    // Иначе рисуем необходимое число последних сигналов
        {
            DrawBothChannels(DS_GetData_RAM(A, i), DS_GetData_RAM(B, i));
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawDataInModeLatest(void)
{
    if (DS)
    {
        curCh = A;
        DrawDataChannel(DATA_LAST(A), GRID_TOP, GridChannelBottom());
        curCh = B;
        DrawDataChannel(DATA_LAST(B), GRID_TOP, GridChannelBottom());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawDataMinMax(void)
{
    ModeDrawSignal modeDrawSignalOld = MODE_DRAW_SIGNAL;
    MODE_DRAW_SIGNAL = ModeDrawSignal_Lines;
    if (LAST_AFFECTED_CH == B)
    {
        curCh = A;
        DrawDataChannel(DS_GetLimitation(A, 0), GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(A, 1), GRID_TOP, GridChannelBottom());
        curCh = B;
        DrawDataChannel(DS_GetLimitation(B, 0), GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(B, 1), GRID_TOP, GridChannelBottom());
    }
    else
    {
        curCh = B;
        DrawDataChannel(DS_GetLimitation(B, 0), GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(B, 1), GRID_TOP, GridChannelBottom());
        curCh = A;
        DrawDataChannel(DS_GetLimitation(A, 0), GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(A, 1), GRID_TOP, GridChannelBottom());
    }
    MODE_DRAW_SIGNAL = modeDrawSignalOld;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawDataChannel(uint8 *dataIn, int minY, int maxY)
{
    bool calculateFiltr = true;
    int sizeBuffer = NumBytesInChannel(DS);
    uint8 data[sizeBuffer];                                 // пїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ

    int firstPoint = 0;
    int lastPoint = 280;

    if (!IN_P2P_MODE ||                                     // Если не находимся в режиме медленных поточечных развёрток
        (IN_P2P_MODE && TIME_MS(DS)))                 // Или в поточечном, но данные уже считаны
    {
        sDisplay_PointsOnDisplay(&firstPoint, &lastPoint);  // то находим первую и последнюю точки, выводимые на экран
    }

    if (IN_P2P_MODE &&                                      // Если находимся в режиме медленных поточечных развёрток
        TIME_MS(DS) == 0)                               // и считывание полного набора данных ещё не произошло
    {
        lastPoint = FillDataP2P(data, &DS);
        if (lastPoint < 2)                                  // Если готово меньше двух точек - выход
        {
            return;
        }
        dataIn = data;
    }
    else if (dataIn == 0)                                   // Значит, для вывода используем последние считаныые данные из Processing_GetData()
    {
        calculateFiltr = false;
        if (curCh == A)
        {
            Processing_GetData(&dataIn, 0, &DS);
        }
        else
        {
            Processing_GetData(0, &dataIn, &DS);
        }
        RAM_MemCpy16(dataIn, data, sizeBuffer);
        dataIn = data;
    }

    if (!sChannel_NeedForDraw(dataIn, curCh, DS))
    {
        return;
    }

    float scaleY = (float)(maxY - minY) / (MAX_VALUE - MIN_VALUE);
    float scaleX = (float)GridWidth() / 280.0f;

    if (SHOW_MEASURES)
    {
        DrawMarkersForMeasure(scaleY);
    }

    Painter_SetColor(gColorChan[curCh]);

    if (!DataBeyondTheBorders(dataIn, firstPoint, lastPoint))   // Если сигнал не выходит за пределы экрана
    {
        if (MODE_DRAW_SIGNAL_LINES)
        {
            DrawSignalLined(dataIn, firstPoint, lastPoint, minY, maxY, scaleY, scaleX, calculateFiltr);
        }
        else
        {
            DrawSignalPointed(dataIn, firstPoint, lastPoint, minY, maxY, scaleY, scaleX);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawDataInRect(int x, uint width, const uint8 *data, int numBytes, bool peackDet)
{
    if (numBytes == 0)
    {
        return;
    }

    width--;
    float elemsInColumn = (float)numBytes / (float)width;
    uint8 min[width + 1];
    uint8 max[width + 1];

    if (SET_PEACKDET_EN)                                                    // Если пик. дет. выключен
    {
        uint8 *iMin = &min[0];
        uint8 *iMax = &max[0];

        for (uint col = 0; col < width; col++, iMin++, iMax++)
        {
            uint firstElem = (uint)(col * elemsInColumn);
            uint lastElem = (uint)firstElem + elemsInColumn - 1;
            *iMin = data[firstElem];
            *iMax = data[firstElem];
            for (uint elem = firstElem + 1; elem <= lastElem; elem++)
            {
                SET_MIN_IF_LESS(data[elem], *iMin);
                SET_MAX_IF_LARGER(data[elem], *iMax);
            }
        }
    }
    else                                                                // Если пик. дет. включён
    {
        for (int col = 0; col < width; col++)
        {
            uint firstElem = (uint)col * elemsInColumn;
            uint lastElem = (uint)firstElem + elemsInColumn - 1;
            min[col] = data[firstElem];
            max[col] = data[firstElem];
            for (uint elem = firstElem + 1; elem <= lastElem; elem++)
            {
                SET_MIN_IF_LESS(data[elem], min[col]);
                SET_MAX_IF_LARGER(data[elem], max[col]);
            }
        }
    }

    int height = 17;
    float scale = (float)height / (float)(MAX_VALUE - MIN_VALUE);

    const int SIZE_BUFFER = width + 1;

    int mines[SIZE_BUFFER];     // Массив для максимальных значений в каждом столбике
    int maxes[SIZE_BUFFER];     // Массив для минимальных значений в каждом столбике

    mines[0] = Ordinate(max[0], scale);
    maxes[0] = Ordinate(min[0], scale);

    for (int i = 1; i < width; i++)
    {
        maxes[i] = Ordinate((uint8)(max[i] < min[i - 1] ? min[i - 1] : max[i]), scale);
        mines[i] = Ordinate((uint8)(min[i] > max[i - 1] ? max[i - 1] : min[i]), scale);
    }

    // Теперь уточним количество точек, которые нужно нарисовать (исходим из того, что в реальном режиме и рандомизаторе рисуем все точки,
    // а в поточечном только начальные до определённой позиции

    uint numPoints = 0;
    for (int i = 0; i < width; i++)
    {
        if (maxes[i] == -1 && mines[i] == -1)   { break; }          // Если обе точки не были считаны, то выходим
        numPoints++;
    }

    if (numPoints != width)                     // Если нужно выводить не все точки,
    {
        numPoints--;                            // то выводим на одну меньше - во избежание артефакта в конце вывода
    }

    if (numPoints > 1)
    {
        if (numPoints < 256)
        {
            SendToDisplayDataInRect(x, mines, maxes, numPoints);
        }
        else
        {
            SendToDisplayDataInRect(x, mines, maxes, 255);
            numPoints -= 255;
            SendToDisplayDataInRect(x + 255, mines + 255, maxes + 255, numPoints);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// Возвращает (-1), если точка не считана (NONE_VALUE)
//----------------------------------------------------------------------------------------------------------------------------------------------------
static int Ordinate(uint8 x, float scale)
{
    const float bottom = 17.0;

    if (x == NONE_VALUE)
    {
        return -1;
    }

    return (bottom - scale * LimitationInt(x - MIN_VALUE, 0, (MAX_VALUE - MIN_VALUE))) + 0.5f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// Процедура ограничивает width числом 255
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SendToDisplayDataInRect(int x, const int *min, const int *max, uint width)
{
    LIMIT_ABOVE(width, 255);

    uint8 points[width * 2];

    for (uint i = 0; i < width; i++)
    {
        points[i * 2] = max[i];
        points[i * 2 + 1] = min[i];
    }

    Painter_DrawVLineArray(x, (int)width, points, gColorChan[curCh]); //-V202
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawTPos(int leftX, int rightX)
{
    int x[] = {leftX, (rightX - leftX) / 2 + leftX, rightX};
    int x0 = x[TPOS];
    Painter_FillRegionC(x0 - 3, 10, 6, 6, gColorBack);
    Painter_DrawCharC(x0 - 3, 10, SYMBOL_TPOS_1, gColorFill);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawTShift(int leftX, int rightX, int numBytes)
{
    float scale = (float)(rightX - leftX + 1) / ((float)numBytes - (numBytes == 281 ? 1 : 0));
    int xShift = (int)(1.5f + (sTime_TPosInBytes() - sTime_TShiftInPoints()) * scale) - 1;
    if (SET_PEACKDET_EN)
    {
        if (TPOS_RIGHT)
        {
            --xShift;
        }
    }
    if (FPGA_NUM_POINTS_512)
    {
        ++xShift;                           // WARN Костыль
    }
    LIMIT_ABOVE(xShift, rightX - 2);

    int dX01 = 1, dX02 = 2, dX11 = 3, dY11 = 7, dY12 = 6;

    if (xShift < leftX - 2)
    {
        xShift = leftX - 2;
        dX01 = 3; dX02 = 1; dY12 = 6;
    }
    else if (xShift > rightX - 1)
    {
        xShift = rightX - 2;
        dX11 = 1;
    }
    else
    {
        dY11 = 5; dY12 = 7;
    }

    Painter_FillRegionC((int)xShift - 1, 1, 6, 6, gColorBack);
    Painter_FillRegionC((int)xShift, 2, 4, 4, gColorFill); //-V112
    Painter_DrawLineC((int)xShift + dX01, 3, (int)xShift + dX11, dY11 - 2, gColorBack);
    Painter_DrawLine((int)xShift + dX02, 4, (int)xShift + 2, dY12 - 2); //-V112
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawBothChannels(uint8 *dataA, uint8 *dataB)
{
    int minY = GRID_TOP;
    int maxY = GridChannelBottom();

    if (LAST_AFFECTED_CH == B)
    {
        if (G_ENABLED_A)
        {
            curCh = A;
            DrawDataChannel(dataA, minY, maxY);
        }
        if (G_ENABLED_B)
        {
            curCh = B;
            DrawDataChannel(dataB, minY, maxY);
        }
    }
    else
    {
        if (G_ENABLED_B)
        {
            curCh = B;
            DrawDataChannel(dataB, minY, maxY);
        }
        if (G_ENABLED_A)
        {
            curCh = A;
            DrawDataChannel(dataA, minY, maxY);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static int FillDataP2P(uint8 *data, DataSettings **ds)
{
    int pointsInScreen = 281;
    if (SET_PEACKDET_EN)
    {
        pointsInScreen *= 2;
    }

    uint8 *dataA = 0;
    uint8 *dataB = 0;

    int numPoints = DS_GetLastFrameP2P_RAM(ds, &dataA, &dataB); // Получаем фрейм поточечного вывода

    int numPointsDS = NumBytesInChannel(*ds);

    uint8 *dat[] = {dataA, dataB};

    return RECORDER_MODE ?
        FillDataP2PforRecorder(numPoints, numPointsDS, pointsInScreen, dat[curCh], data) :   // Это возвращаем, если включен режим регистратора
        FillDataP2PforNormal(numPoints, numPointsDS, pointsInScreen, dat[curCh], data);      // А это в нормальном режиме
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawMarkersForMeasure(float scale)
{
    if (curCh == Math)
    {
        return;
    }
    Painter_SetColor(ColorCursors(curCh));
    for (int numMarker = 0; numMarker < 2; numMarker++)
    {
        int pos = Processing_GetMarkerHorizontal(curCh, numMarker);
        if (pos != ERROR_VALUE_INT && pos > 0 && pos < 200)
        {
            Painter_DrawDashedHLine(GridFullBottom() - (int)(pos * scale), GridLeft(), GridRight(), 3, 2, 0);
        }

        pos = Processing_GetMarkerVertical(curCh, numMarker);
        if (pos != ERROR_VALUE_INT && pos > 0 && pos < GridRight())
        {
            Painter_DrawDashedVLine(GridLeft() + (int)(pos * scale), GRID_TOP, GridFullBottom(), 3, 2, 0);
        }

    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static bool DataBeyondTheBorders(const uint8 *data, int firstPoint, int lastPoint)
{
    int numMin = 0; // Здесь количество отсчётов, меньших или равных MIN_VALUE
    int numMax = 0; // Здесь количество отсчётов, больших или равных MAX_VALUE
    int numPoints = lastPoint - firstPoint;
    for (int i = firstPoint; i < lastPoint; i++)
    {
        if (data[i] <= MIN_VALUE) //-V108
        {
            numMin++;
        }
        if (data[i] >= MAX_VALUE) //-V108
        {
            numMax++;
        }
    }
    if (numMin >= numPoints - 1)
    {
        DrawLimitLabel(-10);
        return true;
    }
    else if (numMax >= numPoints - 1)
    {
        DrawLimitLabel(10);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSignalLined(const uint8 *data, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX, bool calculateFiltr)
{
    if (endPoint < startPoint)
    {
        return;
    }

    uint8 dataCD[281];

    int gridLeft = GridLeft();
    int gridRight = GridRight();

    int numPoints = sMemory_NumBytesInChannel(false);
    int numSmoothing = sDisplay_NumPointSmoothing();

    if (G_PEACKDET == PeackDet_Disable)
    {
        for (int i = startPoint; i < endPoint; i++)
        {

            float x0 = gridLeft + (i - startPoint) * scaleX;
            if (x0 >= gridLeft && x0 <= gridRight)
            {
                int index = i - startPoint;
                int y = calculateFiltr ? Math_CalculateFiltr(data, i, numPoints, numSmoothing) : data[i]; //-V108
                int newY = 0;
                CONVERT_DATA_TO_DISPLAY(newY, y);
                dataCD[index] = (uint8)newY;
            }
        }
    }
    else
    {
        for (int i = 1; i < 280 * 2; i += 2)
        {
            float x = gridLeft + i / 2.0f * scaleX;

            int index = i + startPoint * 2;

            int y0 = 0, y1 = 0;
            { CONVERT_DATA_TO_DISPLAY(y0, data[index++]); } //-V108
            { CONVERT_DATA_TO_DISPLAY(y1, data[index++]); } //-V108

            PLACE_2_ASCENDING(y0, y1);

            Painter_DrawVLine((int)x, y0, y1);

            int z0 = 0;
            int z1 = 0;
            { CONVERT_DATA_TO_DISPLAY(z0, data[index++]); } //-V108
            { CONVERT_DATA_TO_DISPLAY(z1, data[index]); } //-V108

            PLACE_2_ASCENDING(z0, z1);

            if (y1 < z0)
            {
                Painter_DrawVLine((int)x, y1, z0);
            }
            else if (y0 > z1)
            {
                Painter_DrawVLine((int)(x + 1), z1, y0);
            }
        }
    }

    if (endPoint - startPoint < 281)
    {
        int numPoints = 281 - (endPoint - startPoint);
        for (int i = 0; i < numPoints + 1; i++)
        {
            int index = endPoint - startPoint + i;
            CONVERT_DATA_TO_DISPLAY(dataCD[index], 0);
        }
    }

    if (G_PEACKDET == PeackDet_Disable)
    {
        CONVERT_DATA_TO_DISPLAY(dataCD[280], data[endPoint]); //-V108
        Painter_DrawSignal(GridLeft(), dataCD, true);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSignalPointed(const uint8 *data, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX)
{
    int numPoints = sMemory_NumBytesInChannel(false);
    int numSmoothing = sDisplay_NumPointSmoothing();

    if (scaleX == 1.0f) //-V550
    {
        uint8 dataCD[281];
        for (int i = startPoint; i < endPoint; i++)
        {
            int index = i - startPoint;
            CONVERT_DATA_TO_DISPLAY(dataCD[index], Math_CalculateFiltr(data, i, numPoints, numSmoothing));
        }
        Painter_DrawSignal(GridLeft(), dataCD, false);
    }
    else
    {
        for (int i = startPoint; i < endPoint; i++)
        {
            int index = i - startPoint;
            int dat = 0;
            CONVERT_DATA_TO_DISPLAY(dat, Math_CalculateFiltr(data, i, numPoints, numSmoothing));
            Painter_SetPoint(GridLeft() + (int)(index * scaleX), dat);  
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static int FillDataP2PforRecorder(int numPoints, int numPointsDS, int pointsInScreen, uint8 *src, uint8 *dest)
{
    // Если точек меньше, чем вмещается на экран - просто копируем их в буфер
    if (numPoints <= pointsInScreen)
    {
        RAM_MemCpy16(src, dest, numPoints);
        return numPoints;
    }

    // Если точек больше, то в буфер копируем последние 281
    int allPoints = numPoints <= numPointsDS ? numPoints : numPointsDS;
    int startIndex = allPoints - pointsInScreen;
    RAM_MemCpy16(src + startIndex, dest, pointsInScreen);
    return pointsInScreen;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static int FillDataP2PforNormal(int numPoints, int numPointsDS, int pointsInScreen, uint8 *src, uint8 *dest)
{
    int deltaNumPoints = numPoints - numPointsDS;
    if (deltaNumPoints < 0)
    {
        deltaNumPoints = 0;
    }

    if (numPoints > 0)
    {
        RAM_MemCpy16(src, dest, numPoints < numPointsDS ? numPoints : numPointsDS);
    }

    int kP2P = SET_PEACKDET_EN ? 2 : 1;

    if (numPoints > pointsInScreen)
    {
        int numScreens = numPoints / pointsInScreen;                                                        // Число полных нарисованных экранов.
        uint8 dataTemp[pointsInScreen];

        memcpy(dataTemp, dest + (numScreens - 1) * pointsInScreen - deltaNumPoints, pointsInScreen);        // Теперь скопируем последний полный экран в буфер

        memcpy(dataTemp, dest + numScreens * pointsInScreen - deltaNumPoints, numPoints % pointsInScreen);  // Теперь скопируем остаток в начало буфера

        xP2P = GridLeft() + ((numPoints  % pointsInScreen) / kP2P) - 1;

        memcpy(dest, dataTemp, pointsInScreen);                                                             // Теперь скопируем временный буфер в выходной
    }
    else
    {
        xP2P = GridLeft() + numPoints / kP2P - 1;
    }

    return numPoints > pointsInScreen ? pointsInScreen : numPoints;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawLimitLabel(int delta)
{
    int width = 150;
    int height = 20;

    Color color = Painter_GetColor();

    int x = GridWidth() / 2 - width / 2 + GridLeft();
    int y = 0;
    if (delta < 0)
    {
        y = GridFullBottom() + delta - height;
    }
    else
    {
        y = GRID_TOP + delta;
    }

    Painter_FillRegionC(x, y, width, height, gColorBack);
    Painter_DrawRectangleC(x, y, width, height, color);
    Painter_DrawStringInCenterRect(x, y, width, height, "Сигнал за пределами экрана");
}
