#pragma once
#include "Hardware/FLASH.h"
#include "Hardware/Hardware.h"
#include "Menu/Pages/Definition.h"
#include "Menu/MenuFunctions.h"
#include "Menu/MenuDrawing.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page pService;


// СЕРВИС - ИНФОРМАЦИЯ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void PressSB_Information_Exit(void)
{
    Display_SetDrawMode(DrawMode_Auto, 0);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Information_Draw(void)
{
    Language lang = LANG;

    Painter_BeginScene(gColorBack);
    int x = 100;
    int dY = 20;
    int y = 20;
    Painter_DrawRectangleC(0, 0, 319, 239, gColorFill);
    y += dY;
    Painter_DrawText(x, y, lang == Russian ? "ИНФОРМАЦИЯ" : "INFORMATION");
    y += dY;
    Painter_DrawText(x, y, lang == Russian ? "Модель : С8-54" : "Model : S8-54");
    y += dY;

    char buffer[100];
    OTP_GetSerialNumber(buffer);
    if (buffer[0])
    {
        Painter_DrawFormatText(x, y, lang == Russian ? "C/Н : %s" : "S/N : %s", buffer);
        y += dY;
    }

    Painter_DrawText(x, y, lang == Russian ? "Программное обеспечение:" : "Software:");
    y += dY;
    sprintf(buffer, (const char*)((lang == Russian) ? "версия %s" : "version %s"), NUM_VER);
    Painter_DrawText(x, y, buffer);
    y += dY;

    Painter_DrawFormText(x, y, gColorFill, "CRC32 : %X", Hardware_CalculateCRC32());

    dY = -10;
    Painter_DrawStringInCenterRect(0, 190 + dY, 320, 20, "Для получения помощи нажмите и удерживайте кнопку ПОМОЩЬ");
    Painter_DrawStringInCenterRect(0, 205 + dY, 320, 20, "Отдел маркетинга: тел./факс. 8-017-262-57-50");
    Painter_DrawStringInCenterRect(0, 220 + dY, 320, 20, "Разработчики: e-mail: mnipi-24(@)tut.by, тел. 8-017-262-57-51");

    Menu_Draw();
    Painter_EndScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Information(void)
{
    OpenPageAndSetItCurrent(Page_SB_ServiceInformation);
    Display_SetDrawMode(DrawMode_Auto, Information_Draw);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspInformation;
static const SButton sbExitInformation =
{
    Item_SmallButton, &mspInformation,
    COMMON_BEGIN_SB_EXIT,
    PressSB_Information_Exit,
    DrawSB_Exit
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspInformation =
{
    Item_Page, &pService, 0,
    {
        "ИНФОРМАЦИЯ", "INFORMATION",
        "Показывает информацию о приборе",
        "Displays information about the device"
    },
    Page_SB_ServiceInformation,
    {
        (void*)&sbExitInformation
    },
    OnPress_Information
};
