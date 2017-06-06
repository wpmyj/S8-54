// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "FPGA/FPGA.h"
#include "Menu/MenuFunctions.h"
#include "Memory/PageMemory.h"
#include "PageHelp.h"
#include "Panel/Panel.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Choice mcCursorsSource;
extern Choice mcCursorsU;
extern Choice mcCursorsT;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CalculateConditions(int16 pos0, int16 pos1, CursCntrl cursCntrl, bool *cond0, bool *cond1)
{
    bool zeroLessFirst = pos0 < pos1;
    *cond0 = cursCntrl == CursCntrl_1_2 || (cursCntrl == CursCntrl_1 && zeroLessFirst) || (cursCntrl == CursCntrl_2 && !zeroLessFirst);
    *cond1 = cursCntrl == CursCntrl_1_2 || (cursCntrl == CursCntrl_1 && !zeroLessFirst) || (cursCntrl == CursCntrl_2 && zeroLessFirst);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
int CalculateYforCurs(int y, bool top)
{
    return top ? y + MI_HEIGHT / 2 + 4 : y + MI_HEIGHT - 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
int CalculateXforCurs(int x, bool left)
{
    return left ? x + MI_WIDTH - 20 : x + MI_WIDTH - 5;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
void CalculateXY(int *x0, int *x1, int *y0, int *y1)
{
    *x0 = CalculateXforCurs(*x0, true);
    *x1 = CalculateXforCurs(*x1, false);
    *y0 = CalculateYforCurs(*y0, true);
    *y1 = CalculateYforCurs(*y1, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawMenuCursTime(int x, int y, bool left, bool right)
{
    x -= 58;
    y -= 16;
    int x0 = x, x1 = x, y0 = y, y1 = y;
    CalculateXY(&x0, &x1, &y0, &y1);
    for(int i = 0; i < (left ? 3 : 1); i++)
    {
        Painter_DrawVLine(x0 + i, y0, y1);
    }
    for(int i = 0; i < (right ? 3 : 1); i++)
    {
        Painter_DrawVLine(x1 - i, y0, y1);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawMenuCursVoltage(int x, int y, bool top, bool bottom)
{
    x -= 65;
    y -= 21;
    int x0 = x, x1 = x, y0 = y, y1 = y;
    CalculateXY(&x0, &x1, &y0, &y1);
    for(int i = 0; i < (top ? 3 : 1); i++)
    {
        Painter_DrawHLine(y0 + i, x0, x1);
    }
    for(int i = 0; i < (bottom ? 3 : 1); i++)
    {
        Painter_DrawHLine(y1 - i, x0, x1);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
/*
void PressSmallButtonExit()
{
    NamePage namePage = GetNameOpenedPage();
    if (namePage == Page_NoPage)
    {
        return;
    }
    if (namePage == Page_SB_MemInt)
    {   // Для режимов работы с памятью выход из режима малых кнопок означает возвращение в режим нормальных измерений
        set.memory.modeWork = ModeWork_Direct;
        if (gMemory.runningFPGAbeforeSmallButtons == 1)
        {
            FPGA_Start();
            gMemory.runningFPGAbeforeSmallButtons = 0;
        }
    }

    Display_RemoveAddDrawFunction();
    gBF.ledRegSetNeedEnabled = 0;
}
*/

extern const Page mainPage;


#include "PageHelp.c"


extern const Page mpDisplay;
extern const Page mpCursors;
extern const Page mpChanA;
extern const Page mpChanB;
extern const Page mpMemory;
extern const Page mpMeasures;
extern const Page mpDebug;
extern const Page mpService;
extern const Page mpTime;
extern const Page pTrig;

/// МЕНЮ 
const Page mainPage =
{
    Item_Page, 0, 0,
    {
        "МЕНЮ", "MENU",
        "", ""
    },
    Page_MainPage,
    {
        (void*)&mpDisplay,
        (void*)&mpChanA,
        (void*)&mpChanB,
        (void*)&pTrig,
        (void*)&mpTime,
        (void*)&mpCursors,
        (void*)&mpMemory,
        (void*)&mpMeasures,
        (void*)&mpService,
        (void*)&mpHelp,
        (void*)&mpDebug
    }
};

//------------------------------------------------------------------------------------------------------------------------------------------------------
const void *PageForButton(PanelButton button)
{
    static const void *pages[] = {  
        0,                  // B_Empty
        (void*)&mpChanA,    // B_Channel1
        (void*)&mpService,  // B_Service
        (void*)&mpChanB,    // B_Channel2
        (void*)&mpDisplay,  // B_Display
        (void*)&mpTime,     // B_Time
        (void*)&mpMemory,   // B_Memory
        (void*)&pTrig,     // B_Trig
        0,                  // B_Start
        (void*)&mpCursors,  // B_Cursors
        (void*)&mpMeasures, // B_Measures
        0,                  // B_Power
        (void*)&mpHelp,     // B_Help
        0,                  // B_Menu
        0,                  // B_F1
        0,                  // B_F2
        0,                  // B_F3
        0,                  // B_F4
        0,                  // B_F5
    };

    return pages[button];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsMainPage(const void *item)
{
    return item == &mainPage;
}

void DrawSB_Exit(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x2e');
    Painter_SetFont(TypeFont_8);
}
