// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "SettingsMemory.h"
#include "Data/Data.h"
#include "FPGA/FPGAtypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int sMemory_NumPointsInChannel_(void)
{
    static const int numPoints[FPGA_ENUM_POINTS_SIZE] =
    {
        512,
        1024,
        2048,
        4096,
        8192,
        16384,
        32768
    };

    return numPoints[FPGA_ENUM_POINTS];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int sMemory_NumBytesInChannel_(void)
{
    DataSettings ds;
    DataSettings_Fill(&ds);
    return BytesInChannel(&ds);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
ENumPointsFPGA NumPoints_2_ENumPoints(int numPoints)
{
    if (numPoints == 32768)      { return FNP_32k; }
    else if (numPoints == 16384) { return FNP_16k; }
    else if (numPoints == 8192)  { return FNP_8k; }
    else if (numPoints == 4096)  { return FNP_4k; }
    else if (numPoints == 2048)  { return FNP_2k; }
    else if (numPoints == 1024)  { return FNP_1k; }
    return FNP_512;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int ENumPoints_2_NumPoints(ENumPointsFPGA numPoints)
{
    static const int n[FPGA_ENUM_POINTS_SIZE] =
    {
        512,
        1024,
        2048,
        4096,
        8192,
        16384,
        32768
    };
    return n[(uint)numPoints];
}
