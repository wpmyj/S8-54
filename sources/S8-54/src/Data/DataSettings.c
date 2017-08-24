// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "DataSettings.h"
#include "Globals.h"
#include "Hardware/FLASH.h"
#include "Settings/SettingsMemory.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int gAddNStop = 0;
void *extraMEM = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int BytesInChannel(DataSettings *ds)
{
    static const int numPoints[FPGA_ENUM_POINTS_SIZE][3] =
    {
        {512,   1024,  1024},
        {1024,  2048,  2048},
        {2048,  4096,  4096},
        {4096,  8192,  8192},
        {8192,  16384, 16384},
        {16384, 32768, 16384},
        {32768, 32768, 32768}
    };

    return numPoints[FPGA_ENUM_POINTS][PEAKDET(ds)];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool DataSettings_IsEquals(const DataSettings *ds1, const DataSettings *ds2)
{
    /** @todo оптимизировать функцию сравнени€ */
    bool equals = (ENABLED_A(ds1) == ENABLED_A(ds2)) &&
        (ENABLED_B(ds1) == ENABLED_B(ds2)) &&
        (INVERSE_A(ds1) == INVERSE_A(ds2)) &&
        (INVERSE_B(ds1) == INVERSE_B(ds2)) &&
        (RANGE_A(ds1) == RANGE_A(ds2)) &&
        (RANGE_B(ds1) == RANGE_B(ds2)) &&
        (RSHIFT_A(ds1) == RSHIFT_A(ds2)) &&
        (RSHIFT_B(ds1) == RSHIFT_B(ds2)) &&
        (TBASE(ds1) == TBASE(ds2)) &&
        (TSHIFT(ds1) == TSHIFT(ds2)) &&
        (COUPLE_A(ds1) == COUPLE_A(ds2)) &&
        (COUPLE_B(ds1) == COUPLE_B(ds2)) &&
        (TRIGLEV_A(ds1) == TRIGLEV_A(ds2)) &&
        (TRIGLEV_B(ds1) == TRIGLEV_B(ds2)) &&
        (DIVIDER_A(ds1) == DIVIDER_A(ds2)) &&
        (DIVIDER_B(ds1) == DIVIDER_B(ds2)) &&
        (PEAKDET(ds1) == PEAKDET(ds2));

    return equals;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DataSettings_Fill(DataSettings *ds)
{
    Lval_ENABLED_A(ds) = SET_ENABLED_A ? 1 : 0;
    Lval_ENABLED_B(ds) = SET_ENABLED_B ? 1 : 0;
    INVERSE_A(ds) = SET_INVERSE_A ? 1 : 0;
    INVERSE_B(ds) = SET_INVERSE_B ? 1 : 0;
    Lval_RANGE_A(ds) = SET_RANGE_A;
    Lval_RANGE_B(ds) = SET_RANGE_B;
    RSHIFT_A(ds) = SET_RSHIFT_A;
    RSHIFT_B(ds) = SET_RSHIFT_B;
    Lval_TBASE(ds) = SET_TBASE;
    TSHIFT(ds) = SET_TSHIFT;
    Lval_COUPLE_A(ds) = SET_COUPLE_A;
    Lval_COUPLE_B(ds) = SET_COUPLE_B;
    TRIGLEV_A(ds) = SET_TRIGLEV_A;
    TRIGLEV_B(ds) = SET_TRIGLEV_A;
    Lval_PEAKDET(ds) = SET_PEAKDET;
    Lval_DIVIDER_A(ds) = SET_DIVIDER_A;
    Lval_DIVIDER_B(ds) = SET_DIVIDER_B;
    TIME_MS(ds) = 0;                        // Ёто важно дл€ режима поточеного вывода. ќзначает, что полный сигнал ещЄ не считан
    ENUM_POINTS(ds) = FPGA_ENUM_POINTS;
}
