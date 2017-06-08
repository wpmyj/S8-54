// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "SettingsMemory.h"
#include "FPGA/FPGAtypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int sMemory_NumBytesInChannel(bool forCalculate)
{
    static const int numPoints[FPGA_NUM_POINTS_SIZE][3] =
    {
        {512,   1024,  1024},
        {1024,  2048,  2048},
        {2048,  4096,  4096},
        {4096,  8192,  8192},
        {8192,  16384, 16384},
        {16384, 32768, 16384},
        {32768, 32768, 32768}
    };

    if (FPGA_NUM_POINTS >= FNP_1k && forCalculate)
    {
        return FPGA_MAX_POINTS_FOR_CALCULATE;
    }

    return numPoints[FPGA_NUM_POINTS][SET_PEACKDET];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int sMemory_NumPointsInChannel(void)
{
    static const int numPoints[FPGA_NUM_POINTS_SIZE] =
    {
        512,
        1024,
        2048,
        4096,
        8192,
        16384,
        32768
    };

    return numPoints[FPGA_NUM_POINTS];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
ENumPoinstFPGA NumPoints_2_FPGA_NUM_POINTS(int numPoints)
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
int FPGA_NUM_POINTS_2_NumPoints(ENumPoinstFPGA numPoints)
{
    const int n[FPGA_NUM_POINTS_SIZE] =
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
