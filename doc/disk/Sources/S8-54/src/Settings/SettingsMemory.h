#pragma once
#include "Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsMemory Settings Memory
 *  @{
 */

#define FILE_NAME_MASK          (set.mem_FileNameMask)
#define FILE_NAME               (set.mem_FileName)
#define INDEX_SYMBOL            (set.mem_IndexCurSymbolNameMask)
#define FLASH_AUTO_CONNECT      (set.mem_FlashAutoConnect)

#define FILE_NAMING_MODE        (set.mem_FileNamingMode)
#define FILE_NAMING_MODE_MASK   (FILE_NAMING_MODE == FileNamingMode_Mask)
#define FILE_NAMING_MODE_MANUAL (FILE_NAMING_MODE == FileNamingMode_Manually)

#define MODE_WORK               (set.mem_ModeWork)
#define MODE_WORK_DIR           (MODE_WORK == ModeWork_Dir)
#define MODE_WORK_RAM           (MODE_WORK == ModeWork_RAM)
#define MODE_WORK_ROM           (MODE_WORK == ModeWork_ROM)


#define SHOW_IN_INT             (set.mem_ModeShowIntMem)
#define SHOW_IN_INT_SAVED       (SHOW_IN_INT == ModeShowIntMem_Saved)
#define SHOW_IN_INT_DIRECT      (SHOW_IN_INT == ModeShowIntMem_Direct)
#define SHOW_IN_INT_BOTH        (SHOW_IN_INT == ModeShowIntMem_Both)

#define FPGA_ENUM_POINTS        (set.mem_ENumPointsFPGA)
#define FPGA_POINTS_512         (FPGA_ENUM_POINTS == FNP_512)
#define FPGA_POINTS_8k          (FPGA_ENUM_POINTS == FNP_8k)
#define FPGA_POINTS_16k         (FPGA_ENUM_POINTS == FNP_16k)
#define FPGA_POINTS_32k         (FPGA_ENUM_POINTS == FNP_32k)

#define MODE_BTN_MEMORY         (set.mem_ModeBtnMemory)
#define MODE_BTN_MEMORY_MENU    (MODE_BTN_MEMORY == ModeBtnMemory_Menu)
#define MODE_BTN_MEMORY_SAVE    (MODE_BTN_MEMORY == ModeBtnMemory_Save)

#define MODE_SAVE               (set.mem_ModeSaveSignal)
#define MODE_SAVE_BMP           (MODE_SAVE == ModeSaveSignal_BMP)
#define MODE_SAVE_TXT           (MODE_SAVE == ModeSaveSignal_TXT)

#define MEM_DATA_SCALE          (set.mem_DataScale)
#define MEM_DATA_SCALE_RECALC   (MEM_DATA_SCALE == MemDataScale_Recalculated)

#define SET_BYTES_IN_CHANNEL    sMemory_NumBytesInChannel_()
#define SET_POINTS_IN_CHANNEL   sMemory_NumPointsInChannel_()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int sMemory_NumPointsInChannel_(void);
int sMemory_NumBytesInChannel_(void);

ENumPointsFPGA NumPoints_2_ENumPoints(int numPoints);
int ENumPoints_2_NumPoints(ENumPointsFPGA numPoints);


/** @}  @}
 */
