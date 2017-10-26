

#include "defines.h"
#include "Measures.h"
#include "GlobalFunctions.h"
#include "Math.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Display/Grid.h"
#include "Hardware/Sound.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    const char *name;
    const char UGO;
} StructMeasure;

static const StructMeasure measures[Measure_NumMeasures] =
{
    {"",            '\x00'},
    {"U макс",      '\x20'},
    {"U мин",       '\x25'},
    {"U пик",       '\x2a'},
    {"U макс уст",  '\x40'},
    {"U мин уст",   '\x45'},
    {"U ампл",      '\x4a'},
    {"U ср",        '\x60'},
    {"U скз",       '\x65'},
    {"Выброс+",     '\x6a'},
    {"Выброс-",     '\x80'},
    {"Период",      '\x85'},
    {"Частота",     '\x8a'},
    {"Вр нараст",   '\xa0'},
    {"Вр спада",    '\xa5'},
    {"Длит+",       '\xaa'},
    {"Длит-",       '\xc0'},
    {"Скважн+",     '\xc5'},
    {"Скважн-",     '\xca'},
    {"Задержка\xa7",'\xe0'},
    {"Задержка\xa6",'\xe5'},
    {"Фаза\xa7",    '\xe0'},
    {"Фаза\xa6",    '\xe5'}
};

extern int8 posActive;
extern bool pageChoiceIsActive;
extern int8 posOnPageChoice;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Measure_IsActive(int row, int col)
{
    if(posActive >= Measure_NumCols() * Measure_NumRows())
    {
        posActive = 0;
    }
    return (row * Measure_NumCols() + col) == posActive;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Measure_GetActive(int *row, int *col)
{
    *row = posActive / Measure_NumCols();
    *col = posActive - (*row) * Measure_NumCols();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Measure_SetActive(int row, int col)
{
    posActive = (int8)(row * Measure_NumCols() + col);
}

char  Measure_GetChar(Measure measure)
{
    return measures[measure].UGO;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Measure_GetDY(void)
{
    if(SOURCE_MEASURE_A_B && SET_ENABLED_A && SET_ENABLED_B)
    {
        return 30;
    }
    return 21;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Measure_GetDX(void)
{
    return GRID_WIDTH / 5; 
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *Measure_Name(int row, int col)
{
    return measures[MEASURE(row * Measure_NumCols() + col)].name;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
Measure Measure_Type(int row, int col)
{
    return MEASURE(row * Measure_NumCols() + col);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Measure_GetTopTable(void)
{
    if(NUM_MEASURES_6_1 || NUM_MEASURES_6_2)
    {
        return GRID_BOTTOM - Measure_GetDY() * 6;
    }
    return GRID_BOTTOM - Measure_NumRows() * Measure_GetDY();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Measure_NumCols(void)
{
    const int cols[] = {1, 2, 5, 5, 5, 1, 2};
    return cols[NUM_MEASURES];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Measure_NumRows(void)
{
    int rows[] = {1, 1, 1, 2, 3, 6, 6};
    return rows[NUM_MEASURES];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Measure_GetDeltaGridLeft(void)
{
    if(SHOW_MEASURES && MEAS_COMPRESS_GRID)
    {
        if(NUM_MEASURES_6_1)
        {
            return Measure_GetDX();
        }
        else if(NUM_MEASURES_6_2)
        {
            return Measure_GetDX() * 2;
        }
    }
    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Measure_GetDeltaGridBottom(void)
{
    if(SHOW_MEASURES && MEAS_COMPRESS_GRID)
    {
        if(NUM_MEASURES_1_5)
        {
            return Measure_GetDY();
        }
        else if(NUM_MEASURES_2_5)
        {
            return Measure_GetDY() * 2;
        }
        else if(NUM_MEASURES_3_5)
        {
            return Measure_GetDY() * 3;
        }
    }
    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Measure_ShortPressOnSmallButonMarker(void)
{
    if(MEASURE(posActive) == MARKED_MEAS)
    {
        MARKED_MEAS = Measure_None;
    }
    else
    {
        MARKED_MEAS = MEASURE(posActive);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Measure_DrawPageChoice(void)
{
    if(!pageChoiceIsActive)
    {
        return;
    }
    int x = (NUM_MEASURES_6_1 || NUM_MEASURES_6_2) ? (GridRight() - 3 * GRID_WIDTH / 5) : GridLeft();
    int y = GRID_TOP;
    int dX = GRID_WIDTH / 5;
    int dY = 22;
    int maxRow = (NUM_MEASURES_6_1 || NUM_MEASURES_6_2) ? 8 : 5;
    int maxCol = (NUM_MEASURES_6_1 || NUM_MEASURES_6_2) ? 3 : 5;
    Measure meas = Measure_None;
    Painter_SetFont(TypeFont_UGO);
    for(int row = 0; row < maxRow; row++)
    {
        for(int col = 0; col < maxCol; col++)
        {
            if(meas >= Measure_NumMeasures)
            {
                break;
            }
            int x0 = x + col * dX;
            int y0 = y + row * dY;
            bool active = meas == posOnPageChoice;
            Painter_DrawRectangleC(x0, y0, dX, dY, COLOR_WHITE);
            Painter_FillRegionC(x0 + 1, y0 + 1, dX - 2, dY - 2, active ? COLOR_FLASH_10 : gColorBack);
            Painter_SetColor(active ? COLOR_FLASH_01 : gColorFill);
            Painter_Draw10SymbolsInRect(x0 + 2, y0 + 1, Measure_GetChar(meas));
            if(meas < Measure_NumMeasures)
            {
                Painter_SetFont(TypeFont_5);
                Painter_DrawTextRelativelyRightC(x0 + dX, y0 + 12, measures[meas].name, active ? COLOR_FLASH_01 : gColorFill);
                Painter_SetFont(TypeFont_UGO);
            }
            meas++;
        }
    }
    Painter_SetFont(TypeFont_8);
}
