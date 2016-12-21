#include "Display.h"
#include "Painter.h"
#include "Hardware/Timer.h"
#include "main.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawProgressBar(uint dT);

static bool isRun = false;


///////////////////////////////////////////////////////////////////////////////////////
void InitHardware(void)
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Display_Init(void)
{
    gColorBack = COLOR_BLACK;
    gColorFill = COLOR_WHITE;

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


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawButton(int x, int y, char *text)
{
    int width = 25;
    int height = 20;
    Painter_DrawRectangle(x, y, width, height);
    Painter_DrawStringInCenterRect(x, y, width + 2, height - 1, text);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
void Display_Update(void)
{
    isRun = true;
    static uint timePrev = 0;

    uint dT = gTimerMS - timePrev;
    timePrev = gTimerMS;

    Painter_BeginScene(COLOR_BLACK);

    Painter_SetColor(COLOR_WHITE);

    if (state == State_Start)
    {
        Painter_DrawText(10, 10, "Start");
    }
    else if (state == State_Mount)
    {
        DrawProgressBar(dT);
    }
    else if (state == State_WrongFlash)
    {
        Painter_DrawStringInCenterRectC(0, 0, 320, 200, "НЕ УДАЛОСЬ ПРОЧИТАТЬ ДИСК", COLOR_FLASH_10);
        Painter_DrawStringInCenterRectC(0, 20, 320, 200, "УБЕДИТЕСЬ, ЧТО ФАЙЛОВАЯ СИСТЕМА FAT32", COLOR_WHITE);
    }
    else if (state == State_RequestAction)
    {
        Painter_DrawStringInCenterRect(0, 0, 320, 200, "Обнаружено программное обеспечение");
        Painter_DrawStringInCenterRect(0, 20, 320, 200, "Установить его?");

        DrawButton(290, 55, "ДА");
        DrawButton(290, 195, "НЕТ");
    }
    else if (state == State_Upgrade)
    {
        Painter_DrawText(10, 10, "Upgrage");
    }
    else if (state == State_Ok)
    {
        Painter_DrawText(10, 10, "Ok");
    }
    else
    {
        Painter_DrawText(10, 10, "Неизвестное состояние");
    }

    Painter_EndScene();
    isRun = false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawProgressBar(uint dT)
{
    static float value = 0;

    const int WIDTH = 300;
    const int HEIGHT = 20;
    const int X = 10;
    const int Y = 200;

    static float direction = 10.0f;
    
    float step = dT / direction;

    value = value + step;

    if (direction > 0.0f && value > WIDTH)
    {
        direction = -direction;
        value -= step;
    }
    else if (direction < 0.0f && value < 0)
    {
        direction = -direction;
        value -= step;
    }

    int dH = 15;
    int y0 = 50;

    Painter_DrawStringInCenterRectC(X, y0, WIDTH, 10, "Обнаружен USB-диск.", COLOR_WHITE);
    Painter_DrawStringInCenterRect(X, y0 + dH, WIDTH, 10, "Идёт поиск программного обеспечения");
    Painter_DrawStringInCenterRect(X, y0 + 2 * dH, WIDTH, 10, "Подождите...");

    Painter_DrawRectangle(X, Y, WIDTH, HEIGHT);
    Painter_FillRegion(X, Y, value, HEIGHT);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool Display_IsRun(void)
{
    return isRun;
}
