#pragma once


#include "Settings.h"


#define FILE_NAME_MASK          (set.memory.fileNameMask)
#define FILE_NAME               (set.memory.fileName)
#define INDEX_SYMBOL            (set.memory.indexCurSymbolNameMask)

#define FILE_NAMING_MODE        (set.memory.fileNamingMode)
#define FILE_NAMING_MODE_MASK   (FILE_NAMING_MODE == FileNamingMode_Mask)
#define FILE_NAMING_MODE_MANUAL (FILE_NAMING_MODE == FileNamingMode_Manually)

#define MODE_WORK               (set.memory.modeWork)
#define WORK_DIRECT             (MODE_WORK == ModeWork_Direct)
#define WORK_INT                (MODE_WORK == ModeWork_MemInt)
#define WORK_LAST               (MODE_WORK == ModeWork_Latest)

#define SHOW_IN_INT_SAVED       (set.memory.modeShowIntMem == ModeShowIntMem_Saved)
#define SHOW_IN_INT_DIRECT      (set.memory.modeShowIntMem == ModeShowIntMem_Direct)
#define SHOW_IN_INT_BOTH        (set.memory.modeShowIntMem == ModeShowIntMem_Both)

#define FPGA_NUM_POINTS         (set.memory.fpgaNumPoints)
#define FPGA_NUM_POINTS_512     (FPGA_NUM_POINTS == FNP_512)
#define FPGA_NUM_POINTS_8k      (FPGA_NUM_POINTS == FNP_8k)
#define FPGA_NUM_POINTS_16k     (FPGA_NUM_POINTS == FNP_16k)
#define FPGA_NUM_POINTS_32k     (FPGA_NUM_POINTS == FNP_32k)


int sMemory_NumBytesInChannel(bool forCalculate);
int sMemory_NumPointsInChannel(void);
NumPoinstFPGA NumPoints_2_FPGA_NUM_POINTS(int numPoints);
int FPGA_NUM_POINTS_2_NumPoints(NumPoinstFPGA numPoints);
