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
        {&NRST_ADD_STRETCH_20mV_A, &NRST_ADD_STRETCH_20mV_B},   // 20mV
        {&NRST_ADD_STRETCH_50mV_A, &NRST_ADD_STRETCH_50mV_B},   // 50mV
        {&NRST_ADD_STRETCH_100mV_A, &NRST_ADD_STRETCH_100mV_B}, // 100mV
        {0, 0}, // 200mV
        {0, 0}, // 500mV
        {&NRST_ADD_STRETCH_20mV_A, &NRST_ADD_STRETCH_20mV_B},   // 1V
        {&NRST_ADD_STRETCH_2V_A, &NRST_ADD_STRETCH_2V_B},       // 2V
        {&NRST_ADD_STRETCH_100mV_A, &NRST_ADD_STRETCH_100mV_B}  // 5V
    };
    
    const int16 *address = addStretch[SET_RANGE(ch)][ch];

    int16 stretch = NRST_STRETCH_ADC(ch, NRST_STRETCH_ADC_TYPE);

    if (address)
    {
        stretch += (*address);
    }

    return stretch * 1e-4f + 1.0f;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SetStretchADC(Channel ch, float kStretch)
{
    NRST_STRETCH_ADC(ch, NRST_STRETCH_ADC_TYPE) = (int16)((kStretch - 1.0f) * 1e4f);
}
