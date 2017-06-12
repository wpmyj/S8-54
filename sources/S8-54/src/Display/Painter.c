// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Colors.h"
#include "Painter.h"
#include "font/Font.h"
#include "defines.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "VCP/VCP.h"
#include "Ethernet/Ethernet.h"
#include "Ethernet/TcpSocket.h"
#include "Hardware/FSMC.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "FlashDrive/FlashDrive.h"
#include "Menu/FileManager.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool inverseColors = false;
static Color currentColor = NUM_COLORS;
static bool framesElapsed = false;


static enum StateTransmit
{
    StateTransmit_Free,
    StateTransmit_NeedForTransmitFirst,  ///< Это когда нужно передать первый кадр - передаются шрифты.
    StateTransmit_NeedForTransmitSecond, ///< Это когда нужно передать второй и последующий кадры - шрифты не передаются.
    StateTransmit_InProcess
} stateTransmit = StateTransmit_Free;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void RunDisplay(void);
static void OnTimerFlashDisplay(void);
static void CalculateColor(uint8 *color);
static uint8 Read2points(int x, int y);

// Эти функции не используются, но оставлены для образца
//static void Get4Bytes(uint8 bytes[4]);
//static Color GetColor(int x, int y);
//static void Get8Points(int x, int y, uint8 buffer[4]);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Painter_BeginScene(Color color)
{
    if (stateTransmit == StateTransmit_NeedForTransmitFirst || stateTransmit == StateTransmit_NeedForTransmitSecond)
    {
        bool needForLoadFontsAndPalette = stateTransmit == StateTransmit_NeedForTransmitFirst;
        stateTransmit = StateTransmit_InProcess;
        if (needForLoadFontsAndPalette)
        {
            Painter_LoadPalette(0);
            Painter_LoadPalette(1);
            Painter_LoadPalette(2);
            Painter_LoadFont(TypeFont_5);
#define dT 100
            Ethernet_Update(dT);            // WARN Говнокод. Доработать метод посылки в TCPSocket

            Painter_LoadFont(TypeFont_8);
            Ethernet_Update(dT);

            Painter_LoadFont(TypeFont_UGO);
            Ethernet_Update(dT);

            Painter_LoadFont(TypeFont_UGO2);
            Ethernet_Update(dT);
        }
    }

    Painter_FillRegionC(0, 0, 319, 239, color);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_EndScene(void)
{
    if (!framesElapsed)
    {
        framesElapsed = true;
        return;
    }
    uint8 command[4] = {INVALIDATE};
    Painter_SendToDisplay(command, 4);
    Painter_SendToInterfaces(command, 1);
    if (stateTransmit == StateTransmit_InProcess)
    {
        VCP_Flush();
        stateTransmit = StateTransmit_Free;
    }

    RunDisplay();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SendFrame(bool first)
{
    if (stateTransmit == StateTransmit_Free)
    {
        stateTransmit = (first ? StateTransmit_NeedForTransmitFirst : StateTransmit_NeedForTransmitSecond);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_ResetFlash(void)
{
    Timer_SetAndEnable(kFlashDisplay, OnTimerFlashDisplay, 400);
    inverseColors = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SetColor(Color color)
{
    if (color != currentColor)
    {
        currentColor = color;
        if (currentColor > NUM_COLORS)
        {
            CalculateColor((uint8*)(&(color)));
        }
        uint8 command[4] = {SET_COLOR, color};
        Painter_SendToDisplay(command, 4);
        Painter_SendToInterfaces(command, 2);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Color Painter_GetColor(void)
{
    return currentColor;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_LoadPalette(int num)
{
    const int min[] = {0, 5, 10};
    const int max[] = {4, 9, 15};

    int i = min[num];
    int a = max[num];

    for (; i <= a; i++)
    {
        Painter_SetPalette((Color)i);
        Timer_PauseOnTime(1);           /** \todo Эта задержака вставлена потому, что без неё при загрузке палитры иногда подмигивает экран.
                                         А загрузка палитры достаточно хитрая - её надо делать по частям, иначе экран подмигивает. */
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SetPalette(Color color)
{
    uint8 command[4] = {SET_PALETTE_COLOR};
    WRITE_BYTE(1, color);
    WRITE_SHORT(2, COLOR(color));

    Painter_SendToDisplay(command, 4);
    Painter_SendToInterfaces(command, 4);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SetPoint(int x, int y)
{
    uint8 command[4] = {DRAW_PIXEL};
    WRITE_SHORT(1, x);
    WRITE_BYTE(3, y);

    Painter_SendToDisplay(command, 4);
    Painter_SendToInterfaces(command, 4);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawHLine(int y, int x0, int x1)
{
    CalculateCurrentColor();

    uint8 command[8] = {DRAW_HLINE};
    WRITE_BYTE(1, y);
    WRITE_SHORT(2, x0);
    WRITE_SHORT(4, x1);

    Painter_SendToDisplay(command, 8);
    Painter_SendToInterfaces(command, 6);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawVLine(int x, int y0, int y1)
{
    CalculateCurrentColor();

    uint8 command[8] = {DRAW_VLINE};
    WRITE_SHORT(1, x);
    WRITE_BYTE(3, y0);
    WRITE_BYTE(4, y1);

    Painter_SendToDisplay(command, 8);
    Painter_SendToInterfaces(command, 5);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawHPointLine(int y, int x0, int x1, float delta)
{
    for (int x = x0; x <= x1; x += (int)delta)
    {
        Painter_SetPoint(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawMultiVPointLine(int numLines, int y, uint16 x[], int delta, int count, Color color)
{
    ASSERT_RET(numLines > 20, "Число линий слишком большое %d", numLines);

    Painter_SetColor(color);

    uint8 command[60] = {DRAW_MULTI_VPOINT_LINE, numLines, y, count, delta, 0};

    uint8 *pointer = command + 6;
    for (int i = 0; i < numLines; i++)
    {
        *((uint16*)pointer) = x[i];
        pointer += 2;
    }
    int numBytes = 1 + 1 + 1 + numLines * 2 + 1 + 1;
    while (numBytes % 4)
    {
        numBytes++;
    }
    Painter_SendToDisplay(command, numBytes);
    Painter_SendToInterfaces(command, 1 + 1 + 1 + 1 + 1 + 1 + numLines * 2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawMultiHPointLine(int numLines, int x, uint8 y[], int delta, int count, Color color)
{
    if (numLines > 20)
    {
        LOG_ERROR_TRACE("Число линий слишком большое %d", numLines);
        return;
    }
    Painter_SetColor(color);

    uint8 command[30] = {DRAW_MULTI_HPOINT_LINE};
    WRITE_BYTE(1, numLines);
    WRITE_SHORT(2, x);
    WRITE_BYTE(4, count);
    WRITE_BYTE(5, delta);

    uint8 *pointer = command + 6;
    for (int i = 0; i < numLines; i++)
    {
        *pointer = y[i];
        pointer++;
    }
    int numBytes = 1 +  // command
        1 +             // numLines
        2 +             // x
        numLines +      // numLines
        1 +
        1;
    while (numBytes % 4)
    {
        numBytes++;
    }
    Painter_SendToDisplay(command, numBytes);
    Painter_SendToInterfaces(command, 1 + 1 + 2 + 1 + 1 + numLines);

    Painter_DrawHLineC(y[12], 0, 10, gColorChan[A]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawLine(int x1, int y1, int x2, int y2)
{
    if (x1 == x1)
    {
        Painter_DrawVLine(x1, y1, y2);
    }
    else if (y1 == y2)
    {
        Painter_DrawHLine(y1, x1, x2);
    }
    else
    {
        uint8 command[8] = {DRAW_LINE};
        WRITE_SHORT(1, x1);
        WRITE_BYTE(3, y1);
        WRITE_SHORT(4, x2);
        WRITE_BYTE(6, y2);

        Painter_SendToDisplay(command, 8);
        Painter_SendToInterfaces(command, 7);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawDashedHLine(int y, int x0, int x1, int deltaFill, int deltaEmpty, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmpty))
    {
        LOG_ERROR_TRACE("Неправильный аргумент deltaStart = %d", deltaStart);
        return;
    }
    int x = x0;
    if (deltaStart != 0)                // Если линию нужно рисовать не с начала штриха
    {
        x += (deltaFill + deltaEmpty - deltaStart);
        if (deltaStart < deltaFill)     // Если начало линии приходится на штрих
        {
            Painter_DrawHLine(y, x0, x - 1);
        }
    }

    while (x < x1)
    {
        Painter_DrawHLine(y, x, x + deltaFill - 1);
        x += (deltaFill + deltaEmpty);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawDashedVLine(int x, int y0, int y1, int deltaFill, int deltaEmtpy, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmtpy))
    {
        LOG_ERROR_TRACE("Неправильный аргумент deltaStart = %d", deltaStart);
        return;
    }
    int y = y0;
    if (deltaStart != 0)                 // Если линию нужно рисовать не с начала штриха
    {
        y += (deltaFill + deltaEmtpy - deltaStart);
        if (deltaStart < deltaFill)     // Если начало линии приходится на штрих
        {
            Painter_DrawVLine(x, y0, y - 1);
        }
    }

    while (y < y1)
    {
        Painter_DrawVLine(x, y, y + deltaFill - 1);
        y += (deltaFill + deltaEmtpy);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawRectangle(int x, int y, int width, int height)
{
    Painter_DrawHLine(y, x, x + width);
    Painter_DrawVLine(x, y, y + height);
    Painter_DrawHLine(y + height, x, x + width);
    if (x + width < SCREEN_WIDTH)
    {
        Painter_DrawVLine(x + width, y, y + height);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_FillRegion(int x, int y, int width, int height)
{
    CalculateCurrentColor();

    uint8 command[8] = {FILL_REGION};
    WRITE_SHORT(1, x);
    WRITE_BYTE(3, y);
    WRITE_SHORT(4, width);
    WRITE_BYTE(6, height);

    Painter_SendToDisplay(command, 8);
    Painter_SendToInterfaces(command, 7);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawVolumeButton(int x, int y, int width, int height, int thickness, Color normal, Color bright, Color dark, bool isPressed, bool isShade)
{
    Painter_FillRegionC(x + thickness, y + thickness, width - thickness * 2, height - thickness * 2, normal);
    if (isPressed && !isShade)
    {
        for (int i = 0; i < thickness; i++)
        {
            Painter_DrawHLineC(y + i, x + i, x + width - i, dark);
            Painter_DrawVLine(x + i, y + 1 + i, y + height - i);
            Painter_DrawVLineC(x + width - i, y + 1 + i, y + height - i, bright);
            Painter_DrawHLine(y + height - i, x + 1 + i, x + width - i);
        }
    }
    else
    {
        for (int i = 0; i < thickness; i++)
        {
            Painter_DrawHLineC(y + i, x + i, x + width - i, bright);
            Painter_DrawVLine(x + i, y + 1 + i, y + height - i);
            Painter_DrawVLineC(x + width - i, y + 1 + i, y + height - i, dark);
            Painter_DrawHLine(y + height - i, x + 1 + i, x + width - i);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SetBrightnessDisplay(int16 brightness)
{
    float recValue = 1601.0f;
    if (brightness < 100)
    {
        recValue = 64.0f + (600.0f - 63.0f) / 100.0f / 100.0f * brightness * brightness;
    }
    uint8 command[4] = {SET_BRIGHTNESS};
    WRITE_SHORT(1, recValue);

    Painter_SendToDisplay(command, 4);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint16 Painter_ReduceBrightness(uint16 colorValue, float newBrightness)
{
    int red = (int)(R_FROM_COLOR(colorValue) * newBrightness);
    LIMITATION(red, red, 0, 31);
    int green = (int)(G_FROM_COLOR(colorValue) * newBrightness);
    LIMITATION(green, green, 0, 63);
    int blue = (int)(B_FROM_COLOR(colorValue) * newBrightness);
    LIMITATION(blue, blue, 0, 31);
    return MAKE_COLOR(red, green, blue);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawVLineArray(int x, int numLines, uint8 *y0y1, Color color)
{
    Painter_SetColor(color);

    uint8 command[255 * 2 + 4 + 4] = {DRAW_VLINES_ARRAY};
    WRITE_SHORT(1, x);

    if (numLines > 255)
    {
        numLines = 255;
    }

    WRITE_BYTE(3, numLines);

    for (int i = 0; i < numLines; i++)
    {
        WRITE_BYTE(4 + i * 2,     *(y0y1 + i * 2));
        WRITE_BYTE(4 + i * 2 + 1, *(y0y1 + i * 2 + 1));
    }
    int numBytes = numLines * 2 + 4;
    while (numBytes % 4)
    {
        numBytes++;
    }
    Painter_SendToDisplay(command, numBytes);
    Painter_SendToInterfaces(command, 1 + 2 + 1 + 2 * numLines);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawSignal(int x, uint8 data[281], bool modeLines)
{
    uint8 command[284] = {modeLines ? DRAW_SIGNAL_LINES : DRAW_SIGNAL_POINTS};
    WRITE_SHORT(1, x);
    for (int i = 0; i < 281; i++)
    {
        WRITE_BYTE(3 + i, data[i]);
    }

    Painter_SendToDisplay(command, 284);
    Painter_SendToInterfaces(command, 284);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawPicture(int x, int y, int width, int height, uint8 *address)
{
    uint8 command[4] = {LOAD_IMAGE};
    WRITE_SHORT(1, x);
    WRITE_BYTE(3, y);

    Painter_SendToDisplay(command, 4);

    WRITE_SHORT(0, width);
    WRITE_BYTE(2, height);
    WRITE_BYTE(3, 0);

    Painter_SendToDisplay(command, 4);

    for (int i = 0; i < width * height / 2 / 4; i++)
    {
        WRITE_BYTE(0, *address++);
        WRITE_BYTE(1, *address++);
        WRITE_BYTE(2, *address++);
        WRITE_BYTE(3, *address++);

        Painter_SendToDisplay(command, 4);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Painter_SaveScreenToFlashDrive(void)
{

#pragma pack(1)
    typedef struct
    {
        char    type0;      // 0
        char    type1;      // 1
        uint    size;       // 2
        uint16  res1;       // 6
        uint16  res2;       // 8
        uint    offBits;    // 10
    } BITMAPFILEHEADER;
    // 14

    typedef struct
    {
        uint    size;           // 14
        int     width;          // 18
        int     height;         // 22
        uint16  planes;         // 26
        uint16  bitCount;       // 28
        uint    compression;    // 30
        uint    sizeImage;      // 34
        int     xPelsPerMeter;  // 38
        int     yPelsPerMeter;  // 42
        uint    clrUsed;        // 46
        uint    clrImportant;   // 50
        //uint    notUsed[15];
    } BITMAPINFOHEADER;
    // 54
#pragma pack(4)

    BITMAPFILEHEADER bmFH =
    {
        0x42,
        0x4d,
        14 + 40 + 1024 + 320 * 240 / 2,
        0,
        0,
        14 + 40 + 1024
    };

    StructForWrite structForWrite;
    char fileName[255];

    if (!FM_GetNameForNewFile(fileName))
    {
        return false;
    }

    FDrive_OpenNewFileForWrite(fileName, &structForWrite);

    FDrive_WriteToFile((uint8*)(&bmFH), 14, &structForWrite);

    BITMAPINFOHEADER bmIH =
    {
        40, // size;
        320,// width;
        240,// height;
        1,  // planes;
        4,  // bitCount;
        0,  // compression;
        0,  // sizeImage;
        0,  // xPelsPerMeter;
        0,  // yPelsPerMeter;
        0,  // clrUsed;
        0   // clrImportant;
    };

    FDrive_WriteToFile((uint8*)(&bmIH), 40, &structForWrite);

    uint8 buffer[320 * 3] = {0};

    typedef struct tagRGBQUAD
    {
        uint8    blue;
        uint8    green;
        uint8    red;
        uint8    rgbReserved;
    } RGBQUAD;

    RGBQUAD colorStruct;

    for (int i = 0; i < 16; i++)
    {
        uint16 color = COLOR(i);
        colorStruct.blue = (uint8)((float)B_FROM_COLOR(color) / 31.0f * 255.0f);
        colorStruct.green = (uint8)((float)G_FROM_COLOR(color) / 63.0f * 255.0f);
        colorStruct.red = (uint8)((float)R_FROM_COLOR(color) / 31.0f * 255.0f);
        colorStruct.rgbReserved = 0;
        ((RGBQUAD*)(buffer))[i] = colorStruct;
    }

    for (int i = 0; i < 4; i++)
    {
        FDrive_WriteToFile(buffer, 256, &structForWrite);
    }

    FSMC_SET_MODE(ModeFSMC_Display);

    for (int y = 239; y >= 0; y--)
    {
        for (int x = 1; x < 320; x += 2)
        {
            uint8 color = Read2points(x, y);

            while (color != Read2points(x, y))      // Этот цикл нужен потому, что иногда цвета считываются неправильно - 
            {                                       // проверка, что повторное чтение читает тот же цвет.
                color = Read2points(x, y);
            }

            buffer[x / 2] = ((color & 0x0f) << 4) + (color >> 4);
        }

        FDrive_WriteToFile(buffer, 160, &structForWrite);
    }

    FSMC_RESTORE_MODE();

    FDrive_CloseFile(&structForWrite);

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SendToDisplay(uint8 *bytes, int numBytes)
{
    for (int i = 0; i < numBytes; i += 4)
    {
        while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET)
        {
        };
        Timer_PauseOnTicks(75);    /// \todo Здесь время ожидание увеличено по сравнению с С8-53 (там частота 120МГц, здесь - 180МГц)
        *ADDR_CDISPLAY = *bytes++;
        *ADDR_CDISPLAY = *bytes++;
        *ADDR_CDISPLAY = *bytes++;
        *ADDR_CDISPLAY = *bytes++;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SendToInterfaces(uint8 *pointer, int size)
{
    if (stateTransmit == StateTransmit_InProcess)
    {
        VCP_SendDataSynch(pointer, size);
        TCPSocket_Send((const char*)pointer, size);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void RunDisplay(void)
{
    uint8 command[4] = {RUN_BUFFER};

    Painter_SendToDisplay(command, 4);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnTimerFlashDisplay(void)
{
    inverseColors = !inverseColors;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void CalculateColor(uint8 *color)
{
    currentColor = (Color)*color;
    if (*color == COLOR_FLASH_10)
    {
        *color = inverseColors ? (uint8)gColorBack : (uint8)gColorFill;
    }
    else if (*color == COLOR_FLASH_01)
    {
        *color = inverseColors ? (uint8)gColorFill : (uint8)gColorBack;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void CalculateCurrentColor(void)
{
    if (currentColor == COLOR_FLASH_10)
    {
        Painter_SetColor(inverseColors ? gColorBack : gColorFill);
    }
    else if (currentColor == COLOR_FLASH_01)
    {
        Painter_SetColor(inverseColors ? gColorFill : gColorBack);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint8 Read2points(int x, int y)
{
    while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET)
    {
    };
    Timer_PauseOnTicks(12);               // WARN временно увеличено время ожидания - не читает флешку

    *ADDR_CDISPLAY = GET_PIXEL;
    *ADDR_CDISPLAY = (uint8)x;
    *ADDR_CDISPLAY = (uint8)(x >> 8);
    *ADDR_CDISPLAY = (uint8)y;

    while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET)
    {
    };
    //Timer_PauseOnTicks(6);               // временно увеличено время ожидания - не читает флешку

    return *ADDR_CDISPLAY;
}



//----------------------------------------------------------------------------------------------------------------------------------------------------
/*
static void Get4Bytes(uint8 bytes[4])
{
    while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET) {};
    bytes[0] = *ADDR_CDISPLAY;
    bytes[1] = *ADDR_CDISPLAY;
    bytes[2] = *ADDR_CDISPLAY;
    bytes[3] = *ADDR_CDISPLAY;
}
*/


//----------------------------------------------------------------------------------------------------------------------------------------------------
/*
static Color GetColor(int x, int y)
{
    uint8 command[4];
    command[0] = GET_PIXEL;
    *((int16*)(command + 1)) = (int16)x;
    *(command + 3) = (int8)y;
    Painter_SendToDisplay(command, 4);
    Get4Bytes(command);
    return (Color)(command[0] & 0x0f);
}
*/


//----------------------------------------------------------------------------------------------------------------------------------------------------
/*
static void Get8Points(int x, int y, uint8 buffer[4])
{
    uint8 command[4];
    command[0] = GET_PIXEL;
    *((int16*)(command + 1)) = (int16)x;
    *(command + 3) = (int8)y;
    Painter_SendToDisplay(command, 4);
    Get4Bytes(buffer);
}
*/
