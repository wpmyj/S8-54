// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Settings/Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mainPage;


#include "PageHelp.c"


extern const Page pDisplay;
extern const Page pCursors;
extern const Page pChanA;
extern const Page pChanB;
extern const Page pMemory;
extern const Page pMeasures;
extern const Page pDebug;
extern const Page pService;
extern const Page pTime;
extern const Page pTrig;

/// лемч 
const Page mainPage =
{
    Item_Page, 0, 0,
    {
        "лемч", "MENU",
        "", ""
    },
    Page_MainPage,
    {
        (void*)&pDisplay,
        (void*)&pChanA,
        (void*)&pChanB,
        (void*)&pTrig,
        (void*)&pTime,
        (void*)&pCursors,
        (void*)&pMemory,
        (void*)&pMeasures,
        (void*)&pService,
        (void*)&pHelp,
        (void*)&pDebug
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const void *PageForButton(PanelButton button)
{
    static const void *pages[] = {  
        0,                  // B_Empty
        (void*)&pChanA,     // B_Channel1
        (void*)&pService,   // B_Service
        (void*)&pChanB,     // B_Channel2
        (void*)&pDisplay,   // B_Display
        (void*)&pTime,      // B_Time
        (void*)&pMemory,    // B_Memory
        (void*)&pTrig,      // B_Trig
        0,                  // B_Start
        (void*)&pCursors,   // B_Cursors
        (void*)&pMeasures,  // B_Measures
        0,                  // B_Power
        (void*)&pHelp,      // B_Help
        0,                  // B_Menu
        0,                  // B_F1
        0,                  // B_F2
        0,                  // B_F3
        0,                  // B_F4
        0,                  // B_F5
    };

    return pages[button];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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
