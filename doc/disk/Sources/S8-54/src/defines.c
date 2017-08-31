// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HardwareErrorHandler(const char *file, const char *function, int line)
{
    volatile bool run = true;
    while(run)
    {
    }; 
};
