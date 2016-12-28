#pragma once


#define CONSOLE_NUM_STRINGS     (set.debug.numStrings)
#define CONSOLE_SIZE_FONT       (set.debug.sizeFont ? 8 : 5)
#define CONSOLE_IN_PAUSE        (set.debug.consoleInPause)
#define NUM_MEASURES_FOR_GATES  (set.debug.numMeasuresForGates)
#define TIME_COMPENSATION       (set.debug.timeCompensation)
#define SHOW_RAND_INFO          (set.debug.showRandInfo)


float   GetStretchADC(Channel ch);
void    SetStretchADC(Channel ch, float kStretch);
