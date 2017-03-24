// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _INCLUDE_DATA_
#include "Data.h"
#undef _INCLUDE_DATA_
#include "Globals.h"
#include "Hardware/FLASH.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DataSettings *pDSCur;
static DataSettings *pDSInt;
static DataSettings *pDSLast;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Clear(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_GetFromIntMemory(void)
{
    FLASH_GetData(gMemory.currentNumIntSignal, &pDSInt, &dataChanInt[A], &dataChanInt[B]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_GetAverageFromDataStorage(void)
{
    dataChan[A] = DS_GetAverageData(A);
    dataChan[B] = DS_GetAverageData(B);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Data_Load(void)
{
    Clear();

    if (DS_NumElementsInStorage() == 0)
    {
        return;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Clear(void)
{
    pDS = 0;
    pDSCur = 0;
    pDSInt = 0;
    pDSLast = 0;
    dataChan[A] = dataChan[B] = 0;
    dataChanLast[A] = dataChanLast[B] = 0;
    dataChanInt[A] = dataChanInt[B] = 0;
}
