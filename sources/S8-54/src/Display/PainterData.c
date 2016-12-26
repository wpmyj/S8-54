#include "PainterData.h"
#include "DisplayTypes.h"
#include "Settings/SettingsDisplay.h"
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

#ifdef _MS_VS
#pragma warning(push)
#pragma warning(disable:4204)
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawDataMemInt(void);
static void DrawDataChannel(uint8 *dataIn, Channel ch, DataSettings *ds, int minY, int maxY);
static void DrawDataInModeNormal(void);
static void DrawDataInModeWorkLatest(void);
static void DrawDataMinMax(void);
// Нарисовать оба канала. Если data == 0, то данные берутся из Processing_GetData
static void DrawBothChannels(uint8 *dataA, uint8 *dataB);
static int FillDataP2P(uint8 *data, Channel ch, DataSettings **ds);
static void DrawMarkersForMeasure(float scale, Channel ch);
static void DrawSignalLined(const uint8 *data, DataSettings *ds, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX, bool calculateFiltr);
static void DrawSignalPointed(const uint8 *data, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX);
static void DrawDataInRect(int x, int width, const uint8 *data, int numElems, Channel ch, bool peackDet);
static uint8 Ordinate(uint8 x, int bottom, float scale);
static void DrawTPos(int leftX, int rightX);
static void DrawTShift(int leftX, int rightX, int numPoints);

