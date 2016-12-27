#include "Settings.h"
#include "Display/Grid.h"
#include "SettingsCursors.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Menu/MenuFunctions.h"
#include "FPGA/FPGA_Types.h"


#include <math.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float sCursors_GetCursPosU(Channel ch, int numCur)
{
    return set.cursors.posCurU[ch][numCur] / (GridChannelBottom() == GridFullBottom() ? 1.0f : 2.0f);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool sCursors_NecessaryDrawCursors(void)
{
    return ((set.cursors.cntrlU[set.cursors.source] != CursCntrl_Disable) || (set.cursors.cntrlT[set.cursors.source] != CursCntrl_Disable)) && (set.cursors.showCursors || GetNameOpenedPage() == Page_SB_Curs);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const char* sCursors_GetCursVoltage(Channel source, int numCur, char buffer[20])
{
    float voltage = Math_VoltageCursor(sCursors_GetCursPosU(source, numCur), RANGE(source), RSHIFT(source));
    if (DIVIDER_10(source))
    {
        voltage *= 10.0f;
    }
    return Voltage2String(voltage, true, buffer);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const char* sCursors_GetCursorTime(Channel source, int numCur, char buffer[20])
{
    float time = Math_TimeCursor(set.cursors.posCurT[source][numCur], TBASE);
        
    return Time2String(time, true, buffer);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const char* sCursors_GetCursorPercentsU(Channel source, char buffer[20])
{
    buffer[0] = 0;
    float dPerc = set.cursors.deltaU100percents[source];
    float dValue = fabsf(sCursors_GetCursPosU(source, 0) - sCursors_GetCursPosU(source, 1));
    char bufferOut[20];
    char* percents = Float2String(dValue / dPerc * 100.0f, false, 5, bufferOut);
    strcat(buffer, percents);
    strcat(buffer, "%");
    return buffer;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
const char* sCursors_GetCursorPercentsT(Channel source, char buffer[20])
{
    buffer[0] = 0;
    float dPerc = set.cursors.deltaT100percents[source];
    float dValue = fabsf(set.cursors.posCurT[source][0] - set.cursors.posCurT[source][1]);
    char bufferOut[20];
    char* percents = Float2String(dValue / dPerc * 100.0f, false, 6, bufferOut);
    strcat(buffer, percents);
    strcat(buffer, "%");
    return buffer;
}
