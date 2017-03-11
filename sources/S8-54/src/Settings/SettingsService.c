// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Settings.h"
#include "SettingsService.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float sService_MathGetFFTmaxDBabs(void)
{
    static const float db[] = {-40.0f, -60.0f, -80.0f};
    return db[MAX_DB_FFT];
}