static int xP2P = 0;   // Здесь хранится значение для отрисовки вертикальной линии


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PainterData_DrawData(void)
{
    xP2P = 0;

	if (DS_NumElementsInStorage() == 0)
	{
        Painter_DrawRectangleC(GridLeft(), GRID_TOP, GridWidth(), GridFullHeight(), gColorFill);
		return;
	}

	// Режим просмотра сигналов, записанных в ППЗУ
	if (WORK_INT)
	{
		if (SHOW_IN_INT_SAVED || SHOW_IN_INT_BOTH)
		{
			DrawDataMemInt();
		}
		if (SHOW_IN_INT_DIRECT || SHOW_IN_INT_BOTH)
		{
			DrawDataInModeNormal();
		}
	}
	// Режим просмотра сигналов ОЗУ
	else if (WORK_LAST)
	{
		DrawDataInModeWorkLatest();
	}
	// Нормальный режим
	else
	{
		if (gMemory.alwaysShowMemIntSignal == 1)    // Если нужно показывать сигннал из ППЗУ
		{
			DrawDataMemInt();                       // то показываем
		}
		DrawDataInModeNormal();                     // И рисуем последний сигнал    
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


//---------------------------------------------------------------------------------------------------------------------------------------------------
void DrawDataMemInt(void)
{
    if (gDSmemInt != 0)
    {
        DrawDataChannel(gDataAmemInt, A, gDSmemInt, GRID_TOP, GridChannelBottom());
        DrawDataChannel(gDataBmemInt, B, gDSmemInt, GRID_TOP, GridChannelBottom());
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawDataInModeNormal(void)
{
    if ((uint)NumPoints_2_FPGA_NUM_POINTS(sMemory_NumBytesInChannel(false)) != gDSet->indexLength) // Если количество точек в данных не соответствует установленному в настройках - просто выходим
    {
        return;     // WARN Это временно. По хорошему нужно преобразовывать так же, как мы преобразуем tShift, rShift, Range, TBase
    }

    int16 numSignals = (int16)DS_NumElementsWithSameSettings();
    LIMITATION(numSignals, numSignals, 1, DISPLAY_NUM_ACCUM);
    if (numSignals == 1 ||                                          // В хранилище только один сигнал с текущими настройками
        set.display.numAccumulation == NumAccumulation_Infinity ||  // или бесконечное накопление
        set.display.modeAccumulation == ModeAccumulation_Reset ||   // или автоматическая очистка экрана для накопления
        sTime_RandomizeModeEnabled())                               // или в режиме рандомизатора
    {
        DrawBothChannels(0, 0);                                     // когда 0, просто рисуем последний сигнал
    }
    else
    {
        for (int i = 0; i < numSignals; i++)                        // Иначе рисуем необходимое число последних сигналов
        {
            DrawBothChannels(DS_GetData_RAM(A, i), DS_GetData_RAM(B, i));
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawDataInModeWorkLatest(void)
{
    if (gDSmemLast != 0)
    {
        DrawDataChannel(gDataAmemLast, A, gDSmemLast, GRID_TOP, GridChannelBottom());
        DrawDataChannel(gDataBmemLast, B, gDSmemLast, GRID_TOP, GridChannelBottom());
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawDataMinMax(void)
{
    ModeDrawSignal modeDrawSignalOld = set.display.modeDrawSignal;
    set.display.modeDrawSignal = ModeDrawSignal_Lines;
    if (set.display.lastAffectedChannel == B)
    {
        DrawDataChannel(DS_GetLimitation(A, 0), A, gDSet, GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(A, 1), A, gDSet, GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(B, 0), B, gDSet, GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(B, 1), B, gDSet, GRID_TOP, GridChannelBottom());
    }
    else
    {
        DrawDataChannel(DS_GetLimitation(B, 0), B, gDSet, GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(B, 1), B, gDSet, GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(A, 0), A, gDSet, GRID_TOP, GridChannelBottom());
        DrawDataChannel(DS_GetLimitation(A, 1), A, gDSet, GRID_TOP, GridChannelBottom());
    }
    set.display.modeDrawSignal = modeDrawSignalOld;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------]
// Выоводит сообщение на экране о выходе сигнала за границы экрана. delta - расстояние от края сетки, на котором находится сообщение
// Если delta < 0 - выводится внизу сетки
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
// Возвращает true, если изогражение сигнала выходит за пределы экрана
bool DataBeyondTheBorders(uint8 *data, int firstPoint, int lastPoint)
{
    int numMin = 0; // Здесь количество отсчётов, меньших или равных MIN_VALUE
    int numMax = 0; // Здесь количество отсчётов, больших или равных MAX_VALUE
    int numPoints = lastPoint - firstPoint;
    for (int i = firstPoint; i < lastPoint; i++)
    {
        if (data[i] <= MIN_VALUE)
        {
            numMin++;
        }
        if (data[i] >= MAX_VALUE)
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
void DrawDataChannel(uint8 *dataIn, Channel ch, DataSettings *ds, int minY, int maxY)
{
    bool calculateFiltr = true;
    int sizeBuffer = NumBytesInChannel(ds);
#ifndef _MS_VS
    uint8 data[sizeBuffer];     // Место, куда будем считывать данные канала из внешнего ОЗУ
#else
    uint8 data[10];
#endif

    int firstPoint = 0;
    int lastPoint = 280;

    if (!sTime_P2PModeEnabled() ||                          // Если не находимся в режиме медленных поточечных развёрток
        (sTime_P2PModeEnabled() && ds->time.timeMS))        // Или в поточечном, но данные уже считаны
    {
        sDisplay_PointsOnDisplay(&firstPoint, &lastPoint);  // то находим первую и последнюю точки, выводимые на экран
    }
    
    if (sTime_P2PModeEnabled() &&                           // Если находимся в режиме медленных поточечных развёрток
        ds->time.timeMS == 0)                               // и считывание полного набора данных ещё не произошло
    {
        lastPoint = FillDataP2P(data, ch, &ds);           
        if (lastPoint < 2)                                  // Если готово меньше двух точек - выход
        {
            return;
        }
        dataIn = data;
    } 
    else if (dataIn == 0)                                   // Значит, для вывода используем последние считаныые данные из Processing_GetData()
    {
        calculateFiltr = false;
        if (ch == A)
        {
            Processing_GetData(&dataIn, 0, &ds);
        }
        else
        {
            Processing_GetData(0, &dataIn, &ds);
        }
        RAM_MemCpy16(dataIn, data, sizeBuffer);
        dataIn = data;
    }

    if (!sChannel_NeedForDraw(dataIn, ch, ds))
    {
        return;
    }

    float scaleY = (float)(maxY - minY) / (MAX_VALUE - MIN_VALUE);
    float scaleX = (float)GridWidth() / 280.0f;
                                            
    if(set.measures.show)
    {
        DrawMarkersForMeasure(scaleY, ch);
    }

    Painter_SetColor(gColorChan[ch]);

    if (!DataBeyondTheBorders(dataIn, firstPoint, lastPoint))   // Если сигнал не выходит за пределы экрана
    {
        if (set.display.modeDrawSignal == ModeDrawSignal_Lines)
        {
            DrawSignalLined(dataIn, ds, firstPoint, lastPoint, minY, maxY, scaleY, scaleX, calculateFiltr);
        }
        else
        {
            DrawSignalPointed(dataIn, firstPoint, lastPoint, minY, maxY, scaleY, scaleX);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawBothChannels(uint8 *dataA, uint8 *dataB)
{
    if (set.display.lastAffectedChannel == B)
    {
        if (gDSet->enableChA)
        {
            DrawDataChannel(dataA, A, gDSet, GRID_TOP, GridChannelBottom());
        }
        if (gDSet->enableChB)
        {
            DrawDataChannel(dataB, B, gDSet, GRID_TOP, GridChannelBottom());
        }
    }
    else
    {
        if (gDSet->enableChB)
        {
            DrawDataChannel(dataB, B, gDSet, GRID_TOP, GridChannelBottom());
        }
        if (gDSet->enableChA)
        {
            DrawDataChannel(dataA, A, gDSet, GRID_TOP, GridChannelBottom());
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

    int kP2P = PEACKDET_EN ? 2 : 1;

    if (numPoints > pointsInScreen)
    {
        int numScreens = numPoints / pointsInScreen;                                                        // Число полных нарисованных экранов.

#ifdef _MS_VS
        uint8 dataTemp[10];
#else
        uint8 dataTemp[pointsInScreen];
#endif

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


//------------------------------------------------------------------------------------------------------------------------------------------------------
static int FillDataP2P(uint8 *data, Channel ch, DataSettings **ds)
{
    int pointsInScreen = 281;
    if (PEACKDET_EN)
    {
        pointsInScreen *= 2;
    }

    uint8 *dataA = 0;
    uint8 *dataB = 0;

    int numPoints = DS_GetLastFrameP2P_RAM(ds, &dataA, &dataB); // Получаем фрейм поточечного вывода

    int numPointsDS = NumBytesInChannel(*ds);

    return set.service.recorder ?
        FillDataP2PforRecorder(numPoints, numPointsDS, pointsInScreen, ch == A ? dataA : dataB, data) :   // Это возвращаем, если включен режим регистратора
        FillDataP2PforNormal(numPoints, numPointsDS, pointsInScreen, ch == A ? dataA : dataB, data);      // А это в нормальном режиме
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawMarkersForMeasure(float scale, Channel ch)
{
    if (ch == Math)
    {
        return;
    }
    Painter_SetColor(ColorCursors(ch));
    for (int numMarker = 0; numMarker < 2; numMarker++)
    {
        int pos = Processing_GetMarkerHorizontal(ch, numMarker);
        if (pos != ERROR_VALUE_INT && pos > 0 && pos < 200)
        {
            Painter_DrawDashedHLine(GridFullBottom() - (int)(pos * scale), GridLeft(), GridRight(), 3, 2, 0);
        }

        pos = Processing_GetMarkerVertical(ch, numMarker);
        if (pos != ERROR_VALUE_INT && pos > 0 && pos < GridRight())
        {
            Painter_DrawDashedVLine(GridLeft() + (int)(pos * scale), GRID_TOP, GridFullBottom(), 3, 2, 0);
        }

    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
#define CONVERT_DATA_TO_DISPLAY(out, inVal)                     \
    int in = inVal;                                             \
    if(in < MIN_VALUE) { in = MIN_VALUE; }                      \
    if(in > MAX_VALUE) { in = MAX_VALUE; }                      \
    out = (uint8)(maxY - (int)(((in) - MIN_VALUE) * scaleY));   \
    if(out < (uint8)minY)   { out = (uint8)minY; }              \
    if(out > (uint8)maxY)   { out = (uint8)maxY; };

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Размещает два значения по возрастанию : val1 - меньшее, val2 - большее
#define PLACE_2_ASCENDING(v1, v2) \
    if((v1) > (v2)) { int qwerty = v1; v1 = v2; v2 = qwerty; }

//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawSignalLined(const uint8 *data, DataSettings *ds, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX, bool calculateFiltr)
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

    if (ds->peackDet == PeackDet_Disable)
    {
        for (int i = startPoint; i < endPoint; i++)
        {

            float x0 = gridLeft + (i - startPoint) * scaleX;
            if (x0 >= gridLeft && x0 <= gridRight)
            {
                int index = i - startPoint;
                int y = calculateFiltr ? Math_CalculateFiltr(data, i, numPoints, numSmoothing) : data[i];
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
            float x = gridLeft + i / 2 * scaleX;

            int index = i + startPoint * 2;

            int y0 = 0, y1 = 0;
            { CONVERT_DATA_TO_DISPLAY(y0, data[index++]); }
            { CONVERT_DATA_TO_DISPLAY(y1, data[index++]); }

            PLACE_2_ASCENDING(y0, y1);

            Painter_DrawVLine((int)x, y0, y1);

            int z0 = 0;
            int z1 = 0;
            { CONVERT_DATA_TO_DISPLAY(z0, data[index++]); }
            { CONVERT_DATA_TO_DISPLAY(z1, data[index]); }

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

    if (ds->peackDet == PeackDet_Disable)
    {
        CONVERT_DATA_TO_DISPLAY(dataCD[280], data[endPoint]);
        Painter_DrawSignal(GridLeft(), dataCD, true);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawSignalPointed(const uint8 *data, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX)
{
    int numPoints = sMemory_NumBytesInChannel(false);
    int numSmoothing = sDisplay_NumPointSmoothing();

    if (scaleX == 1.0f)
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
void PainterData_DrawMath(void)
{
    if (!FUNC_ENABLED || DS_GetData_RAM(A, 0) == 0 || DS_GetData_RAM(B, 0) == 0)
    {
        return;
    }
    
    uint8 *dataRel0 = 0;
    uint8 *dataRel1 = 0;
    DataSettings *ds = 0;
    Processing_GetData(&dataRel0, &dataRel1, &ds);

    float *dataAbsA = (float*)RAM(DRAW_MATH_DATA_REL_A);
    float *dataAbsB = (float*)RAM(DRAW_MATH_DATA_REL_B);

    int numPoints = NumBytesInChannel(ds);

    Math_PointsRelToVoltage(dataRel0, numPoints, (Range)ds->range[A], ds->rShift[A], dataAbsA);
    Math_PointsRelToVoltage(dataRel1, numPoints, (Range)ds->range[B], ds->rShift[B], dataAbsB);

    Math_CalculateMathFunction(dataAbsA, dataAbsB, numPoints);

    uint8 points[FPGA_MAX_POINTS];

    Math_PointsVoltageToRel(dataAbsA, numPoints, RANGE_MATH, RSHIFT_MATH, points);

    DrawDataChannel(points, Math, ds, GridMathTop(), GridMathBottom());

    static const int WIDTH = 71;
    static const int HEIGHT = 10;
    int delta = (SHOW_STRING_NAVI_ALL && FUNC_TOGETHER) ? 10 : 0;
    Painter_DrawRectangleC(GridLeft(), GridMathTop() + delta, WIDTH, HEIGHT, gColorFill);
    Painter_FillRegionC(GridLeft() + 1, GridMathTop() + 1 + delta, WIDTH - 2, HEIGHT - 2, gColorBack);
    Divider divider = set.math.divider;
    Painter_DrawTextC(GridLeft() + 2, GridMathTop() + 1 + delta, sChannel_Range2String(RANGE_MATH, divider), gColorFill);
    Painter_DrawText(GridLeft() + 25, GridMathTop() + 1 + delta, ":");
    char buffer[20];
    Painter_DrawText(GridLeft() + 27, GridMathTop() + 1 + delta, sChannel_RShift2String(RSHIFT_MATH, RANGE_MATH, divider, buffer));
}

/*
void DrawPeackDet(int x, int y, int width, int height, uint8 *data, int numPoints)
{
    BitSet16 *pointer = (BitSet16*)data;

    float scaleX = width / (float)numPoints;
    float scaleY = height / 250.0f;

    Painter_SetColor(gColorChan[B]);

    for (int i = 0; i < numPoints; i++)
    {
        uint8 min = pointer[i].byte0;
        uint8 max = pointer[i].byte1;
        Painter_DrawVLine(x + scaleX * i, y + height - min * scaleY, y + height - max * scaleY);
    }
}
*/

//------------------------------------------------------------------------------------------------------------------------------------------------------
// Нарисовать окно памяти
void PainterData_DrawMemoryWindow(void)
{
    uint8 *datA = gDataAmemInt;
    uint8 *datB = gDataBmemInt;
    DataSettings *ds = gDSmemInt;

    uint8 *dA = 0;  // Сюда считаем данные каналов из RAM
    uint8 *dB = 0;

    if (WORK_DIRECT || WORK_LAST)
    {
        datA = gDataA;
        datB = gDataB;
        ds = gDSet;

        if (DS_DataSettingsFromEnd(0)->tBase >= MIN_TBASE_P2P)          // Если находимся в режиме поточечного вывода
        {
            DS_GetLastFrameP2P_RAM(&ds, &datA, &datB);
        }

        // Нужно переписать из внешнего ОЗУ в стек, потому чт
        dA = AllocMemForChannelFromHeap(A, ds);
        dB = AllocMemForChannelFromHeap(B, ds);

        int numBytes = NumBytesInChannel(ds);

        RAM_MemCpy16(datA, dA, numBytes);
        RAM_MemCpy16(datB, dB, numBytes);

        datA = dA;
        datB = dB;
    }

    int leftX = 3;
    int top = 0;
    int height = GRID_TOP - 3;
    int bottom = top + height;

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

    Channel lastAffectedChannel = set.display.lastAffectedChannel;
    if (((uint)NumPoints_2_FPGA_NUM_POINTS(sMemory_NumBytesInChannel(false)) == gDSet->indexLength) && (gDataA || gDataB))
    {
        Channel chanFirst = lastAffectedChannel == A ? B : A;
        Channel chanSecond = lastAffectedChannel == A ? A : B;
        const uint8 *dataFirst = lastAffectedChannel == A ? datB : datA;
        const uint8 *dataSecond = lastAffectedChannel == A ? datA : datB;

        bool peackDet = ds->peackDet != PeackDet_Disable;
        
        if (sChannel_NeedForDraw(dataFirst, chanFirst, ds))
        {
            DrawDataInRect(1, rightX + 3, dataFirst, sMemory_NumBytesInChannel(false), chanFirst, peackDet);
        }
        if (sChannel_NeedForDraw(dataSecond, chanSecond, ds))
        {
            DrawDataInRect(1, rightX + 3, dataSecond, sMemory_NumBytesInChannel(false), chanSecond, peackDet);
        }
    }

    Painter_DrawRectangleC(xVert0, top, width - (set.memory.fpgaNumPoints == FNP_8k ? 1 : 0), bottom - top, gColorFill);

    DrawTPos(leftX, rightX);

    DrawTShift(leftX, rightX, sMemory_NumBytesInChannel(false));

    free(dA);
    free(dB);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawTPos(int leftX, int rightX)
{
    int x[] = {leftX, (rightX - leftX) / 2 + leftX, rightX};
    int x0 = x[set.time.tPos];
    Painter_FillRegionC(x0 - 3, 9, 6, 6, gColorBack);
    Painter_DrawCharC(x0 - 3, 9, SYMBOL_TPOS_1, gColorFill);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawTShift(int leftX, int rightX, int numBytes)
{
    float scale = (float)(rightX - leftX + 1) / ((float)numBytes - (numBytes == 281 ? 1 : 0));
    int xShift = (int)(1.5f + (sTime_TPosInBytes() - sTime_TShiftInPoints()) * scale) - 1;
    if (PEACKDET_EN)
    {
        if (set.time.tPos == TPos_Right)
        {
            --xShift;
        }
    }
    if (set.memory.fpgaNumPoints == FNP_512)
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
    Painter_FillRegionC((int)xShift, 2, 4, 4, gColorFill);
    Painter_DrawLineC((int)xShift + dX01, 3, (int)xShift + dX11, dY11 - 2, gColorBack);
    Painter_DrawLine((int)xShift + dX02, 4, (int)xShift + 2, dY12 - 2);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawDataInRect(int x, int width, const uint8 *data, int numBytes, Channel ch, bool peackDet)
{
    if (numBytes == 0)
    {
        return;
    }

    width--;
    float elemsInColumn = (float)numBytes / (float)width;
    uint8 min[300];
    uint8 max[300];

    if (peackDet == PeackDet_Disable)              // Если пик. дет. выключен
    {
        uint8 *iMin = &min[0];
        uint8 *iMax = &max[0];

        for (int col = 0; col < width; col++, iMin++, iMax++)
        {
            int firstElem = (int)(col * elemsInColumn);
            int lastElem = firstElem + (int)elemsInColumn - 1;
            *iMin = data[firstElem];
            *iMax = data[firstElem];
            for (int elem = firstElem + 1; elem <= lastElem; elem++)
            {
                SET_MIN_IF_LESS(data[elem], *iMin);
                SET_MAX_IF_LARGER(data[elem], *iMax);
            }
        }
    }
    else                                // Если пик. дет. включён
    {
        for (int col = 0; col < width; col++)
        {
            float firstElem = col * elemsInColumn;
            float lastElem = firstElem + elemsInColumn - 1;
            min[col] = data[(int)firstElem];
            max[col] = data[(int)firstElem];
            for (int elem = (int)firstElem + 1; elem <= (int)lastElem; elem++)
            {
                SET_MIN_IF_LESS(data[elem], min[col]);
                SET_MAX_IF_LARGER(data[elem], max[col]);
            }
        }
    }

    int bottom = 17;
    int height = 17;
    float scale = (float)height / (float)(MAX_VALUE - MIN_VALUE);

    uint8 points[300 * 2];

    points[0] = Ordinate(max[0], bottom, scale);
    points[1] = Ordinate(min[0], bottom, scale);


    for (int i = 1; i < width; i++)
    {
        int value0 = min[i] > max[i - 1] ? max[i - 1] : min[i];
        int value1 = max[i] < min[i - 1] ? min[i - 1] : max[i];
        points[i * 2] = Ordinate((uint8)value1, bottom, scale);
        points[i * 2 + 1] = Ordinate((uint8)value0, bottom, scale);
    }
    if (width < 256)
    {
        Painter_DrawVLineArray(x, width, points, gColorChan[ch]);
    }
    else
    {
        Painter_DrawVLineArray(x, 255, points, gColorChan[ch]);
        Painter_DrawVLineArray(x + 255, width - 255, points + 255 * 2, gColorChan[ch]);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static uint8 Ordinate(uint8 x, int bottom, float scale)
{
    return (uint8)(((float)bottom - scale * LimitationInt(x - MIN_VALUE, 0, (MAX_VALUE - MIN_VALUE))) + 0.5f);
}

#ifdef _MS_VS
#pragma warning(pop)
#endif
