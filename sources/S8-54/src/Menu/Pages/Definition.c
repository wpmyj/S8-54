#include "defines.h"
#include "Settings/Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page pDisplay;
/*
extern const Page pChanA;
extern const Page pChanB;
extern const Page pTrig;
extern const Page pTime;
extern const Page pCursors;
extern const Page pMemory;
extern const Page pMeasures;
extern const Page pService;
extern const Page pHelp;
extern const Page pDebug;
*/


/// Ã≈Õﬁ 
Page mainPage =
{
    Item_Page, 0, 0,
    {
        "Ã≈Õﬁ", "MENU",
        "", ""
    },
    Page_Main,
    {
        (void *)&pDisplay,  // ƒ»—œÀ≈…
        /*/
        (void *)&pChanA,    //  ¿Õ¿À 1
        (void *)&pChanB,    //  ¿Õ¿À 2
        (void *)&pTrig,     // —»Õ’–
        (void *)&pTime,     // –¿«¬≈–“ ¿
        (void *)&pCursors,  //  ”–—Œ–€
        (void *)&pMemory,   // œ¿Ãﬂ“‹
        (void *)&pMeasures, // »«Ã≈–≈Õ»ﬂ
        (void *)&pService,  // —≈–¬»—
        (void *)&pHelp,     // œŒÃŒŸ‹
        (void *)&pDebug     // Œ“À¿ƒ ¿
        */
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const void *PageForButton(PanelButton button)
{
    static const void *pages[] =
    {  
        0,                  // B_Empty
        /*
        (void *)&pChanA,    // B_Channel1
        (void *)&pService,  // B_Service
        (void *)&pChanB,    // B_Channel2
        */
        0,
        0,
        0,
        (void *)&pDisplay,  // B_Display
        /*
        (void *)&pTime,     // B_Time
        (void *)&pMemory,   // B_Memory
        (void *)&pTrig,     // B_Trig
        0,                  // B_Start
        (void *)&pCursors,  // B_Cursors
        (void *)&pMeasures, // B_Measures
        0,                  // B_Power
        (void *)&pHelp,     // B_Help
        0,                  // B_Menu
        0,                  // B_F1
        0,                  // B_F2
        0,                  // B_F3
        0,                  // B_F4
        0,                  // B_F5
        */
    };

    return pages[button];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool IsMainPage(const void *item)
{
    return item == &mainPage;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawSB_Exit(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x2e');
    Painter_SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnPressSB_Exit(void)
{
    Display_RemoveAddDrawFunction();
}
