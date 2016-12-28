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
        {&set.nr.addStretch20mV[A], &set.nr.addStretch20mV[B]},   // 20mV
        {&set.nr.addStretch50mV[A], &set.nr.addStretch50mV[B]},   // 50mV
        {&set.nr.addStretch100mV[A], &set.nr.addStretch100mV[B]}, // 100mV
        {0, 0}, // 200mV
        {0, 0}, // 500mV
        {&set.nr.addStretch20mV[A], &set.nr.addStretch20mV[B]},   // 1V
        {&set.nr.addStretch2V[A], &set.nr.addStretch2V[B]},       // 2V
        {&set.nr.addStretch100mV[A], &set.nr.addStretch100mV[B]}  // 5V
    };
    
    const int16 *address = addStretch[RANGE(ch)][ch];

    int16 stretch = set.nr.stretchADC[ch][set.nr.stretchADCtype];

    if (address)
    {
        stretch += (*address);
    }

    return stretch * 1e-4f + 1.0f;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SetStretchADC(Channel ch, float kStretch)
{
    set.nr.stretchADC[ch][set.nr.stretchADCtype] = (int16)((kStretch - 1.0f) * 1e4f);
}
