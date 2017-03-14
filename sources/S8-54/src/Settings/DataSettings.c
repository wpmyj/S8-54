// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "DataSettings.h"
#include "Globals.h"
#include "Settings/SettingsMemory.h"


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
        return ds->addrData;
    }

    if (ch == B && ds->enableB)
    {
        return ds->enableA ? (ds->addrData + NumBytesInChannel(ds)) : ds->addrData;
    }

    return 0;
}

DataSettings gDatas[NUM_DATAS];
uint8 gDataAve[NumChannels][FPGA_MAX_POINTS];

int gAddNStop = 0;

void *extraMEM = 0;
