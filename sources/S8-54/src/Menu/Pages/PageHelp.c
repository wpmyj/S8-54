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
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 2, '\x4a');
    painter.SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawSB_Help_ParagraphLeave(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, '\x48');
    painter.SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawSB_Help_ParagraphPrev(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 5, '\x4c');
    painter.SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawSB_Help_ParagraphNext(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 5, '\x4e');
    painter.SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnHelpRegSet(int angle) 
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbHelpParagraphEnter
(
    "Открыть",  "Open",
    "Открывает раздел справки",
    "Opens the section of the reference",
    &pHelp, HelpContent_EnterParagraph, DrawSB_Help_ParagraphEnter, HelpContent_EnterParagraphIsActive
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbHelpParagraphLeave
(
     "Закрыть", "Close",
     "Закрывает раздел справки",
     "Closes the section of the reference",
    &pHelp, HelpContent_LeaveParagraph, DrawSB_Help_ParagraphLeave, HelpContent_LeaveParagraphIsActive
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbHelpParagraphPrev
(
    "Предыдущий раздел", "Previous section",
    "Выбрать предыдущий раздел справки",
    "To choose the previous section of the reference",
    &pHelp, HelpContent_PrevParagraph, DrawSB_Help_ParagraphPrev
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbHelpParagraphNext
(
    "", "",
    "", "",
    &pHelp, HelpContent_NextParagraph, DrawSB_Help_ParagraphNext
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SButton sbExitHelp
(
    COMMON_BEGIN_SB_EXIT,
    &pHelp, OnPressSB_Exit, DrawSB_Exit
);

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
        (void *)&sbExitHelp,
        (void *)&sbHelpParagraphEnter,
        (void *)&sbHelpParagraphLeave,
        (void *)0,
        (void *)&sbHelpParagraphPrev,
        (void *)&sbHelpParagraphNext
    },  
    true, 0, HelpContent_Draw, OnHelpRegSet
};
