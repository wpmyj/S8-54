#include "Display.h"
#include "Painter.h"
#include "Hardware/Timer.h"
#include "main.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawProgressBar(uint dT);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    ms->display.value = 0.0f;
    ms->display.isRun = false;
    ms->display.timePrev = 0;
    ms->display.direction = 10.0f;

    gColorBack = COLOR_BLACK;
    gColorFill = COLOR_WHITE;

    Painter_ResetFlash();

    InitHardware();

    Painter_LoadPalette(0);
    Painter_LoadPalette(1);
    Painter_LoadPalette(2);

    //Painter_LoadFont(TypeFont_5);
    Painter_LoadFont(TypeFont_8);
    //Painter_LoadFont(TypeFont_UGO);
    //Painter_LoadFont(TypeFont_UGO2);
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
    ms->display.isRun = true;

    uint dT = gTimerMS - ms->display.timePrev;
    ms->display.timePrev = gTimerMS;

    Painter_BeginScene(COLOR_BLACK);

    Painter_SetColor(COLOR_WHITE);

    if (ms->state == State_Mount)
    {
        DrawProgressBar(dT);
    }
    else if (ms->state == State_WrongFlash)
    {
        Painter_DrawStringInCenterRectC(0, 0, 320, 200, "НЕ УДАЛОСЬ ПРОЧИТАТЬ ДИСК", COLOR_FLASH_10);
        Painter_DrawStringInCenterRectC(0, 20, 320, 200, "УБЕДИТЕСЬ, ЧТО ФАЙЛОВАЯ СИСТЕМА FAT32", COLOR_WHITE);
    }
    else if (ms->state == State_RequestAction)
    {
        Painter_DrawStringInCenterRect(0, 0, 320, 200, "Обнаружено программное обеспечение");
        Painter_DrawStringInCenterRect(0, 20, 320, 200, "Установить его?");

        DrawButton(290, 55, "ДА");
        DrawButton(290, 195, "НЕТ");
    }
    else if (ms->state == State_Upgrade)
    {
        Painter_DrawStringInCenterRect(0, 0, 320, 190, "Подождите завершения");
        Painter_DrawStringInCenterRect(0, 0, 320, 220, "установки программного обеспечения");

        int height = 30;
        int fullWidth = 280;
        int width = fullWidth * ms->percentUpdate;

        Painter_FillRegion(20, 130, width, height);
        Painter_DrawRectangle(20, 130, fullWidth, height);
    }

    Painter_EndScene();
    ms->display.isRun = false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawProgressBar(uint dT)
{
    const int WIDTH = 300;
    const int HEIGHT = 20;
    const int X = 10;
    const int Y = 200;
    
    float step = dT / ms->display.direction;

    ms->display.value += step;

    if (ms->display.direction > 0.0f && ms->display.value > WIDTH)
    {
        ms->display.direction = -ms->display.direction;
        ms->display.value -= step;
    }
    else if (ms->display.direction < 0.0f && ms->display.value < 0)
    {
        ms->display.direction = -ms->display.direction;
        ms->display.value -= step;
    }

    int dH = 15;
    int y0 = 50;

    Painter_DrawStringInCenterRectC(X, y0, WIDTH, 10, "Обнаружен USB-диск.", COLOR_WHITE);
    Painter_DrawStringInCenterRect(X, y0 + dH, WIDTH, 10, "Идёт поиск программного обеспечения");
    Painter_DrawStringInCenterRect(X, y0 + 2 * dH, WIDTH, 10, "Подождите...");

    Painter_DrawRectangle(X, Y, WIDTH, HEIGHT);
    Painter_FillRegion(X, Y, ms->display.value, HEIGHT);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool Display_IsRun(void)
{
    return ms->display.isRun;
}
