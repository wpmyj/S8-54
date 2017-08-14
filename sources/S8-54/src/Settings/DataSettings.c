// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "DataSettings.h"
#include "Globals.h"
#include "Log.h"
#include "Hardware/FLASH.h"
#include "Settings/SettingsMemory.h"

DataSettings gDatas[NUM_DATAS];
uint8 gDataAve[NumChannels][FPGA_MAX_POINTS];
int gAddNStop = 0;
void *extraMEM = 0;
StateOSCI gState = StateOSCI_Start;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NumBytesInChannel_(DataSettings *ds, bool forCalculate)
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

    if(FPGA_ENUM_POINTS >= FNP_1k && forCalculate)
    {
        return FPGA_MAX_POINTS_FOR_CALCULATE;
    }



    return numPoints[FPGA_ENUM_POINTS][PEACKDET(ds)];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *AddressChannel(DataSettings *ds, Channel ch)
{
    if (ch == A && ENABLED_A(ds))
    {
        return ADDRESS_DATA(ds);
    }

    if (ch == B && ENABLED_B(ds))
    {
        return ADDRESS_DATA(ds) + (ENABLED_A(ds) ? BYTES_IN_CHANNEL(ds) : 0);
    }

    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool GetInverse(Channel ch)
{
    if (gState == StateOSCI_Start)
    {

    }
    else if (gState == StateOSCI_DrawLoPart)
    {
        if (MODE_WORK_DIR || (MODE_WORK_ROM && SHOW_IN_INT_DIRECT))
        {
            return SET_INVERSE(ch);
        }
        else
        {
            //return G_INVERSE(ds)
        }
    }

    return false;
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
        (PEACKDET(ds1) == PEACKDET(ds2));

    return equals;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DataSettings_Log(DataSettings *ds)
{
    LOG_WRITE("%d %d %d %d %d %d %d %d %d %d", ENABLED_A(ds), ENABLED_B(ds), INVERSE_A(ds), INVERSE_B(ds), RANGE_A(ds),
              RANGE_B(ds), RSHIFT_A(ds), RSHIFT_B(ds), TBASE(ds), TSHIFT(ds));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DataSettings_Fill(DataSettings *ds)
{
    Lval_ENABLED_A(ds) = sChannel_Enabled(A) ? 1 : 0;
    Lval_ENABLED_B(ds) = sChannel_Enabled(B) ? 1 : 0;
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
    Lval_PEACKDET(ds) = SET_PEACKDET;
    Lval_DIVIDER_A(ds) = SET_DIVIDER_A;
    Lval_DIVIDER_B(ds) = SET_DIVIDER_B;
    TIME_MS(ds) = 0;                        // Ёто важно дл€ режима поточеного вывода. ќзначает, что полный сигнал ещЄ не считан
    ENUM_POINTS(ds) = FPGA_ENUM_POINTS;
}
