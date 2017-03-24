// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Settings.h"
#include "SettingsDebug.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float GetStretchADC(Channel ch)
{
    static const int16 *addStretch[RangeSize][2] =
    {
        {0, 0},  // 2mV
        {0, 0},  // 5mV
        {0, 0},  // 10mV
        {&setNR.addStretch20mV[A], &setNR.addStretch20mV[B]},   // 20mV
        {&setNR.addStretch50mV[A], &setNR.addStretch50mV[B]},   // 50mV
        {&setNR.addStretch100mV[A], &setNR.addStretch100mV[B]}, // 100mV
        {0, 0}, // 200mV
        {0, 0}, // 500mV
        {&setNR.addStretch20mV[A], &setNR.addStretch20mV[B]},   // 1V
        {&setNR.addStretch2V[A], &setNR.addStretch2V[B]},       // 2V
        {&setNR.addStretch100mV[A], &setNR.addStretch100mV[B]}  // 5V
    };
    
    const int16 *address = addStretch[SET_RANGE(ch)][ch];

    int16 stretch = setNR.stretchADC[ch][setNR.stretchADCtype];

    if (address)
    {
        stretch += (*address);
    }

    return stretch * 1e-4f + 1.0f;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SetStretchADC(Channel ch, float kStretch)
{
    setNR.stretchADC[ch][setNR.stretchADCtype] = (int16)((kStretch - 1.0f) * 1e4f);
}
