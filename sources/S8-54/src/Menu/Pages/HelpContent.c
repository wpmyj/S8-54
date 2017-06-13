// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Display/Painter.h"
#include "Display/Grid.h"
#include "Display/font/Font.h"
#include "HelpContentPages.h"
#include "HelpContent.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int currentParagraph = 0;   // Если TypePage(currentPage) == TypePage_Content, то указывает не текущий раздел оглавления
const PageHelp *currentPage = &helpMain;


static const int WIDTH = 295;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawPageContent(void)
{
    int y = 50;
    Painter_DrawStringInCenterRect(0, y, WIDTH, 10, currentPage->titleHint[LANG]);

    int numPage = 0;

    y += 40;

    while(currentPage->pages[numPage])
    {
        Page *page = (Page*)currentPage->pages[numPage];
        const char *title = page->titleHint[LANG];
        if(currentParagraph == numPage)
        {
            Painter_DrawStringInCenterRectOnBackgroundC(0, y, WIDTH, 10, title, gColorBack, 2, gColorFill);
        }
        else
        {
            Painter_DrawStringInCenterRectC(0, y, WIDTH, 10, title, gColorFill);
        }
        y += 16;
        numPage++;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawPageDescription(void)
{
    Painter_DrawStringInCenterRect(0, 3, WIDTH, 10, currentPage->titleHint[LANG]);
    Painter_DrawTextInRectWithTransfers(2, 15, WIDTH - 5, 240, currentPage->titleHint[2 + LANG]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HelpContent_Draw(void)
{
    Painter_FillRegionC(GridRight(), 0, 319 - GridRight(), 20, gColorBack);
    Painter_FillRegion(GridRight(), 219, 319 - GridRight(), 21);
    Painter_FillRegion(1, 1, WIDTH, 237);
    Painter_DrawRectangleC(1, 0, WIDTH + 1, 239, gColorFill);   /** \todo Здесь непонятно, почему так. Координаты верхнего левого угла должны быть 
                                                                0, 0, но в таком случае левой вертикальной полосы не видно */

    /*
    uint16 *addr1 = (uint16*)(0x08000000 + (rand() % 65535));
    uint8 *addr2 = (uint8*)(0x08000000 + (rand() % 65535));

    Painter_SetColor(COLOR_FILL);
    for (int i = 0; i < 10000; i++)
    {
        Painter_SetPoint((*addr1) % WIDTH, Math_LimitationInt(*addr2, 0, 239));
        addr1++;
        addr2++;
    }
    */

    if(currentPage->type == TypePage_Content)
    {
        DrawPageContent();
    }
    else if(currentPage->type == TypePage_Description)
    {
        DrawPageDescription();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int NumParagraphs(const PageHelp *page)
{
    int retValue = 0;
    while(page->pages[retValue])
    {
        retValue++;
    }
    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HelpContent_NextParagraph(void)
{
    if(currentPage->type == TypePage_Content)
    {
        CircleIncreaseInt(&currentParagraph, 0, NumParagraphs(currentPage) - 1);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HelpContent_PrevParagraph(void)
{
    if(currentPage->type == TypePage_Content)
    {
        CircleDecreaseInt(&currentParagraph, 0, NumParagraphs(currentPage) - 1);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HelpContent_EnterParagraph(void)
{
    if(currentPage->type == TypePage_Content)
    {
        currentPage = currentPage->pages[currentParagraph];
    }
    currentParagraph = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HelpContent_LeaveParagraph(void)
{
    currentParagraph = 0;
    if(currentPage->parent)
    {
        currentPage = currentPage->parent;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool HelpContent_LeaveParagraphIsActive(void)
{
    return currentPage->parent != 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool HelpContent_EnterParagraphIsActive(void)
{
    return currentPage->type == TypePage_Content;
}
