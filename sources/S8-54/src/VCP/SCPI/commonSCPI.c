// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "commonSCPI.h"
#include "VCP/VCP.h"
#include "VCP/SCPI/SCPI.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Process_RUN(uint8 *buffer)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_STOP(uint8 *buffer)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_RESET(uint8 *buffer)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_AUTOSCALE(uint8 *buffer)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Process_REQUEST(uint8 *buffer)
{
    SCPI_SEND("S8-54");
}
