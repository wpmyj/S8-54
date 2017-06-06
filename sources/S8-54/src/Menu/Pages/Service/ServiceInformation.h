#pragma once
#include "Menu/Pages/Definition.h"
#include "Menu/MenuFunctions.h"
#include "Menu/MenuDrawing.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mpService;


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
    int x = 0;
    int dY = 30;
    int y = 0;
    int width = 320;
    Painter_DrawRectangleC(0, 0, 319, 239, gColorFill);
    Painter_DrawStringInCenterRectC(x, y + dY, width, 30, lang == Russian ? "ИНФОРМАЦИЯ" : "INFORMATION", gColorFill);
    Painter_DrawStringInCenterRect(x, y + 30 + dY, width, 30, lang == Russian ? "Модель     С8-54" : "Model     S8-54");
    char buffer[100];
    //sprintf(buffer, lang == Russian ? "c/н - %s" : "s/n - %s", SER_NUM);
    //Painter_DrawText(x, y, buffer);
    Painter_DrawStringInCenterRect(x, 60 + dY, width, 30, lang == Russian ? "Программное обеспечение:" : "Software:");
    sprintf(buffer, (const char*)((lang == Russian) ? "версия %s" : "version %s"), NUM_VER);
    Painter_DrawText(110, 90 + dY, buffer);
    Painter_DrawText(110, 105 + dY, "CRC32 A1C8760F");

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
static const SmallButton sbExitInformation =
{
    Item_SmallButton, &mspInformation,
    COMMON_BEGIN_SB_EXIT,
    PressSB_Information_Exit,
    DrawSB_Exit
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspInformation =
{
    Item_Page, &mpService, 0,
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
