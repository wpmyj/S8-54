#pragma once


#include "Settings.h"


#define MODE_WORK           (set.memory.modeWork)
#define WORK_DIRECT         (set.memory.modeWork == ModeWork_Direct)
#define WORK_INT            (set.memory.modeWork == ModeWork_MemInt)
#define WORK_LAST           (set.memory.modeWork == ModeWork_Latest)
#define SHOW_IN_INT_SAVED   (set.memory.modeShowIntMem == ModeShowIntMem_Saved)
#define SHOW_IN_INT_DIRECT  (set.memory.modeShowIntMem == ModeShowIntMem_Direct)
#define SHOW_IN_INT_BOTH    (set.memory.modeShowIntMem == ModeShowIntMem_Both)


int sMemory_NumBytesInChannel(bool forCalculate);
int sMemory_NumPointsInChannel(void);
FPGA_NUM_POINTS NumPoints_2_FPGA_NUM_POINTS(int numPoints);
int FPGA_NUM_POINTS_2_NumPoints(FPGA_NUM_POINTS numPoints);
