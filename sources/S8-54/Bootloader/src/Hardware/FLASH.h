#pragma once


#include "defines.h"


#define ADDR_FLASH_SECTOR_0     ((uint)0x08000000)  // 16k
#define ADDR_FLASH_SECTOR_1     ((uint)0x08004000)  // 16k
#define ADDR_FLASH_SECTOR_2     ((uint)0x08008000)  // 16k
#define ADDR_FLASH_SECTOR_3     ((uint)0x0800C000)  // 16k
#define ADDR_FLASH_SECTOR_4     ((uint)0x08010000)  // 64k
#define ADDR_SECTOR_PROGRAM_0   ((uint)0x08020000)  // 128k |
#define ADDR_SECTOR_PROGRAM_1   ((uint)0x08040000)  // 128k | В этих секторах хранится наша программа 
#define ADDR_SECTOR_PROGRAM_2   ((uint)0x08060000)  // 128k /
#define ADDR_FLASH_SECTOR_8     ((uint)0x08080000)  // 128k
#define ADDR_FLASH_SECTOR_9     ((uint)0x080A0000)  // 128k
#define ADDR_FLASH_SECTOR_10    ((uint)0x080C0000)  // 128k
#define ADDR_FLASH_SECTOR_11    ((uint)0x080E0000)  // 128k
#define ADDR_FLASH_SECTOR_12    ((uint)0x08100000)  // 16k
#define ADDR_FLASH_SECTOR_13    ((uint)0x08104000)  // 16k
#define ADDR_FLASH_SECTOR_14    ((uint)0x08108000)  // 16k
#define ADDR_FLASH_SECTOR_15    ((uint)0x0810C000)  // 16k
#define ADDR_FLASH_SECTOR_16    ((uint)0x08110000)  // 64k
#define ADDR_FLASH_SECTOR_17    ((uint)0x08120000)  // 128k
#define ADDR_FLASH_SECTOR_18    ((uint)0x08140000)  // 128k
#define ADDR_FLASH_SECTOR_19    ((uint)0x08160000)  // 128k
#define ADDR_FLASH_SECTOR_20    ((uint)0x08180000)  // 128k
#define ADDR_FLASH_SECTOR_21    ((uint)0x081A0000)  // 128k
#define ADDR_FLASH_SECTOR_22    ((uint)0x081C0000)  // 128k
#define ADDR_FLASH_SECTOR_23    ((uint)0x081E0000)  // 128k


void FLASH_Prepare(void);                                // Очищает сектора, предназначенные для записи прошивки
void WriteData(uint address, uint8 *data, int size);
