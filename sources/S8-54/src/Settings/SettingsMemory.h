#pragma once


#include "Settings.h"


#define FILE_NAME_MASK          (set.memory.fileNameMask)
#define FILE_NAME               (set.memory.fileName)
#define INDEX_SYMBOL            (set.memory.indexCurSymbolNameMask)
#define FLASH_AUTO_CONNECT      (set.memory.flashAutoConnect)

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

#define MODE_BTN_MEMORY         (set.memory.modeBtnMemory)
#define MODE_BTN_MEMORY_MENU    (MODE_BTN_MEMORY == ModeBtnMemory_Menu)
#define MODE_BTN_MEMORY_SAVE    (MODE_BTN_MEMORY == ModeBtnMemory_Save)

#define MODE_SAVE               (set.memory.modeSaveSignal)
#define MODE_SAVE_BMP           (MODE_SAVE == ModeSaveSignal_BMP)
#define MODE_SAVE_TXT           (MODE_SAVE == ModeSaveSignal_TXT)

#define SHOW_DIRECT_IN_MEM_INT  (set.memory.modeShowIntMem == ModeShowIntMem_Direct)


int sMemory_NumBytesInChannel(bool forCalculate);
int sMemory_NumPointsInChannel(void);
NumPoinstFPGA NumPoints_2_FPGA_NUM_POINTS(int numPoints);
int FPGA_NUM_POINTS_2_NumPoints(NumPoinstFPGA numPoints);
