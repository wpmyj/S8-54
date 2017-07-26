#include "defines.h"
#include "Utils/GlobalFunctions.h"
#include "Globals.h"
#include "Menu/MenuItems.h"
#include "Definition.h"
#include "HelpContent.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page pHelp;
extern const Page mainPage;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawSB_Help_ParagraphEnter(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 2, '\x4a');
    Painter_SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawSB_Help_ParagraphLeave(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x48');
    Painter_SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawSB_Help_ParagraphPrev(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 5, '\x4c');
    Painter_SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawSB_Help_ParagraphNext(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 5, '\x4e');
    Painter_SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnHelpRegSet(int angle) 
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbHelpParagraphEnter =
{
    Item_SmallButton, &pHelp, HelpContent_EnterParagraphIsActive,
    {
        "Открыть",  "Open",
        "Открывает раздел справки",
        "Opens the section of the reference"
    },
    HelpContent_EnterParagraph,
    DrawSB_Help_ParagraphEnter
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbHelpParagraphLeave =
{
    Item_SmallButton, &pHelp, HelpContent_LeaveParagraphIsActive,
    {
        "Закрыть", "Close",
        "Закрывает раздел справки",
        "Closes the section of the reference"
    },
    HelpContent_LeaveParagraph,
    DrawSB_Help_ParagraphLeave
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbHelpParagraphPrev =
{
    Item_SmallButton, &pHelp, 0,
    {
        "Предыдущий раздел", "Previous section",
        "Выбрать предыдущий раздел справки",
        "To choose the previous section of the reference"
    },
    HelpContent_PrevParagraph,
    DrawSB_Help_ParagraphPrev
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbHelpParagraphNext =
{
    Item_SmallButton, &pHelp, 0,
    {
        "", "",
        "", ""
    },
    HelpContent_NextParagraph,
    DrawSB_Help_ParagraphNext
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbExitHelp =
{
    Item_SmallButton, &pHelp,
    COMMON_BEGIN_SB_EXIT,
    OnPressSB_Exit,
    DrawSB_Exit
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const Page pHelp =
{
    Item_Page, &mainPage, 0,
    {
        "ПОМОЩЬ", "HELP",
        "Открыть разделы помощи",
        "To open sections of the help"
    },
    PageSB_Help, 
    {
        (void*)&sbExitHelp,
        (void*)&sbHelpParagraphEnter,
        (void*)&sbHelpParagraphLeave,
        (void*)0,
        (void*)&sbHelpParagraphPrev,
        (void*)&sbHelpParagraphNext
    },  
    true, 0, HelpContent_Draw, OnHelpRegSet
};
