// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "DataSettings.h"
#include "Globals.h"
#include "Hardware/FLASH.h"
#include "Settings/SettingsMemory.h"

DataSettings gDatas[NUM_DATAS];
uint8 gDataAve[NumChannels][FPGA_MAX_POINTS];
int gAddNStop = 0;
void *extraMEM = 0;
StateOSCI gState = StateOSCI_Start;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NumBytesInChannel(const DataSettings *ds)
{
    return FPGA_NUM_POINTS_2_NumPoints((NumPoinstFPGA)ds->indexLength);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int NumBytesInData(const DataSettings *ds)
{
    return NumBytesInChannel(ds) * (ds->enableA + ds->enableB);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int NumPointsInChannel(const DataSettings *ds)
{
    if (ds->peackDet == 0)
    {
        return NumBytesInChannel(ds);
    }

    return NumBytesInChannel(ds) * 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *AddressChannel(DataSettings *ds, Channel ch)
{
    if (ch == A && ds->enableA)
    {
        return ADDRESS_DATA(ds);
    }

    if (ch == B && ds->enableB)
    {
        return ADDRESS_DATA(ds) + (ds->enableA ? NumBytesInChannel(ds) : 0);
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
        if (WORK_DIRECT || (WORK_INT && SHOW_DIRECT_IN_MEM_INT))
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
